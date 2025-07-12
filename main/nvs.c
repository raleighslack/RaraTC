#include "nvs.h"

uint64_t serial_number;
const char* serial_number_key = "serial_no";
nvs_handle_t handle;
static const char *TAG = "nvs";

esp_err_t nvs_rara_init() {
    esp_err_t currentError = ESP_OK;
    currentError = nvs_flash_init();
    return currentError;
}

esp_err_t nvs_rara_deinit() {
    esp_err_t currentError = ESP_OK;
    currentError = nvs_flash_deinit();
    return currentError;
}

uint64_t get_serial_no() {
    ESP_ERROR_CHECK(nvs_open(serial_number_key, NVS_READONLY, &handle));
    nvs_type_t nvs_type;
    ESP_ERROR_CHECK(nvs_find_key(handle, serial_number_key, &nvs_type));
    if (nvs_type == NULL) {
        ESP_LOGE(TAG, "No serial number found");
    }
    ESP_ERROR_CHECK(nvs_get_u64(handle, serial_number_key, &serial_number));
    nvs_close(handle);
    return serial_number;
}
esp_err_t set_serial_no(uint64_t serial_number_write) {
    esp_err_t currentError = ESP_OK;
    currentError += nvs_open(serial_number_key, NVS_READWRITE, &handle);
    currentError += nvs_set_u64(handle, serial_number_key, serial_number_write);
    currentError += nvs_commit(handle);
    nvs_close(handle);
    return currentError;
}
