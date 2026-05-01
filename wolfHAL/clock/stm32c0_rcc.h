#ifndef WHAL_STM32C0_RCC_H
#define WHAL_STM32C0_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32c0_rcc.h
 * @brief STM32C0 RCC (Reset and Clock Control) driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init by calling
 * the helpers below. The STM32C0 has no PLL — the primary high-speed
 * clock is HSI48 (48 MHz) with a configurable divider (HSIDIV) producing
 * HSISYS.
 */

/*
 * @brief HSI divider selection (RCC_CR.HSIDIV).
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
} whal_Stm32c0_Rcc_HsiDiv;

/*
 * @brief System clock source selection (RCC_CFGR.SW).
 */
typedef enum {
    WHAL_STM32C0_RCC_SYSCLK_SRC_HSISYS = 0,
    WHAL_STM32C0_RCC_SYSCLK_SRC_HSE    = 1,
    WHAL_STM32C0_RCC_SYSCLK_SRC_LSI    = 3,
    WHAL_STM32C0_RCC_SYSCLK_SRC_LSE    = 4,
} whal_Stm32c0_Rcc_SysClockSrc;

/*
 * @brief Peripheral clock descriptor (RCC *ENR enable bit).
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32c0_Rcc_PeriphClk;

/*
 * @brief Enable HSI, set the HSIDIV divider, and wait for ready.
 */
whal_Error whal_Stm32c0_Rcc_EnableHsi(whal_Clock *clkDev,
                                     whal_Stm32c0_Rcc_HsiDiv hsidiv);

/*
 * @brief Switch SYSCLK to the given source. Blocks until RCC_CFGR.SWS
 *        reflects the new source.
 */
whal_Error whal_Stm32c0_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32c0_Rcc_SysClockSrc src);

/*
 * @brief Enable a peripheral clock.
 */
whal_Error whal_Stm32c0_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32c0_Rcc_PeriphClk *clk);
/*
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Stm32c0_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32c0_Rcc_PeriphClk *clk);

#endif /* WHAL_STM32C0_RCC_H */
