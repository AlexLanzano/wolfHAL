#ifndef WHAL_STM32C0_RCC_H
#define WHAL_STM32C0_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32c0_rcc.h
 * @brief STM32C0 RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32C0 RCC peripheral controls:
 * - System clock source selection (HSISYS, HSE, LSI, LSE)
 * - HSI48 oscillator with configurable HSIDIV prescaler
 * - Peripheral clock gating (IOP, AHB, APB buses)
 * - Bus clock prescalers
 *
 * The STM32C0 has no PLL. The primary high-speed clock is HSI48 (48 MHz)
 * with a configurable divider (HSIDIV) producing HSISYS.
 */

/*
 * @brief HSI divider selection.
 *
 * The HSI48 oscillator runs at 48 MHz. HSIDIV divides it to produce
 * HSISYS which can be selected as the system clock.
 */
typedef enum {
    WHAL_STM32C0_RCC_HSIDIV_1,   /* 48 MHz */
    WHAL_STM32C0_RCC_HSIDIV_2,   /* 24 MHz */
    WHAL_STM32C0_RCC_HSIDIV_4,   /* 12 MHz */
    WHAL_STM32C0_RCC_HSIDIV_8,   /* 6 MHz */
    WHAL_STM32C0_RCC_HSIDIV_16,  /* 3 MHz */
    WHAL_STM32C0_RCC_HSIDIV_32,  /* 1.5 MHz */
    WHAL_STM32C0_RCC_HSIDIV_64,  /* 750 kHz */
    WHAL_STM32C0_RCC_HSIDIV_128, /* 375 kHz */
} whal_Stm32c0Rcc_HsiDiv;

/*
 * @brief Peripheral clock enable descriptor.
 *
 * Describes the register offset and bit mask needed to enable/disable
 * a peripheral's bus clock. Used with whal_Stm32c0Rcc_Enable/Disable.
 *
 * Example for GPIOA:
 *   { .regOffset = 0x034, .enableMask = (1 << 0), .enablePos = 0 }
 */
typedef struct whal_Stm32c0Rcc_Clk {
    size_t regOffset;   /* Offset from RCC base to enable register */
    size_t enableMask;  /* Bit mask for the peripheral enable bit */
    size_t enablePos;   /* Bit position for the peripheral enable bit */
} whal_Stm32c0Rcc_Clk;

/*
 * @brief RCC driver configuration.
 *
 * Contains all parameters needed to configure the system clock.
 */
typedef struct whal_Stm32c0Rcc_Cfg {
    whal_Stm32c0Rcc_HsiDiv hsidiv; /* HSI divider for HSISYS frequency */
} whal_Stm32c0Rcc_Cfg;

/*
 * @brief Driver instance for the STM32C0 RCC clock controller.
 */
extern const whal_ClockDriver whal_Stm32c0Rcc_Driver;

/*
 * @brief Initialize the RCC peripheral.
 *
 * Configures HSIDIV, enables HSI, and selects HSISYS as SYSCLK.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32c0Rcc_Init(whal_Clock *clkDev);

/*
 * @brief Deinitialize the RCC peripheral.
 *
 * Resets HSIDIV to default (div1) and selects HSISYS as SYSCLK.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32c0Rcc_Deinit(whal_Clock *clkDev);

/*
 * @brief Enable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32c0Rcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock enabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32c0Rcc_Enable(whal_Clock *clkDev, const void *clk);

/*
 * @brief Disable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32c0Rcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32c0Rcc_Disable(whal_Clock *clkDev, const void *clk);

/*
 * @brief Compute the current system clock rate.
 *
 * Returns 48 MHz divided by the configured HSIDIV setting.
 *
 * @param clkDev  Clock device instance.
 * @param rateOut Output for the computed rate in Hz.
 *
 * @retval WHAL_SUCCESS Rate computed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32c0Rcc_GetRate(whal_Clock *clkDev, size_t *rateOut);

#endif /* WHAL_STM32C0_RCC_H */
