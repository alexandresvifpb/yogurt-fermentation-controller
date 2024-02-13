#ifndef DS18B20_WRAPPER_H
#define DS18B20_WRAPPER_H

/**
 * @file ds18b20_wrapper.h
 * @brief Header file for the DS18B20 temperature sensor wrapper component.
 */

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the DS18B20 temperature sensor.
 * 
 * This function initializes the DS18B20 temperature sensor by specifying the GPIO pin to which it is connected.
 * 
 * @param pin The GPIO pin number to which the DS18B20 sensor is connected.
 * @return esp_err_t Returns ESP_OK if the initialization is successful, otherwise an error code.
 */
esp_err_t ds18b20_wrapper_init(int pin);

/**
 * @brief Trigger a temperature reading from the DS18B20 sensor.
 * 
 * This function triggers a temperature reading from the DS18B20 sensor. The result of the reading can be obtained by calling ds18b20_wrapper_get_temperature.
 * 
 * @return esp_err_t Returns ESP_OK if the reading trigger is successful, otherwise an error code.
 */
esp_err_t ds18b20_wrapper_read_temperature(void);

/**
 * @brief Get the most recent temperature reading.
 * 
 * This function retrieves the most recent temperature reading obtained by ds18b20_wrapper_read_temperature.
 * 
 * @param temperature Pointer to a float variable where the temperature will be stored.
 * @return esp_err_t Returns ESP_OK if the temperature is successfully retrieved, otherwise an error code.
 */
esp_err_t ds18b20_wrapper_get_temperature(float* temperature);

/**
 * @brief Deinitialize the DS18B20 sensor.
 * 
 * This function deinitializes the DS18B20 sensor and frees any resources allocated by ds18b20_wrapper_init.
 */
void ds18b20_wrapper_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* DS18B20_WRAPPER_H */