/**
 * @file display_wrapper.h
 * @brief Wrapper functions for SSD1306 display using ESP-IDF's I2C driver.
 *
 * This header file provides an abstraction layer for working with SSD1306 OLED displays.
 * It encapsulates initialization, writing messages, clearing the display, and deinitialization
 * processes, making it easier to work with SSD1306 displays in ESP-IDF projects.
 * @author alexandre sales vasconcelos (alexandre.vasconcelos@ifpb.edu.br)
 * @version 0.1
 * @date 2024-02-15
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#ifndef DISPLAY_WRAPPER_H
#define DISPLAY_WRAPPER_H

#include <esp_err.h>
#include "ssd1306.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct display_t
 * @brief Structure to hold SSD1306 display configuration.
 *
 * This structure contains information required to initialize and interact with an SSD1306 OLED display,
 * including the I2C port and address, clock speed, GPIO pins for SDA and SCL lines, reset pin, and a
 * handle to the SSD1306 device.
 */
typedef struct
{
    i2c_port_t i2c_port;       /*!< I2C port number */
    uint8_t i2c_address;       /*!< I2C address of the SSD1306 display */
    uint32_t clk_speed;        /*!< Clock speed for the I2C communication */
    gpio_num_t sda_pin;        /*!< GPIO number for I2C SDA line */
    gpio_num_t scl_pin;        /*!< GPIO number for I2C SCL line */
    gpio_num_t reset_pin;      /*!< GPIO number for the display reset line */
    ssd1306_handle_t ssd1306_dev; /*!< Handle to the SSD1306 device */
} display_t;

/**
 * @brief Initializes the SSD1306 display with custom configuration.
 *
 * @param display Pointer to the display_t structure to be initialized.
 * @param i2c_port I2C port number to be used for communication.
 * @param scl_pin GPIO number for I2C SCL line.
 * @param sda_pin GPIO number for I2C SDA line.
 * @param rst_pin GPIO number for the display reset line.
 * @param clock_speed Clock speed for the I2C communication.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_init(display_t *display, i2c_port_t i2c_port, gpio_num_t scl_pin, gpio_num_t sda_pin, gpio_num_t rst_pin, uint32_t clock_speed);

/**
 * @brief Initializes the SSD1306 display with default configuration.
 *
 * @param display Pointer to the display_t structure to be initialized.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_init_default(display_t *display);

/**
 * @brief Clears the SSD1306 display.
 *
 * @param display Pointer to the initialized display_t structure.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_clear(display_t *display);

/**
 * @brief Writes a message to the SSD1306 display.
 *
 * @param display Pointer to the initialized display_t structure.
 * @param message Null-terminated string to be displayed.
 * @param Xpos X position on the display where the message starts.
 * @param Ypos Y position on the display where the message starts.
 * @param size Font size to be used for the message.
 * @param mode Display mode (e.g., normal or inverse).
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_write(display_t *display, const char* message, uint8_t Xpos, uint8_t Ypos, uint8_t size, uint8_t mode);

/**
 * @brief Deinitializes the SSD1306 display.
 *
 * @param display Pointer to the initialized display_t structure.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_deinit(display_t *display);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_WRAPPER_H
