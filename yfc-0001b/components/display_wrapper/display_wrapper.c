#include <stdio.h>
#include <esp_err.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

// Inicializa o display SSD1306
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

// Inicializa o display SSD1306 com os pinos e a velocidade de clock default
esp_err_t ssd1306_wrapper_init_default(display_t *display) 
{
    if (!display) {
        // Verifica se o ponteiro para display foi fornecido
        return ESP_ERR_INVALID_ARG;
    }

    // Inicializa o display com os valores padrão
    return display_wrapper_init(display, DEFAULT_I2C_MASTER_NUM, DEFAULT_I2C_MASTER_SCL_IO, DEFAULT_I2C_MASTER_SDA_IO, DEFAULT_RST_PIN, DEFAULT_I2C_MASTER_FREQ_HZ);
}

// Limpa o display
esp_err_t display_wrapper_clear(display_t *display) 
{
    ssd1306_refresh_gram(display->ssd1306_dev);  // Atualizar a memória de vídeo
    ssd1306_clear_screen(display->ssd1306_dev, 0x00); // Limpar a tela
    return ESP_OK;
}

// Escreve uma mensagem no display
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

// Desaloca qualquer recurso e desliga o display
esp_err_t display_wrapper_deinit(display_t *display) 
{
    ssd1306_delete(display->ssd1306_dev);
    return ESP_OK;
}