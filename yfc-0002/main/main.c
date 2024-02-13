/**
 * @file main.c
 * @brief This file contains the main application code for reading temperature using DS18B20 sensor.
 * @author Alexandre Sales Vasconcelos
 * @email alexandre.vasconcelos@ifpb.edu.br
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "ds18b20_wrapper.h"

// foi preciso colocar esse ifndef para que o compilador nao reclamasse
#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 1000
#endif

#define TAG "main"
#define CONFIG_SENSOR_TEMP_DS18B20_PIN 13

static float temperature = 0.0f;

// Semaforo para proteger a variável temperature
static SemaphoreHandle_t temperature_mutex;

/**
 * @brief Task to read temperature from DS18B20 sensor periodically.
 *
 * This task initializes the DS18B20 sensor and reads the temperature value
 * at regular intervals. The temperature value is then logged using ESP_LOGI.
 * The task delays for 1 second between each temperature reading.
 *
 * @param pvParameters Pointer to task parameters (not used in this task).
 */
void temperature_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Initializing DS18B20 sensor");
    // ds18b20_wrapper_init(CONFIG_SENSOR_TEMP_DS18B20_PIN);
    char temperature_sensor = 0;
    if (ds18b20_wrapper_init(CONFIG_SENSOR_TEMP_DS18B20_PIN) == ESP_OK)
    {
        ESP_LOGI(TAG, "DS18B20 sensor initialized");
        temperature_sensor = 1;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to initialize DS18B20 sensor");
    }
    while (1)
    {
        // ds18b20_wrapper_read_temperature(&temperature);
        if (temperature_sensor)
        {
            if (ds18b20_wrapper_read_temperature() == ESP_OK)
            {
                if (xSemaphoreTake(temperature_mutex, portMAX_DELAY) == pdTRUE)
                {
                    ds18b20_wrapper_get_temperature(&temperature);
                    ESP_LOGI(TAG, "Temperature: %.2f°C", temperature);
                    xSemaphoreGive(temperature_mutex);
                }
            }
            else
            {
                ESP_LOGE(TAG, "Failed to read temperature from DS18B20 sensor");
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else if (ds18b20_wrapper_init(CONFIG_SENSOR_TEMP_DS18B20_PIN) == ESP_OK)
        {
            ESP_LOGI(TAG, "DS18B20 sensor initialized");
            temperature_sensor = 1;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize DS18B20 sensor");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        // Após o loop ou em pontos estratégicos dentro do loop, verifique o stack high water mark
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        ESP_LOGI(TAG, "Menor tamanho de stack livre registrado: %u words", uxHighWaterMark);

        // Para converter em bytes e imprimir o valor, considerando 4 bytes por palavra
        ESP_LOGI(TAG, "Menor tamanho de stack livre registrado: %u bytes", uxHighWaterMark * sizeof(portSTACK_TYPE));
    }
}

/**
 * @brief The entry point of the application.
 *
 * This function is called by the system when the application starts.
 * It creates a task to read temperature from DS18B20 sensor.
 */
void app_main(void)
{
    temperature_mutex = xSemaphoreCreateMutex();
    if (temperature_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create mutex");
        return; // Não continue se não conseguir criar o mutex
    }
    xTaskCreate(temperature_task, "temperature_task", 2300, NULL, 5, NULL);
}