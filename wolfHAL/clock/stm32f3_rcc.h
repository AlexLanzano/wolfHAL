#ifndef WHAL_STM32F3_RCC_H
#define WHAL_STM32F3_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32f3_rcc.h
 * @brief STM32F3 RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32F3 RCC peripheral controls:
 * - System clock source selection (HSI, HSE, PLL)
 * - PLL with PREDIV prescaler and PLLMUL multiplier
 * - Peripheral clock gating (AHB, APB1, APB2 buses)
 *
 * Clock sources:
 *   HSI  = 8 MHz internal RC oscillator
 *   HSE  = 4-32 MHz external oscillator
 *   PLL  = HSI/2 or HSE/PREDIV * PLLMUL (output must not exceed 72 MHz)
 *
 * RCC register map (RM0365 Table 29):
 *   RCC_CR      = 0x00  (HSION, HSIRDY, HSEON, HSERDY, PLLON, PLLRDY)
 *   RCC_CFGR    = 0x04  (SW, SWS, PLLSRC, PLLMUL, HPRE, PPRE1, PPRE2)
 *   RCC_CIR     = 0x08
 *   RCC_AHBENR  = 0x14
 *   RCC_APB2ENR = 0x18
 *   RCC_APB1ENR = 0x1C
 *   RCC_CFGR2   = 0x2C  (PREDIV)
 *   RCC_CFGR3   = 0x30  (USARTxSW, I2CxSW, TIMxSW)
 */

/*
 * @brief System clock source selection.
 *
 * Determines which oscillator/PLL drives the system clock (SYSCLK).
 */
typedef enum {
    WHAL_STM32F3_RCC_SYSCLK_SRC_HSI, /* 8 MHz internal RC oscillator */
    WHAL_STM32F3_RCC_SYSCLK_SRC_HSE, /* External crystal/oscillator */
    WHAL_STM32F3_RCC_SYSCLK_SRC_PLL, /* PLL output */
} whal_Stm32f3Rcc_SysClockSrc;

/*
 * @brief PLL input clock source selection.
 *
 * Determines which oscillator feeds the PLL input.
 */
typedef enum {
    WHAL_STM32F3_RCC_PLLSRC_HSI_DIV2,   /* HSI divided by 2 */
    WHAL_STM32F3_RCC_PLLSRC_HSE_PREDIV, /* HSE divided by PREDIV */
} whal_Stm32f3Rcc_PllClockSrc;

/*
 * @brief I2C1 peripheral clock source selection (RCC_CFGR3.I2C1SW).
 */
typedef enum {
    WHAL_STM32F3_RCC_I2C1_SRC_HSI    = 0, /* HSI (8 MHz), reset default */
    WHAL_STM32F3_RCC_I2C1_SRC_SYSCLK = 1, /* System clock */
} whal_Stm32f3Rcc_I2c1ClkSrc;

/*
 * @brief PLL configuration parameters.
 *
 * The PLL output frequency is (input / prediv) * pllmul, where input is
 * HSI/2 (fixed) or HSE. Output must not exceed 72 MHz.
 */
typedef struct {
    whal_Stm32f3Rcc_PllClockSrc clkSrc;
    uint8_t prediv;  /* 1-16 (written as value - 1) */
    uint8_t pllmul;  /* 2-16 (written as value - 2) */
} whal_Stm32f3Rcc_PllCfg;

/*
 * @brief Peripheral clock enable descriptor.
 *
 * Describes the register offset and bit position needed to enable/disable
 * a peripheral's bus clock. Used with whal_Stm32f3Rcc_Enable/Disable.
 */
typedef struct {
    size_t regOffset;   /* Offset from RCC base to enable register */
    size_t enableMask;  /* Bit mask for the peripheral enable bit */
    size_t enablePos;   /* Bit position for the peripheral enable bit */
} whal_Stm32f3Rcc_Clk;

/*
 * @brief RCC driver configuration.
 *
 * Contains all parameters needed to configure the system clock.
 */
typedef struct whal_Stm32f3Rcc_Cfg {
    whal_Stm32f3Rcc_SysClockSrc sysClkSrc; /* System clock source */
    whal_Stm32f3Rcc_PllCfg *pllCfg;        /* Required when sysClkSrc == PLL */
} whal_Stm32f3Rcc_Cfg;

#ifndef WHAL_CFG_CLOCK_API_MAPPING_STM32F3
/*
 * @brief Driver instance for the STM32F3 RCC clock controller.
 */
extern const whal_ClockDriver whal_Stm32f3Rcc_Driver;

/*
 * @brief Initialize the RCC peripheral and switch to the configured SYSCLK.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f3Rcc_Init(whal_Clock *clkDev);

/*
 * @brief Deinitialize the RCC peripheral and switch SYSCLK back to HSI.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f3Rcc_Deinit(whal_Clock *clkDev);

/*
 * @brief Enable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32f3Rcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock enabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f3Rcc_Enable(whal_Clock *clkDev, const void *clk);

/*
 * @brief Disable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32f3Rcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f3Rcc_Disable(whal_Clock *clkDev, const void *clk);
#endif /* !WHAL_CFG_CLOCK_API_MAPPING_STM32F3 */

/*
 * @brief Select the I2C1 peripheral clock source via RCC_CFGR3.I2C1SW.
 *
 * Reset default is HSI (8 MHz). Select SYSCLK to drive I2C1 timing from the
 * main system clock instead.
 *
 * @param clkDev Clock controller instance.
 * @param src    I2C1 clock source (HSI or SYSCLK).
 *
 * @retval WHAL_SUCCESS Clock source updated.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f3Rcc_Ext_SetI2c1ClkSrc(whal_Clock *clkDev,
                                              whal_Stm32f3Rcc_I2c1ClkSrc src);

#endif /* WHAL_STM32F3_RCC_H */
