#ifndef WHAL_STM32F0_RCC_H
#define WHAL_STM32F0_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32f0_rcc.h
 * @brief STM32F0 RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32F0 RCC peripheral controls:
 * - System clock source selection (HSI, HSE, PLL, HSI48)
 * - PLL with PREDIV prescaler and PLLMUL multiplier
 * - Peripheral clock gating (AHB, APB1, APB2 buses)
 * - HSI48 oscillator (available on F04x/F07x/F09x)
 *
 * Clock sources:
 *   HSI  = 8 MHz internal RC oscillator
 *   HSE  = 4-32 MHz external oscillator
 *   HSI48 = 48 MHz internal RC (F04x/F07x/F09x only)
 *   PLL  = HSI/2, HSI/PREDIV, HSE/PREDIV, or HSI48/PREDIV * PLLMUL
 */

typedef enum {
    WHAL_STM32F0_RCC_SYSCLK_SRC_HSI,
    WHAL_STM32F0_RCC_SYSCLK_SRC_HSE,
    WHAL_STM32F0_RCC_SYSCLK_SRC_PLL,
    WHAL_STM32F0_RCC_SYSCLK_SRC_HSI48,
} whal_Stm32f0Rcc_SysClockSrc;

typedef enum {
    WHAL_STM32F0_RCC_PLLSRC_HSI_DIV2,
    WHAL_STM32F0_RCC_PLLSRC_HSI_PREDIV,
    WHAL_STM32F0_RCC_PLLSRC_HSE_PREDIV,
    WHAL_STM32F0_RCC_PLLSRC_HSI48_PREDIV,
} whal_Stm32f0Rcc_PllClockSrc;

typedef struct {
    whal_Stm32f0Rcc_PllClockSrc clkSrc;
    uint8_t prediv;  /* 1-16 (written as value - 1) */
    uint8_t pllmul;  /* 2-16 (written as value - 2) */
} whal_Stm32f0Rcc_PllCfg;

typedef struct whal_Stm32f0Rcc_Clk {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32f0Rcc_Clk;

typedef struct whal_Stm32f0Rcc_Cfg {
    whal_Stm32f0Rcc_SysClockSrc sysClkSrc;
    whal_Stm32f0Rcc_PllCfg *pllCfg;
} whal_Stm32f0Rcc_Cfg;

#ifndef WHAL_CFG_CLOCK_API_MAPPING_STM32F0
extern const whal_ClockDriver whal_Stm32f0Rcc_Driver;

whal_Error whal_Stm32f0Rcc_Init(whal_Clock *clkDev);
whal_Error whal_Stm32f0Rcc_Deinit(whal_Clock *clkDev);
whal_Error whal_Stm32f0Rcc_Enable(whal_Clock *clkDev, const void *clk);
whal_Error whal_Stm32f0Rcc_Disable(whal_Clock *clkDev, const void *clk);
#endif /* !WHAL_CFG_CLOCK_API_MAPPING_STM32F0 */

#endif /* WHAL_STM32F0_RCC_H */
