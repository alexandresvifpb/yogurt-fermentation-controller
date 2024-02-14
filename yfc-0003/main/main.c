/**
 * @file main.c
 * @author alexandre sales vasconcelos (alexandre.vasconcelos@ifpb.edu.br)
 * @brief example of using the SSR component with two SSR modules.
 * @version 0.1
 * @date 2023-06-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ssr.h"    // SSR component

// foi preciso colocar esse ifndef para que o compilador nao reclamasse
#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 1000
#endif

#define SSR1_PSSR_GPIO_PIN GPIO_NUM_12
#define SSR2_PSSR_GPIO_PIN GPIO_NUM_14
#define STACK_SIZE 2048
// #define ESP32S2

static const char *TAG = "SSR";

/**
 * @brief task for testing the SSR component, two SSR GPIO modules 2 and 4 and the PWM configured with 10 bit resolution (1024 values) will be created.
 */
void ssr_task(void *pvParameter)
{
    ESP_LOGW(TAG, "ssr_task() called");
    // verify which ESP32 is being used
    #ifdef CONFIG_IDF_TARGET_ESP32S2
    ESP_LOGI(TAG, "ESP32-S2 detected");
    #else
    ESP_LOGI(TAG, "ESP32 detected");
    #endif
    // Initialize SSR1
    SSR ssr1;
    ssr_init(&ssr1, SSR1_PSSR_GPIO_PIN, LEDC_CHANNEL_0, LEDC_TIMER_10_BIT, 1000);
    // Initialize SSR2
    SSR ssr2;
    ssr_init(&ssr2, SSR2_PSSR_GPIO_PIN, LEDC_CHANNEL_1, LEDC_TIMER_10_BIT, 1000);
    // Define variables
    uint16_t pwm1_value = 0;
    uint16_t pwm2_value = 0;
    // Loop
    while (1)
    {
        /* code  */
        for (size_t i = 0; i < 1024; i++)
        {
            // Set the duty cycle for SSR1
            pwm1_value = i;
            ssr_set_duty_cycle(&ssr1, pwm1_value);
            // Set the duty cycle for SSR2
            pwm2_value = 1023 - i;
            ssr_set_duty_cycle(&ssr2, pwm2_value);
            // Delay
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

/**
 * @brief Main function, create the task.
 */
void app_main(void)
{
    ESP_LOGW(TAG, "app_main() called");
    // Create task
    xTaskCreate(ssr_task, "ssr_task", STACK_SIZE, NULL, 1, NULL);
}