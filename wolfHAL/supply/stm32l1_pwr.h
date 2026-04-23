#ifndef WHAL_STM32L1_PWR_H
#define WHAL_STM32L1_PWR_H

#include <stdint.h>
#include <wolfHAL/supply/supply.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32l1_pwr.h
 * @brief STM32L1 PWR (power control) driver configuration.
 *
 * Controls the internal voltage regulator output range (PWR_CR.VOS), which
 * bounds the maximum permitted SYSCLK and PLL VCO frequencies. Must be
 * configured before bringing the PLL above the reset-default limits.
 *
 *   Range 1 (1.8 V): SYSCLK <= 32 MHz, PLL VCO <= 96 MHz.
 *   Range 2 (1.5 V): SYSCLK <= 16 MHz, PLL VCO <= 48 MHz. Reset default.
 *   Range 3 (1.2 V): SYSCLK <=  4 MHz, PLL disabled.
 */

/*
 * @brief Internal voltage regulator output voltage ranges (PWR_CR.VOS).
 */
typedef enum {
    WHAL_STM32L1_PWR_VOS_RANGE_1 = 1,
    WHAL_STM32L1_PWR_VOS_RANGE_2 = 2,
    WHAL_STM32L1_PWR_VOS_RANGE_3 = 3,
} whal_Stm32l1Pwr_VosRange;

/*
 * @brief STM32L1 PWR driver configuration.
 */
typedef struct whal_Stm32l1Pwr_Cfg {
    whal_Stm32l1Pwr_VosRange vosRange;
    whal_Timeout *timeout; /**< Timeout instance for VOSF polling */
} whal_Stm32l1Pwr_Cfg;

#ifndef WHAL_CFG_SUPPLY_API_MAPPING_STM32L1
/*
 * @brief Driver instance for STM32L1 PWR peripheral.
 */
extern const whal_SupplyDriver whal_Stm32l1Pwr_Driver;

/*
 * @brief Initialize the STM32L1 PWR peripheral.
 *
 * Programs PWR_CR.VOS from the cfg and polls PWR_CSR.VOSF until voltage
 * scaling completes. The PWR APB1 clock must already be enabled (via
 * whal_Clock_Enable with WHAL_STM32L152_PWR_CLOCK) before calling.
 *
 * @param supplyDev Supply instance to initialize.
 *
 * @retval WHAL_SUCCESS  Voltage scaling settled at the requested range.
 * @retval WHAL_EINVAL   Null pointer or missing cfg.
 * @retval WHAL_ETIMEOUT VOSF did not clear within the configured timeout.
 */
whal_Error whal_Stm32l1Pwr_Init(whal_Supply *supplyDev);

/*
 * @brief Deinitialize the STM32L1 PWR peripheral.
 *
 * @param supplyDev Supply instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Always.
 */
whal_Error whal_Stm32l1Pwr_Deinit(whal_Supply *supplyDev);

/*
 * @brief Unused for STM32L1 PWR; provided for vtable completeness.
 */
whal_Error whal_Stm32l1Pwr_Enable(whal_Supply *supplyDev, void *supply);

/*
 * @brief Unused for STM32L1 PWR; provided for vtable completeness.
 */
whal_Error whal_Stm32l1Pwr_Disable(whal_Supply *supplyDev, void *supply);
#endif /* !WHAL_CFG_SUPPLY_API_MAPPING_STM32L1 */

#endif /* WHAL_STM32L1_PWR_H */
