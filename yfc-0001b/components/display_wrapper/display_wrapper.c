/**
 * @file display_wrapper.c
 * @brief Implementation of wrapper functions for SSD1306 display using ESP-IDF's I2C driver.
 *
 * Provides implementation for initialization, clearing the display, writing messages, and deinitialization
 * of the SSD1306 OLED displays, abstracting the underlying SSD1306 and I2C driver details.
 * @author alexandre sales vasconcelos (alexandre.vasconcelos@ifpb.edu.br)
 * @version 0.1
 * @date 2024-02-15
 *
 * @copyright Copyright (c) 2024
 */

#include <esp_err.h>
#include "display_wrapper.h"

// foi preciso colocar esse ifndef para que o compilador nao reclamasse
#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define DEFAULT_I2C_MASTER_SCL_IO 15        /*!< gpio number for I2C master clock */
#define DEFAULT_I2C_MASTER_SDA_IO 4        /*!< gpio number for I2C master data  */
#define DEFAULT_RST_PIN 16
#define DEFAULT_I2C_MASTER_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define DEFAULT_I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */

static ssd1306_handle_t ssd1306_dev = NULL;
static const char* TAG = "display_wrapper";

/**
 * @brief Initializes the SSD1306 display with custom configuration.
 *
 * Detailed description of how the display is initialized with custom parameters including
 * setting up the I2C driver, configuring the GPIO pins, and initializing the SSD1306 library.
 *
 * @param display Pointer to the display_t structure to be initialized.
 * @param i2c_port I2C port number to be used for communication.
 * @param scl_pin GPIO number for I2C SCL line.
 * @param sda_pin GPIO number for I2C SDA line.
 * @param rst_pin GPIO number for the display reset line.
 * @param clock_speed Clock speed for the I2C communication.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_init(display_t *display, i2c_port_t i2c_port, gpio_num_t scl_pin, gpio_num_t sda_pin, gpio_num_t rst_pin, uint32_t clock_speed) 
{
    // Configurar as GPIOs para o sinais de clock (SCL) e dados (SDA) do I2C e sinal de reset (RST) do display
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << scl_pin) | (1ULL << sda_pin) | (1ULL << rst_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // Configurar o controlador I2C
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = scl_pin,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = clock_speed,
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
    };
    i2c_param_config(i2c_port, &i2c_conf);

    // Instalar o driver I2C
    i2c_driver_install(i2c_port, i2c_conf.mode, 0, 0, 0);

    // Resetar o display
    gpio_set_level(rst_pin, 1); // RST pin is active low
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(rst_pin, 0);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    gpio_set_level(rst_pin, 1);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // Inicializar o display
    ssd1306_dev = ssd1306_create(i2c_port, SSD1306_I2C_ADDRESS);
    if (ssd1306_dev == NULL) { // Verificar se a inicialização foi bem sucedida
        return ESP_FAIL;
    }
    ssd1306_refresh_gram(ssd1306_dev); // Atualizar a memória de vídeo
    ssd1306_clear_screen(ssd1306_dev, 0x00); // Limpar a tela

    display->i2c_port = i2c_port;
    display->i2c_address = SSD1306_I2C_ADDRESS;
    display->clk_speed = clock_speed;
    display->sda_pin = sda_pin;
    display->scl_pin = scl_pin;
    display->reset_pin = rst_pin;
    display->ssd1306_dev = ssd1306_dev;
    
    return ESP_OK;
}

/**
 * @brief Initializes the SSD1306 display with default configuration.
 *
 * Initializes the display using predefined default settings for the I2C port, GPIO pins,
 * and clock speed. Useful for standard setups and quick starts.
 *
 * @param display Pointer to the display_t structure to be initialized.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t ssd1306_wrapper_init_default(display_t *display) 
{
    if (!display) {
        // Verifica se o ponteiro para display foi fornecido
        return ESP_ERR_INVALID_ARG;
    }

    // Inicializa o display com os valores padrão
    return display_wrapper_init(display, DEFAULT_I2C_MASTER_NUM, DEFAULT_I2C_MASTER_SCL_IO, DEFAULT_I2C_MASTER_SDA_IO, DEFAULT_RST_PIN, DEFAULT_I2C_MASTER_FREQ_HZ);
}

/**
 * @brief Clears the SSD1306 display.
 *
 * Sends a command to clear all pixels on the display, effectively blanking the screen.
 *
 * @param display Pointer to the initialized display_t structure.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_clear(display_t *display) 
{
    ssd1306_refresh_gram(display->ssd1306_dev);  // Atualizar a memória de vídeo
    ssd1306_clear_screen(display->ssd1306_dev, 0x00); // Limpar a tela
    return ESP_OK;
}

/**
 * @brief Writes a message to the SSD1306 display.
 *
 * Writes the specified message at the given position on the display using the specified
 * font size and display mode. Supports positioning and basic text formatting.
 *
 * @param display Pointer to the initialized display_t structure.
 * @param message Null-terminated string to be displayed.
 * @param Xpos X position on the display where the message starts.
 * @param Ypos Y position on the display where the message starts.
 * @param size Font size to be used for the message.
 * @param mode Display mode (e.g., normal or inverse).
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_write(display_t *display, const char* message, uint8_t Xpos, uint8_t Ypos, uint8_t size, uint8_t mode) 
{

    // size_t message_size = strlen(message);  // Tamanho da mensagem
    // if (message_size == 0) { // Verificar se a mensagem está vazia
    //     return ESP_OK;
    // }

    // Escrever a mensagem no display
    ssd1306_draw_string(display->ssd1306_dev, Xpos, Ypos, (const uint8_t *)message, size, mode);
    ssd1306_refresh_gram(display->ssd1306_dev); // Atualizar a memória de vídeo    

    return ESP_OK;
}

/**
 * @brief Deinitializes the SSD1306 display.
 *
 * Frees up resources used by the display and the I2C driver, and resets the display hardware.
 * It is recommended to call this function before the application terminates.
 *
 * @param display Pointer to the initialized display_t structure.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t display_wrapper_deinit(display_t *display) 
{
    ssd1306_delete(display->ssd1306_dev);
    return ESP_OK;
}