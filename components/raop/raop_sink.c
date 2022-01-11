#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "mdns.h"
#include "nvs.h"
#include "tcpip_adapter.h"
// IDF-V4++ #include "esp_netif.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_pthread.h"
#include "esp_system.h"
#include "freertos/timers.h"
#include "platform_config.h"
#include "raop.h"
#include "audio_controls.h"
#include "display.h"
#include "accessors.h"
#include "log_util.h"
#include "trace.h"

#ifndef CONFIG_AIRPLAY_NAME
#define CONFIG_AIRPLAY_NAME		"ESP32-AirPlay"
#endif

static EXT_RAM_ATTR struct raop_cb_s {
	raop_cmd_vcb_t cmd;
	raop_data_cb_t data;
} raop_cbs;

log_level	raop_loglevel = lINFO;
log_level	util_loglevel;

static log_level *loglevel = &raop_loglevel;
static struct raop_ctx_s *raop;
static raop_cmd_vcb_t cmd_handler_chain;

static void raop_volume_up(bool pressed) {
	if (!pressed) return;
	raop_cmd(raop, RAOP_VOLUME_UP, NULL);
	LOG_INFO("AirPlay volume up");
}

static void raop_volume_down(bool pressed) {
	if (!pressed) return;
	raop_cmd(raop, RAOP_VOLUME_DOWN, NULL);
	LOG_INFO("AirPlay volume down");
}

static void raop_toggle(bool pressed) {
	if (!pressed) return;
	raop_cmd(raop, RAOP_TOGGLE, NULL);
	LOG_INFO("AirPlay play/pause");
}

static void raop_pause(bool pressed) {
	if (!pressed) return;
	raop_cmd(raop, RAOP_PAUSE, NULL);
	LOG_INFO("AirPlay pause");
}

static void raop_play(bool pressed) {
	if (!pressed) return;
	raop_cmd(raop, RAOP_PLAY, NULL);
	LOG_INFO("AirPlay play");
}

static void raop_stop(bool pressed) {
	if (!pressed) return;
	raop_cmd(raop, RAOP_STOP, NULL);
	LOG_INFO("AirPlay stop");
}

static void raop_prev(bool pressed) {
	if (!pressed) return;
	raop_cmd(raop, RAOP_PREV, NULL);
	LOG_INFO("AirPlay previous");
}

static void raop_next(bool pressed) {
	if (!pressed) return;
	raop_cmd(raop, RAOP_NEXT, NULL);
	LOG_INFO("AirPlay next");
}

const static actrls_t controls = {
	NULL,								// power
	raop_volume_up, raop_volume_down,	// volume up, volume down
	raop_toggle, raop_play,				// toggle, play
	raop_pause, raop_stop,				// pause, stop
	NULL, NULL,							// rew, fwd
	raop_prev, raop_next,				// prev, next
	NULL, NULL, NULL, NULL, // left, right, up, down
	NULL, NULL, NULL, NULL, NULL, NULL, // pre1-6
	raop_volume_down, raop_volume_up, raop_toggle// knob left, knob_right, knob push
};

/****************************************************************************************
 * Command handler
 */
static bool cmd_handler(raop_event_t event, ...) {
	va_list args;	
	
	va_start(args, event);
	
	// handle audio event and stop if forbidden
	if (!cmd_handler_chain(event, args)) {
		va_end(args);
		return false;
	}

	// now handle events for display
	switch(event) {
	case RAOP_SETUP:
		actrls_set(controls, false, NULL, actrls_ir_action);
		displayer_control(DISPLAYER_ACTIVATE, "AIRPLAY", true);
		break;
	case RAOP_PLAY:
		displayer_control(DISPLAYER_TIMER_RUN);
		break;		
	case RAOP_FLUSH:
		displayer_control(DISPLAYER_TIMER_PAUSE);
		break;		
	case RAOP_STOP:
		actrls_unset();
		displayer_control(DISPLAYER_SUSPEND);
		break;
	case RAOP_METADATA: {
		char *artist = va_arg(args, char*), *album = va_arg(args, char*), *title = va_arg(args, char*);
		displayer_metadata(artist, album, title);
		displayer_artwork(NULL);
		break;
	}	
	case RAOP_ARTWORK: {
		uint8_t *data = va_arg(args, uint8_t*);
		displayer_artwork(data);
		break;
	}
	case RAOP_PROGRESS: {
		int elapsed = va_arg(args, int), duration = va_arg(args, int);
		displayer_timer(DISPLAYER_ELAPSED, elapsed, duration);
		break;
	}	
	default: 
		break;
	}
	
	va_end(args);
	
	return true;
}

/****************************************************************************************
 * Airplay sink de-initialization
 */
void raop_sink_deinit(void) {
	raop_delete(raop);
	mdns_free();
}	

/****************************************************************************************
 * Airplay sink startup
 */
static bool raop_sink_start(raop_cmd_vcb_t cmd_cb, raop_data_cb_t data_cb) {
    const char *hostname = NULL;
	char sink_name[64-6] = CONFIG_AIRPLAY_NAME;
	tcpip_adapter_ip_info_t ipInfo = { }; 
	tcpip_adapter_if_t ifs[] = { TCPIP_ADAPTER_IF_ETH, TCPIP_ADAPTER_IF_STA, TCPIP_ADAPTER_IF_AP };
   	
	// get various IP info
	for (int i = 0; i < sizeof(ifs) / sizeof(tcpip_adapter_if_t); i++) 
		if (tcpip_adapter_get_ip_info(ifs[i], &ipInfo) == ESP_OK && ipInfo.ip.addr != IPADDR_ANY) {
			tcpip_adapter_get_hostname(ifs[i], &hostname);			
			break;
		}
	
	if (!hostname) {
		LOG_INFO( "no hostname/IP found, can't start AirPlay");
		return false;
	}

    // initialize mDNS
    ESP_ERROR_CHECK( mdns_init() );
    ESP_ERROR_CHECK( mdns_hostname_set(hostname) );
        
    char * sink_name_buffer= (char *)config_alloc_get(NVS_TYPE_STR,"airplay_name");
    if (sink_name_buffer != NULL){
    	memset(sink_name, 0x00, sizeof(sink_name));
    	strncpy(sink_name,sink_name_buffer,sizeof(sink_name)-1 );
    	free(sink_name_buffer);
    }

	LOG_INFO( "mdns hostname for ip %s set to: [%s] with servicename %s", inet_ntoa(ipInfo.ip.addr), hostname, sink_name);

    // create RAOP instance, latency is set by controller
	uint8_t mac[6];	
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
	cmd_handler_chain = cmd_cb;
	raop = raop_create(ipInfo.ip.addr, sink_name, mac, 0, cmd_handler, data_cb);
	
	return true;
}

/****************************************************************************************
 * Airplay sink timer handler
 */
static void raop_start_handler( TimerHandle_t xTimer ) {
	if (raop_sink_start(raop_cbs.cmd, raop_cbs.data)) {
		xTimerDelete(xTimer, portMAX_DELAY);
	}	
}	

/****************************************************************************************
 * Airplay sink initialization
 */
void raop_sink_init(raop_cmd_vcb_t cmd_cb, raop_data_cb_t data_cb) {
	if (!raop_sink_start(cmd_cb, data_cb)) {
		raop_cbs.cmd = cmd_cb;
		raop_cbs.data = data_cb;
		TimerHandle_t timer = xTimerCreate("raopStart", 5000 / portTICK_RATE_MS, pdTRUE, NULL, raop_start_handler);
		xTimerStart(timer, portMAX_DELAY);
		LOG_INFO( "Delaying AirPlay start");		
	}	
}

/****************************************************************************************
 * Airplay forced disconnection
 */
void raop_disconnect(void) {
	LOG_INFO("forced disconnection");
	displayer_control(DISPLAYER_SHUTDOWN);
	// in case we can't communicate with AirPlay controller, abort session 
	if (!raop_cmd(raop, RAOP_STOP, NULL)) raop_abort(raop);
	actrls_unset();
}
