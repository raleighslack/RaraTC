#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "smpte_timecode.h"
#include "mcp7940.h"

#define USB_WAKE            0
#define LIPO_VOLTAGE        1
#define BTN_INPUT_PIN       3
#define LTC_OUTPUT_PIN      4
#define RTC_INPUT_PIN       5
#define LTC_FRAMERATE       24
#define LTC_BITS_PER_FRAME  80

#define GPIO_OUTPUT_PIN_SEL ((1ULL<<LTC_OUTPUT_PIN) | (1ULL<<LTC_OUTPUT_PIN))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<RTC_INPUT_PIN) | (1ULL<<BTN_INPUT_PIN))

static const char *TAG = "main";
static QueueHandle_t gpio_evt_queue;

const float period_us = (((float)1/LTC_FRAMERATE)/LTC_BITS_PER_FRAME)*1000000;
const int64_t half_period_us = period_us/2;
const float frame_period_us = ((float)1/LTC_FRAMERATE) * 1000000;

bool timerIsRunning = false;
simple_frame current_simple_frame;
ltc_frame current_frame;
uint8_t current_bits[10];
esp_timer_handle_t periodic_timecode;
bool state = true;
volatile int bit_index = 0;                                                                      //value from 0-79, the current transmitting bit
volatile int bit_local_counter = 0;                                                              //value either 0 or 1, either first half of period or 2nd half

bool get_bit(uint8_t value, uint8_t index) {
    if (index > 7) return false;                                                        // Ensure index is within bounds
    return (value >> index) & 1;
}

static void gpio_task(void* arg) {
    int gpio_num;
    while (1) {
        if (xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY)) {
            if(gpio_num == RTC_INPUT_PIN) {
                uint8_t hours = get_rtc_hours();
                uint8_t minutes = get_rtc_minutes();
                uint8_t seconds = get_rtc_seconds();

                int timeSinceTenSecs = ((hours * 60 * 60) + (minutes * 60) + seconds) - 10;
                ESP_LOGI(TAG, "RTC ISR: %d:%d:%d, TIME SINCE 10secs: %d", hours, minutes, seconds, timeSinceTenSecs);

                if (timerIsRunning) { //Makes sure the timer is not running, in case of updating the rtc inbetween frames
                    ESP_ERROR_CHECK(esp_timer_stop(periodic_timecode));
                }

                current_simple_frame.frame = 0;
                current_simple_frame.second = seconds;
                current_simple_frame.minute = minutes;
                current_simple_frame.hour = hours;

                ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timecode, half_period_us));
                timerIsRunning = true;
                vTaskDelay(100);
                ESP_ERROR_CHECK(esp_timer_stop(periodic_timecode));
                timerIsRunning = false;
            } else {
                ESP_LOGI(TAG, "BTN ISR");
            }
        }
    }
}

static void gpio_isr_handler(void* arg)
{
    int gpio_num = (int) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void IRAM_ATTR periodic_timecode_callback(void* arg)
{
    bool bit = get_bit(reverse_bits(current_bits[(bit_index / 8)]), bit_index % 8);     //finally, a use for the modulo
    if(bit_local_counter == 0) {
        state = !state;
    }
    if(bit_local_counter == 1 && bit) {
        state = !state;
    }
    gpio_set_level(LTC_OUTPUT_PIN, state);
    bit_local_counter++;
    if(bit_local_counter >1) {
        bit_local_counter = 0;
        bit_index++;
    }
    if(bit_index > 79) {
        bit_index = 0;
        current_simple_frame.frame++;
        if(current_simple_frame.frame >= LTC_FRAMERATE) {
            current_simple_frame.frame = 0;
        }
        create_frame_from_timecode(&current_frame, current_simple_frame.frame, current_simple_frame.second, current_simple_frame.minute, current_simple_frame.hour);
        create_bits_from_frame(current_bits, current_frame);
    }
}

void print_binary(uint8_t bits[10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (bits[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

void app_main(void)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.pull_up_en = false;
    io_conf.pull_down_en = false;
    gpio_config(&io_conf);
    
    ESP_ERROR_CHECK(init_rtc());                                                        //Initializes the RTC

    current_simple_frame.hour = get_rtc_hours();
    current_simple_frame.minute = get_rtc_minutes();

    const esp_timer_create_args_t periodic_timecode_args = {
        .callback = &periodic_timecode_callback,
        /* name is optional, but may help identify the timer when debugging */
        .name = "periodic_timecode"
    };
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timecode_args, &periodic_timecode));

    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1));
    ESP_ERROR_CHECK(gpio_isr_handler_add(RTC_INPUT_PIN, gpio_isr_handler, (void*) RTC_INPUT_PIN));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_INPUT_PIN, gpio_isr_handler, (void*) BTN_INPUT_PIN));

    gpio_evt_queue = xQueueCreate(10, sizeof(int));
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    if((get_rtc_register(REG_SECONDS) | 127) != 0xFF) {
        ESP_ERROR_CHECK(set_rtc_register(REG_CONTROL, 0x48));                           //sets 1hz square wave output, and external clock input in the rtc
        ESP_ERROR_CHECK(set_rtc_register(REG_SECONDS, 0x80));                           //tells the rtc to actually start keeping time
    }
}
