#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "bt_app_core.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_console.h"
#include "esp_pthread.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/timers.h"
#include "argtable3/argtable3.h"
#include "platform_config.h"
#include "messaging.h"
#include "cJSON.h"
#include "trace.h"

static const char * TAG = "bt_app_source";
static const char * BT_RC_CT_TAG="RCCT";
extern int32_t 	output_bt_data(uint8_t *data, int32_t len);
extern void 	output_bt_tick(void);
extern char*	output_state_str(void);
extern bool		output_stopped(void);

static void bt_app_av_state_connecting(uint16_t event, void *param);
static void filter_inquiry_scan_result(esp_bt_gap_cb_param_t *param);

char * APP_AV_STATE_DESC[] = {
	    "APP_AV_STATE_IDLE",
	    "APP_AV_STATE_DISCOVERING",
	    "APP_AV_STATE_DISCOVERED",
	    "APP_AV_STATE_UNCONNECTED",
	    "APP_AV_STATE_CONNECTING",
	    "APP_AV_STATE_CONNECTED",
	    "APP_AV_STATE_DISCONNECTING"
};
static char *  ESP_AVRC_CT_DESC[]={
  "ESP_AVRC_CT_CONNECTION_STATE_EVT",
  "ESP_AVRC_CT_PASSTHROUGH_RSP_EVT",
  "ESP_AVRC_CT_METADATA_RSP_EVT",
  "ESP_AVRC_CT_PLAY_STATUS_RSP_EVT",
  "ESP_AVRC_CT_CHANGE_NOTIFY_EVT",
  "ESP_AVRC_CT_REMOTE_FEATURES_EVT",
  "ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT",
  "ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT" 
  };

#define BT_APP_HEART_BEAT_EVT                (0xff00)
// AVRCP used transaction label
#define APP_RC_CT_TL_GET_CAPS            (0)
#define APP_RC_CT_TL_RN_VOLUME_CHANGE    (1)
#define PEERS_LIST_MAINTAIN_RESET -129
#define PEERS_LIST_MAINTAIN_PURGE -129

/// handler for bluetooth stack enabled events
static void bt_av_hdl_stack_evt(uint16_t event, void *p_param);

/// callback function for A2DP source
static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

/// callback function for AVRCP controller
static void bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);

/// avrc CT event handler
static void bt_av_hdl_avrc_ct_evt(uint16_t event, void *p_param);

/// callback function for A2DP source audio data stream
static void a2d_app_heart_beat(void *arg);

/// A2DP application state machine
static void bt_app_av_sm_hdlr(uint16_t event, void *param);

/* A2DP application state machine handler for each state */
static void bt_app_av_state_unconnected(uint16_t event, void *param);
static void bt_app_av_state_connecting(uint16_t event, void *param);
static void bt_app_av_state_connected(uint16_t event, void *param);
static void bt_app_av_state_disconnecting(uint16_t event, void *param);
static void handle_connect_state_unconnected(uint16_t event, esp_a2d_cb_param_t *param);
static void handle_connect_state_connecting(uint16_t event, esp_a2d_cb_param_t *param);
static void handle_connect_state_connected(uint16_t event, esp_a2d_cb_param_t *param);
static void handle_connect_state_disconnecting(uint16_t event, esp_a2d_cb_param_t *param);
static void bt_av_notify_evt_handler(uint8_t event_id, esp_avrc_rn_param_t *event_parameter);

static esp_bd_addr_t s_peer_bda = {0};
static uint8_t s_peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
int bt_app_source_a2d_state = APP_AV_STATE_IDLE;
int bt_app_source_media_state = APP_AV_MEDIA_STATE_IDLE;
static uint32_t s_pkt_cnt = 0;
static TimerHandle_t s_tmr=NULL;
static int prev_duration=10000;
static esp_avrc_rn_evt_cap_mask_t s_avrc_peer_rn_cap;
static int s_connecting_intv = 0;
cJSON * peers_list=NULL;

static struct {
	char * sink_name;
} squeezelite_conf;	

static cJSON * peers_list_get_entry(const char * s_peer_bdname){
    cJSON * element=NULL;
    cJSON_ArrayForEach(element,peers_list){
        cJSON * name = cJSON_GetObjectItem(element,"name");
        if(name && !strcmp(cJSON_GetStringValue(name),s_peer_bdname)){
            ESP_LOGV(TAG,"Entry name %s found in current scan list", s_peer_bdname);
            return element;
        }
    }
    ESP_LOGV(TAG,"Entry name %s NOT found in current scan list", s_peer_bdname);
    return NULL;
}

static void peers_list_reset(){
    cJSON * element=NULL;
    cJSON_ArrayForEach(element,peers_list){
        cJSON * rssi = cJSON_GetObjectItem(element,"rssi");
        if(rssi){
            rssi->valuedouble = -129;
            rssi->valueint = -129;
        }
    }
}

static void peers_list_purge(){
    cJSON * element=NULL;
    cJSON_ArrayForEach(element,peers_list){
        cJSON * rssi_val = cJSON_GetObjectItem(element,"rssi");
        if(rssi_val && rssi_val->valuedouble == -129){
            cJSON * name = cJSON_GetObjectItem(element,"name");
            ESP_LOGV(TAG,"Purging %s", cJSON_GetStringValue(name)?cJSON_GetStringValue(name):"Unknown");
            cJSON_DetachItemViaPointer(peers_list,element);
            cJSON_Delete(element);
        }
    }    
}

static cJSON * peers_list_create_entry(const char * s_peer_bdname, int32_t rssi){
    cJSON * entry = cJSON_CreateObject();
    cJSON_AddStringToObject(entry,"name",s_peer_bdname);
    cJSON_AddNumberToObject(entry,"rssi",rssi);
    return entry;
}

static void peers_list_update_add(const char * s_peer_bdname, int32_t rssi){
    cJSON * element= peers_list_get_entry(s_peer_bdname);
    if(element){
        cJSON * rssi_val = cJSON_GetObjectItem(element,"rssi");
        if(rssi_val && rssi_val->valuedouble != rssi){
            ESP_LOGV(TAG,"Updating BT Sink Device: %s rssi to %i", s_peer_bdname,rssi);
            rssi_val->valuedouble = rssi;
            rssi_val->valueint = rssi;
        }
    }
    else {
        ESP_LOGI(TAG,"Found BT Sink Device: %s rssi is %i", s_peer_bdname,rssi);
        element = peers_list_create_entry( s_peer_bdname,  rssi);
        cJSON_AddItemToArray(peers_list,element);
    }
}

static void peers_list_maintain(const char * s_peer_bdname, int32_t rssi){
    if(!peers_list){
        ESP_LOGV(TAG,"Initializing BT peers list");
        peers_list=cJSON_CreateArray();
    }
    if(rssi==PEERS_LIST_MAINTAIN_RESET){
        ESP_LOGV(TAG,"Resetting BT peers list");
        peers_list_reset();
    }
    else if(rssi==PEERS_LIST_MAINTAIN_PURGE){
        ESP_LOGV(TAG,"Purging BT peers list");
        peers_list_purge();
    }
    if(s_peer_bdname) {
        ESP_LOGV(TAG,"Adding/Updating peer %s rssi %i", s_peer_bdname,rssi);
        peers_list_update_add(s_peer_bdname, rssi);
    }
    char * list_json = cJSON_Print(peers_list);
    if(list_json){
        messaging_post_message(MESSAGING_INFO, MESSAGING_CLASS_BT, list_json);
        ESP_LOGV(TAG,"%s", list_json);
        free(list_json);
    }    
}

int bt_app_source_get_a2d_state(){
    ESP_LOGD(TAG,"a2dp status: %u = %s", bt_app_source_a2d_state, APP_AV_STATE_DESC[bt_app_source_a2d_state]);
    return bt_app_source_a2d_state;
}

int bt_app_source_get_media_state(){
    ESP_LOGD(TAG,"media state : %u ", bt_app_source_media_state);
    return bt_app_source_media_state;
}

void set_app_source_state(int new_state){
    if(bt_app_source_a2d_state!=new_state){
        ESP_LOGD(TAG, "Updating state from %s to %s", APP_AV_STATE_DESC[bt_app_source_a2d_state], APP_AV_STATE_DESC[new_state]);
        bt_app_source_a2d_state=new_state;
    }
}

void set_a2dp_media_state(int new_state){
    if(bt_app_source_media_state!=new_state){
        bt_app_source_media_state=new_state;
    }
}

void hal_bluetooth_init(const char * options)
{
	struct {
		struct arg_str *sink_name;
		struct arg_end *end;
	} squeezelite_args;
	
	ESP_LOGD(TAG,"Initializing Bluetooth HAL");

	squeezelite_args.sink_name = arg_str0("n", "name", "<sink name>", "the name of the bluetooth to connect to");
	squeezelite_args.end = arg_end(2);

	ESP_LOGD(TAG,"Copying parameters");
	char * opts = strdup(options);
	char **argv = malloc(sizeof(char**)*15);

	size_t argv_size=15;

	// change parms so ' appear as " for parsing the options
	for (char* p = opts; (p = strchr(p, '\'')); ++p) *p = '"';
	ESP_LOGD(TAG,"Splitting arg line: %s", opts);

	argv_size = esp_console_split_argv(opts, argv, argv_size);
	ESP_LOGD(TAG,"Parsing parameters");
	int nerrors = arg_parse(argv_size , argv, (void **) &squeezelite_args);
	if (nerrors != 0) {
		ESP_LOGD(TAG,"Parsing Errors");
		arg_print_errors(stdout, squeezelite_args.end, "BT");
		arg_print_glossary_gnu(stdout, (void **) &squeezelite_args);
		free(opts);
		free(argv);
		return;
	}

    if(squeezelite_args.sink_name->count == 0)
    {
        squeezelite_conf.sink_name = config_alloc_get_default(NVS_TYPE_STR, "a2dp_sink_name", NULL, 0);
        if(!squeezelite_conf.sink_name  || strlen(squeezelite_conf.sink_name)==0 ){
            ESP_LOGW(TAG,"Unable to retrieve the a2dp sink name from nvs.");
        }
    } else {
        squeezelite_conf.sink_name=strdup(squeezelite_args.sink_name->sval[0]);
        // sync with NVS
        esp_err_t err=ESP_OK;
        if((err= config_set_value(NVS_TYPE_STR, "a2dp_sink_name", squeezelite_args.sink_name->sval[0]))!=ESP_OK){
            ESP_LOGE(TAG,"Error setting Bluetooth audio device name %s. %s",squeezelite_args.sink_name->sval[0], esp_err_to_name(err));
        }
        else {
            ESP_LOGI(TAG,"Bluetooth audio device name changed to %s",squeezelite_args.sink_name->sval[0]);
        }                
    }

	ESP_LOGD(TAG,"Freeing options");
	free(argv);
	free(opts);
	
	// create task and run event loop
    bt_app_task_start_up(bt_av_hdl_stack_evt);

	/*
	 * Set default parameters for Legacy Pairing
	 * Use variable pin, input pin code when pairing
	*/
	esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
	esp_bt_pin_code_t pin_code;
	esp_bt_gap_set_pin(pin_type, 0, pin_code);

}

void hal_bluetooth_stop(void) {
	bt_app_task_shut_down();
}	

static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    bt_app_work_dispatch(bt_app_av_sm_hdlr, event, param, sizeof(esp_a2d_cb_param_t), NULL);
}

static void handle_bt_gap_pin_req(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param){
    char * pin_str = config_alloc_get_default(NVS_TYPE_STR, "a2dp_spin", "0000", 0);
    int pinlen=pin_str?strlen(pin_str):0;
    if (pin_str && ((!param->pin_req.min_16_digit && pinlen==4) || (param->pin_req.min_16_digit && pinlen==16)))  {
        ESP_LOGI(TAG,"Input pin code %s: ",pin_str);
        esp_bt_pin_code_t pin_code;
        for (size_t i = 0; i < pinlen; i++)
        {
            pin_code[i] = pin_str[i];
        }
        esp_bt_gap_pin_reply(param->pin_req.bda, true, pinlen, pin_code);
    }
    else {
        if(pinlen>0){
            ESP_LOGW(TAG,"Pin length: %u does not match the length expected by the device: %u", pinlen, ((param->pin_req.min_16_digit)?16:4));
        }
        else {
            ESP_LOGW(TAG, "No security Pin provided. Trying with default pins.");
        }
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(TAG,"Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(TAG,"Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }            
    }
    FREE_AND_NULL(pin_str);
}

static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{

    switch (event) {
    case ESP_BT_GAP_DISC_RES_EVT: {
        filter_inquiry_scan_result(param);
        break;
    }
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: {
		
		if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
            peers_list_maintain(NULL, PEERS_LIST_MAINTAIN_PURGE);
            if (bt_app_source_a2d_state == APP_AV_STATE_DISCOVERED) {
				set_app_source_state(APP_AV_STATE_CONNECTING);
				ESP_LOGI(TAG,"Discovery completed.  Ready to start connecting to %s. ", s_peer_bdname);
                esp_a2d_source_connect(s_peer_bda);
            } else {
                // not discovered, continue to discover
                ESP_LOGI(TAG, "Device discovery failed, continue to discover...");
                esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
            }
        } else if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) {
            ESP_LOGI(TAG, "Discovery started.");
			peers_list_maintain(NULL, PEERS_LIST_MAINTAIN_RESET);
        }
        break;
    }
    case ESP_BT_GAP_RMT_SRVCS_EVT:
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_BT_GAP_RMT_SRVCS_EVT));
    	break;
    case ESP_BT_GAP_RMT_SRVC_REC_EVT:
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_BT_GAP_RMT_SRVC_REC_EVT));
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT: {
    	if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG,"authentication success: %s", param->auth_cmpl.device_name);
            //esp_log_buffer_hex(param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(TAG,"authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT: 
        handle_bt_gap_pin_req(event, param);
        break;

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(TAG,"ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(TAG,"ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
        ESP_LOGI(TAG,"ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    default: {
        ESP_LOGI(TAG,"event: %d", event);
        break;
    }
    }
    return;
}
int heart_beat_delay[] = {
    1000,
    1000,
    1000,
    1000,
    10000,
    500,
    1000
};

static void a2d_app_heart_beat(void *arg)
{
    bt_app_work_dispatch(bt_app_av_sm_hdlr, BT_APP_HEART_BEAT_EVT, NULL, 0, NULL);
    int tmrduration=heart_beat_delay[bt_app_source_a2d_state];
    if(prev_duration!=tmrduration){
        xTimerChangePeriod(s_tmr,tmrduration, portMAX_DELAY);
        ESP_LOGD(TAG,"New heartbeat is %u",tmrduration);
        prev_duration=tmrduration;
    }
    else {
        ESP_LOGD(TAG,"Starting Heart beat timer for %ums",tmrduration);
    }
    xTimerStart(s_tmr, portMAX_DELAY);
}

static const char * conn_state_str(esp_a2d_connection_state_t state){
    char * statestr = "Unknown";
     switch (state)
        {
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
            statestr=STR(ESP_A2D_CONNECTION_STATE_DISCONNECTED);
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING:
            statestr=STR(ESP_A2D_CONNECTION_STATE_CONNECTING);
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTED:
            statestr=STR(ESP_A2D_CONNECTION_STATE_CONNECTED);
            break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTING:
            statestr=STR(ESP_A2D_CONNECTION_STATE_DISCONNECTING);
            break;
        default:
            break;
        }
    return statestr;
}

static void unexpected_connection_state(int from, esp_a2d_connection_state_t to)
{
    ESP_LOGW(TAG,"Unexpected connection state change. App State: %s (%u) Connection State %s (%u)", APP_AV_STATE_DESC[from], from,conn_state_str(to), to);
}

static void handle_connect_state_unconnected(uint16_t event, esp_a2d_cb_param_t *param){
    ESP_LOGV(TAG, "A2DP Event while unconnected ");
    switch (param->conn_stat.state)
    {
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTED:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);
            ESP_LOGE(TAG,"Connection state event received while status was unconnected.  Routing message to connecting state handler. State : %u",param->conn_stat.state);
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED){
                handle_connect_state_connecting(event, param);
            }
        break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTING:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);
            break;
        default:
            break;
    }

}

static void handle_connect_state_connecting(uint16_t event, esp_a2d_cb_param_t *param){
    ESP_LOGV(TAG, "A2DP connection state event : %s ",conn_state_str(param->conn_stat.state));

    switch (param->conn_stat.state)
    {
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
            if(param->conn_stat.disc_rsn!=ESP_A2D_DISC_RSN_NORMAL){
                ESP_LOGE(TAG,"A2DP had an abnormal disconnect event");
            }
            else {
                ESP_LOGW(TAG,"A2DP connect unsuccessful");
            }
            set_app_source_state(APP_AV_STATE_UNCONNECTED);
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING:
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTED:
            set_app_source_state(APP_AV_STATE_CONNECTED);
            set_a2dp_media_state(APP_AV_MEDIA_STATE_IDLE);
            ESP_LOGD(TAG,"Setting scan mode to ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE");
            esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
            ESP_LOGD(TAG,"Done setting scan mode. App state is now CONNECTED and media state IDLE.");
            break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTING:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state); 
            set_app_source_state(APP_AV_STATE_DISCONNECTING);       
            break;
        default:
            break;
    }
}
static void handle_connect_state_connected(uint16_t event, esp_a2d_cb_param_t *param){
    ESP_LOGV(TAG, "A2DP Event while connected ");
    switch (param->conn_stat.state)
    {
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
            ESP_LOGW(TAG,"a2dp disconnected");
            set_app_source_state(APP_AV_STATE_UNCONNECTED);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);  
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTED:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);
            break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTING:
            set_app_source_state(APP_AV_STATE_DISCONNECTING);        

            break;
        default:
            break;
    }
}

static void handle_connect_state_disconnecting(uint16_t event, esp_a2d_cb_param_t *param){
    ESP_LOGV(TAG, "A2DP Event while disconnecting ");
    switch (param->conn_stat.state)
    {
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
            ESP_LOGI(TAG,"a2dp disconnected");
            set_app_source_state(APP_AV_STATE_UNCONNECTED);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);        
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);  
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTED:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);  
            break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTING:
            unexpected_connection_state(bt_app_source_a2d_state, param->conn_stat.state);
            break;
        default:
            break;
    }

}

static void bt_app_av_sm_hdlr(uint16_t event, void *param)
{
    ESP_LOGV(TAG,"bt_app_av_sm_hdlr.%s a2d state: %s", event==BT_APP_HEART_BEAT_EVT?"Heart Beat.":"",APP_AV_STATE_DESC[bt_app_source_a2d_state]);
    switch (bt_app_source_a2d_state) {
    case APP_AV_STATE_DISCOVERING:
    	ESP_LOGV(TAG,"state %s, evt 0x%x, output state: %s", APP_AV_STATE_DESC[bt_app_source_a2d_state], event, output_state_str());
    	break;
    case APP_AV_STATE_DISCOVERED:
    	ESP_LOGV(TAG,"state %s, evt 0x%x, output state: %s", APP_AV_STATE_DESC[bt_app_source_a2d_state], event, output_state_str());
        break;
    case APP_AV_STATE_UNCONNECTED:
        bt_app_av_state_unconnected(event, param);
        break;
    case APP_AV_STATE_CONNECTING:
        bt_app_av_state_connecting(event, param);
        break;
    case APP_AV_STATE_CONNECTED:
        bt_app_av_state_connected(event, param);
        break;
    case APP_AV_STATE_DISCONNECTING:
        bt_app_av_state_disconnecting(event, param);
        break;
    default:
        ESP_LOGE(TAG,"%s invalid state %d", __func__, bt_app_source_a2d_state);
        break;
    }
}

static char *bda2str(esp_bd_addr_t bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}
static bool get_name_from_eir(uint8_t *eir, uint8_t *bdname, uint8_t *bdname_len)
{
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    if (!eir) {
        return false;
    }

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
            rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }

        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }

    return false;
}

static void filter_inquiry_scan_result(esp_bt_gap_cb_param_t *param)
{
    char bda_str[18];
    uint32_t cod = 0;
    int32_t rssi = -129; /* invalid value */
    uint8_t *eir = NULL;
    uint8_t nameLen = 0;
    esp_bt_gap_dev_prop_t *p;
    memset(bda_str, 0x00, sizeof(bda_str));
    if(bt_app_source_a2d_state != APP_AV_STATE_DISCOVERING)
    {
    	// Ignore messages that might have been queued already
    	// when we've discovered the target device.
    	return;
    }
    memset(s_peer_bdname, 0x00,sizeof(s_peer_bdname));

    bda2str(param->disc_res.bda, bda_str, 18);

    ESP_LOGV(TAG,"\n=======================\nScanned device: %s",bda_str );
    for (int i = 0; i < param->disc_res.num_prop; i++) {
        p = param->disc_res.prop + i;
        switch (p->type) {
        case ESP_BT_GAP_DEV_PROP_COD:
            cod = *(uint32_t *)(p->val);
            ESP_LOGV(TAG,"-- Class of Device: 0x%x", cod);
            break;
        case ESP_BT_GAP_DEV_PROP_RSSI:
            rssi = *(int8_t *)(p->val);
            ESP_LOGV(TAG,"-- RSSI: %d", rssi);
            break;
        case ESP_BT_GAP_DEV_PROP_EIR:
            eir = (uint8_t *)(p->val);
            ESP_LOGV(TAG,"-- EIR: %u", *eir);
            break;
        case ESP_BT_GAP_DEV_PROP_BDNAME:
            nameLen = (p->len > ESP_BT_GAP_MAX_BDNAME_LEN) ? ESP_BT_GAP_MAX_BDNAME_LEN : (uint8_t)p->len;
            memcpy(s_peer_bdname, (uint8_t *)(p->val), nameLen);
            s_peer_bdname[nameLen] = '\0';
            ESP_LOGV(TAG,"-- Name: %s", s_peer_bdname);
            break;
        default:
            break;
        }
    }
    if (!esp_bt_gap_is_valid_cod(cod)){
    /* search for device with MAJOR service class as "rendering" in COD */
    	ESP_LOGV(TAG,"--Invalid class of device. Skipping.\n");
    	return;
    }
    else if (!(esp_bt_gap_get_cod_srvc(cod) & ESP_BT_COD_SRVC_RENDERING))
    {
    	ESP_LOGV(TAG,"--Not a rendering device. Skipping.\n");
    	return;
    }

    if (eir) {
    	ESP_LOGV(TAG,"--Getting details from eir.\n");
        get_name_from_eir(eir, s_peer_bdname, NULL);
        ESP_LOGV(TAG,"--Device name is %s\n",s_peer_bdname);
    }
    if(strlen((char *)s_peer_bdname)>0) {
        peers_list_maintain((const char *)s_peer_bdname, rssi);    
    }

    if (squeezelite_conf.sink_name && strlen(squeezelite_conf.sink_name) >0 && strcmp((char *)s_peer_bdname, squeezelite_conf.sink_name) == 0) {
        ESP_LOGI(TAG,"Found our target device. address %s, name %s", bda_str, s_peer_bdname);
		memcpy(s_peer_bda, param->disc_res.bda, ESP_BD_ADDR_LEN);
        set_app_source_state(APP_AV_STATE_DISCOVERED);
        esp_bt_gap_cancel_discovery();
    } else {
    	ESP_LOGV(TAG,"Not the device we are looking for (%s). Continuing scan", squeezelite_conf.sink_name?squeezelite_conf.sink_name:"N/A");
    }
}

static void bt_av_hdl_stack_evt(uint16_t event, void *p_param)
{
    switch (event) {
    case BT_APP_EVT_STACK_UP: {
    	ESP_LOGI(TAG,"BT Stack going up.");
        /* set up device name */


        char * a2dp_dev_name = 	config_alloc_get_default(NVS_TYPE_STR, "a2dp_dev_name", CONFIG_A2DP_DEV_NAME, 0);
    	if(a2dp_dev_name  == NULL){
    		ESP_LOGW(TAG,"Unable to retrieve the a2dp device name from nvs");
    		esp_bt_dev_set_device_name(CONFIG_A2DP_DEV_NAME);
    	}
    	else {
    		esp_bt_dev_set_device_name(a2dp_dev_name);
    		free(a2dp_dev_name);
    	}

        ESP_LOGI(TAG,"Preparing to connect");

        /* register GAP callback function */
        esp_bt_gap_register_callback(bt_app_gap_cb);

        /* initialize AVRCP controller */
        esp_avrc_ct_init();
        esp_avrc_ct_register_callback(bt_app_rc_ct_cb);

        esp_avrc_rn_evt_cap_mask_t evt_set = {0};
        esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_SET, &evt_set, ESP_AVRC_RN_VOLUME_CHANGE);
        assert(esp_avrc_tg_set_rn_evt_cap(&evt_set) == ESP_OK);


        /* initialize A2DP source */
        esp_a2d_register_callback(&bt_app_a2d_cb);
        esp_a2d_source_register_data_callback(&output_bt_data);
        esp_a2d_source_init();

        /* set discoverable and connectable mode */
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

        /* start device discovery */
        ESP_LOGI(TAG,"Starting device discovery...");
        set_app_source_state(APP_AV_STATE_DISCOVERING);
        esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);

        /* create and start heart beat timer */
        int tmr_id = 0;
        s_tmr = xTimerCreate("connTmr", ( prev_duration/ portTICK_RATE_MS),pdFALSE, (void *)tmr_id, a2d_app_heart_beat);        
        xTimerStart(s_tmr, portMAX_DELAY);
        break;
    }
    default:
    	ESP_LOGE(TAG,"%s unhandled evt %d", __func__, event);
        break;
    }
}
static void bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    switch (event) {
    case ESP_AVRC_CT_METADATA_RSP_EVT:
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT:
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT:
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT:
    case ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT: {
        ESP_LOGD(TAG,"Received %s message", ESP_AVRC_CT_DESC[event]);
        bt_app_work_dispatch(bt_av_hdl_avrc_ct_evt, event, param, sizeof(esp_avrc_ct_cb_param_t), NULL);
        break;
    }
    default:
        ESP_LOGE(BT_RC_CT_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}
static void bt_app_av_media_proc(uint16_t event, void *param)
{
    esp_a2d_cb_param_t *a2d = NULL;
    switch (bt_app_source_media_state) {
    case APP_AV_MEDIA_STATE_IDLE: {
    	if (event == BT_APP_HEART_BEAT_EVT) {
            if(!output_stopped())
            {
            	ESP_LOGI(TAG,"Output state is %s, Checking if A2DP is ready.", output_state_str());
            	esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
            }

        } else if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT) {
        	a2d = (esp_a2d_cb_param_t *)(param);
			if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY &&
					a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS
					) {
				ESP_LOGI(TAG,"a2dp media ready, starting playback!");
				set_a2dp_media_state(APP_AV_MEDIA_STATE_STARTING);
				esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_START);
			}
        }
        break;
    }

    case APP_AV_MEDIA_STATE_STARTING: {
    	if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT) {
            a2d = (esp_a2d_cb_param_t *)(param);
            if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_START &&
                    a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
            	ESP_LOGI(TAG,"a2dp media started successfully.");
                set_a2dp_media_state(APP_AV_MEDIA_STATE_STARTED);
            } else {
                // not started succesfully, transfer to idle state
            	ESP_LOGI(TAG,"a2dp media start failed.");
                set_a2dp_media_state(APP_AV_MEDIA_STATE_IDLE);
            }
        }
        break;
    }
    case APP_AV_MEDIA_STATE_STARTED: {
        if (event == BT_APP_HEART_BEAT_EVT) {
        	if(output_stopped()) {
        		ESP_LOGI(TAG,"Output state is %s. Stopping a2dp media ...", output_state_str());
                set_a2dp_media_state(APP_AV_MEDIA_STATE_STOPPING);
                esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_STOP);
            } else {
				output_bt_tick();	
        	}
        }
        break;
    }
    case APP_AV_MEDIA_STATE_STOPPING: {
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(APP_AV_MEDIA_STATE_STOPPING));
        if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT) {
            a2d = (esp_a2d_cb_param_t *)(param);
            if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_STOP &&
                    a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
                ESP_LOGI(TAG,"a2dp media stopped successfully...");
               	set_a2dp_media_state(APP_AV_MEDIA_STATE_IDLE);
            } else {
                ESP_LOGI(TAG,"a2dp media stopping...");
                esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_STOP);
            }
        }
        break;
    }

    case APP_AV_MEDIA_STATE_WAIT_DISCONNECT:{
    	esp_a2d_source_disconnect(s_peer_bda);
		set_app_source_state(APP_AV_STATE_DISCONNECTING);
		ESP_LOGI(TAG,"a2dp disconnecting...");
    }
    }
}

static void bt_app_av_state_unconnected(uint16_t event, void *param)
{
    ESP_LOGV(TAG, "Handling state unconnected A2DP event");
	switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
        handle_connect_state_unconnected(event, (esp_a2d_cb_param_t *)param);
    	break;
    case ESP_A2D_AUDIO_STATE_EVT:
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_AUDIO_STATE_EVT));
    	break;
    case ESP_A2D_AUDIO_CFG_EVT:
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_AUDIO_CFG_EVT));
    	break;
    case ESP_A2D_MEDIA_CTRL_ACK_EVT:
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_MEDIA_CTRL_ACK_EVT));
    	break;
    case BT_APP_HEART_BEAT_EVT: {
        ESP_LOG_DEBUG_EVENT(TAG,QUOTE(BT_APP_HEART_BEAT_EVT));
        switch (esp_bluedroid_get_status()) {
		case ESP_BLUEDROID_STATUS_UNINITIALIZED:
			ESP_LOGV(TAG,"BlueDroid Status is ESP_BLUEDROID_STATUS_UNINITIALIZED.");
			break;
		case ESP_BLUEDROID_STATUS_INITIALIZED:
			ESP_LOGV(TAG,"BlueDroid Status is ESP_BLUEDROID_STATUS_INITIALIZED.");
			break;
		case ESP_BLUEDROID_STATUS_ENABLED:
			ESP_LOGV(TAG,"BlueDroid Status is ESP_BLUEDROID_STATUS_ENABLED.");
			break;
		default:
			break;
		}
        uint8_t *p = s_peer_bda;
        ESP_LOGI(TAG, "a2dp connecting to %s, BT peer: %02x:%02x:%02x:%02x:%02x:%02x",s_peer_bdname,p[0], p[1], p[2], p[3], p[4], p[5]);
        if(esp_a2d_source_connect(s_peer_bda)==ESP_OK) {  
            set_app_source_state(APP_AV_STATE_CONNECTING);
            s_connecting_intv = 0;
		}
		else {
            set_app_source_state(APP_AV_STATE_UNCONNECTED);
			// there was an issue connecting... continue to discover
			ESP_LOGE(TAG,"Attempt at connecting failed, restart at discover...");
			esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
        }
        break;
    }
    default:
    	ESP_LOGE(TAG,"%s unhandled evt %d", __func__, event);
        break;
    }
}

static void bt_app_av_state_connecting(uint16_t event, void *param)
{
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
        handle_connect_state_connecting(event, (esp_a2d_cb_param_t *)param);
        break;
    case ESP_A2D_AUDIO_STATE_EVT:
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_AUDIO_STATE_EVT));
    	break;
    case ESP_A2D_AUDIO_CFG_EVT:
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_AUDIO_CFG_EVT));
    	break;
    case ESP_A2D_MEDIA_CTRL_ACK_EVT:
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_MEDIA_CTRL_ACK_EVT));
    	break;
    case BT_APP_HEART_BEAT_EVT:
        if (++s_connecting_intv >= 2) {
            set_app_source_state(APP_AV_STATE_UNCONNECTED);
            ESP_LOGW(TAG,"A2DP Connect time out!  Setting state to Unconnected. ");
            s_connecting_intv = 0;
        }
        break;
    default:
        ESP_LOGE(TAG,"%s unhandled evt %d", __func__, event);
        break;
    }
}


static void bt_app_av_state_connected(uint16_t event, void *param)
{
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT: {
        handle_connect_state_connected(event, (esp_a2d_cb_param_t *)param);
        break;
    }
    case ESP_A2D_AUDIO_STATE_EVT: {
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_AUDIO_STATE_EVT));
        a2d = (esp_a2d_cb_param_t *)(param);
        if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
            s_pkt_cnt = 0;
        }
        break;
    }
    case ESP_A2D_AUDIO_CFG_EVT:
        // not suppposed to occur for A2DP source
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_AUDIO_CFG_EVT));
        break;
    case ESP_A2D_MEDIA_CTRL_ACK_EVT:{
    	ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_MEDIA_CTRL_ACK_EVT));
            bt_app_av_media_proc(event, param);
            break;
        }
    case BT_APP_HEART_BEAT_EVT: {
    	ESP_LOGV(TAG,QUOTE(BT_APP_HEART_BEAT_EVT));
        bt_app_av_media_proc(event, param);
        break;
    }
    default:
        ESP_LOGE(TAG,"%s unhandled evt %d", __func__, event);
        break;
    }
}

static void bt_app_av_state_disconnecting(uint16_t event, void *param)
{
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT: 
            handle_connect_state_disconnecting( event, (esp_a2d_cb_param_t *)param);
            break;
        case ESP_A2D_AUDIO_STATE_EVT:
            ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_AUDIO_STATE_EVT));
            break;
        case ESP_A2D_AUDIO_CFG_EVT:
            ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_AUDIO_CFG_EVT));
            break;
        case ESP_A2D_MEDIA_CTRL_ACK_EVT:
            ESP_LOG_DEBUG_EVENT(TAG,QUOTE(ESP_A2D_MEDIA_CTRL_ACK_EVT));
            break;
        case BT_APP_HEART_BEAT_EVT:
            ESP_LOG_DEBUG_EVENT(TAG,QUOTE(BT_APP_HEART_BEAT_EVT));
            break;
        default:
            ESP_LOGE(TAG,"%s unhandled evt %d", __func__, event);
            break;
        }
}

static void bt_av_volume_changed(void)
{
    if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST, &s_avrc_peer_rn_cap,
                                           ESP_AVRC_RN_VOLUME_CHANGE)) {
        esp_avrc_ct_send_register_notification_cmd(APP_RC_CT_TL_RN_VOLUME_CHANGE, ESP_AVRC_RN_VOLUME_CHANGE, 0);
    }
}

static void bt_av_notify_evt_handler(uint8_t event_id, esp_avrc_rn_param_t *event_parameter)
{
    switch (event_id) {
    case ESP_AVRC_RN_VOLUME_CHANGE:
        ESP_LOGI(BT_RC_CT_TAG, "Volume changed: %d", event_parameter->volume);
        ESP_LOGI(BT_RC_CT_TAG, "Set absolute volume: volume %d", event_parameter->volume + 5);
        esp_avrc_ct_send_set_absolute_volume_cmd(APP_RC_CT_TL_RN_VOLUME_CHANGE, event_parameter->volume + 5);
        bt_av_volume_changed();
        break;
    }
}
static void bt_av_hdl_avrc_ct_evt(uint16_t event, void *p_param)
{
    ESP_LOGD(BT_RC_CT_TAG, "%s evt %d", __func__, event);
    esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(p_param);
    switch (event) {
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        ESP_LOGI(BT_RC_CT_TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

        if (rc->conn_stat.connected) {
            // get remote supported event_ids of peer AVRCP Target
            esp_avrc_ct_send_get_rn_capabilities_cmd(APP_RC_CT_TL_GET_CAPS);
        } else {
            // clear peer notification capability record
            s_avrc_peer_rn_cap.bits = 0;
        }
        break;
    }
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC passthrough rsp: key_code 0x%x, key_state %d", rc->psth_rsp.key_code, rc->psth_rsp.key_state);
        break;
    }
    case ESP_AVRC_CT_METADATA_RSP_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);
        free(rc->meta_rsp.attr_text);
        break;
    }
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC event notification: %d", rc->change_ntf.event_id);
        bt_av_notify_evt_handler(rc->change_ntf.event_id, &rc->change_ntf.event_parameter);
        break;
    }
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "AVRC remote features %x, TG features %x", rc->rmt_feats.feat_mask, rc->rmt_feats.tg_feat_flag);
        break;
    }
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "remote rn_cap: count %d, bitmask 0x%x", rc->get_rn_caps_rsp.cap_count,
                 rc->get_rn_caps_rsp.evt_set.bits);
        s_avrc_peer_rn_cap.bits = rc->get_rn_caps_rsp.evt_set.bits;

        bt_av_volume_changed();
        break;
    }
    case ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT: {
        ESP_LOGI(BT_RC_CT_TAG, "Set absolute volume rsp: volume %d", rc->set_volume_rsp.volume);
        break;
    }

    default:
        ESP_LOGE(BT_RC_CT_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}
