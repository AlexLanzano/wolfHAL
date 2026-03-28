#ifndef WHAL_STM32H5_RCC_H
#define WHAL_STM32H5_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32h5_rcc.h
 * @brief STM32H5 RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32H5 RCC peripheral controls:
 * - System clock source selection (HSI, CSI, HSE, PLL1)
 * - PLL1/PLL2/PLL3 configuration
 * - Peripheral clock gating (AHB1, AHB2, APB1, APB2, APB3 buses)
 * - Bus clock prescalers
 *
 * Two driver variants are provided:
 * - whal_Stm32h5RccPll_Driver: Uses PLL1 as system clock source
 * - whal_Stm32h5RccHsi_Driver: Uses HSI oscillator as system clock source
 */

/*
 * @brief System clock source selection.
 *
 * Determines which oscillator/PLL drives the system clock (SYSCLK).
 */
typedef enum {
    WHAL_STM32H5_RCC_SYSCLK_SRC_HSI,  /* 64 MHz High-Speed Internal */
    WHAL_STM32H5_RCC_SYSCLK_SRC_CSI,  /* 4 MHz Low-Power Internal */
    WHAL_STM32H5_RCC_SYSCLK_SRC_HSE,  /* High-Speed External crystal */
    WHAL_STM32H5_RCC_SYSCLK_SRC_PLL1, /* PLL1 output */
} whal_Stm32h5Rcc_SysClockSrc;

/*
 * @brief PLL input clock source selection.
 *
 * Determines which oscillator feeds PLL1/PLL2/PLL3.
 */
typedef enum {
    WHAL_STM32H5_RCC_PLLCLK_SRC_NONE, /* No clock (PLL disabled) */
    WHAL_STM32H5_RCC_PLLCLK_SRC_HSI,  /* HSI as PLL input */
    WHAL_STM32H5_RCC_PLLCLK_SRC_CSI,  /* CSI as PLL input */
    WHAL_STM32H5_RCC_PLLCLK_SRC_HSE,  /* HSE as PLL input */
} whal_Stm32h5Rcc_PllClockSrc;

/*
 * @brief PLL1 configuration parameters.
 *
 * The PLL1 output frequency is calculated as:
 *   f_vco = (f_input / m) * (n + 1)
 *   f_pllp = f_vco / (p + 1)  (main PLL output, used for SYSCLK)
 *   f_pllq = f_vco / (q + 1)  (used for USB, RNG, etc.)
 *   f_pllr = f_vco / (r + 1)  (general purpose)
 *
 * Constraints:
 *   - PLL input (f_input / m) must be 1-16 MHz
 *   - VCO frequency must be 192-836 MHz (wide range) or 150-420 MHz (medium)
 *   - m: 1-63 (divides by m, 0=disabled)
 *   - n: 3-511 (multiplies by n+1, so 3=4x, 4=5x, ..., 511=512x)
 *   - p, q, r: 0-127 (divides by value+1, so 1=/2, 2=/3, ...)
 */
typedef struct whal_Stm32h5Rcc_PllClkCfg {
    whal_Stm32h5Rcc_PllClockSrc clkSrc; /* PLL input source */
    uint16_t n; /* PLLN multiplier (3-511, VCO = input * (n+1)) */
    uint8_t m;  /* PLLM divider (1-63, input / m) */
    uint8_t p;  /* PLLP divider (0-127, output / (p+1)) */
    uint8_t q;  /* PLLQ divider (0-127, output / (q+1)) */
    uint8_t r;  /* PLLR divider (0-127, output / (r+1)) */
} whal_Stm32h5Rcc_PllClkCfg;

/*
 * @brief HSI clock configuration parameters.
 *
 * The HSI is a 64 MHz internal RC oscillator with a configurable divider.
 * Default after reset is 64 MHz (div1).
 */
typedef struct whal_Stm32h5Rcc_HsiClkCfg {
    uint8_t div; /* HSI divider (0=div1, 1=div2, 2=div4, 3=div8) */
} whal_Stm32h5Rcc_HsiClkCfg;

/*
 * @brief Peripheral clock enable descriptor.
 *
 * Describes the register offset and bit mask needed to enable/disable
 * a peripheral's bus clock. Used with whal_Stm32h5Rcc_Enable/Disable.
 *
 * Example for GPIOA:
 *   { .regOffset = 0x088, .enableMask = (1 << 0), .enablePos = 0 }
 */
typedef struct whal_Stm32h5Rcc_Clk {
    size_t regOffset;   /* Offset from RCC base to enable register */
    size_t enableMask;  /* Bit mask for the peripheral enable bit */
    size_t enablePos;   /* Bit position for the peripheral enable bit */
} whal_Stm32h5Rcc_Clk;

/*
 * @brief RCC driver configuration.
 *
 * Contains all parameters needed to configure the system clock.
 */
typedef struct whal_Stm32h5Rcc_Cfg {
    whal_Stm32h5Rcc_SysClockSrc sysClkSrc; /* System clock source */
    void *sysClkCfg; /* Pointer to PllClkCfg or HsiClkCfg based on driver */
} whal_Stm32h5Rcc_Cfg;

/*
 * @brief Driver instance for PLL1 clock source.
 */
extern const whal_ClockDriver whal_Stm32h5RccPll_Driver;

/*
 * @brief Driver instance for HSI clock source.
 */
extern const whal_ClockDriver whal_Stm32h5RccHsi_Driver;

/*
 * @brief Initialize the RCC peripheral with PLL1 as system clock.
 *
 * Configures PLL1 with the parameters in the device configuration,
 * enables PLL1, and switches SYSCLK to PLL1 output.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5RccPll_Init(whal_Clock *clkDev);

/*
 * @brief Deinitialize the RCC peripheral from PLL1 mode.
 *
 * Switches SYSCLK back to HSI and disables PLL1.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5RccPll_Deinit(whal_Clock *clkDev);

/*
 * @brief Initialize the RCC peripheral with HSI as system clock.
 *
 * Configures the HSI divider and selects HSI as SYSCLK source.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5RccHsi_Init(whal_Clock *clkDev);

/*
 * @brief Deinitialize the RCC peripheral from HSI mode.
 *
 * Resets HSI divider to default (div1, 64 MHz).
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5RccHsi_Deinit(whal_Clock *clkDev);

/*
 * @brief Enable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32h5Rcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock enabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Rcc_Enable(whal_Clock *clkDev, const void *clk);

/*
 * @brief Disable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32h5Rcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Rcc_Disable(whal_Clock *clkDev, const void *clk);

/*
 * @brief Compute the current system clock rate (PLL1 mode).
 *
 * Calculates the PLL1 output frequency from the configured source,
 * multiplier, and dividers.
 *
 * @param clkDev  Clock device instance.
 * @param rateOut Output for the computed rate in Hz.
 *
 * @retval WHAL_SUCCESS Rate computed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5RccPll_GetRate(whal_Clock *clkDev, size_t *rateOut);

/*
 * @brief Compute the current system clock rate (HSI mode).
 *
 * Returns 64 MHz divided by the configured HSI divider.
 *
 * @param clkDev  Clock device instance.
 * @param rateOut Output for the computed rate in Hz.
 *
 * @retval WHAL_SUCCESS Rate computed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5RccHsi_GetRate(whal_Clock *clkDev, size_t *rateOut);

/*
 * @brief Enable or disable the HSI48 oscillator.
 *
 * The HSI48 provides a 48 MHz clock used as kernel clock for the RNG
 * and USB peripherals. When enabling, this function waits for the
 * oscillator to stabilize.
 *
 * @param clkDev Clock device instance (RCC regmap).
 * @param enable 1 to enable, 0 to disable.
 *
 * @retval WHAL_SUCCESS HSI48 state changed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Rcc_Ext_EnableHsi48(whal_Clock *clkDev, uint8_t enable);

#endif /* WHAL_STM32H5_RCC_H */
