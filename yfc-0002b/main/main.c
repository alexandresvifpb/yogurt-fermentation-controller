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

#define CONFIG_SENSOR_TEMP_DS18B20_PIN 13
#define MAX_SENSORS 10 // Defina o número máximo de sensores que você espera encontrar

static const char* TAG = "main";
static ds18b20_config_t sensors[MAX_SENSORS]; // Vetor de sensores DS18B20
static float temperature = 0.0f;
static int found_sensors = 0;

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
    char temperature_sensor_initialized = false;
    if (ds18b20_wrapper_init(sensors, MAX_SENSORS, CONFIG_SENSOR_TEMP_DS18B20_PIN, &found_sensors) == ESP_OK)
    {
        ESP_LOGI(TAG, "DS18B20 sensor initialized");
        temperature_sensor_initialized = true;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to initialize DS18B20 sensor");
    }
    while (1)
    {
        if (temperature_sensor_initialized)
        {
            if (ds18b20_wrapper_read_temperature(sensors, found_sensors) == ESP_OK)
            {
                if (xSemaphoreTake(temperature_mutex, portMAX_DELAY) == pdTRUE)
                {
                    for (int i = 0; i < found_sensors; i++)
                    {
                        if (ds18b20_wrapper_get_temperature(&sensors[i], &temperature) == ESP_OK)
                        {
                            ESP_LOGI(TAG, "Temperature sensor [%i]: %.2f°C", i, temperature);
                        }
                    }
                    xSemaphoreGive(temperature_mutex);
                }
            }
            else
            {
                ESP_LOGE(TAG, "Failed to read temperature from DS18B20 sensor");
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else if (ds18b20_wrapper_init(sensors, MAX_SENSORS, CONFIG_SENSOR_TEMP_DS18B20_PIN, &found_sensors) == ESP_OK)
        {
            ESP_LOGI(TAG, "DS18B20 sensor initialized");
            temperature_sensor_initialized = true;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize DS18B20 sensor");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        // Após o loop ou em pontos estratégicos dentro do loop, verifique o stack high water mark
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        ESP_LOGW(TAG, "Menor tamanho de stack livre registrado: %u words", uxHighWaterMark);

        // Para converter em bytes e imprimir o valor, considerando 4 bytes por palavra
        ESP_LOGW(TAG, "Menor tamanho de stack livre registrado: %u bytes", uxHighWaterMark * sizeof(portSTACK_TYPE));
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