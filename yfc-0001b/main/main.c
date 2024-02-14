#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "display_wrapper.h"

// foi preciso colocar esse ifndef para que o compilador nao reclamasse
#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define SDA_PIN 4
#define SCL_PIN 15
#define RST_PIN 16
#define I2C_PORT I2C_NUM_1
#define I2C_FREQ 100000

#define MESSAGE_SIZE 20

// Variável global para armazenar o display
display_t display;

// Variável global para armazenar a mensagem a ser exibida no display
static char message[MESSAGE_SIZE];

// Mutex para proteger a variável global message
static SemaphoreHandle_t xMessage_mutex;

// Função para escrever a mensagem no display
void write_message_to_display(const char* message) {

    if (xSemaphoreTake(xMessage_mutex, portMAX_DELAY) == pdTRUE) {
        display_wrapper_clear(&display);
        display_wrapper_write(&display, message, 0, 0, 16, 1);
        xSemaphoreGive(xMessage_mutex);
    }
}

// Task para atualizar a mensagem no display
void update_display_task(void *pvParameter) {
    const char* messages[] = {
        "Hello, World!",
        "Ola, Mundo!",
        "Bonjour, Monde!",
        "Hallo, Welt!",
        "Ciao, Mondo!",
        "Hola, Mundo!",
        "Hej, Verden!",
        "Hei, Maailma!",
        "Salut, Monde!",
        "Hallo, Wereld!",
        "Hallo, Verden!",
        "deu certo!"
    };
    int message_index = 0;

    while (1) {
        write_message_to_display(messages[message_index]);
        message_index = (message_index + 1) % (sizeof(messages) / sizeof(messages[0]));
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // Inicializar o display
    display_wrapper_init(&display, I2C_PORT, SCL_PIN, SDA_PIN, RST_PIN, I2C_FREQ);

    // Inicializar o mutex
    xMessage_mutex = xSemaphoreCreateMutex();

    // Criar a task para atualizar a mensagem no display
    xTaskCreate(&update_display_task, "update_display_task", 2048, NULL, 5, NULL);

}
