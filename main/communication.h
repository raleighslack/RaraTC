#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_crc.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "nvs_flash.h"

#define CONFIG_ESPNOW_CHANNEL       1
#define CONFIG_ESPNOW_LMK           "lmk1234567890123"
#define CONFIG_ESPNOW_PMK           "pmk1234567890123"
#define ESPNOW_MAXDELAY             512
#define ESPNOW_QUEUE_SIZE           6
#define ESPNOW_WIFI_MODE            WIFI_MODE_STA
#define ESPNOW_WIFI_IF              ESP_IF_WIFI_STA
#define CONFIG_ESPNOW_SEND_COUNT    100
#define CONFIG_ESPNOW_SEND_DELAY    1000
#define CONFIG_ESPNOW_SEND_LEN      10

#define IS_BROADCAST_ADDR(addr) (memcmp(addr, s_example_broadcast_mac, ESP_NOW_ETH_ALEN) == 0)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EXAMPLE_ESPNOW_SEND_CB,
    EXAMPLE_ESPNOW_RECV_CB,
} example_espnow_event_id_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
} example_espnow_event_send_cb_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    int data_len;
} example_espnow_event_recv_cb_t;

typedef union {
    example_espnow_event_send_cb_t send_cb;
    example_espnow_event_recv_cb_t recv_cb;
} example_espnow_event_info_t;

/* When ESPNOW sending or receiving callback function is called, post event to ESPNOW task. */
typedef struct {
    example_espnow_event_id_t id;
    example_espnow_event_info_t info;
} example_espnow_event_t;

enum {
    EXAMPLE_ESPNOW_DATA_BROADCAST,
    EXAMPLE_ESPNOW_DATA_UNICAST,
    EXAMPLE_ESPNOW_DATA_MAX,
};

/* User defined field of ESPNOW data in this example. */
typedef struct {
    uint8_t type;                         //Broadcast or unicast ESPNOW data.
    uint8_t state;                        //Indicate that if has received broadcast ESPNOW data or not.
    uint16_t seq_num;                     //Sequence number of ESPNOW data.
    uint16_t crc;                         //CRC16 value of ESPNOW data.
    uint32_t magic;                       //Magic number which is used to determine which device to send unicast ESPNOW data.
    uint8_t payload[0];                   //Real payload of ESPNOW data.
} __attribute__((packed)) example_espnow_data_t;

/* Parameters of sending ESPNOW data. */
typedef struct {
    bool unicast;                         //Send unicast ESPNOW data.
    bool broadcast;                       //Send broadcast ESPNOW data.
    uint8_t state;                        //Indicate that if has received broadcast ESPNOW data or not.
    uint32_t magic;                       //Magic number which is used to determine which device to send unicast ESPNOW data.
    uint16_t count;                       //Total count of unicast ESPNOW data to be sent.
    uint16_t delay;                       //Delay between sending two ESPNOW data, unit: ms.
    int len;                              //Length of ESPNOW data to be sent, unit: byte.
    uint8_t *buffer;                      //Buffer pointing to ESPNOW data.
    uint8_t dest_mac[ESP_NOW_ETH_ALEN];   //MAC address of destination device.
} example_espnow_send_param_t;

void wifi_init(void);
esp_err_t espnow_init();
void example_espnow_deinit(example_espnow_send_param_t *send_param);
void example_espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
void example_espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
void example_espnow_task(void *pvParameter);
void example_espnow_data_prepare(example_espnow_send_param_t *send_param);
int example_espnow_data_parse(uint8_t *data, uint16_t data_len, uint8_t *state, uint16_t *seq, uint32_t *magic);

#ifdef __cplusplus
}
#endif
