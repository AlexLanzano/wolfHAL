#ifndef WHAL_STM32L1_RCC_H
#define WHAL_STM32L1_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32l1_rcc.h
 * @brief STM32L1 RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32L1 RCC peripheral controls:
 * - System clock source selection (MSI, HSI, HSE, PLL)
 * - PLL with PLLMUL multiplier and PLLDIV divider
 * - Peripheral clock gating (AHB, APB1, APB2 buses)
 *
 * Clock sources:
 *   MSI  = 65.536 kHz to 4.194 MHz multispeed internal oscillator (default)
 *   HSI  = 16 MHz internal RC oscillator
 *   HSE  = 1-24 MHz external oscillator
 *   PLL  = HSI or HSE * PLLMUL / PLLDIV (output max 32 MHz)
 *
 * RCC register map (RM0038 Table 36):
 *   RCC_CR      = 0x00  (HSION, HSIRDY, MSION, MSIRDY, HSEON, HSERDY, PLLON, PLLRDY)
 *   RCC_ICSCR   = 0x04  (MSIRANGE)
 *   RCC_CFGR    = 0x08  (SW, SWS, HPRE, PPRE1, PPRE2, PLLSRC, PLLMUL, PLLDIV)
 *   RCC_CIR     = 0x0C
 *   RCC_AHBRSTR = 0x10
 *   RCC_APB2RSTR = 0x14
 *   RCC_APB1RSTR = 0x18
 *   RCC_AHBENR  = 0x1C
 *   RCC_APB2ENR = 0x20
 *   RCC_APB1ENR = 0x24
 */

/*
 * @brief System clock source selection.
 */
typedef enum {
    WHAL_STM32L1_RCC_SYSCLK_SRC_MSI, /* Multispeed internal (default) */
    WHAL_STM32L1_RCC_SYSCLK_SRC_HSI, /* 16 MHz internal RC */
    WHAL_STM32L1_RCC_SYSCLK_SRC_HSE, /* External crystal/oscillator */
    WHAL_STM32L1_RCC_SYSCLK_SRC_PLL, /* PLL output */
} whal_Stm32l1Rcc_SysClockSrc;

/*
 * @brief PLL input clock source selection.
 */
typedef enum {
    WHAL_STM32L1_RCC_PLLSRC_HSI, /* HSI (16 MHz) */
    WHAL_STM32L1_RCC_PLLSRC_HSE, /* HSE */
} whal_Stm32l1Rcc_PllClockSrc;

/*
 * @brief PLL multiplication factor (PLLMUL field in RCC_CFGR).
 *
 * PLLVCO = PLL_input * pllmul
 * PLLVCO must not exceed 96 MHz (range 1), 48 MHz (range 2), 24 MHz (range 3).
 */
typedef enum {
    WHAL_STM32L1_RCC_PLLMUL_3  = 0,
    WHAL_STM32L1_RCC_PLLMUL_4  = 1,
    WHAL_STM32L1_RCC_PLLMUL_6  = 2,
    WHAL_STM32L1_RCC_PLLMUL_8  = 3,
    WHAL_STM32L1_RCC_PLLMUL_12 = 4,
    WHAL_STM32L1_RCC_PLLMUL_16 = 5,
    WHAL_STM32L1_RCC_PLLMUL_24 = 6,
    WHAL_STM32L1_RCC_PLLMUL_32 = 7,
    WHAL_STM32L1_RCC_PLLMUL_48 = 8,
} whal_Stm32l1Rcc_PllMul;

/*
 * @brief PLL output division factor (PLLDIV field in RCC_CFGR).
 *
 * SYSCLK = PLLVCO / plldiv
 */
typedef enum {
    WHAL_STM32L1_RCC_PLLDIV_2 = 1,
    WHAL_STM32L1_RCC_PLLDIV_3 = 2,
    WHAL_STM32L1_RCC_PLLDIV_4 = 3,
} whal_Stm32l1Rcc_PllDiv;

/*
 * @brief PLL configuration parameters.
 */
typedef struct {
    whal_Stm32l1Rcc_PllClockSrc clkSrc;
    whal_Stm32l1Rcc_PllMul pllmul;
    whal_Stm32l1Rcc_PllDiv plldiv;
} whal_Stm32l1Rcc_PllCfg;

/*
 * @brief Peripheral clock enable descriptor.
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32l1Rcc_Clk;

/*
 * @brief RCC driver configuration.
 */
typedef struct whal_Stm32l1Rcc_Cfg {
    whal_Stm32l1Rcc_SysClockSrc sysClkSrc;
    whal_Stm32l1Rcc_PllCfg *pllCfg;
} whal_Stm32l1Rcc_Cfg;

#ifndef WHAL_CFG_CLOCK_API_MAPPING_STM32L1
extern const whal_ClockDriver whal_Stm32l1Rcc_Driver;

whal_Error whal_Stm32l1Rcc_Init(whal_Clock *clkDev);
whal_Error whal_Stm32l1Rcc_Deinit(whal_Clock *clkDev);
whal_Error whal_Stm32l1Rcc_Enable(whal_Clock *clkDev, const void *clk);
whal_Error whal_Stm32l1Rcc_Disable(whal_Clock *clkDev, const void *clk);
#endif /* !WHAL_CFG_CLOCK_API_MAPPING_STM32L1 */

#endif /* WHAL_STM32L1_RCC_H */
