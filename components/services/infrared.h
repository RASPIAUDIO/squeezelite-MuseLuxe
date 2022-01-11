/* 
 *  (c) Philippe G. 2019, philippe_44@outlook.com
 *
 *  This software is released under the MIT License.
 *  https://opensource.org/licenses/MIT
 *
 */

#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
 
typedef void (*infrared_handler)(uint16_t addr, uint16_t cmd);
void infrared_receive(RingbufHandle_t rb, infrared_handler handler);
void infrared_init(RingbufHandle_t *rb, int gpio);
