#ifndef WHAL_STM32F4_RCC_H
#define WHAL_STM32F4_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32f4_rcc.h
 * @brief STM32F4 RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32F4 RCC peripheral controls:
 * - System clock source selection (HSI, HSE, PLL)
 * - PLL configuration (M, N, P, Q dividers)
 * - Peripheral clock gating (AHB1, APB1, APB2 buses)
 * - Bus clock prescalers
 *
 * PLL output frequency:
 *   f_vco = (f_input / PLLM) * PLLN
 *   f_pll = f_vco / PLLP
 *   f_usb = f_vco / PLLQ
 *
 * One driver variant is provided:
 * - whal_Stm32f4RccPll_Driver: Uses PLL as system clock source
 */

/*
 * @brief System clock source selection.
 *
 * Determines which oscillator/PLL drives the system clock (SYSCLK).
 */
typedef enum {
    WHAL_STM32F4_RCC_SYSCLK_SRC_HSI, /* 16 MHz High-Speed Internal */
    WHAL_STM32F4_RCC_SYSCLK_SRC_HSE, /* High-Speed External crystal */
    WHAL_STM32F4_RCC_SYSCLK_SRC_PLL, /* PLL output */
} whal_Stm32f4Rcc_SysClockSrc;

/*
 * @brief PLL input clock source selection.
 *
 * Bit 22 of RCC_PLLCFGR selects the PLL input.
 */
typedef enum {
    WHAL_STM32F4_RCC_PLLCLK_SRC_HSI, /* HSI (16 MHz) as PLL input */
    WHAL_STM32F4_RCC_PLLCLK_SRC_HSE, /* HSE as PLL input */
} whal_Stm32f4Rcc_PllClockSrc;

/*
 * @brief PLL configuration parameters.
 *
 * The PLL output frequency is calculated as:
 *   f_vco = (f_input / m) * n
 *   f_pll = f_vco / p  (main PLL output, used for SYSCLK)
 *   f_usb = f_vco / q  (used for USB OTG FS, SDIO)
 *
 * Constraints:
 *   - m: 2-63 (VCO input should be 1-2 MHz)
 *   - n: 50-432 (VCO output must be 100-432 MHz)
 *   - p: 2, 4, 6, or 8 (register value 0-3 maps to 2/4/6/8)
 *   - q: 2-15 (USB OTG FS requires 48 MHz)
 */
typedef struct whal_Stm32f4Rcc_PllClkCfg {
    whal_Stm32f4Rcc_PllClockSrc clkSrc; /* PLL input source */
    uint16_t n; /* PLLN multiplier (50-432) */
    uint8_t m;  /* PLLM divider (2-63) */
    uint8_t p;  /* PLLP divider (0=div2, 1=div4, 2=div6, 3=div8) */
    uint8_t q;  /* PLLQ divider (2-15) */
} whal_Stm32f4Rcc_PllClkCfg;

/*
 * @brief Peripheral clock enable descriptor.
 *
 * Describes the register offset and bit mask needed to enable/disable
 * a peripheral's bus clock. Used with whal_Stm32f4Rcc_Enable/Disable.
 *
 * Example for GPIOA:
 *   { .regOffset = 0x030, .enableMask = (1 << 0), .enablePos = 0 }
 */
typedef struct whal_Stm32f4Rcc_Clk {
    size_t regOffset;   /* Offset from RCC base to enable register */
    size_t enableMask;  /* Bit mask for the peripheral enable bit */
    size_t enablePos;   /* Bit position for the peripheral enable bit */
} whal_Stm32f4Rcc_Clk;

/*
 * @brief RCC driver configuration.
 *
 * Contains all parameters needed to configure the system clock.
 */
typedef struct whal_Stm32f4Rcc_Cfg {
    whal_Stm32f4Rcc_SysClockSrc sysClkSrc; /* System clock source */
    void *sysClkCfg; /* Pointer to PllClkCfg based on driver */
    uint8_t ppre1; /* APB1 prescaler (0=/1, 4=/2, 5=/4, 6=/8, 7=/16) */
    uint8_t ppre2; /* APB2 prescaler (0=/1, 4=/2, 5=/4, 6=/8, 7=/16) */
} whal_Stm32f4Rcc_Cfg;

/*
 * @brief Driver instance for the STM32F4 RCC PLL clock controller.
 */
extern const whal_ClockDriver whal_Stm32f4RccPll_Driver;

/*
 * @brief Initialize the RCC peripheral with PLL as system clock.
 *
 * Enables HSE if needed, configures PLL with the parameters in the
 * device configuration, enables PLL, and switches SYSCLK to PLL output.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f4RccPll_Init(whal_Clock *clkDev);

/*
 * @brief Deinitialize the RCC peripheral from PLL mode.
 *
 * Switches SYSCLK back to HSI and disables PLL.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f4RccPll_Deinit(whal_Clock *clkDev);

/*
 * @brief Enable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32f4Rcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock enabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f4Rcc_Enable(whal_Clock *clkDev, const void *clk);

/*
 * @brief Disable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32f4Rcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f4Rcc_Disable(whal_Clock *clkDev, const void *clk);

/*
 * @brief Compute the current system clock rate (PLL mode).
 *
 * Calculates the PLL output frequency from the configured source,
 * multiplier, and dividers.
 *
 * @param clkDev  Clock device instance.
 * @param rateOut Output for the computed rate in Hz.
 *
 * @retval WHAL_SUCCESS Rate computed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32f4RccPll_GetRate(whal_Clock *clkDev, size_t *rateOut);

#endif /* WHAL_STM32F4_RCC_H */
