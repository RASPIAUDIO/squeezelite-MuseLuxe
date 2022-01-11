/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once
#include "esp_system.h"
#ifdef __cplusplus
extern "C" {
#endif

// Register system functions
void register_system();
esp_err_t guided_factory();
esp_err_t guided_restart_ota();
void simple_restart();

#ifdef __cplusplus
}
#endif
