#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

#include "wifi.h"

#define FIRMWARE_ACTUAL	0.2
#define URL_JSON		"https://raw.githubusercontent.com/javicano8tfg/ota/master/update_info.json"
#define LED 			GPIO_NUM_32
#define SENSOR			GPIO_NUM_14
#define BLINK			1
#define DELAY_NEW_FIRMWARE	60 	// segundos que tarda en comprobar si hay un nuevo firmware


// Certificado .pem del servidor donde está alojado el binario, obtenido con:
// openssl s_client -showcerts -connect github.com:443
extern const char github_cert_pem_start[] asm("_binary_github_cert_pem_start");
extern const char github_cert_pem_end[] asm("_binary_github_cert_pem_end");

char dato_json[500];
xTaskHandle xAlarm_task;

esp_err_t _http_event_handler(esp_http_client_event_t *evento) {

	switch(evento->event_id) {
        	case HTTP_EVENT_ON_DATA:
		    if (!esp_http_client_is_chunked_response(evento->client)) {
		    	   strncpy(dato_json, (char*)evento->data, evento->data_len);
		    }
		    break;
		default:
		    break;
    }
    return ESP_OK;
}



// Tarea con la funcionalidad de la alarma
void alarm_task(void *pvParameter){
	bool on = true;
  while(1) {
	if(gpio_get_level(SENSOR)==1){	      
		printf("Movimiento detectado\n");
		if(BLINK){
	    		if(on){
				gpio_set_level(LED, 1);
				on = false;
			}	
			else{
				gpio_set_level(LED, 0);
				on = true;
			}
	  	}
	  	else{
			gpio_set_level(LED, 1);
		}
      }
      else{
          gpio_set_level(LED, 0);
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


// Tarea que comprueba si hay actualización disponible cada DELAY_NEW_FIRMWARE segundos
void check_firmware_task(void *pvParameter) {

	while(1) {

		printf("Buscando una nueva versión de software...\n");

		esp_http_client_config_t http_config = {
			.url = URL_JSON,
			.event_handler = _http_event_handler,
		};

		esp_http_client_handle_t http_client = esp_http_client_init(&http_config);
		esp_http_client_perform(http_client);

		cJSON *json = cJSON_Parse(dato_json);

		cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");
		cJSON *archivo = cJSON_GetObjectItemCaseSensitive(json, "archivo");
		cJSON *fecha = cJSON_GetObjectItemCaseSensitive(json, "fecha");
		cJSON *autor = cJSON_GetObjectItemCaseSensitive(json, "autor");

		if( version == NULL || archivo == NULL || fecha == NULL || autor == NULL){
			printf("Faltan parámetros en el archivo de configuración JSON \n");
			printf("Comprueba %s\n", URL_JSON );
		}
		else{
			double firmware_servidor = version->valuedouble;

			if(firmware_servidor < FIRMWARE_ACTUAL){printf("Tu versión de firmware actual (%.1f) es superior a la que se encuentra en el servidor (%.1f) \n", FIRMWARE_ACTUAL, firmware_servidor);}
			else if (firmware_servidor == FIRMWARE_ACTUAL){ printf("Ya dispones de la última versión de firmware disponible (%.1f) \n", FIRMWARE_ACTUAL);}
			else {

				printf("Hay una versión nueva disponible! Actualizando a la v%.1f\n", firmware_servidor);
				printf("	Desarrollador: %s\n", autor->valuestring );
				printf("	Fecha: %s\n", fecha->valuestring );

				vTaskDelete(xAlarm_task);

				char *urlArchivo = archivo->valuestring;
				printf("Descargando %s \n", urlArchivo);

				esp_http_client_config_t ota_client_config = {
					.url = urlArchivo,
					.cert_pem = github_cert_pem_start,
				};

				if (esp_https_ota(&ota_client_config) == ESP_OK) {
					printf("Reiniciando en 3,");
					vTaskDelay((1000) / portTICK_PERIOD_MS);
					printf(" 2,");
					vTaskDelay((1000) / portTICK_PERIOD_MS);
					printf(" 1");
					vTaskDelay((1000) / portTICK_PERIOD_MS);
					printf(".");
					vTaskDelay((333) / portTICK_PERIOD_MS);
					printf(".");
					vTaskDelay((333) / portTICK_PERIOD_MS);
					printf(".\n");
					vTaskDelay((333) / portTICK_PERIOD_MS);
					
					esp_restart();
				}
			}
		}

		esp_http_client_cleanup(http_client);
        	vTaskDelay((DELAY_NEW_FIRMWARE * 1000) / portTICK_PERIOD_MS);
    }
}

void app_main() {
	
	printf("VERSIÓN ACTUAL:  %.1f\n", FIRMWARE_ACTUAL );

	gpio_set_direction(LED, GPIO_MODE_OUTPUT);

	// Inicializa wifi
	bool flag = false;
	while(!flag){
		flag = start_wifi();
	}
	
	wait_wifi();
	printf("WIFI conectado \n");

	// Lanza tarea de alarma
	xTaskCreate(&alarm_task, "alarm_task", 2048, NULL, 5, &xAlarm_task);

	// Lanza la tarea que comprueba si hay actualización disponible
	xTaskCreate(&check_firmware_task, "check_firmware_task", 8192, NULL, 5, NULL);
}


