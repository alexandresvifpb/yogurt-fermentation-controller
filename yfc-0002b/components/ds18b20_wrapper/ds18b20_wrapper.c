/**
 * @file sensor_temp_ds18b20.c
 * @brief Implementation of the DS18B20 temperature sensor module.
 * @author Alexandre Sales Vasconcelos
 * @email alexandre.vasconcelos@ifpb.edu.br
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds18b20_wrapper.h"
#include "esp_log.h"
#include "esp_err.h"

#include "ds18b20.h"
#include "onewire_bus.h"

// foi preciso colocar esse ifndef para que o compilador nao reclamasse
#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 1000
#endif

static const char* TAG = "ds18b20_wrapper";
static ds18b20_device_handle_t sensor = NULL;
static float last_temperature_read = 0.0f; // Variable to store the last temperature read

/**
 * @brief Implementation of the DS18B20 initialization function.
 *
 * This function initializes DS18B20 sensors connected to the specified GPIO pin.
 * It searches for up to `max_sensors` sensors and initializes them.
 *
 * @param sensors Array to store initialized sensor handles.
 * @param max_sensors The maximum number of sensors to search for.
 * @param pin GPIO pin number where the sensors are connected.
 * @param found_sensors Pointer to store the number of sensors found and initialized.
 * @return esp_err_t Returns ESP_OK on success or an error code on failure.
 */
esp_err_t ds18b20_wrapper_init(ds18b20_sensor_t *sensors, int max_sensors, int pin, int *found_sensors)
{
    if (!sensors || max_sensors <= 0)
    {
        ESP_LOGE(TAG, "Invalid parameters");
        return ESP_ERR_INVALID_ARG;
    }

    // Install 1-wire bus
    onewire_bus_handle_t bus = NULL;
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = pin,
    };
    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10, // 1byte ROM command + 8byte ROM number + 1byte device command
    };
    ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus));

    // Create 1-wire device iterator for device search
    onewire_device_iter_handle_t iter = NULL;
    ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));

    onewire_device_t next_onewire_device;
    esp_err_t search_result = ESP_OK;
    int sensors_found = 0;

    // Start searching for DS18B20 devices
    ESP_LOGI(TAG, "Starting search for DS18B20 sensors...");
    while (sensors_found < max_sensors && (search_result = onewire_device_iter_get_next(iter, &next_onewire_device)) == ESP_OK)
    {
        ds18b20_config_t ds_cfg = {};

        if (ds18b20_new_device(&next_onewire_device, &ds_cfg, &sensors[sensors_found].device_handle) == ESP_OK)
        {
            sensors[sensors_found].address = next_onewire_device.address;
            sensors_found++;
            ESP_LOGI(TAG, "Found DS18B20 sensor with address: %016llX", next_onewire_device.address);
        }
    }

    // Check if any sensors were found
    if (sensors_found == 0)
    {
        ESP_LOGE(TAG, "No DS18B20 sensors found. Please check your connections.");
        onewire_del_device_iter(iter);
        return ESP_FAIL;
    }

    *found_sensors = sensors_found;
    ESP_LOGI(TAG, "Total DS18B20 sensors found: %d", *found_sensors);

    // Clean up
    ESP_ERROR_CHECK(onewire_del_device_iter(iter));

    return ESP_OK;
}

/**
 * @brief Reads the temperature from all DS18B20 sensors.
 *
 * Iterates over all initialized DS18B20 sensors and triggers a temperature
 * conversion. It stores the temperature readings internally within each sensor's
 * structure for later retrieval.
 *
 * @param sensors Array of initialized DS18B20 sensor structures.
 * @param sensor_count Number of sensors in the array.
 * @return esp_err_t ESP_OK on success; error code on failure.
 */
esp_err_t ds18b20_wrapper_read_temperature(ds18b20_sensor_t *sensors, int sensor_count) {
    if (!sensors || sensor_count <= 0) {
        ESP_LOGE(TAG, "Invalid parameters for reading temperature");
        return ESP_ERR_INVALID_ARG;
    }

    for (int i = 0; i < sensor_count; i++) {
        if (sensors[i].device_handle != NULL) {
            // Trigger temperature conversion
            ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(sensors[i].device_handle));
            vTaskDelay(750 / portTICK_PERIOD_MS); // Maximum conversion time for DS18B20

            // Read the temperature
            float temperature;
            ESP_ERROR_CHECK(ds18b20_get_temperature(sensors[i].device_handle, &temperature));
            sensors[i].last_temperature_read = temperature;
            ESP_LOGI(TAG, "Sensor %d - Temperature: %.2f°C", i, temperature);
        } else {
            ESP_LOGE(TAG, "Sensor %d not initialized", i);
            return ESP_ERR_INVALID_STATE;
        }
    }
    return ESP_OK;
}

/**
 * @brief Retrieves the last temperature reading from a specific DS18B20 sensor.
 *
 * This function returns the most recent temperature reading obtained from the
 * specified DS18B20 sensor. Ensure `ds18b20_wrapper_read_temperature` is called
 * beforehand to update temperature readings.
 *
 * @param sensor Pointer to the DS18B20 sensor structure.
 * @param temperature Pointer to a float variable where the temperature will be stored.
 * @return esp_err_t ESP_OK on success; error code on failure.
 */
esp_err_t ds18b20_wrapper_get_temperature(ds18b20_sensor_t *sensor, float *temperature) {
    if (sensor == NULL || temperature == NULL) {
        ESP_LOGE(TAG, "Invalid argument(s)");
        return ESP_ERR_INVALID_ARG;
    }

    // Verifica se o handle do dispositivo é válido
    if (sensor->device_handle == NULL) {
        ESP_LOGE(TAG, "Sensor not initialized");
        return ESP_FAIL;
    }

    // Retorna a última temperatura lida armazenada na estrutura do sensor
    *temperature = sensor->last_temperature_read;
    
    ESP_LOGI(TAG, "Temperature read: %.2f°C", *temperature);
    return ESP_OK;
}

/**
 * @brief Deinitializes a DS18B20 sensor and frees associated resources.
 *
 * Cleans up any resources allocated for the DS18B20 sensor and deinitializes it.
 * This function should be called when the sensor is no longer needed.
 *
 * @param sensor Pointer to the DS18B20 sensor structure to deinitialize.
 */
void ds18b20_wrapper_deinit(ds18b20_sensor_t *sensor) {
    if (sensor->device_handle != NULL) {
        ds18b20_del_device(sensor->device_handle);
        sensor->device_handle = NULL;
    } else {
        ESP_LOGE(TAG, "Sensor not initialized");
    }
}