//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "nvs_utilities.h"

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_utilities.h"
#include "platform_config.h"

const char current_namespace[] = "config";
const char settings_partition[] = "settings";
static const char * TAG = "nvs_utilities";

typedef struct {
    nvs_type_t type;
    const char *str;
} type_str_pair_t;

static const type_str_pair_t type_str_pair[] = {
    { NVS_TYPE_I8, "i8" },
    { NVS_TYPE_U8, "u8" },
    { NVS_TYPE_U16, "u16" },
    { NVS_TYPE_I16, "i16" },
    { NVS_TYPE_U32, "u32" },
    { NVS_TYPE_I32, "i32" },
    { NVS_TYPE_U64, "u64" },
    { NVS_TYPE_I64, "i64" },
    { NVS_TYPE_STR, "str" },
    { NVS_TYPE_BLOB, "blob" },
    { NVS_TYPE_ANY, "any" },
};

static const size_t TYPE_STR_PAIR_SIZE = sizeof(type_str_pair) / sizeof(type_str_pair[0]);
void print_blob(const char *blob, size_t len)
{
    for (int i = 0; i < len; i++) {
        printf("%02x", blob[i]);
    }
    printf("\n");
}
nvs_type_t str_to_type(const char *type)
{
    for (int i = 0; i < TYPE_STR_PAIR_SIZE; i++) {
        const type_str_pair_t *p = &type_str_pair[i];
        if (strcmp(type, p->str) == 0) {
            return  p->type;
        }
    }

    return NVS_TYPE_ANY;
}
const char *type_to_str(nvs_type_t type)
{
    for (int i = 0; i < TYPE_STR_PAIR_SIZE; i++) {
        const type_str_pair_t *p = &type_str_pair[i];
        if (p->type == type) {
            return  p->str;
        }
    }

    return "Unknown";
}
void initialize_nvs() {
	ESP_LOGI(TAG,  "Initializing flash nvs ");
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_LOGW(TAG,  "%s. Erasing nvs flash", esp_err_to_name(err));
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	if(err != ESP_OK){
		ESP_LOGE(TAG,  "nvs_flash_init failed. %s.", esp_err_to_name(err));
	}
	ESP_ERROR_CHECK(err);
	ESP_LOGI(TAG,  "Initializing nvs partition %s",settings_partition);
	err = nvs_flash_init_partition(settings_partition);
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_LOGW(TAG,  "%s. Erasing nvs on partition %s",esp_err_to_name(err),settings_partition);
		ESP_ERROR_CHECK(nvs_flash_erase_partition(settings_partition));
		err = nvs_flash_init_partition(settings_partition);
	}
	if(err!=ESP_OK){
		ESP_LOGE(TAG,  "nvs_flash_init_partition failed. %s",esp_err_to_name(err));
	}
	ESP_ERROR_CHECK(err);
	ESP_LOGD(TAG,  "nvs init completed");
}

esp_err_t nvs_load_config(){
	nvs_entry_info_t info;
	esp_err_t err = ESP_OK;
	size_t malloc_int = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	size_t malloc_spiram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

	nvs_iterator_t it = nvs_entry_find(settings_partition, NULL, NVS_TYPE_ANY);
	if(it == NULL) {
		ESP_LOGW(TAG,   "empty nvs partition %s, namespace %s",settings_partition,current_namespace );
	}
	while (it != NULL) {
		nvs_entry_info(it, &info);

		if(strstr(info.namespace_name, current_namespace)) {
			void * value = get_nvs_value_alloc(info.type,info.key);
			if(value==NULL)
			{
				ESP_LOGE(TAG,  "nvs read failed.");
				return ESP_FAIL;
			}
			config_set_value(info.type, info.key, value);
			free(value );
		}
		it = nvs_entry_next(it);
	}
	char * json_string= config_alloc_get_json(false);
	if(json_string!=NULL) {
		ESP_LOGD(TAG,  "config json : %s\n", json_string);
		free(json_string);
	}
	ESP_LOGD(TAG,"Config memory usage.  Heap internal:%zu (min:%zu) (used:%zu) external:%zu (min:%zu) (used:%zd)",
						heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
						heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL),
						malloc_int-heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
						heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
						heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM),
						malloc_spiram -heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
	return err;
}


esp_err_t store_nvs_value(nvs_type_t type, const char *key, void * data) {
	if (type == NVS_TYPE_BLOB)
		return ESP_ERR_NVS_TYPE_MISMATCH;
	return store_nvs_value_len(type, key, data,0);
}
esp_err_t store_nvs_value_len(nvs_type_t type, const char *key, void * data,
		size_t data_len) {
	esp_err_t err;
	nvs_handle nvs;

	if (type == NVS_TYPE_ANY) {
		return ESP_ERR_NVS_TYPE_MISMATCH;
	}

	err = nvs_open_from_partition(settings_partition, current_namespace, NVS_READWRITE, &nvs);
	if (err != ESP_OK) {
		return err;
	}

	if (type == NVS_TYPE_I8) {
		err = nvs_set_i8(nvs, key, *(int8_t *) data);
	} else if (type == NVS_TYPE_U8) {
		err = nvs_set_u8(nvs, key, *(uint8_t *) data);
	} else if (type == NVS_TYPE_I16) {
		err = nvs_set_i16(nvs, key, *(int16_t *) data);
	} else if (type == NVS_TYPE_U16) {
		err = nvs_set_u16(nvs, key, *(uint16_t *) data);
	} else if (type == NVS_TYPE_I32) {
		err = nvs_set_i32(nvs, key, *(int32_t *) data);
	} else if (type == NVS_TYPE_U32) {
		err = nvs_set_u32(nvs, key, *(uint32_t *) data);
	} else if (type == NVS_TYPE_I64) {
		err = nvs_set_i64(nvs, key, *(int64_t *) data);
	} else if (type == NVS_TYPE_U64) {
		err = nvs_set_u64(nvs, key, *(uint64_t *) data);
	} else if (type == NVS_TYPE_STR) {
		err = nvs_set_str(nvs, key, data);
	} else if (type == NVS_TYPE_BLOB) {
		err = nvs_set_blob(nvs, key, (void *) data, data_len);
	}
	if (err == ESP_OK) {
		err = nvs_commit(nvs);
		if (err == ESP_OK) {
			ESP_LOGI(TAG,   "Value stored under key '%s'", key);
		}
	}
	nvs_close(nvs);
	return err;
}
void * get_nvs_value_alloc(nvs_type_t type, const char *key) {
	nvs_handle nvs;
	esp_err_t err;
	void * value=NULL;

	err = nvs_open_from_partition(settings_partition, current_namespace, NVS_READONLY, &nvs);
	if (err != ESP_OK) {
		ESP_LOGE(TAG,  "Could not open the nvs storage.");
		return NULL;
	}

	if (type == NVS_TYPE_I8) {
		value=malloc(sizeof(int8_t));
		err = nvs_get_i8(nvs, key, (int8_t *) value);
	} else if (type == NVS_TYPE_U8) {
		value=malloc(sizeof(uint8_t));
		err = nvs_get_u8(nvs, key, (uint8_t *) value);
	} else if (type == NVS_TYPE_I16) {
		value=malloc(sizeof(int16_t));
		err = nvs_get_i16(nvs, key, (int16_t *) value);
	} else if (type == NVS_TYPE_U16) {
		value=malloc(sizeof(uint16_t));
		err = nvs_get_u16(nvs, key, (uint16_t *) value);
	} else if (type == NVS_TYPE_I32) {
		value=malloc(sizeof(int32_t));
		err = nvs_get_i32(nvs, key, (int32_t *) value);
	} else if (type == NVS_TYPE_U32) {
		value=malloc(sizeof(uint32_t));
		err = nvs_get_u32(nvs, key, (uint32_t *) value);
	} else if (type == NVS_TYPE_I64) {
		value=malloc(sizeof(int64_t));
		err = nvs_get_i64(nvs, key, (int64_t *) value);
	} else if (type == NVS_TYPE_U64) {
		value=malloc(sizeof(uint64_t));
		err = nvs_get_u64(nvs, key, (uint64_t *) value);
	} else if (type == NVS_TYPE_STR) {
		size_t len=0;
		err = nvs_get_str(nvs, key, NULL, &len);
		if (err == ESP_OK) {
			value=malloc(len);
			err = nvs_get_str(nvs, key, value, &len);
			}
	} else if (type == NVS_TYPE_BLOB) {
		size_t len;
		err = nvs_get_blob(nvs, key, NULL, &len);
		if (err == ESP_OK) {
			value=malloc(len+1);
			err = nvs_get_blob(nvs, key, value, &len);
		}
	}
	if(err!=ESP_OK){
		ESP_LOGD(TAG,  "Value not found for key %s",key);
		if(value!=NULL)
			free(value);
		value=NULL;
	}
	nvs_close(nvs);
	return value;
}
esp_err_t get_nvs_value(nvs_type_t type, const char *key, void*value, const uint8_t buf_size) {
	nvs_handle nvs;
	esp_err_t err;

	err = nvs_open_from_partition(settings_partition, current_namespace, NVS_READONLY, &nvs);
	if (err != ESP_OK) {
		return err;
	}

	if (type == NVS_TYPE_I8) {
		err = nvs_get_i8(nvs, key, (int8_t *) value);
	} else if (type == NVS_TYPE_U8) {
		err = nvs_get_u8(nvs, key, (uint8_t *) value);
	} else if (type == NVS_TYPE_I16) {
		err = nvs_get_i16(nvs, key, (int16_t *) value);
	} else if (type == NVS_TYPE_U16) {
		err = nvs_get_u16(nvs, key, (uint16_t *) value);
	} else if (type == NVS_TYPE_I32) {
		err = nvs_get_i32(nvs, key, (int32_t *) value);
	} else if (type == NVS_TYPE_U32) {
		err = nvs_get_u32(nvs, key, (uint32_t *) value);
	} else if (type == NVS_TYPE_I64) {
		err = nvs_get_i64(nvs, key, (int64_t *) value);
	} else if (type == NVS_TYPE_U64) {
		err = nvs_get_u64(nvs, key, (uint64_t *) value);
	} else if (type == NVS_TYPE_STR) {
		size_t len;
		if ((err = nvs_get_str(nvs, key, NULL, &len)) == ESP_OK) {
			if (len > buf_size) {
				//ESP_LOGE("Error reading value for %s.  Buffer size: %d, Value Length: %d", key, buf_size, len);
				err = ESP_FAIL;
			} else {
				err = nvs_get_str(nvs, key, value, &len);
			}
		}
	} else if (type == NVS_TYPE_BLOB) {
		size_t len;
		if ((err = nvs_get_blob(nvs, key, NULL, &len)) == ESP_OK) {

			if (len > buf_size) {
				//ESP_LOGE("Error reading value for %s.  Buffer size: %d, Value Length: %d",
				//		key, buf_size, len);
				err = ESP_FAIL;
			} else {
				err = nvs_get_blob(nvs, key, value, &len);
			}
		}
	}

	nvs_close(nvs);
	return err;
}
esp_err_t erase_nvs(const char *key)
{
    nvs_handle nvs;

    esp_err_t err = nvs_open(current_namespace, NVS_READWRITE, &nvs);
    if (err == ESP_OK) {
        err = nvs_erase_key(nvs, key);
        if (err == ESP_OK) {
            err = nvs_commit(nvs);
            if (err == ESP_OK) {
                ESP_LOGI(TAG,   "Value with key '%s' erased", key);
            }
        }
        nvs_close(nvs);
    }

    return err;
}

