#pragma once
#include "esp_err.h"
#include "nvs.h"
#ifdef __cplusplus
extern "C" {
#endif
extern const char current_namespace[];
extern const char settings_partition[];

#define NUM_BUFFER_LEN 101
void initialize_nvs();
esp_err_t store_nvs_value_len(nvs_type_t type, const char *key, void * data, size_t data_len);
esp_err_t store_nvs_value(nvs_type_t type, const char *key, void * data);
esp_err_t get_nvs_value(nvs_type_t type, const char *key, void*value, const uint8_t buf_size);
void * get_nvs_value_alloc(nvs_type_t type, const char *key);
esp_err_t erase_nvs(const char *key);
void print_blob(const char *blob, size_t len);
const char *type_to_str(nvs_type_t type);
nvs_type_t str_to_type(const char *type);
#ifdef __cplusplus
}
#endif
