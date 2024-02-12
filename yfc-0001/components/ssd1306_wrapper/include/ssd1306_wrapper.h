#ifndef SSD1306_WRAPPER_H
#define SSD1306_WRAPPER_H

#include "ssd1306.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ssd1306_wrapper_init(i2c_port_t i2c_port, gpio_num_t scl_pin, gpio_num_t sda_pin, gpio_num_t rst_pin, uint32_t clock_speed);
esp_err_t ssd1306_wrapper_init_default(void);
void ssd1306_wrapper_clear(void);
esp_err_t ssd1306_wrapper_write(const char* message, uint8_t Xpos, uint8_t Ypos, uint8_t size, uint8_t mode);
void ssd1306_wrapper_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // SSD1306_WRAPPER_H
