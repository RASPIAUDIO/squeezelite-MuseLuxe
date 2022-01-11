/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "bt_app_core.h"

static const char *TAG = "btappcore";

static void bt_app_task_handler(void *arg);
static bool bt_app_send_msg(bt_app_msg_t *msg);
static void bt_app_work_dispatched(bt_app_msg_t *msg);

static xQueueHandle s_bt_app_task_queue;
static bool running;

bool bt_app_work_dispatch(bt_app_cb_t p_cback, uint16_t event, void *p_params, int param_len, bt_app_copy_cb_t p_copy_cback)
{
	ESP_LOGV(TAG,"%s event 0x%x, param len %d", __func__, event, param_len);

    bt_app_msg_t msg;
    memset(&msg, 0, sizeof(bt_app_msg_t));

    msg.sig = BT_APP_SIG_WORK_DISPATCH;
    msg.event = event;
    msg.cb = p_cback;

    if (param_len == 0) {
        return bt_app_send_msg(&msg);
    } else if (p_params && param_len > 0) {
        if ((msg.param = malloc(param_len)) != NULL) {
            memcpy(msg.param, p_params, param_len);
            /* check if caller has provided a copy callback to do the deep copy */
            if (p_copy_cback) {
                p_copy_cback(&msg, msg.param, p_params);
            }
            return bt_app_send_msg(&msg);
        }
    }

    return false;
}

static bool bt_app_send_msg(bt_app_msg_t *msg)
{
    if (msg == NULL) {
        return false;
    }

    if (xQueueSend(s_bt_app_task_queue, msg, 10 / portTICK_RATE_MS) != pdTRUE) {
    	ESP_LOGE(TAG,"%s xQueue send failed", __func__);
        return false;
    }
    return true;
}

static void bt_app_work_dispatched(bt_app_msg_t *msg)
{
    if (msg->cb) {
        msg->cb(msg->event, msg->param);
    }
}

static void bt_app_task_handler(void *arg)
{
    bt_app_msg_t msg;
	esp_err_t err;
	
	s_bt_app_task_queue = xQueueCreate(10, sizeof(bt_app_msg_t));
	
	esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	
    if ((err = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(err));
        goto exit;
    }

    if ((err = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(err));
		goto exit;
    }

    if ((err = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(err));
		goto exit;
    }

    if ((err = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(err));
		goto exit;
    }
	
	/* Bluetooth device name, connection mode and profile set up */
	bt_app_work_dispatch((bt_av_hdl_stack_evt_t*) arg, BT_APP_EVT_STACK_UP, NULL, 0, NULL);
	
#if (CONFIG_BT_SSP_ENABLED)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif
	
	running = true;
	
	while (running) {
        if (pdTRUE == xQueueReceive(s_bt_app_task_queue, &msg, (portTickType)portMAX_DELAY)) {
        	ESP_LOGV(TAG,"%s, sig 0x%x, 0x%x", __func__, msg.sig, msg.event);
			
            switch (msg.sig) {
            case BT_APP_SIG_WORK_DISPATCH:
                bt_app_work_dispatched(&msg);
                break;
            default:
                ESP_LOGW(TAG,"%s, unhandled sig: %d", __func__, msg.sig);
                break;
            }

            if (msg.param) {
                free(msg.param);
            }
        } else {
        	ESP_LOGW(TAG,"No messaged received from queue.");
        }
    }
	
	ESP_LOGD(TAG, "bt_app_task shutting down");
	
	if (esp_bluedroid_disable() != ESP_OK) goto exit;
	// this disable has a sleep timer BTA_DISABLE_DELAY in bt_target.h and 
	// if we don't wait for it then disable crashes... don't know why
	vTaskDelay(2*200 / portTICK_PERIOD_MS);	
	
    ESP_LOGD(TAG, "esp_bluedroid_disable called successfully");
    if (esp_bluedroid_deinit() != ESP_OK) goto exit;
	
    ESP_LOGD(TAG, "esp_bluedroid_deinit called successfully");
    if (esp_bt_controller_disable() != ESP_OK) goto exit;
	
    ESP_LOGD(TAG, "esp_bt_controller_disable called successfully");
    if (esp_bt_controller_deinit() != ESP_OK) goto exit;
	
	ESP_LOGD(TAG, "bt stopped successfully");	

exit:
	vQueueDelete(s_bt_app_task_queue);
	running = false;		
    vTaskDelete(NULL);
}

void bt_app_task_start_up(bt_av_hdl_stack_evt_t* handler)
{
    xTaskCreate(bt_app_task_handler, "BtAppT", 4096, handler, configMAX_PRIORITIES - 3, NULL);
}

void bt_app_task_shut_down(void)
{
	running = false;
}
