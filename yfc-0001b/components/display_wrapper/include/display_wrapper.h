#pragma once

#ifndef DISPLAY_WRAPPER_H
#define DISPLAY_WRAPPER_H

#include <esp_err.h>
#include "ssd1306.h"

#ifdef __cplusplus
extern "C" 
{
#endif

    typedef struct
    {
        i2c_port_t i2c_port;
        uint8_t i2c_address;
        uint32_t clk_speed;
        gpio_num_t sda_pin;
        gpio_num_t scl_pin;
        gpio_num_t reset_pin;
        ssd1306_handle_t ssd1306_dev;
    } display_t;

    esp_err_t display_wrapper_init(display_t *display, i2c_port_t i2c_port, gpio_num_t scl_pin, gpio_num_t sda_pin, gpio_num_t rst_pin, uint32_t clock_speed);

    esp_err_t display_wrapper_init_default(display_t *display);

    esp_err_t display_wrapper_clear(display_t *display);

    esp_err_t display_wrapper_write(display_t *display, const char* message, uint8_t Xpos, uint8_t Ypos, uint8_t size, uint8_t mode);

    esp_err_t display_wrapper_deinit(display_t *display);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_WRAPPER_H