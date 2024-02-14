#pragma once

#ifndef DS18B20_WRAPPER_H
#define DS18B20_WRAPPER_H

#include "esp_err.h"
// #include "esp_idf_version.h"
// #include "driver/gpio.h"
#include "ds18b20.h" // Certifique-se de incluir o cabeçalho correto para a biblioteca DS18B20

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ds18b20_wrapper.h
 * @brief Header file for the DS18B20 temperature sensor wrapper component.
 * @author Alexandre Sales Vasconcelos
 * @email alexandre.vasconcelos@ifpb.edu.br
 * @version 0.1
 * @date 2024-02-15
 *
 * @copyright Copyright (c) 2024
*/

#define DS18B20_WRAPPER_VERSION "1.0.0"

/**
 * @brief Estrutura para representar um sensor DS18B20.
 */
typedef struct {
    ds18b20_device_handle_t device_handle; /*!< Handle do dispositivo DS18B20 */
    float last_temperature_read;           /*!<  Última leitura de temperatura */
    int pin;          /*!< GPIO pin to which the DS18B20 sensor is connected. */
    uint64_t address;        /*!< Endereço único do sensor DS18B20. */
} ds18b20_sensor_t;

/**
 * @brief Inicializa múltiplos sensores DS18B20 conectados ao mesmo GPIO.
 * 
 * Esta função busca por sensores DS18B20 conectados ao pino especificado,
 * inicializando cada um deles. Assume-se que `sensors` é um vetor pré-alocado
 * com espaço suficiente para todos os sensores encontrados até `max_sensors`.
 * 
 * @param sensors Vetor de estruturas ds18b20_sensor_t a serem inicializadas.
 * @param max_sensors Número máximo de sensores que o vetor `sensors` pode armazenar.
 * @param pin O número do pino GPIO ao qual os sensores estão conectados.
 * @param found_sensors Referência para a variável onde o número de sensores encontrados será armazenado.
 * @return esp_err_t ESP_OK em caso de sucesso, ou um código de erro em caso de falha.
 */
esp_err_t ds18b20_wrapper_init(ds18b20_sensor_t *sensors, int max_sensors, int pin, int *found_sensors);

/**
 * @brief Lê a temperatura do sensor DS18B20.
 * 
 * @param sensor Referência para a estrutura do sensor ds18b20_sensor_t.
 * @return esp_err_t ESP_OK em caso de sucesso, ou um código de erro em caso de falha.
 */
esp_err_t ds18b20_wrapper_read_temperature(ds18b20_sensor_t *sensors, int sensor_count);

/**
 * @brief Obtém a última temperatura lida do sensor DS18B20.
 * 
 * @param sensor Referência para a estrutura do sensor ds18b20_sensor_t.
 * @param temperature Referência para a variável onde a temperatura será armazenada.
 * @return esp_err_t ESP_OK em caso de sucesso, ou um código de erro em caso de falha.
 */
esp_err_t ds18b20_wrapper_get_temperature(ds18b20_sensor_t *sensor, float *temperature);

/**
 * @brief Desinicializa o sensor DS18B20, liberando quaisquer recursos alocados.
 * 
 * @param sensor Referência para a estrutura do sensor ds18b20_sensor_t.
 */
void ds18b20_wrapper_deinit(ds18b20_sensor_t *sensor);

#ifdef __cplusplus
}
#endif

#endif // DS18B20_WRAPPER_Hvoid func(void);
