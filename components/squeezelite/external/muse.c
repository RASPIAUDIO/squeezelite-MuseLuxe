/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>
#include <esp_log.h>
#include <esp_types.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c.h>
#include <driver/i2s.h>
#include "adac.h"
#include "muse.h"
#include <driver/adc.h>
#include "driver/rmt.h"

#define SPKOUT_EN ((1 << 9) | (1 << 11) | (1 << 7) | (1 << 5))
#define EAROUT_EN ((1 << 11) | (1 << 12) | (1 << 13))
#define BIN(a,b,c,d)	0b##a##b##c##d

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

#define AC_ASSERT(a, format, b, ...) \
    if ((a) != 0) { \
        ESP_LOGE(TAG, format, ##__VA_ARGS__); \
        return b;\
    }
/////////////////////////////////////////////////////////////////
//*********************** NeoPixels  ***************************
////////////////////////////////////////////////////////////////
#define NUM_LEDS  1
#define LED_RMT_TX_CHANNEL   0
#define LED_RMT_TX_GPIO      22


#define BITS_PER_LED_CMD 24 
#define LED_BUFFER_ITEMS ((NUM_LEDS * BITS_PER_LED_CMD))

// These values are determined by measuring pulse timing with logic analyzer and adjusting to match datasheet. 
#define T0H 14  // 0 bit high time
#define T1H 52 // 1 bit high time
#define TL  52  // low time for either bit

#define GREEN   0xFF0000
#define RED 	0x00FF00
#define BLUE  	0x0000FF
#define WHITE   0xFFFFFF
#define YELLOW  0xE0F060
struct led_state {
    uint32_t leds[NUM_LEDS];
};

void ws2812_control_init(void);
void ws2812_write_leds(struct led_state new_state);

///////////////////////////////////////////////////////////////////

static const char TAG[] = "es8388";	
static bool init(char *config, int i2c_port_num, i2s_config_t *i2s_config);
static void deinit(void);
static void speaker(bool active);
static void headset(bool active);
static bool volume(unsigned left, unsigned right);
static void power(adac_power_e mode);
static void battery(void *data);

const struct adac_s dac_muse = { "Muse", init, deinit, power, speaker, headset, volume };

void ES8388_Write_Reg(uint8_t reg, uint8_t val);
	
static int i2c_port;

/****************************************************************************************
 * init
 */
static bool init(char *config, int i2c_port_num, i2s_config_t *i2s_config) {	 
	esp_err_t res = ESP_OK;
	char *p;
//********** for battery monitoring **********************	
        xTaskCreate(battery, "battery", 5000, NULL, 1, NULL);
//****************************************************        	
	// configure i2c
	i2c_config_t i2c_config = {
			.mode = I2C_MODE_MASTER,
			.sda_io_num = -1,
			.sda_pullup_en = GPIO_PULLUP_ENABLE,
			.scl_io_num = -1,
			.scl_pullup_en = GPIO_PULLUP_ENABLE,
			.master.clk_speed = 250000,
		};
	
	if ((p = strcasestr(config, "sda")) != NULL) i2c_config.sda_io_num = atoi(strchr(p, '=') + 1);
	if ((p = strcasestr(config, "scl")) != NULL) i2c_config.scl_io_num = atoi(strchr(p, '=') + 1);
	
	i2c_port = i2c_port_num;
	i2c_param_config(i2c_port, &i2c_config);
	i2c_driver_install(i2c_port, I2C_MODE_MASTER, false, false, false);

        printf("-------------------->>>> init ES8388\n");

// CLK_OUT1 ==> MCLK
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
        WRITE_PERI_REG(PIN_CTRL, READ_PERI_REG(PIN_CTRL)& 0xFFFFFFF0);

//amplifier validation
	gpio_reset_pin(PA);
        gpio_set_direction(PA, GPIO_MODE_OUTPUT);
	gpio_set_level(PA, 1);
//green led validation (test)
        gpio_reset_pin(GLED);
        gpio_set_direction(GLED, GPIO_MODE_OUTPUT);
        gpio_set_level(GLED, 0);

///////////// init ES8388
//
/////////////////////////////////////////// 
// reset 
	ES8388_Write_Reg(0, 0x80); 

	ES8388_Write_Reg(0, 0x00); 
// mute
	ES8388_Write_Reg(25, 0x04);
	ES8388_Write_Reg(1, 0x50); 
//powerup
	ES8388_Write_Reg(2, 0x00);
// slave mode
	ES8388_Write_Reg(8, 0x00);
// DAC powerdown
	ES8388_Write_Reg(4, 0xC0);
// vmidsel/500k ADC/DAC idem
	ES8388_Write_Reg(0, 0x12);

	ES8388_Write_Reg(1, 0x00);
// i2s 16 bits
	ES8388_Write_Reg(23, 0x18);
// sample freq 256
	ES8388_Write_Reg(24, 0x02);
// LIN2/RIN2 for mixer
	ES8388_Write_Reg(38, 0x09);
// left DAC to left mixer
	ES8388_Write_Reg(39, 0x90);
// right DAC to right mixer
	ES8388_Write_Reg(42, 0x90);
// DACLRC ADCLRC idem
	ES8388_Write_Reg(43, 0x80);
	ES8388_Write_Reg(45, 0x00);
// DAC volume max
	ES8388_Write_Reg(27, 0x00);
	ES8388_Write_Reg(26, 0x00);

	ES8388_Write_Reg(2 , 0xF0);
	ES8388_Write_Reg(2 , 0x00);
	ES8388_Write_Reg(29, 0x1C);
// DAC power-up LOUT1/ROUT1 enabled
	ES8388_Write_Reg(4, 0x30);
// unmute
	ES8388_Write_Reg(25, 0x00);
// max volume
	ES8388_Write_Reg(46, 0x21);
	ES8388_Write_Reg(47, 0x21);
	ESP_LOGI(TAG, "ES8388 uses I2C sda:%d, scl:%d", i2c_config.sda_io_num, i2c_config.scl_io_num);
	return (res == ESP_OK);
}	

/****************************************************************************************
 * deinit
 */
static void deinit(void)	{	 
	i2c_driver_delete(i2c_port);
}

/****************************************************************************************
 * change volume
 */
static bool volume(unsigned left, unsigned right) {

        printf("VOLUME=============>Vgauche %u  Vdroit %u\n",left,right);
        uint8_t l, r;
        l = left >> 11;
        r = right >> 11;
        ES8388_Write_Reg(46, l);
	ES8388_Write_Reg(47, r);
	return false;
} 

/****************************************************************************************
 * power
 */
static void power(adac_power_e mode) {
	
}

/****************************************************************************************
 * speaker
 */
static void speaker(bool active) {

} 

/****************************************************************************************
 * headset
 */
static void headset(bool active) {
		
} 	


///////////////////////////////////////////////////////////////////////
// Write ES8388 register (Muse board)
///////////////////////////////////////////////////////////////////////
#define ES8388_ADDR 0x10

void ES8388_Write_Reg(uint8_t reg, uint8_t val) {
	esp_err_t ret;
        printf("R%02d = %02x\n",reg, val);
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
	
	i2c_master_write_byte(cmd, (ES8388_ADDR<< 1) | I2C_MASTER_WRITE, I2C_MASTER_NACK);
	i2c_master_write_byte(cmd, reg, I2C_MASTER_NACK);
	i2c_master_write_byte(cmd, val, I2C_MASTER_NACK);
	
	i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(0, cmd, 100 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);
        vTaskDelay(100 / portTICK_PERIOD_MS); 
	if (ret != ESP_OK) {
		ESP_LOGW(TAG, "I2C write failed");
	}	
    return ;
}
// Battery monitoring
static void battery(void *data)
{
#define VGREEN  2300

#define VRED    2000
#define NM      10
  static int val;
  static int V[NM];
  static int I=0;
  int S;
  for(int i=0;i<NM;i++)V[i]=VGREEN;
  vTaskDelay(1000 / portTICK_PERIOD_MS);	  
  struct led_state new_state;
  ws2812_control_init();
// init ADC interface for battery survey
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_GPIO33_CHANNEL, ADC_ATTEN_DB_11);
  while(true)
	{
	vTaskDelay(1000 / portTICK_PERIOD_MS);	
	V[I++] = adc1_get_raw(ADC1_GPIO33_CHANNEL);
	if(I >= NM)I = 0;
	S = 0;
	for(int i=0;i<NM;i++)S = S + V[i];	
	val = S / NM;	
	new_state.leds[0] = YELLOW;
	if(val > VGREEN) new_state.leds[0] = GREEN;	
	if(val < VRED) new_state.leds[0] = RED;
        printf("====> %d  %6x\n", val, new_state.leds[0]);	        
	ws2812_write_leds(new_state);	        

	}
}



// This is the buffer which the hw peripheral will access while pulsing the output pin
rmt_item32_t led_data_buffer[LED_BUFFER_ITEMS];

void setup_rmt_data_buffer(struct led_state new_state);

void ws2812_control_init(void)
{
  rmt_config_t config;
  config.rmt_mode = RMT_MODE_TX;
  config.channel = LED_RMT_TX_CHANNEL;
  config.gpio_num = LED_RMT_TX_GPIO;
  config.mem_block_num = 3;
  config.tx_config.loop_en = false;
  config.tx_config.carrier_en = false;
  config.tx_config.idle_output_en = true;
  config.tx_config.idle_level = 0;
  config.clk_div = 2;

  ESP_ERROR_CHECK(rmt_config(&config));
  ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
}

void ws2812_write_leds(struct led_state new_state) {
  setup_rmt_data_buffer(new_state);
  ESP_ERROR_CHECK(rmt_write_items(LED_RMT_TX_CHANNEL, led_data_buffer, LED_BUFFER_ITEMS, false));
  ESP_ERROR_CHECK(rmt_wait_tx_done(LED_RMT_TX_CHANNEL, portMAX_DELAY));
}

void setup_rmt_data_buffer(struct led_state new_state) 
{
  for (uint32_t led = 0; led < NUM_LEDS; led++) {
    uint32_t bits_to_send = new_state.leds[led];
    uint32_t mask = 1 << (BITS_PER_LED_CMD - 1);
    for (uint32_t bit = 0; bit < BITS_PER_LED_CMD; bit++) {
      uint32_t bit_is_set = bits_to_send & mask;
      led_data_buffer[led * BITS_PER_LED_CMD + bit] = bit_is_set ?
                                                      (rmt_item32_t){{{T1H, 1, TL, 0}}} : 
                                                      (rmt_item32_t){{{T0H, 1, TL, 0}}};
      mask >>= 1;
    }
  }
  }

