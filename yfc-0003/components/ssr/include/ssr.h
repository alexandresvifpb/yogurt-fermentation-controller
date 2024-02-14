#pragma once

/**
 * @file ssr.h
 * @author alexandre sales vasconcelos (alexandre.vasconcelos@ifpb.edu.br)
 * @brief component for using the SSR module that can be created multiple instances.
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __SSR_H__
#define __SSR_H__

#include <esp_err.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Create a structure to store SSR module parameters.
     *
     * @param pin pin number that will be used to control the SSR module.
     * @param channel: channel of pwm of the SSR module.
     * @param resolution: resolution of pwm of the SSR module.
     * @param duty_cycle: duty_cycle of pwm of the SSR module.
     */
    typedef struct
    {
        uint8_t pin;
        uint8_t channel;
        uint8_t resolution;
        uint16_t frequency;
    } SSR;

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
    esp_err_t ssr_init(SSR *ssr, uint8_t pin, uint8_t channel, uint8_t resolution, uint16_t frequency);

    /**
     * @brief Set the SSR pwm.
     *
     * @param ssr SSR object.
     * @param duty_cycle duty_cycle of pwm of the SSR module.
     * @return esp_err_t
     */
    esp_err_t ssr_set_duty_cycle(SSR *ssr, uint16_t duty_cycle);

    /**
     * @brief Set turn off the SSR module.
     * @param ssr SSR object.
     * @return esp_err_t
     */
    esp_err_t ssr_turn_off(SSR *ssr);

#ifdef __cplusplus
}
#endif

#endif // __SSR_H__