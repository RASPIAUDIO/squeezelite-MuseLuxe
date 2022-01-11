/* 
 *  Squeezelite for esp32
 *
 *  (c) Sebastien 2019
 *      Philippe G. 2019, philippe_44@outlook.com
 *
 *  This software is released under the MIT License.
 *  https://opensource.org/licenses/MIT
 *
 */
#include "squeezelite.h"
#include "pthread.h"
#include "esp_pthread.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "monitor.h"
#include "platform_config.h"

mutex_type slimp_mutex;

void get_mac(u8_t mac[]) {
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
}

_sig_func_ptr signal(int sig, _sig_func_ptr func) {
	return NULL;
}

void *audio_calloc(size_t nmemb, size_t size) {
	return calloc(nmemb, size);
}

int	pthread_create_name(pthread_t *thread, _CONST pthread_attr_t  *attr, 
				   void *(*start_routine)( void * ), void *arg, char *name) {
	esp_pthread_cfg_t cfg = esp_pthread_get_default_config(); 
	cfg.thread_name = name; 
	cfg.inherit_cfg = true; 
	esp_pthread_set_cfg(&cfg); 
	return pthread_create(thread, attr, start_routine, arg);
}

uint32_t _gettime_ms_(void) {
	return (uint32_t) (esp_timer_get_time() / 1000);
}

extern void sb_controls_init(void);
extern bool sb_displayer_init(void);

u8_t custom_player_id = 12;

void embedded_init(void) {
	mutex_create(slimp_mutex);
	sb_controls_init();
	custom_player_id = sb_displayer_init() ? 100 : 101;
}

u16_t get_RSSI(void) {
    wifi_ap_record_t wifidata;
    esp_wifi_sta_get_ap_info(&wifidata);
	// we'll assume dBm, -30 to -100
    if (wifidata.primary != 0) return 100 + wifidata.rssi + 30;
    else return 0xffff;
}	

u16_t get_plugged(void) {
    return jack_inserted_svc() ? PLUG_HEADPHONE : 0;
}

u16_t get_battery(void) {
	return (u16_t) (battery_value_svc() * 128) & 0x0fff;
}	 

void set_name(char *name) {
	char *cmd = config_alloc_get(NVS_TYPE_STR, "autoexec1");
	char *p, *q;
	
	if (!cmd) return;

	if ((p = strstr(cmd, " -n")) != NULL) {
		q = p + 3;
		// in case some smart dude has a " -" in player's name
		while ((q = strstr(q, " -")) != NULL) {
			if (!strchr(q, '"') || !strchr(q+1, '"')) break;
			q++;
		}
		if (q) memmove(p, q, strlen(q) + 1);
		else *p = '\0';
	}

	asprintf(&q, "%s -n \"%s\"", cmd, name);
    config_set_value(NVS_TYPE_STR, "autoexec1", q);
	
	free(q);
	free(cmd);
}
