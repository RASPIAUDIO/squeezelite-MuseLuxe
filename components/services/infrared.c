/* 
 *  infrared receiver (using espressif's example)
 *
 *  (c) Philippe G. 2020, philippe_44@outlook.com
 *
 *  This software is released under the MIT License.
 *  https://opensource.org/licenses/MIT
 *
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "infrared.h"

static const char* TAG = "IR";

#define RMT_RX_ACTIVE_LEVEL  0   /*!< If we connect with a IR receiver, the data is active low */

#define RMT_RX_CHANNEL    0     /*!< RMT channel for receiver */
#define RMT_CLK_DIV      100    /*!< RMT counter clock divider */
#define RMT_TICK_10_US    (80000000/RMT_CLK_DIV/100000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) */

#define NEC_HEADER_HIGH_US    9000                         /*!< NEC protocol header: positive 9ms */
#define NEC_HEADER_LOW_US     4500                         /*!< NEC protocol header: negative 4.5ms*/
#define NEC_BIT_ONE_HIGH_US    560                         /*!< NEC protocol data bit 1: positive 0.56ms */
#define NEC_BIT_ONE_LOW_US    (2250-NEC_BIT_ONE_HIGH_US)   /*!< NEC protocol data bit 1: negative 1.69ms */
#define NEC_BIT_ZERO_HIGH_US   560                         /*!< NEC protocol data bit 0: positive 0.56ms */
#define NEC_BIT_ZERO_LOW_US   (1120-NEC_BIT_ZERO_HIGH_US)  /*!< NEC protocol data bit 0: negative 0.56ms */
#define NEC_BIT_MARGIN         150                          /*!< NEC parse margin time */

#define NEC_ITEM_DURATION(d)  ((d & 0x7fff)*10/RMT_TICK_10_US)  /*!< Parse duration time from memory register value */
#define NEC_DATA_ITEM_NUM   34  /*!< NEC code item number: header + 32bit data + end */
#define rmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value(us) */

/****************************************************************************************
 * 
 */
static bool nec_check_in_range(int duration_ticks, int target_us, int margin_us) {
    if(( NEC_ITEM_DURATION(duration_ticks) < (target_us + margin_us))
        && ( NEC_ITEM_DURATION(duration_ticks) > (target_us - margin_us))) {
        return true;
    } else {
        return false;
    }
}

/****************************************************************************************
 * 
 */
static bool nec_header_if(rmt_item32_t* item) {
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && nec_check_in_range(item->duration0, NEC_HEADER_HIGH_US, NEC_BIT_MARGIN)
        && nec_check_in_range(item->duration1, NEC_HEADER_LOW_US, NEC_BIT_MARGIN)) {
        return true;
    }
    return false;
}

/****************************************************************************************
 * 
 */
static bool nec_bit_one_if(rmt_item32_t* item) {
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && nec_check_in_range(item->duration0, NEC_BIT_ONE_HIGH_US, NEC_BIT_MARGIN)
        && nec_check_in_range(item->duration1, NEC_BIT_ONE_LOW_US, NEC_BIT_MARGIN)) {
        return true;
    }
    return false;
}

/****************************************************************************************
 * 
 */
static bool nec_bit_zero_if(rmt_item32_t* item) {
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && nec_check_in_range(item->duration0, NEC_BIT_ZERO_HIGH_US, NEC_BIT_MARGIN)
        && nec_check_in_range(item->duration1, NEC_BIT_ZERO_LOW_US, NEC_BIT_MARGIN)) {
        return true;
    }
    return false;
}

/****************************************************************************************
 * 
 */
static int nec_parse_items(rmt_item32_t* item, int item_num, uint16_t* addr, uint16_t* data) {
    int w_len = item_num;
    if(w_len < NEC_DATA_ITEM_NUM) {
        return -1;
    }
    int i = 0, j = 0;
    if(!nec_header_if(item++)) {
        return -1;
    }
    uint16_t addr_t = 0;
    for(j = 15; j >= 0; j--) {
        if(nec_bit_one_if(item)) {
            addr_t |= (1 << j);
        } else if(nec_bit_zero_if(item)) {
            addr_t |= (0 << j);
        } else {
            return -1;
        }
        item++;
        i++;
    }
    uint16_t data_t = 0;
    for(j = 15; j >= 0; j--) {
        if(nec_bit_one_if(item)) {
            data_t |= (1 << j);
        } else if(nec_bit_zero_if(item)) {
            data_t |= (0 << j);
        } else {
            return -1;
        }
        item++;
        i++;
    }
    *addr = addr_t;
    *data = data_t;
    return i;
}

/****************************************************************************************
 * 
 */
void infrared_receive(RingbufHandle_t rb, infrared_handler handler) {
	size_t rx_size = 0;
	rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 10 / portTICK_RATE_MS);
	
	if (item) {
		uint16_t addr, cmd;
		int offset = 0;
		
		while (1) {
			// parse data value from ringbuffer.
			int res = nec_parse_items(item + offset, rx_size / 4 - offset, &addr, &cmd);
			if (res > 0) {
				offset += res + 1;
				handler(addr, cmd);
				ESP_LOGD(TAG, "RMT RCV --- addr: 0x%04x cmd: 0x%04x", addr, cmd);
			} else break;
        }
		
		// after parsing the data, return spaces to ringbuffer.
        vRingbufferReturnItem(rb, (void*) item);
    }
}

/****************************************************************************************
 * 
 */
void infrared_init(RingbufHandle_t *rb, int gpio) {
	rmt_config_t rmt_rx;
	
	ESP_LOGI(TAG, "Starting Infrared Receiver on gpio %d", gpio);
	
	// initialize RMT driver
    rmt_rx.channel = RMT_RX_CHANNEL;
    rmt_rx.gpio_num = gpio;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = rmt_item32_tIMEOUT_US / 10 * (RMT_TICK_10_US);
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, 1000, 0);
	
	// get RMT RX ringbuffer
    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, rb);
    rmt_rx_start(RMT_RX_CHANNEL, 1);
}
