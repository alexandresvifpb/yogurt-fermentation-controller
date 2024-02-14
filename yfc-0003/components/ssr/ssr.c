/**
 * @file ssr.c
 * @author alexandre sales vasconcelos (alexandre.vasconcelos@ifpb.edu.br)
 * @brief component for using the SSR module that can be created multiple instances.
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "ssr.h"

#define PWM_CHANNEL_DEFAULT LEDC_CHANNEL_0
#define PWM_FREQ_HZ_DEFAULT 1000
#define PWM_RESOLUTION_DEFAULT LEDC_TIMER_10_BIT
#define LEDC_CHANNEL_IDLE_LOW 0
#define LEDC_CHANNEL_IDLE_HIGH 1

static const char *TAG = "SSR";

/**
 * @brief Initialize a SSR object.
 *
 * @param pin pin number that will be used to control the SSR module.
 * @param channel: channel of pwm of the SSR module.
 * @param resolution: resolution of pwm of the SSR module.
 * @param frequency: frequency of pwm of the SSR module.
 * @param ssr: SSR object.
 * @return esp_err_t
 */
esp_err_t ssr_init(SSR *ssr, uint8_t pin, uint8_t channel, uint8_t resolution, uint16_t frequency)
{
    ESP_LOGW(TAG, "ssr_init() called");
    // Configure the timer for PWM
    ledc_timer_config_t cfg;
#ifdef CONFIG_IDF_TARGET_ESP32
    cfg.speed_mode = LEDC_HIGH_SPEED_MODE; // high speed mode
#elif CONFIG_IDF_TARGET_ESP32S2
    cfg.speed_mode = LEDC_LOW_SPEED_MODE; // low speed mode
#else
    cfg.speed_mode = LEDC_LOW_SPEED_MODE; // low speed mode
#endif
    cfg.duty_resolution = resolution;     // Resolution
    cfg.timer_num = LEDC_TIMER_0;         // timer used
    cfg.freq_hz = frequency;              // Frequency PWM
    cfg.clk_cfg = LEDC_AUTO_CLK;          // clock used
    ledc_timer_config(&cfg);
    // Configure the PWM channel
    ledc_channel_config_t cfg2;
    cfg2.gpio_num = pin; // GPIO used
#ifdef CONFIG_IDF_TARGET_ESP32
    cfg2.speed_mode = LEDC_HIGH_SPEED_MODE; // high speed mode
#elif CONFIG_IDF_TARGET_ESP32S2
    cfg2.speed_mode = LEDC_LOW_SPEED_MODE; // low speed mode
#else
    cfg2.speed_mode = LEDC_LOW_SPEED_MODE; // low speed mode
#endif
    cfg2.channel = channel;                // PWM0 channel
    cfg2.intr_type = LEDC_INTR_DISABLE;    // OFF interrupts
    cfg2.timer_sel = LEDC_TIMER_0;         // timer used
    cfg2.duty = 0;                         // Initial duty cycle value
    cfg2.hpoint = 0;
    ledc_channel_config(&cfg2);
    // Set the pin
    ssr->pin = pin;
    // Set the pwm channel
    ssr->channel = channel;
    // Set the pwm resolution
    ssr->resolution = resolution;
    // Set the pwm frequency
    ssr->frequency = frequency;
    // Return success
    return ESP_OK;
}

/**
 * @brief Set the SSR pwm.
 *
 * @param ssr SSR object.
 * @param duty_cycle duty_cycle of pwm of the SSR module.
 * @return esp_err_t
 */
esp_err_t ssr_set_duty_cycle(SSR *ssr, uint16_t duty_cycle)
{
    ESP_LOGW(TAG, "ssr_setPWM() called, duty_cycle: %d", duty_cycle);
    // Set the maximum duty cycle
    uint16_t max_duty_cycle = (1 << ssr->resolution) - 1;
    // Check if the duty cycle is valid
    if (duty_cycle > max_duty_cycle)
    {
        ESP_LOGE(TAG, "ssr_set_duty_cycle() called with invalid duty_cycle value: %d", duty_cycle);
        return ESP_ERR_INVALID_ARG;
    }
    // Check if the duty cycle is 0 and set the SSR to off
    else if (duty_cycle == 0)
    {
        // Turn off the SSR
#ifdef CONFIG_IDF_TARGET_ESP32
        ESP_LOGI(TAG, "ESP32");
        ledc_stop(LEDC_HIGH_SPEED_MODE, ssr->channel, LEDC_CHANNEL_IDLE_HIGH);
#elif CONFIG_IDF_TARGET_ESP32S2
        ESP_LOGI(TAG, "ESP32S2");
        ledc_stop(LEDC_LOW_SPEED_MODE, ssr->channel, LEDC_CHANNEL_IDLE_LOW);
#else
        ESP_LOGI(TAG, "DEFAULT");
        ledc_stop(LEDC_LOW_SPEED_MODE, ssr->channel, LEDC_CHANNEL_IDLE_HIGH);
#endif
    }
    // Check if the duty cycle is into the maximum and minimum values and set the SSR to on value
    else
    {
    // Set the duty cycle
#ifdef CONFIG_IDF_TARGET_ESP32
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, ssr->channel, duty_cycle);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, ssr->channel);
#elif CONFIG_IDF_TARGET_ESP32S2
        ledc_set_duty(LEDC_LOW_SPEED_MODE, ssr->channel, duty_cycle);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, ssr->channel);
#else
        ledc_set_duty(LEDC_LOW_SPEED_MODE, ssr->channel, duty_cycle);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, ssr->channel);
#endif
    }
    // Return success
    return ESP_OK;
}

/**
 * @brief Set turn off the SSR module.
 * @param ssr SSR object.
 * @return esp_err_t
 */
esp_err_t ssr_turn_off(SSR *ssr)
{
    // Turn off the SSR
    ESP_LOGW(TAG, "ssr_turn_off called");
#ifdef CONFIG_IDF_TARGET_ESP32
    ledc_stop(LEDC_HIGH_SPEED_MODE, ssr->channel, LEDC_CHANNEL_IDLE_HIGH);
#elif CONFIG_IDF_TARGET_ESP32S2
    ledc_stop(LEDC_LOW_SPEED_MODE, ssr->channel, LEDC_CHANNEL_IDLE_LOW);
#else
    ledc_stop(LEDC_LOW_SPEED_MODE, ssr->channel, LEDC_CHANNEL_IDLE_HIGH);
#endif
    // Return success
    return ESP_OK;
}