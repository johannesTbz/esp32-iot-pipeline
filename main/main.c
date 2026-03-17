#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

// WiFi headers
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"

//mqtt headers
#include "mqtt_client.h"

static const char *TAG = "LDR";

// WiFi config
#define WIFI_SSID "YOUR_WIFI"
#define WIFI_PASS "YOUR_PASSWORD"

//mqtt config
#define MQTT_BROKER_URI "mqtt://192.168.1.11:1883"

// Function declarations
static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
void wifi_init_sta(void);
//global mqtt client
static esp_mqtt_client_handle_t mqtt_client = NULL;
void mqtt_app_start(void);


static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("WIFI", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        NULL,
        &instance_any_id
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip
    ));

    wifi_config_t wifi_config = 
    {
        .sta = 
        {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WIFI", "wifi_init_sta finished");
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = 
    {
        .broker.address.uri = MQTT_BROKER_URI,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_client));

    ESP_LOGI("MQTT", "MQTT client started");
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();
    mqtt_app_start();

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = 
    {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = 
    {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config));

    while (1) 
    {
        int raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &raw));

        int percent = 100 - ((raw * 100) / 4095);
        const char *day_status = (raw > 2500) ? "night" : "day";

        ESP_LOGI(TAG, "Light level: %d (%d%%)", raw, percent);
        ESP_LOGI(TAG, "%s detected", day_status);

        char msg[96];
        snprintf(msg, sizeof(msg),
                "{\"raw\":%d,\"percent\":%d,\"status\":\"%s\"}",
                raw, percent, day_status);

        if (mqtt_client != NULL) 
        {
            esp_mqtt_client_publish(mqtt_client, "light-sensor/data", msg, 0, 1, 0);
            esp_mqtt_client_publish(mqtt_client, "light-sensor/status", day_status, 0, 1, 0);

            ESP_LOGI("MQTT", "Published: %s", msg);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}