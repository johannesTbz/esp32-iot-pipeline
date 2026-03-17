#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "LDR";

void app_main(void)
{
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

        //light level i procent
        int percent = 100 - ((raw * 100) / 4095);
        ESP_LOGI(TAG, "Light level: %d (%d%%)", raw, percent);
        
        //dag natt detection
        if (raw > 2500)   
        {
            ESP_LOGI(TAG, "Night detected");
        } 
        else 
        {
            ESP_LOGI(TAG, "Day detected");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}