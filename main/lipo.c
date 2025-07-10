#include "lipo.h"

static const char *TAG = "lipo";
esp_adc_cal_characteristics_t *adc_characas;

void init_lipo() {
    static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
    static const adc1_channel_t adc_channel = ADC_CHANNEL_1;
    adc1_config_width(width);
    adc1_config_channel_atten(adc_channel, ADC_ATTEN_DB_12);

    adc_characas = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, width, 1100, adc_characas);
}

float get_lipo_voltage() {
    int raw = adc1_get_raw(ADC1_CHANNEL_1);
    int voltage_divided = esp_adc_cal_raw_to_voltage(raw, adc_characas);
    float voltage = (voltage_divided/1000.0)*2.0;
    return voltage;
}