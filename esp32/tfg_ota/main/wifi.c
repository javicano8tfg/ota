#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include <string.h>

#include "wifi.h"


static EventGroupHandle_t wifi_event_group;

static esp_err_t event_handler(void *ctx, system_event_t *evt){

    switch(evt->event_id) {

    	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;

	case SYSTEM_EVENT_STA_GOT_IP:
		xEventGroupSetBits(wifi_event_group, BIT0);
		break;

	case SYSTEM_EVENT_STA_DISCONNECTED:
		esp_wifi_connect();
        	break;

	default: break;
    }

	return ESP_OK;
}


bool start_wifi(void) {

	  nvs_flash_init(); // necesario 
	  wifi_event_group = xEventGroupCreate();
	  tcpip_adapter_init();
	  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	  wifi_init_config_t wifi_init_config_default = WIFI_INIT_CONFIG_DEFAULT();
	  esp_wifi_init(&wifi_init_config_default);

	  esp_wifi_set_storage(WIFI_STORAGE_FLASH);
	  wifi_config_t wifi_config = {};
	  ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config));

	  if (wifi_config.sta.ssid[0] == '\0' || wifi_config.sta.password[0] == '\0'){
	  	wifi_sta_config_t wifi_sta_config = {
		       .ssid = WIFI_SSID,
		       .password = WIFI_PASS,
		};

		wifi_config.sta = wifi_sta_config;

	  }

	  esp_wifi_set_mode(WIFI_MODE_STA);

	  esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
	  esp_wifi_start();

  return true;
}

void wait_wifi()
{
	xEventGroupWaitBits(wifi_event_group, BIT0, false, true, portMAX_DELAY);
}

