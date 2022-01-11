/*
 *  Squeezelite for esp32
 *
 *  (c) Sebastien 2019
 *      Philippe G. 2019, philippe_44@outlook.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "platform_config.h"
#include "nvs_utilities.h"
#include "platform_esp32.h"
#include "trace.h"
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
#include "cJSON.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"


#define CONFIG_COMMIT_DELAY 1000
#define LOCK_MAX_WAIT 20*CONFIG_COMMIT_DELAY
static const char * TAG = "config";
static cJSON * nvs_json=NULL;
static TimerHandle_t timer;
static SemaphoreHandle_t config_mutex = NULL;
static EventGroupHandle_t config_group;
/* @brief indicate that the ESP32 is currently connected. */
static const int CONFIG_NO_COMMIT_PENDING = BIT0;
static const int CONFIG_LOAD_BIT = BIT1;

bool config_lock(TickType_t xTicksToWait);
void config_unlock();
extern esp_err_t nvs_load_config();
void config_raise_change(bool flag);
cJSON_bool config_is_entry_changed(cJSON * entry);
bool config_set_group_bit(int bit_num,bool flag);
cJSON * config_set_value_safe(nvs_type_t nvs_type, const char *key,const void * value);
static void vCallbackFunction( TimerHandle_t xTimer );
void config_set_entry_changed_flag(cJSON * entry, cJSON_bool flag);
#define IMPLEMENT_SET_DEFAULT(t,nt) void config_set_default_## t (const char *key, t  value){\
	void * pval = malloc(sizeof(value));\
	*((t *) pval) = value;\
	config_set_default(nt, key,pval,0);\
	free(pval); }
#define IMPLEMENT_GET_NUM(t,nt) esp_err_t config_get_## t (const char *key, t *  value){\
		void * pval = config_alloc_get(nt, key);\
		if(pval!=NULL){ *value = *(t * )pval; free(pval); return ESP_OK; }\
		return ESP_FAIL;}
static void * malloc_fn(size_t sz){

	void * ptr = is_recovery_running?malloc(sz):heap_caps_malloc(sz, MALLOC_CAP_SPIRAM);
	if(ptr==NULL){
		ESP_LOGE(TAG,"malloc_fn:  unable to allocate memory!");
	}
	return ptr;
}
void init_cJSON(){
	static cJSON_Hooks hooks;
	hooks.malloc_fn=&malloc_fn;
	cJSON_InitHooks(&hooks);
}
void config_init(){
	ESP_LOGD(TAG, "Creating mutex for Config");
	config_mutex = xSemaphoreCreateMutex();
	ESP_LOGD(TAG, "Creating event group");
	config_group = xEventGroupCreate();
	ESP_LOGD(TAG, "Loading config from nvs");

	init_cJSON();
	if(nvs_json !=NULL){
		cJSON_Delete(nvs_json);
	}
	nvs_json = cJSON_CreateObject();

	config_set_group_bit(CONFIG_LOAD_BIT,true);
	nvs_load_config();
	config_set_group_bit(CONFIG_LOAD_BIT,false);
	config_start_timer();
}

void config_start_timer(){
	ESP_LOGD(TAG, "Starting config timer");
	timer = xTimerCreate("configTimer", CONFIG_COMMIT_DELAY / portTICK_RATE_MS, pdFALSE, NULL, vCallbackFunction);
    if( xTimerStart( timer , CONFIG_COMMIT_DELAY/ portTICK_RATE_MS ) != pdPASS )    {
        ESP_LOGE(TAG, "config commitment timer failed to start.");
    }

}

nvs_type_t  config_get_item_type(cJSON * entry){
	if(entry==NULL){
		ESP_LOGE(TAG,"null pointer received!");
		return true;
	}
	cJSON * item_type = cJSON_GetObjectItemCaseSensitive(entry, "type");
	if(item_type ==NULL ) {
		ESP_LOGE(TAG, "Item type not found! ");
		return 0;
	}
	ESP_LOGD(TAG,"Found item type %f",item_type->valuedouble);
	return item_type->valuedouble;
}


cJSON * config_set_value_safe(nvs_type_t nvs_type, const char *key,  const void * value){
	cJSON * entry = cJSON_CreateObject();

	double numvalue = 0;
	if(entry == NULL) {
		ESP_LOGE(TAG, "Unable to allocate memory for entry %s",key);
		return NULL;
	}

	cJSON * existing = cJSON_GetObjectItemCaseSensitive(nvs_json, key);
	if(existing !=NULL && nvs_type == NVS_TYPE_STR && config_get_item_type(existing) != NVS_TYPE_STR  ) {
		ESP_LOGW(TAG, "Storing numeric value from string");
		numvalue = atof((char *)value);
		cJSON_AddNumberToObject(entry,"value", numvalue	);
		nvs_type_t exist_type = config_get_item_type(existing);
		ESP_LOGW(TAG, "Stored  value %f from string %s as type %d",numvalue, (char *)value,exist_type);
		cJSON_AddNumberToObject(entry,"type", exist_type);
	}
	else {
		cJSON_AddNumberToObject(entry,"type", nvs_type	);
		switch (nvs_type) {
			case NVS_TYPE_I8:
				cJSON_AddNumberToObject(entry,"value", *(int8_t*)value	);
				break;
			case NVS_TYPE_I16:
				cJSON_AddNumberToObject(entry,"value", *(int16_t*)value	);
				break;
			case NVS_TYPE_I32:
				cJSON_AddNumberToObject(entry,"value", *(int32_t*)value	);
				break;
			case NVS_TYPE_U8:
				cJSON_AddNumberToObject(entry,"value", *(uint8_t*)value	);
				break;
			case NVS_TYPE_U16:
				cJSON_AddNumberToObject(entry,"value", *(uint16_t*)value	);
				break;
			case NVS_TYPE_U32:
				cJSON_AddNumberToObject(entry,"value", *(uint32_t*)value	);
				break;
			case NVS_TYPE_STR:
				cJSON_AddStringToObject(entry, "value", (char *)value);
				break;
			case NVS_TYPE_I64:
			case NVS_TYPE_U64:
			default:
				ESP_LOGE(TAG, "nvs type %u not supported", nvs_type);
				break;
		}
	}
	if(existing!=NULL ) {
		ESP_LOGV(TAG, "Changing existing entry [%s].", key);
		char * exist_str = cJSON_PrintUnformatted(existing);
		if(exist_str!=NULL){
			ESP_LOGV(TAG,"Existing entry: %s", exist_str);
			free(exist_str);
		}
		else {
			ESP_LOGV(TAG,"Failed to print existing entry");
		}
		// set commit flag as equal so we can compare
		cJSON_AddBoolToObject(entry,"chg",config_is_entry_changed(existing));
		if(!cJSON_Compare(entry,existing,false)){
			char * entry_str = cJSON_PrintUnformatted(entry);
			if(entry_str!=NULL){
				ESP_LOGD(TAG,"New config object: \n%s", entry_str );
				free(entry_str);
			}
			else {
				ESP_LOGD(TAG,"Failed to print entry");
			}
			ESP_LOGI(TAG, "Setting changed flag config [%s]", key);
			config_set_entry_changed_flag(entry,true);
			ESP_LOGI(TAG, "Updating config [%s]", key);
			cJSON_ReplaceItemInObject(nvs_json,key, entry);
			entry_str = cJSON_PrintUnformatted(entry);
			if(entry_str!=NULL){
				ESP_LOGD(TAG,"New config: %s", entry_str );
				free(entry_str);
			}
			else {
				ESP_LOGD(TAG,"Failed to print entry");
			}
		}
		else {
			ESP_LOGD(TAG, "Config not changed. ");
			cJSON_Delete(entry);
			entry = existing;
		}
	}
	else {
		// This is a new entry.
		config_set_entry_changed_flag(entry,true);
		cJSON_AddItemToObject(nvs_json, key, entry);
	}

	return entry;
}

nvs_type_t config_get_entry_type(cJSON * entry){
	if(entry==NULL){
		ESP_LOGE(TAG,"null pointer received!");
		return 0;
	}
	cJSON * entry_type = cJSON_GetObjectItemCaseSensitive(entry, "type");
	if(entry_type ==NULL ) {
		ESP_LOGE(TAG, "Entry type not found in nvs cache for existing setting.");
		return 0;
	}
	ESP_LOGV(TAG,"Found type %s",type_to_str(entry_type->valuedouble));
	return entry_type->valuedouble;
}
void config_set_entry_changed_flag(cJSON * entry, cJSON_bool flag){
	ESP_LOGV(TAG, "config_set_entry_changed_flag: begin");
	if(entry==NULL){
		ESP_LOGE(TAG,"null pointer received!");
		return;
	}
	bool bIsConfigLoading=((xEventGroupGetBits(config_group) & CONFIG_LOAD_BIT)!=0);
	bool changedFlag=bIsConfigLoading?false:flag;
	ESP_LOGV(TAG, "config_set_entry_changed_flag: retrieving chg flag from entry");
	cJSON * changed = cJSON_GetObjectItemCaseSensitive(entry, "chg");
	if(changed ==NULL ) {
		ESP_LOGV(TAG, "config_set_entry_changed_flag: chg flag not found. Adding. ");
		cJSON_AddBoolToObject(entry,"chg",changedFlag);
	}
	else {
		ESP_LOGV(TAG, "config_set_entry_changed_flag: Existing change flag found. ");
		if(cJSON_IsTrue(changed) && changedFlag){
			ESP_LOGW(TAG, "Commit flag not changed!");
		}
		else{
			ESP_LOGV(TAG, "config_set_entry_changed_flag: Updating change flag to %s",changedFlag?"TRUE":"FALSE");
			changed->type = changedFlag?cJSON_True:cJSON_False ;
		}
	}

	if(changedFlag) {
		ESP_LOGV(TAG, "config_set_entry_changed_flag: Calling config_raise_change. ");
		config_raise_change(true);
	}
	ESP_LOGV(TAG, "config_set_entry_changed_flag: done. ");
}
cJSON_bool config_is_entry_changed(cJSON * entry){
	if(entry==NULL){
		ESP_LOGE(TAG,"null pointer received!");
		return true;
	}
	cJSON * changed = cJSON_GetObjectItemCaseSensitive(entry, "chg");
	if(changed ==NULL ) {
		ESP_LOGE(TAG, "Change flag not found! ");
		return true;
	}
	return cJSON_IsTrue(changed);
}




void * config_safe_alloc_get_entry_value(nvs_type_t nvs_type, cJSON * entry){
	void * value=NULL;
	if(entry==NULL){
		ESP_LOGE(TAG,"null pointer received!");
	}
	ESP_LOGV(TAG, "getting config value type %s", type_to_str(nvs_type));
	cJSON * entry_value = cJSON_GetObjectItemCaseSensitive(entry, "value");
	if(entry_value==NULL ) {
		char * entry_str = cJSON_PrintUnformatted(entry);
		if(entry_str!=NULL){
			ESP_LOGE(TAG, "Missing config value!. Object: \n%s", entry_str);
			free(entry_str);
		}
		else{
			ESP_LOGE(TAG, "Missing config value");
		}
		return NULL;
	}

	nvs_type_t type = config_get_entry_type(entry);
	if(nvs_type != type){
		// requested value type different than the stored type
		char * entry_str = cJSON_PrintUnformatted(entry);
		if(entry_str!=NULL){
			ESP_LOGE(TAG, "Requested value type %s, found value type %s instead, Object: \n%s", type_to_str(nvs_type), type_to_str(type),entry_str);
			free(entry_str);
		}
		else{
			ESP_LOGE(TAG, "Requested value type %s, found value type %s instead", type_to_str(nvs_type), type_to_str(type));
		}

		return NULL;
	}
	if (nvs_type == NVS_TYPE_I8) {
		value=malloc(sizeof(int8_t));
		*(int8_t *)value = (int8_t)entry_value->valuedouble;
	} else if (nvs_type == NVS_TYPE_U8) {
		value=malloc(sizeof(uint8_t));
		*(uint8_t *)value = (uint8_t)entry_value->valuedouble;
	} else if (nvs_type == NVS_TYPE_I16) {
		value=malloc(sizeof(int16_t));
		*(int16_t *)value = (int16_t)entry_value->valuedouble;
	} else if (nvs_type == NVS_TYPE_U16) {
		value=malloc(sizeof(uint16_t));
		*(uint16_t *)value = (uint16_t)entry_value->valuedouble;
	} else if (nvs_type == NVS_TYPE_I32) {
		value=malloc(sizeof(int32_t));
		*(int32_t *)value = (int32_t)entry_value->valuedouble;
	} else if (nvs_type == NVS_TYPE_U32) {
		value=malloc(sizeof(uint32_t));
		*(uint32_t *)value = (uint32_t)entry_value->valuedouble;
	} else if (nvs_type == NVS_TYPE_I64) {
		value=malloc(sizeof(int64_t));
		*(int64_t *)value = (int64_t)entry_value->valuedouble;
	} else if (nvs_type == NVS_TYPE_U64) {
		value=malloc(sizeof(uint64_t));
		*(uint64_t *)value = (uint64_t)entry_value->valuedouble;
	} else if (nvs_type == NVS_TYPE_STR) {
		if(!cJSON_IsString(entry_value)){
			char * entry_str = cJSON_PrintUnformatted(entry);
			if(entry_str!=NULL){
				ESP_LOGE(TAG, "requested value type string, config type is different. key: %s, value: %s, type %d, Object: \n%s",
						str_or_null(entry_value->string),
						str_or_null(entry_value->valuestring),
						entry_value->type,
						str_or_null(entry_str));
				free(entry_str);
			}
			else {
				ESP_LOGE(TAG, "requested value type string, config type is different. key: %s, value: %s, type %d",
						str_or_null(entry_value->string),
						str_or_null(entry_value->valuestring),
						entry_value->type);
			}
		}
		else {
			size_t len=strlen(cJSON_GetStringValue(entry_value));
			value=(void *)heap_caps_malloc(len+1, MALLOC_CAP_DMA);
			memset(value,0x00,len+1);
			memcpy(value,cJSON_GetStringValue(entry_value),len);
			if(value==NULL){
				char * entry_str = cJSON_PrintUnformatted(entry);
				if(entry_str!=NULL){
					ESP_LOGE(TAG, "strdup failed on value for object \n%s",entry_str);
					free(entry_str);
				}
				else {
					ESP_LOGE(TAG, "strdup failed on value");
				}
			}
		}
	} else if (nvs_type == NVS_TYPE_BLOB) {
		ESP_LOGE(TAG, "Unsupported type NVS_TYPE_BLOB");
	}
	return value;
}

void config_commit_to_nvs(){
	ESP_LOGI(TAG,"Committing configuration to nvs. Locking config object.");
	if(!config_lock(LOCK_MAX_WAIT/portTICK_PERIOD_MS)){
		ESP_LOGE(TAG, "config_commit_to_nvs: Unable to lock config for commit ");
		return ;
	}
	if(nvs_json==NULL){
		ESP_LOGE(TAG, ": cJSON nvs cache object not set.");
		return;
	}
	ESP_LOGV(TAG,"config_commit_to_nvs. Config Locked!");
	cJSON * entry=nvs_json->child;
	while(entry!= NULL){
		char * entry_str = cJSON_PrintUnformatted(entry);
		if(entry_str!=NULL){
			ESP_LOGV(TAG,"config_commit_to_nvs processing item %s",entry_str);
			free(entry_str);
		}

		if(config_is_entry_changed(entry)){
			ESP_LOGD(TAG, "Committing entry %s value to nvs.",(entry->string==NULL)?"UNKNOWN":entry->string);
			nvs_type_t type = config_get_entry_type(entry);
			void * value = config_safe_alloc_get_entry_value(type, entry);
			if(value!=NULL){
				size_t len=strlen(entry->string);
				char * key=(void *)heap_caps_malloc(len+1, MALLOC_CAP_DMA);
				memset(key,0x00,len+1);
				memcpy(key,entry->string,len);
				esp_err_t err = store_nvs_value(type,key,value);
				free(key);
				free(value);

				if(err!=ESP_OK){
					char * entry_str = cJSON_PrintUnformatted(entry);
					if(entry_str!=NULL){
						ESP_LOGE(TAG, "Error comitting value to nvs for key %s, Object: \n%s",entry->string,entry_str);
						free(entry_str);
					}
					else {
						ESP_LOGE(TAG, "Error comitting value to nvs for key %s",entry->string);
					}
				}
				else {
					config_set_entry_changed_flag(entry, false);
				}
			}
			else {
				char * entry_str = cJSON_PrintUnformatted(entry);
				if(entry_str!=NULL){
					ESP_LOGE(TAG, "Unable to retrieve value. Error comitting value to nvs for key %s, Object: \n%s",entry->string,entry_str);
					free(entry_str);
				}
				else {
					ESP_LOGE(TAG, "Unable to retrieve value. Error comitting value to nvs for key %s",entry->string);
				}
			}
		}
		else {
			ESP_LOGV(TAG,"config_commit_to_nvs. Item already committed.  Ignoring.");
		}
		taskYIELD();  /* allows the freeRTOS scheduler to take over if needed. */
		entry = entry->next;
	}
	ESP_LOGV(TAG,"config_commit_to_nvs. Resetting the global commit flag.");
	config_raise_change(false);
	ESP_LOGV(TAG,"config_commit_to_nvs. Releasing the lock object.");
	config_unlock();
	ESP_LOGI(TAG,"Done Committing configuration to nvs.");
}
bool config_has_changes(){
	return  (xEventGroupGetBits(config_group) & CONFIG_NO_COMMIT_PENDING)==0;
}


bool wait_for_commit(){
	bool commit_pending=(xEventGroupGetBits(config_group) & CONFIG_NO_COMMIT_PENDING)==0;
	while (commit_pending){
		ESP_LOGW(TAG,"Waiting for config commit ...");
		commit_pending = (xEventGroupWaitBits(config_group, CONFIG_NO_COMMIT_PENDING,pdFALSE, pdTRUE, (CONFIG_COMMIT_DELAY*2) / portTICK_PERIOD_MS) & CONFIG_NO_COMMIT_PENDING)==0;
		if(commit_pending){
			ESP_LOGW(TAG,"Timeout waiting for config commit.");
	    }
	    else {
	    	ESP_LOGI(TAG,"Config committed!");
	    }
	}
	return !commit_pending;
}

bool config_lock(TickType_t xTicksToWait) {
	ESP_LOGV(TAG, "Locking config json object");
	if( xSemaphoreTake( config_mutex, xTicksToWait ) == pdTRUE ) {
		ESP_LOGV(TAG, "config Json object locked!");
		return true;
	}
	else {
		ESP_LOGE(TAG, "Semaphore take failed. Unable to lock config Json object mutex");
		return false;
	}
}

void config_unlock() {
	ESP_LOGV(TAG, "Unlocking json buffer!");
	xSemaphoreGive( config_mutex );
}

static void vCallbackFunction( TimerHandle_t xTimer ) {
	static int cnt=0;
	if(config_has_changes()){
		ESP_LOGI(TAG, "configuration has some uncommitted entries");
		config_commit_to_nvs();
	}
	else{
		if(++cnt>=15){
			ESP_LOGV(TAG,"commit timer: commit flag not set");
			cnt=0;
		}
	}
	xTimerReset( xTimer, 10 );
}
void config_raise_change(bool change_found){
	if(config_set_group_bit(CONFIG_NO_COMMIT_PENDING,!change_found))
	{
		ESP_LOGD(TAG,"Config commit set to %s",change_found?"Pending Commit":"Committed");
	}
}
bool config_set_group_bit(int bit_num,bool flag){
	bool result = true;
	int curFlags=xEventGroupGetBits(config_group);
	if((curFlags & CONFIG_LOAD_BIT) && bit_num == CONFIG_NO_COMMIT_PENDING ){
		ESP_LOGD(TAG,"Loading config, ignoring changes");
		result = false;
	}
	if(result){
		bool curBit=(xEventGroupGetBits(config_group) & bit_num);
		if(curBit == flag){
			ESP_LOGV(TAG,"Flag %d already %s", bit_num, flag?"Set":"Cleared");
			result = false;
		}
	}
	if(result){
		ESP_LOGV(TAG,"%s Flag %d ", flag?"Setting":"Clearing",bit_num);
		if(!flag){
			xEventGroupClearBits(config_group, bit_num);
		}
		else {
			xEventGroupSetBits(config_group, bit_num);
		}
	}
	return result;
}

void config_set_default(nvs_type_t type, const char *key, void * default_value, size_t blob_size) {
	if(!config_lock(LOCK_MAX_WAIT/portTICK_PERIOD_MS)){
		ESP_LOGE(TAG, "Unable to lock config");
		return;
	}

	ESP_LOGV(TAG, "Checking if key %s exists in nvs cache for type %s.", key,type_to_str(type));
	cJSON * entry = cJSON_GetObjectItemCaseSensitive(nvs_json, key);

	if(entry !=NULL){
		ESP_LOGV(TAG, "Entry found.");
	}
	else {
		// Value was not found
		ESP_LOGW(TAG, "Adding default value for [%s].", key);
		entry=config_set_value_safe(type, key, default_value);
		if(entry == NULL){
			ESP_LOGE(TAG, "Failed to add value to cache!");
		}
		char * entry_str = cJSON_PrintUnformatted(entry);
		if(entry_str!=NULL){
			ESP_LOGD(TAG, "Value added to default for object: \n%s",entry_str);
			free(entry_str);
		}
	}

	config_unlock();

}

void config_delete_key(const char *key){
	nvs_handle nvs;
	ESP_LOGD(TAG, "Deleting nvs entry for [%s]", key);
	if(!config_lock(LOCK_MAX_WAIT/portTICK_PERIOD_MS)){
		ESP_LOGE(TAG, "Unable to lock config for delete");
		return ;
	}
	esp_err_t err = nvs_open_from_partition(settings_partition, current_namespace, NVS_READWRITE, &nvs);
	if (err == ESP_OK) {
		err = nvs_erase_key(nvs, key);
		if (err == ESP_OK) {
			ESP_LOGD(TAG, "key [%s] erased from nvs.",key);
			err = nvs_commit(nvs);
			if (err == ESP_OK) {
				ESP_LOGD(TAG, "nvs erase committed.");
			}
			else {
				ESP_LOGE(TAG, "Unable to commit nvs erase operation for key [%s]. %s.",key,esp_err_to_name(err));
			}
		}
		else {
			ESP_LOGE(TAG, "Unable to delete nvs key [%s]. %s. ",key, esp_err_to_name(err));
		}
		nvs_close(nvs);
	}
	else {
		ESP_LOGE(TAG, "Error opening nvs: %s. Unable to delete nvs key [%s].",esp_err_to_name(err),key);
	}
	char * struc_str = cJSON_PrintUnformatted(nvs_json);
	if(struc_str!=NULL){
		ESP_LOGV(TAG, "Structure before delete \n%s", struc_str);
		free(struc_str);
	}
	cJSON * entry = cJSON_DetachItemFromObjectCaseSensitive(nvs_json, key);
	if(entry !=NULL){
		ESP_LOGI(TAG, "Removing config key [%s]", entry->string);
		cJSON_Delete(entry);
		struc_str = cJSON_PrintUnformatted(nvs_json);
		if(struc_str!=NULL){
			ESP_LOGV(TAG, "Structure after delete \n%s", struc_str);
			free(struc_str);
		}
	}
	else {
		ESP_LOGW(TAG, "Unable to remove config key [%s]: not found.", key);
	}
	config_unlock();
}

void * config_alloc_get(nvs_type_t nvs_type, const char *key) {
	return config_alloc_get_default(nvs_type, key, NULL, 0);
}

void * config_alloc_get_str(const char *key, char *lead, char *fallback) {
	if (lead && *lead) return strdup(lead);
	char *value = config_alloc_get_default(NVS_TYPE_STR, key, NULL, 0);
	if ((!value || !*value) && fallback) {
		if (value) free(value);
		value = strdup(fallback);
	}
	return value;
}

void * config_alloc_get_default(nvs_type_t nvs_type, const char *key, void * default_value, size_t blob_size) {

	void * value = NULL;
	ESP_LOGV(TAG, "Retrieving key %s from nvs cache for type %s.", key,type_to_str(nvs_type));
	if(nvs_json==NULL){
		ESP_LOGE(TAG,"configuration not loaded!");
		return value;
	}
	if(!config_lock(LOCK_MAX_WAIT/portTICK_PERIOD_MS)){
		ESP_LOGE(TAG, "Unable to lock config");
		return value;
	}
	ESP_LOGD(TAG,"Getting config entry for key %s",key);
	cJSON * entry = cJSON_GetObjectItemCaseSensitive(nvs_json, key);
	if(entry !=NULL){
		ESP_LOGV(TAG, "Entry found, getting value.");
		value = config_safe_alloc_get_entry_value(nvs_type, entry);
	}
	else if(default_value!=NULL){
		// Value was not found
		ESP_LOGW(TAG, "Adding new config value for key [%s]",key);
		entry=config_set_value_safe(nvs_type, key, default_value);
		if(entry == NULL){
			ESP_LOGE(TAG, "Failed to add value to cache");
		}
		else {
			char * entry_str = cJSON_PrintUnformatted(entry);
			if(entry_str!=NULL){
				ESP_LOGV(TAG, "Value added configuration object for key [%s]: \n%s", entry->string,entry_str);
				free(entry_str);
			}
			else {
				ESP_LOGV(TAG, "Value added configuration object for key [%s]", entry->string);
			}
			value = config_safe_alloc_get_entry_value(nvs_type, entry);
		}
	}
	else{
		ESP_LOGW(TAG,"Value not found for key %s",key);
	}
	config_unlock();
	return value;
}
char * config_alloc_get_json(bool bFormatted){
	char * json_buffer = NULL;
	if(!config_lock(LOCK_MAX_WAIT/portTICK_PERIOD_MS)){
		ESP_LOGE(TAG, "Unable to lock config after %d ms",LOCK_MAX_WAIT);
		return strdup("{\"error\":\"Unable to lock configuration object.\"}");
	}
	if(bFormatted){
		json_buffer= cJSON_Print(nvs_json);
	}
	else {
		json_buffer= cJSON_PrintUnformatted(nvs_json);
	}
	config_unlock();
	return json_buffer;
}
esp_err_t config_set_value(nvs_type_t nvs_type, const char *key, const void * value){
	esp_err_t result = ESP_OK;
	if(!config_lock(LOCK_MAX_WAIT/portTICK_PERIOD_MS)){
			ESP_LOGE(TAG, "Unable to lock config after %d ms",LOCK_MAX_WAIT);
			result = ESP_FAIL;
	}
	cJSON * entry = config_set_value_safe(nvs_type, key, value);
	if(entry == NULL){
		result = ESP_FAIL;
	}
	else{
		char * entry_str = cJSON_PrintUnformatted(entry);
		if(entry_str!=NULL){
			ESP_LOGV(TAG,"config_set_value result: \n%s",entry_str);
			free(entry_str);
		}
		else {
			ESP_LOGV(TAG,"config_set_value completed");
		}
	}
	config_unlock();
	return result;
}

IMPLEMENT_SET_DEFAULT(uint8_t,NVS_TYPE_U8);
IMPLEMENT_SET_DEFAULT(int8_t,NVS_TYPE_I8);
IMPLEMENT_SET_DEFAULT(uint16_t,NVS_TYPE_U16);
IMPLEMENT_SET_DEFAULT(int16_t,NVS_TYPE_I16);
IMPLEMENT_SET_DEFAULT(uint32_t,NVS_TYPE_U32);
IMPLEMENT_SET_DEFAULT(int32_t,NVS_TYPE_I32);

IMPLEMENT_GET_NUM(uint8_t,NVS_TYPE_U8);
IMPLEMENT_GET_NUM(int8_t,NVS_TYPE_I8);
IMPLEMENT_GET_NUM(uint16_t,NVS_TYPE_U16);
IMPLEMENT_GET_NUM(int16_t,NVS_TYPE_I16);
IMPLEMENT_GET_NUM(uint32_t,NVS_TYPE_U32);
IMPLEMENT_GET_NUM(int32_t,NVS_TYPE_I32);
