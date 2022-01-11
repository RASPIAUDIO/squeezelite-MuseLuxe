/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)
    
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc_cntl_reg.h"
#include "esp32/rom/uart.h"
#include "cmd_system.h"
#include "sdkconfig.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "platform_esp32.h"
#include "platform_config.h"
#include "esp_sleep.h"
#include "driver/uart.h"            // for the uart driver access
#include "messaging.h"				  
#include "platform_console.h"
#include "trace.h"
#ifdef CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
#pragma message("Runtime stats enabled")
#define WITH_TASKS_INFO 1
#else 
#pragma message("Runtime stats disabled")
#endif
static struct {
	struct arg_str *scanmode;
	struct arg_end *end;
} wifi_parms_arg;
static struct {
	struct arg_str *name;
	struct arg_end *end;
} name_args;
static struct {
 	struct arg_lit *btspeaker;
 	struct arg_lit *airplay;
 	struct arg_str *telnet;
#if WITH_TASKS_INFO    
 	struct arg_lit *stats;
#endif  
    struct arg_end *end;
} set_services_args;
static const char * TAG = "cmd_system";

//static void register_setbtsource();
static void register_free();
static void register_setdevicename();
static void register_heap();
static void register_version();
static void register_restart();
static void register_deep_sleep();
static void register_light_sleep();
static void register_factory_boot();
static void register_restart_ota();
static void register_update_certs();
static void register_set_services();
static void register_set_wifi_parms();
#if WITH_TASKS_INFO
static void register_tasks();
#endif
extern BaseType_t wifi_manager_task;
void register_system()
{
    register_set_wifi_parms();
//	register_setbtsource();
    register_free();
    register_set_services();
    register_heap();
    register_setdevicename();
    register_version();
    register_restart();
    register_deep_sleep();
    register_light_sleep();
    register_update_certs();
    register_factory_boot();
    register_restart_ota();
#if WITH_TASKS_INFO
    register_tasks();
#endif
}

/* 'version' command */
static int get_version(int argc, char **argv)
{
    esp_chip_info_t info;
    esp_chip_info(&info);
    cmd_send_messaging(argv[0],MESSAGING_INFO,
    "IDF Version:%s\r\n"
    "Chip info:\r\n"
    "\tmodel:%s\r\n"
    "\tcores:%d\r\n"
    "\tfeature:%s%s%s%s%d%s\r\n"
    "\trevision number:%d\r\n",
		esp_get_idf_version(), info.model == CHIP_ESP32 ? "ESP32" : "Unknow", info.cores,
		info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
		info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
		info.features & CHIP_FEATURE_BT ? "/BT" : "",
		info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:" : "/External-Flash:",
		spi_flash_get_chip_size() / (1024 * 1024), " MB", info.revision);
    return 0;
}

static void register_version()
{
    const esp_console_cmd_t cmd = {
        .command = "version",
        .help = "Get version of chip and SDK",
        .hint = NULL,
        .func = &get_version,
    };
    cmd_to_json(&cmd);
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

esp_err_t guided_boot(esp_partition_subtype_t partition_subtype)
{
    if(is_recovery_running){
        if(partition_subtype ==ESP_PARTITION_SUBTYPE_APP_FACTORY){
            log_send_messaging(MESSAGING_WARNING,"RECOVERY application is already active");
            if(!wait_for_commit()){
                log_send_messaging(MESSAGING_WARNING,"Unable to commit configuration. ");
            }
            
            vTaskDelay(750/ portTICK_PERIOD_MS);
            esp_restart();
            return ESP_OK;
        }
    }
    else {
        if(partition_subtype !=ESP_PARTITION_SUBTYPE_APP_FACTORY){
            log_send_messaging(MESSAGING_WARNING,"SQUEEZELITE application is already active");
            if(!wait_for_commit()){
                log_send_messaging(MESSAGING_WARNING,"Unable to commit configuration. ");
            }
            
            vTaskDelay(750/ portTICK_PERIOD_MS);
            esp_restart();
            return ESP_OK;
        }
    }
	esp_err_t err = ESP_OK;
	bool bFound=false;
    log_send_messaging(MESSAGING_INFO, "Looking for partition type %u",partition_subtype);
    const esp_partition_t *partition;
	esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, partition_subtype, NULL);

	if(it == NULL){
		log_send_messaging(MESSAGING_ERROR,"Reboot failed. Cannot iterate through partitions");
	}
	else
	{
		ESP_LOGD(TAG, "Found partition. Getting info.");
		partition = (esp_partition_t *) esp_partition_get(it);
		ESP_LOGD(TAG, "Releasing partition iterator");
		esp_partition_iterator_release(it);
		if(partition != NULL){
			log_send_messaging(MESSAGING_INFO, "Found application partition %s sub type %u", partition->label,partition_subtype);
			err=esp_ota_set_boot_partition(partition);
			if(err!=ESP_OK){
				bFound=false;
				log_send_messaging(MESSAGING_ERROR,"Unable to select partition for reboot: %s",esp_err_to_name(err));
			}
			else{
                bFound=true;
			}
		}
		else
		{
			log_send_messaging(MESSAGING_ERROR,"partition type %u not found!  Unable to reboot to recovery.",partition_subtype);

		}
		ESP_LOGD(TAG, "Yielding to other processes");
		taskYIELD();
		if(bFound) {
			if(!wait_for_commit()){
				log_send_messaging(MESSAGING_WARNING,"Unable to commit configuration changes. ");
			}
			vTaskDelay(750/ portTICK_PERIOD_MS);
			esp_restart();
		}
	}

	return ESP_OK;
}

static int restart(int argc, char **argv)
{
	log_send_messaging(MESSAGING_WARNING, "\n\nPerforming a simple restart to the currently active partition.");
	if(!wait_for_commit()){
		cmd_send_messaging(argv[0],MESSAGING_WARNING,"Unable to commit configuration. ");
	}
    vTaskDelay(750/ portTICK_PERIOD_MS);
    esp_restart();
    return 0;
}

void simple_restart()
{
	log_send_messaging(MESSAGING_WARNING,"System reboot requested.");
	if(!wait_for_commit()){
		log_send_messaging(MESSAGING_WARNING,"Unable to commit configuration. ");
	}

											   
	vTaskDelay(750/ portTICK_PERIOD_MS);
    esp_restart();
}

esp_err_t guided_restart_ota(){
	log_send_messaging(MESSAGING_WARNING,"System reboot to Application requested");
    guided_boot(ESP_PARTITION_SUBTYPE_APP_OTA_0);
	return ESP_FAIL; // return fail.  This should never return... we're rebooting!
}
esp_err_t guided_factory(){
	log_send_messaging(MESSAGING_WARNING,"System reboot to recovery requested");
	guided_boot(ESP_PARTITION_SUBTYPE_APP_FACTORY);
	return ESP_FAIL; // return fail.  This should never return... we're rebooting!
}
static int restart_factory(int argc, char **argv)
{
	cmd_send_messaging(argv[0],MESSAGING_WARNING, "Executing guided boot into recovery");
	guided_boot(ESP_PARTITION_SUBTYPE_APP_FACTORY);
	return 0; // return fail.  This should never return... we're rebooting!
}
static int restart_ota(int argc, char **argv)
{
	cmd_send_messaging(argv[0],MESSAGING_WARNING, "Executing guided boot into ota app 0");
	guided_boot(ESP_PARTITION_SUBTYPE_APP_OTA_0);
	return 0; // return fail.  This should never return... we're rebooting!
}
static void register_restart()
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Reboot system",
        .hint = NULL,
        .func = &restart,
    };
    cmd_to_json(&cmd);
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
static void register_restart_ota()
{
    const esp_console_cmd_t cmd = {
        .command = "restart_ota",
        .help = "Reboot system to Squeezelite",
        .hint = NULL,
        .func = &restart_ota,
    };
    cmd_to_json(&cmd);
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_factory_boot()
{
    const esp_console_cmd_t cmd = {
        .command = "recovery",
        .help = "Reboot system to Recovery",
        .hint = NULL,
        .func = &restart_factory,
    };
    cmd_to_json(&cmd);
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
/** 'free' command prints available heap memory */

static int free_mem(int argc, char **argv)
{
	cmd_send_messaging(argv[0],MESSAGING_INFO,"%d", esp_get_free_heap_size());
    return 0;
}


static void register_free()
{
    const esp_console_cmd_t cmd = {
        .command = "free",
        .help = "Get the current size of free heap memory",
        .hint = NULL,
        .func = &free_mem,
    };
    cmd_to_json(&cmd);
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/* 'heap' command prints minumum heap size */
static int heap_size(int argc, char **argv)
{
    uint32_t heap_size = heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
    cmd_send_messaging(argv[0],MESSAGING_INFO, "min heap size: %u", heap_size);
    return 0;
}
cJSON * setdevicename_cb(){
	char * default_host_name = config_alloc_get_str("host_name",NULL,"Squeezelite");
	cJSON * values = cJSON_CreateObject();
	cJSON_AddStringToObject(values,"name",default_host_name);
	free(default_host_name);
	return values;
}
static int setnamevar(char * nvsname, FILE *f, char * value){
	esp_err_t err=ESP_OK;
	if((err=config_set_value(NVS_TYPE_STR, nvsname, value))!=ESP_OK){
		fprintf(f,"Unable to set %s=%s. %s\n",nvsname,value,esp_err_to_name(err));
	}
	return err==ESP_OK?0:1;
}
typedef enum {
    SCANNING,
    PROCESSING_NAME
} scanstate_t;
int set_squeezelite_player_name(FILE * f,const char * name){
	char * nvs_config= config_alloc_get(NVS_TYPE_STR, "autoexec1");
	char **argv = NULL;
    esp_err_t err=ESP_OK;
    int nerrors=0;
    bool bFoundParm=false;
    scanstate_t state=SCANNING;
    char * newCommandLine = NULL;
    char * parm = " -n ";
    char * cleaned_name = strdup(name);
    for(char * p=cleaned_name;*p!='\0';p++){
        if(*p == ' '){
            *p='_'; // no spaces allowed
        }
    }
	if(nvs_config && strlen(nvs_config)>0){
        // allocate enough memory to hold the new command line
        size_t cmdLength = strlen(nvs_config) + strlen(cleaned_name) + strlen(parm) +1 ;
        newCommandLine = malloc(cmdLength);
        memset(newCommandLine,0x00, cmdLength);
		ESP_LOGD(TAG,"Parsing command %s",nvs_config);
		argv = (char **) calloc(22, sizeof(char *));
		if (argv == NULL) {
			FREE_AND_NULL(nvs_config);
			return 1;
		}
		size_t argc = esp_console_split_argv(nvs_config, argv,22);
		for(int i=0;i<argc;i++) {
            if(i>0){
                strcat(newCommandLine," ");
            }
            switch (state)
            {
            case SCANNING:
                strcat(newCommandLine,argv[i]);
                if(strcasecmp(argv[i],"--name")==0 || strcasecmp(argv[i],"-n")==0 ){
                    state = PROCESSING_NAME;
                }
                break;
            case PROCESSING_NAME:
                bFoundParm=true;
                strcat(newCommandLine,cleaned_name);
                state = SCANNING;
                break;
            
            default:
                break;
            }
        }
        if(!bFoundParm){
            strcat(newCommandLine,parm);
            strcat(newCommandLine,name);
        }
        fprintf(f,"Squeezelite player name changed to %s\n",newCommandLine);
        if((err=config_set_value(NVS_TYPE_STR, "autoexec1",newCommandLine))!=ESP_OK){
            nerrors++;
            fprintf(f,"Failed updating squeezelite command. %s", esp_err_to_name(err));
        }
		
	}

	FREE_AND_NULL(nvs_config);
	FREE_AND_NULL(argv);
	free(cleaned_name);
	return nerrors;
	
}
static int setdevicename(int argc, char **argv)
{
	char * name = NULL;
    int nerrors = arg_parse_msg(argc, argv,(struct arg_hdr **)&name_args);
    if (nerrors != 0) {
        return 1;
    }

	/* Check "--name" option */
	if (name_args.name->count) {
		name=strdup(name_args.name->sval[0]);
	}
	else {
		cmd_send_messaging(argv[0],MESSAGING_ERROR,"Name must be specified.");
		return 1;
	}

	char *buf = NULL;
	size_t buf_size = 0;
	FILE *f = open_memstream(&buf, &buf_size);
	if (f == NULL) {
		cmd_send_messaging(argv[0],MESSAGING_ERROR,"Unable to open memory stream.");
		return 1;
	}
	nerrors+=setnamevar("a2dp_dev_name", f, name);
	nerrors+=setnamevar("airplay_name", f, name);
	nerrors+=setnamevar("ap_ssid", f, name);
	nerrors+=setnamevar("bt_name", f, name);
	nerrors+=setnamevar("host_name", f, name);
    nerrors+=set_squeezelite_player_name(f, name);
	if(nerrors==0){
		fprintf(f,"Device name changed to %s\n",name);
	}
	if(!nerrors ){
		fprintf(f,"Done.\n");
	}
	FREE_AND_NULL(name);
	fflush (f);
	cmd_send_messaging(argv[0],nerrors>0?MESSAGING_ERROR:MESSAGING_INFO,"%s", buf);
	fclose(f);
	FREE_AND_NULL(buf);
	return nerrors;

}

static void register_heap()
{
    const esp_console_cmd_t heap_cmd = {
        .command = "heap",
        .help = "Get minimum size of free heap memory found during execution",
        .hint = NULL,
        .func = &heap_size,
    };
    cmd_to_json(&heap_cmd);
    ESP_ERROR_CHECK( esp_console_cmd_register(&heap_cmd) );

}


static void register_setdevicename()
{
	char * default_host_name = config_alloc_get_str("host_name",NULL,"Squeezelite");
	name_args.name = arg_str0("n", "name", default_host_name, "New Name");
	name_args.end = arg_end(8);
	const esp_console_cmd_t set_name= {
	 		.command = CFG_TYPE_SYST("name"),
			.help="Device Name",
			.hint = NULL,
			.func = &setdevicename,
			.argtable = &name_args
	};
	cmd_to_json_with_cb(&set_name,&setdevicename_cb);
	ESP_ERROR_CHECK(esp_console_cmd_register(&set_name));
}
/** 'tasks' command prints the list of tasks and related information */
#if WITH_TASKS_INFO

static int tasks_info(int argc, char **argv)
{
    const size_t bytes_per_task = 40; /* see vTaskList description */
    char *task_list_buffer = malloc(uxTaskGetNumberOfTasks() * bytes_per_task);
    if (task_list_buffer == NULL) {
    	cmd_send_messaging(argv[0],MESSAGING_ERROR, "failed to allocate buffer for vTaskList output");
        return 1;
    }
    cmd_send_messaging(argv[0],MESSAGING_INFO,"Task Name\tStatus\tPrio\tHWM\tTask#"
#ifdef CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID
    "\tAffinity"
#endif
    "\n");
    vTaskList(task_list_buffer);
    cmd_send_messaging(argv[0],MESSAGING_INFO,"%s", task_list_buffer);
    free(task_list_buffer);
    return 0;
}

static void register_tasks()
{
    const esp_console_cmd_t cmd = {
        .command = "tasks",
        .help = "Get information about running tasks",
        .hint = NULL,
        .func = &tasks_info,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

#endif // WITH_TASKS_INFO

extern esp_err_t update_certificates(bool force);
static int force_update_cert(int argc, char **argv){
	return update_certificates(true);
}

static void register_update_certs()
{
    const esp_console_cmd_t cmd = {
        .command = "update_certificates",
        .help = "Force updating the certificates from binary",
        .hint = NULL,
        .func = &force_update_cert,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}



/** 'deep_sleep' command puts the chip into deep sleep mode */

static struct {
    struct arg_int *wakeup_time;
    struct arg_int *wakeup_gpio_num;
    struct arg_int *wakeup_gpio_level;
    struct arg_end *end;
} deep_sleep_args;


static int deep_sleep(int argc, char **argv)
{
	int nerrors = arg_parse_msg(argc, argv,(struct arg_hdr **)&deep_sleep_args);
    if (nerrors != 0) {
        return 1;
    }
    if (deep_sleep_args.wakeup_time->count) {
        uint64_t timeout = 1000ULL * deep_sleep_args.wakeup_time->ival[0];
        cmd_send_messaging(argv[0],MESSAGING_INFO, "Enabling timer wakeup, timeout=%lluus", timeout);
        ESP_ERROR_CHECK( esp_sleep_enable_timer_wakeup(timeout) );
    }
    if (deep_sleep_args.wakeup_gpio_num->count) {
        int io_num = deep_sleep_args.wakeup_gpio_num->ival[0];
        if (!rtc_gpio_is_valid_gpio(io_num)) {
        	cmd_send_messaging(argv[0],MESSAGING_ERROR, "GPIO %d is not an RTC IO", io_num);
            return 1;
        }
        int level = 0;
        if (deep_sleep_args.wakeup_gpio_level->count) {
            level = deep_sleep_args.wakeup_gpio_level->ival[0];
            if (level != 0 && level != 1) {
            	cmd_send_messaging(argv[0],MESSAGING_ERROR, "Invalid wakeup level: %d", level);
                return 1;
            }
        }
        cmd_send_messaging(argv[0],MESSAGING_INFO, "Enabling wakeup on GPIO%d, wakeup on %s level",
                 io_num, level ? "HIGH" : "LOW");

        ESP_ERROR_CHECK( esp_sleep_enable_ext1_wakeup(1ULL << io_num, level) );
    }
    rtc_gpio_isolate(GPIO_NUM_12);
    esp_deep_sleep_start();
    return 0; // this code will never run. deep sleep will cause the system to restart
}

static void register_deep_sleep()
{
    deep_sleep_args.wakeup_time =
        arg_int0("t", "time", "<t>", "Wake up time, ms");
    deep_sleep_args.wakeup_gpio_num =
        arg_int0(NULL, "io", "<n>",
                 "If specified, wakeup using GPIO with given number");
    deep_sleep_args.wakeup_gpio_level =
        arg_int0(NULL, "io_level", "<0|1>", "GPIO level to trigger wakeup");
    deep_sleep_args.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "deep_sleep",
        .help = "Enter deep sleep mode. "
        "Two wakeup modes are supported: timer and GPIO. "
        "If no wakeup option is specified, will sleep indefinitely.",
        .hint = NULL,
        .func = &deep_sleep,
        .argtable = &deep_sleep_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
static int enable_disable(FILE * f,char * nvs_name, struct arg_lit *arg){
	esp_err_t err = config_set_value(NVS_TYPE_STR, nvs_name, arg->count>0?"Y":"N");
	const char * name = arg->hdr.longopts?arg->hdr.longopts:arg->hdr.glossary;

	if(err!=ESP_OK){
		fprintf(f,"Error %s %s. %s\n",arg->count>0?"Enabling":"Disabling", name, esp_err_to_name(err));
	}
	else {
		fprintf(f,"%s %s\n",arg->count>0?"Enabled":"Disabled",name);
	}
	return err;
}
static int do_configure_wifi(int argc, char **argv){
    esp_err_t err = ESP_OK;
    int nerrors = arg_parse_msg(argc, argv,(struct arg_hdr **)&wifi_parms_arg);
    if (nerrors != 0) {
        return 1;
    }
	char *buf = NULL;
	size_t buf_size = 0;
	FILE *f = open_memstream(&buf, &buf_size);
	if (f == NULL) {
		cmd_send_messaging(argv[0],MESSAGING_ERROR,"Unable to open memory stream.");
		return 1;
	}
    
    if(wifi_parms_arg.scanmode->count>0){
        if(strcasecmp(wifi_parms_arg.scanmode->sval[0],"Comprehensive") == 0){
            err = config_set_value(NVS_TYPE_STR, "wifi_smode", "A");
        } 
        else {
            err = config_set_value(NVS_TYPE_STR, "wifi_smode", "F");
        }
        if(err!=ESP_OK){
            nerrors++;
            fprintf(f,"Error setting wifi scan mode to %s. %s\n",wifi_parms_arg.scanmode->sval[0], esp_err_to_name(err));
        }
        else {
            fprintf(f,"Wifi Scan Mode changed to %s\n",wifi_parms_arg.scanmode->sval[0]);
        }
    }
	if(!nerrors ){
		fprintf(f,"Done.\n");
	}
	fflush (f);
	cmd_send_messaging(argv[0],nerrors>0?MESSAGING_ERROR:MESSAGING_INFO,"%s", buf);
	fclose(f);
	FREE_AND_NULL(buf);
	return nerrors;
}
static int do_set_services(int argc, char **argv)
{
    esp_err_t err = ESP_OK;
    int nerrors = arg_parse_msg(argc, argv,(struct arg_hdr **)&set_services_args);
    if (nerrors != 0) {
        return 1;
    }
	char *buf = NULL;
	size_t buf_size = 0;
	FILE *f = open_memstream(&buf, &buf_size);
	if (f == NULL) {
		cmd_send_messaging(argv[0],MESSAGING_ERROR,"Unable to open memory stream.");
		return 1;
	}

	nerrors += enable_disable(f,"enable_airplay",set_services_args.airplay);
	nerrors += enable_disable(f,"enable_bt_sink",set_services_args.btspeaker);

    if(set_services_args.telnet->count>0){
        if(strcasecmp(set_services_args.telnet->sval[0],"Disabled") == 0){
            err = config_set_value(NVS_TYPE_STR, "telnet_enable", "N");
        } 
        else if(strcasecmp(set_services_args.telnet->sval[0],"Telnet Only") == 0){
            err = config_set_value(NVS_TYPE_STR, "telnet_enable", "Y");
        }
        else if(strcasecmp(set_services_args.telnet->sval[0],"Telnet and Serial") == 0){
            err = config_set_value(NVS_TYPE_STR, "telnet_enable", "D");
        }
        
        if(err!=ESP_OK){
            nerrors++;
            fprintf(f,"Error setting telnet service to %s. %s\n",set_services_args.telnet->sval[0], esp_err_to_name(err));
        }
        else {
            fprintf(f,"Telnet service changed to %s\n",set_services_args.telnet->sval[0]);
        }
    }

#if WITH_TASKS_INFO    
	nerrors += enable_disable(f,"stats",set_services_args.stats);
#endif
	if(!nerrors ){
		fprintf(f,"Done.\n");
	}
	fflush (f);
	cmd_send_messaging(argv[0],nerrors>0?MESSAGING_ERROR:MESSAGING_INFO,"%s", buf);
	fclose(f);
	FREE_AND_NULL(buf);
	return nerrors;
}

cJSON * configure_wifi_cb(){
	cJSON * values = cJSON_CreateObject();
	char * p=NULL;

    if ((p = config_alloc_get(NVS_TYPE_STR, "wifi_smode")) != NULL) {
        cJSON_AddStringToObject(values,"scanmode",strcasecmp(p,"a") == 0 ?"Comprehensive":"Fast");
        FREE_AND_NULL(p);
	}
    return values;
}



cJSON * set_services_cb(){
	cJSON * values = cJSON_CreateObject();
	char * p=NULL;
	if ((p = config_alloc_get(NVS_TYPE_STR, "enable_bt_sink")) != NULL) {
		cJSON_AddBoolToObject(values,"BT_Speaker",strcmp(p,"1") == 0 || strcasecmp(p,"y") == 0);
		FREE_AND_NULL(p);
	}
	if ((p = config_alloc_get(NVS_TYPE_STR, "enable_airplay")) != NULL) {
		cJSON_AddBoolToObject(values,"AirPlay",strcmp(p,"1") == 0 || strcasecmp(p,"y") == 0);
		FREE_AND_NULL(p);
	}
	if ((p = config_alloc_get(NVS_TYPE_STR, "telnet_enable")) != NULL) {
        if(strcasestr("YX",p)!=NULL){
		    cJSON_AddStringToObject(values,"telnet","Telnet Only");
        }
        else if(strcasestr("D",p)!=NULL){
            cJSON_AddStringToObject(values,"telnet","Telnet and Serial");
        }
        else {
            cJSON_AddStringToObject(values,"telnet","Disabled");
        }

		FREE_AND_NULL(p);
	}
#if WITH_TASKS_INFO        
	if((p = config_alloc_get_default(NVS_TYPE_STR, "stats", "n", 0))!=NULL){
		cJSON_AddBoolToObject(values,"stats",(*p == '1' || *p == 'Y' || *p == 'y')) ;
	}
#endif
	return values;
}

static void register_set_services(){
	set_services_args.airplay = arg_lit0(NULL, "AirPlay", "AirPlay");
	set_services_args.btspeaker = arg_lit0(NULL, "BT_Speaker", "Bluetooth Speaker");
	set_services_args.telnet= arg_str0("t", "telnet","Disabled|Telnet Only|Telnet and Serial","Telnet server. Use only for troubleshooting");
#if WITH_TASKS_INFO    
	set_services_args.stats= arg_lit0(NULL, "stats", "System Statistics. Use only for troubleshooting");
#endif    
    set_services_args.end=arg_end(2);
	const esp_console_cmd_t cmd = {
        .command = CFG_TYPE_SYST("services"),
        .help = "Services",
		.argtable = &set_services_args,
        .hint = NULL,
        .func = &do_set_services,
    };
	cmd_to_json_with_cb(&cmd,&set_services_cb);
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_set_wifi_parms(){
	wifi_parms_arg.scanmode = arg_str0(NULL, "scanmode", "Fast|Comprehensive","Sets the WiFi Scan Mode. Use Comprehensive where more than one AP has the same name on different channels. This will ensure that the AP with the strongest signal is chosen.");
	wifi_parms_arg.end=arg_end(2);
	const esp_console_cmd_t cmd = {
        .command = CFG_TYPE_SYST("wifi"),
        .help = "WiFi",
		.argtable = &wifi_parms_arg,
        .hint = NULL,
        .func = &do_configure_wifi,
    };
	cmd_to_json_with_cb(&cmd,&configure_wifi_cb);
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
/** 'light_sleep' command puts the chip into light sleep mode */

static struct {
    struct arg_int *wakeup_time;
    struct arg_int *wakeup_gpio_num;
    struct arg_int *wakeup_gpio_level;
    struct arg_end *end;
} light_sleep_args;

static int light_sleep(int argc, char **argv)
{
	int nerrors = arg_parse_msg(argc, argv,(struct arg_hdr **)&light_sleep_args);
    if (nerrors != 0) {
        return 1;
    }
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    if (light_sleep_args.wakeup_time->count) {
        uint64_t timeout = 1000ULL * light_sleep_args.wakeup_time->ival[0];
        cmd_send_messaging(argv[0],MESSAGING_INFO, "Enabling timer wakeup, timeout=%lluus", timeout);
        ESP_ERROR_CHECK( esp_sleep_enable_timer_wakeup(timeout) );
    }
    int io_count = light_sleep_args.wakeup_gpio_num->count;
    if (io_count != light_sleep_args.wakeup_gpio_level->count) {
    	cmd_send_messaging(argv[0],MESSAGING_INFO,  "Should have same number of 'io' and 'io_level' arguments");
        return 1;
    }
    for (int i = 0; i < io_count; ++i) {
        int io_num = light_sleep_args.wakeup_gpio_num->ival[i];
        int level = light_sleep_args.wakeup_gpio_level->ival[i];
        if (level != 0 && level != 1) {
        	cmd_send_messaging(argv[0],MESSAGING_ERROR, "Invalid wakeup level: %d", level);
            return 1;
        }
        cmd_send_messaging(argv[0],MESSAGING_INFO,  "Enabling wakeup on GPIO%d, wakeup on %s level",
                 io_num, level ? "HIGH" : "LOW");

        ESP_ERROR_CHECK( gpio_wakeup_enable(io_num, level ? GPIO_INTR_HIGH_LEVEL : GPIO_INTR_LOW_LEVEL) );
    }
    if (io_count > 0) {
        ESP_ERROR_CHECK( esp_sleep_enable_gpio_wakeup() );
    }
    if (CONFIG_ESP_CONSOLE_UART_NUM <= UART_NUM_1) {
    	cmd_send_messaging(argv[0],MESSAGING_INFO,  "Enabling UART wakeup (press ENTER to exit light sleep)");
        ESP_ERROR_CHECK( uart_set_wakeup_threshold(CONFIG_ESP_CONSOLE_UART_NUM, 3) );
        ESP_ERROR_CHECK( esp_sleep_enable_uart_wakeup(CONFIG_ESP_CONSOLE_UART_NUM) );
    }
    fflush(stdout);
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    esp_light_sleep_start();
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    const char *cause_str;
    switch (cause) {
    case ESP_SLEEP_WAKEUP_GPIO:
        cause_str = "GPIO";
        break;
    case ESP_SLEEP_WAKEUP_UART:
        cause_str = "UART";
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        cause_str = "timer";
        break;
    default:
        cause_str = "unknown";
        printf("%d\n", cause);
    }
    cmd_send_messaging(argv[0],MESSAGING_INFO, "Woke up from: %s", cause_str);
    return 0;
}

static void register_light_sleep()
{
    light_sleep_args.wakeup_time =
        arg_int0("t", "time", "<t>", "Wake up time, ms");
    light_sleep_args.wakeup_gpio_num =
        arg_intn(NULL, "io", "<n>", 0, 8,
                 "If specified, wakeup using GPIO with given number");
    light_sleep_args.wakeup_gpio_level =
        arg_intn(NULL, "io_level", "<0|1>", 0, 8, "GPIO level to trigger wakeup");
    light_sleep_args.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "light_sleep",
        .help = "Enter light sleep mode. "
        "Two wakeup modes are supported: timer and GPIO. "
        "Multiple GPIO pins can be specified using pairs of "
        "'io' and 'io_level' arguments. "
        "Will also wake up on UART input.",
        .hint = NULL,
        .func = &light_sleep,
        .argtable = &light_sleep_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

