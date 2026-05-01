#ifndef WHAL_STM32L1_PWR_H
#define WHAL_STM32L1_PWR_H

#include <stdint.h>
#include <wolfHAL/power/power.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32l1_pwr.h
 * @brief STM32L1 PWR (power control) driver.
 *
 * Boards bring up the regulator imperatively from Board_Init by calling
 * the helpers below. Power is a board-level driver — there is no generic
 * whal_Power_* API or vtable.
 *
 * The internal voltage regulator output range (PWR_CR.VOS) bounds the
 * maximum permitted SYSCLK and PLL VCO frequencies. Must be configured
 * before bringing the PLL above the reset-default limits.
 *
 *   Range 1 (1.8 V): SYSCLK <= 32 MHz, PLL VCO <= 96 MHz.
 *   Range 2 (1.5 V): SYSCLK <= 16 MHz, PLL VCO <= 48 MHz. Reset default.
 *   Range 3 (1.2 V): SYSCLK <=  4 MHz, PLL disabled.
 */

/*
 * @brief Internal voltage regulator output ranges (PWR_CR.VOS).
 */
typedef enum {
    WHAL_STM32L1_PWR_VOS_RANGE_1 = 1,
    WHAL_STM32L1_PWR_VOS_RANGE_2 = 2,
    WHAL_STM32L1_PWR_VOS_RANGE_3 = 3,
} whal_Stm32l1_Pwr_VosRange;

/*
 * @brief Set the voltage regulator output range. Polls PWR_CSR.VOSF
 *        until voltage scaling completes. The PWR APB1 clock must already
 *        be enabled (via whal_Stm32l1_Rcc_EnablePeriphClk with
 *        WHAL_STM32L152_PWR_CLOCK) before calling.
 *
 * @param powerDev Power device instance.
 * @param range    Desired VOS range (RANGE_1, RANGE_2, or RANGE_3).
 * @param timeout  Optional timeout for the VOSF poll loop.
 *
 * @retval WHAL_SUCCESS  Voltage scaling settled at the requested range.
 * @retval WHAL_EINVAL   Null powerDev.
 * @retval WHAL_ETIMEOUT VOSF did not clear within the configured timeout.
 */
whal_Error whal_Stm32l1_Pwr_SetVosRange(whal_Power *powerDev,
                                       whal_Stm32l1_Pwr_VosRange range,
                                       whal_Timeout *timeout);

#endif /* WHAL_STM32L1_PWR_H */
