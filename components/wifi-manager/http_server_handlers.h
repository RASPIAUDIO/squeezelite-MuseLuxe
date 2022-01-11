/*
Copyright (c) 2017-2019 Tony Pottier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

@file http_server.h
@author Tony Pottier
@brief Defines all functions necessary for the HTTP server to run.

Contains the freeRTOS task for the HTTP listener and all necessary support
function to process requests, decode URLs, serve files, etc. etc.

@note http_server task cannot run without the wifi_manager task!
@see https://idyl.io
@see https://github.com/tonyp7/esp32-wifi-manager
*/

#ifndef HTTP_SERVER_H_INCLUDED
#define HTTP_SERVER_H_INCLUDED
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "esp_http_server.h"
#include "wifi_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include <esp_event.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "mdns.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/opt.h"
#include "lwip/memp.h"
#include "lwip/ip.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/priv/api_msg.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/priv/tcpip_priv.h"
#include "esp_vfs.h"
#include "esp_console.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_LOGE_LOC(t,str, ...)  ESP_LOGE(t, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ESP_LOGI_LOC(t,str, ...)  ESP_LOGI(t, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ESP_LOGD_LOC(t,str, ...)  ESP_LOGD(t, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ESP_LOGW_LOC(t,str, ...)  ESP_LOGW(t, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ESP_LOGV_LOC(t,str, ...)  ESP_LOGV(t, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__)


esp_err_t root_get_handler(httpd_req_t *req);
esp_err_t resource_filehandler(httpd_req_t *req);
esp_err_t resource_filehandler(httpd_req_t *req);
esp_err_t resource_filehandler(httpd_req_t *req);
esp_err_t resource_filehandler(httpd_req_t *req);
esp_err_t resource_filehandler(httpd_req_t *req);
esp_err_t resource_filehandler(httpd_req_t *req);
esp_err_t ap_get_handler(httpd_req_t *req);
esp_err_t config_get_handler(httpd_req_t *req);
esp_err_t config_post_handler(httpd_req_t *req);
esp_err_t connect_post_handler(httpd_req_t *req);
esp_err_t connect_delete_handler(httpd_req_t *req);
esp_err_t reboot_ota_post_handler(httpd_req_t *req);
esp_err_t reboot_post_handler(httpd_req_t *req);
esp_err_t recovery_post_handler(httpd_req_t *req);
esp_err_t flash_post_handler(httpd_req_t *req);
esp_err_t status_get_handler(httpd_req_t *req);
esp_err_t messages_get_handler(httpd_req_t *req);
esp_err_t console_cmd_get_handler(httpd_req_t *req);
esp_err_t console_cmd_post_handler(httpd_req_t *req);
esp_err_t ap_scan_handler(httpd_req_t *req);
esp_err_t redirect_ev_handler(httpd_req_t *req);
esp_err_t redirect_200_ev_handler(httpd_req_t *req);


esp_err_t err_handler(httpd_req_t *req, httpd_err_code_t error);
#define SCRATCH_BUFSIZE (10240)
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;
/**
 * @brief RTOS task for the HTTP server. Do not start manually.
 * @see void http_server_start()
 */
void http_server(void *pvParameters);

/* @brief helper function that processes one HTTP request at a time */
void http_server_netconn_serve(struct netconn *conn);

/* @brief create the task for the http server */
esp_err_t http_server_start();

/**
 * @brief gets a char* pointer to the first occurence of header_name withing the complete http request request.
 *
 * For optimization purposes, no local copy is made. memcpy can then be used in coordination with len to extract the
 * data.
 *
 * @param request the full HTTP raw request.
 * @param header_name the header that is being searched.
 * @param len the size of the header value if found.
 * @return pointer to the beginning of the header value.
 */
char* http_server_get_header(char *request, char *header_name, int *len);

void strreplace(char *src, char *str, char *rep);
/* @brief lock the json config object */
bool http_server_lock_json_object(TickType_t xTicksToWait);
/* @brief unlock the json config object */
void http_server_unlock_json_object();
#define PROTECTED_JSON_CALL(a)  if(http_server_lock_json_object( portMAX_DELAY )){ \ a; http_server_unlocklock_json_object(); }  else{  ESP_LOGE(TAG, "could not get access to json mutex in wifi_scan"); }



#ifdef __cplusplus
}
#endif

#endif
