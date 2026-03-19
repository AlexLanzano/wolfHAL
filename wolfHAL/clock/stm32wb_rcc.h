#ifndef WHAL_STM32WB_RCC_H
#define WHAL_STM32WB_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32wb_rcc.h
 * @brief STM32WB RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32WB RCC peripheral controls:
 * - System clock source selection (MSI, HSI16, HSE, PLL)
 * - PLL configuration for high-speed operation
 * - Peripheral clock gating (AHB, APB1, APB2 buses)
 * - Bus clock prescalers
 *
 * Two driver variants are provided:
 * - whal_Stm32wbRccPll_Driver: Uses PLL as system clock source
 * - whal_Stm32wbRccMsi_Driver: Uses MSI oscillator as system clock source
 *
 * When changing clock speeds, flash latency must be adjusted appropriately
 * to ensure reliable flash access at the new frequency.
 */

/*
 * @brief System clock source selection.
 *
 * Determines which oscillator/PLL drives the system clock (SYSCLK).
 */
typedef enum {
    WHAL_STM32WB_RCC_SYSCLK_SRC_MSI,   /* Multi-Speed Internal oscillator */
    WHAL_STM32WB_RCC_SYSCLK_SRC_HSI16, /* 16 MHz High-Speed Internal */
    WHAL_STM32WB_RCC_SYSCLK_SRC_HSE,   /* High-Speed External crystal */
    WHAL_STM32WB_RCC_SYSCLK_SRC_PLL,   /* PLL output */
} whal_Stm32wbRcc_SysClockSrc;

/*
 * @brief PLL input clock source selection.
 *
 * Determines which oscillator feeds the PLL input.
 */
typedef enum {
    WHAL_STM32WB_RCC_PLLCLK_SRC_NONE,  /* No clock (PLL disabled) */
    WHAL_STM32WB_RCC_PLLCLK_SRC_MSI,   /* MSI as PLL input */
    WHAL_STM32WB_RCC_PLLCLK_SRC_HSI16, /* HSI16 as PLL input */
    WHAL_STM32WB_RCC_PLLCLK_SRC_HSE,   /* HSE as PLL input */
} whal_Stm32wbRcc_PllClockSrc;

/*
 * @brief MSI oscillator frequency range selection.
 *
 * The MSI is a low-power RC oscillator with selectable frequency ranges.
 * Default after reset is 4 MHz.
 */
typedef enum whal_Stm32wbRcc_MsiRange {
    WHAL_STM32WB_RCC_MSIRANGE_100kHz,
    WHAL_STM32WB_RCC_MSIRANGE_200kHz,
    WHAL_STM32WB_RCC_MSIRANGE_400kHz,
    WHAL_STM32WB_RCC_MSIRANGE_800kHz,
    WHAL_STM32WB_RCC_MSIRANGE_1MHz,
    WHAL_STM32WB_RCC_MSIRANGE_2MHz,
    WHAL_STM32WB_RCC_MSIRANGE_4MHz,   /* Default after reset */
    WHAL_STM32WB_RCC_MSIRANGE_8MHz,
    WHAL_STM32WB_RCC_MSIRANGE_16MHz,
    WHAL_STM32WB_RCC_MSIRANGE_24MHz,
    WHAL_STM32WB_RCC_MSIRANGE_32MHz,
    WHAL_STM32WB_RCC_MSIRANGE_48MHz,
} whal_Stm32wbRcc_MsiRange;

/*
 * @brief PLL configuration parameters.
 *
 * The PLL output frequency is calculated as:
 *   f_vco = (f_input / m) * n
 *   f_pllr = f_vco / r  (main PLL output, used for SYSCLK)
 *   f_pllq = f_vco / q  (used for USB, RNG, etc.)
 *   f_pllp = f_vco / p  (used for SAI, etc.)
 *
 * Constraints:
 *   - VCO frequency must be 96-344 MHz
 *   - PLL input (f_input / m) must be 2.66-16 MHz
 *   - n: 8-127
 *   - m: 1-8 (register value 0-7)
 *   - r, q: 2, 4, 6, 8 (register value 0-3 maps to div by 2/4/6/8)
 *   - p: 2-32 (register value 1-31, 0 reserved)
 */
typedef struct whal_Stm32wbRcc_PllClkCfg {
    whal_Stm32wbRcc_PllClockSrc clkSrc; /* PLL input source */
    uint8_t r; /* PLLR divider (0=div2, 1=div4, 2=div6, 3=div8) */
    uint8_t q; /* PLLQ divider (0=div2, 1=div4, 2=div6, 3=div8) */
    uint8_t p; /* PLLP divider (2-32) */
    uint8_t n; /* PLLN multiplier (8-127) */
    uint8_t m; /* PLLM divider (0-7 maps to 1-8) */
} whal_Stm32wbRcc_PllClkCfg;

/*
 * @brief MSI clock configuration parameters.
 */
typedef struct whal_Stm32wbRcc_MsiClkCfg {
    whal_Stm32wbRcc_MsiRange freq; /* MSI frequency range */
} whal_Stm32wbRcc_MsiClkCfg;

/*
 * @brief Peripheral clock enable descriptor.
 *
 * Describes the register offset and bit mask needed to enable/disable
 * a peripheral's bus clock. Used with whal_Stm32wbRcc_Enable/Disable.
 *
 * Example for GPIOA:
 *   { .regOffset = 0x04C, .enableMask = (1 << 0), .enablePos = 0 }  // AHB2ENR.GPIOAEN
 */
typedef struct whal_Stm32wbRcc_Clk {
    size_t regOffset;   /* Offset from RCC base to enable register */
    size_t enableMask;  /* Bit mask for the peripheral enable bit */
    size_t enablePos;   /* Bit position for the peripheral enable bit */
} whal_Stm32wbRcc_Clk;

/*
 * @brief RCC driver configuration.
 *
 * Contains all parameters needed to configure the system clock.
 */
typedef struct whal_Stm32wbRcc_Cfg {
    whal_Stm32wbRcc_SysClockSrc sysClkSrc;   /* System clock source */
    void *sysClkCfg; /* Pointer to PllClkCfg or MsiClkCfg based on driver */
} whal_Stm32wbRcc_Cfg;

/*
 * @brief Driver instance for the STM32 RCC clock controller.
 */
extern const whal_ClockDriver whal_Stm32wbRccPll_Driver;
extern const whal_ClockDriver whal_Stm32wbRccMsi_Driver;

/*
 * @brief Initialize the RCC peripheral.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRccPll_Init(whal_Clock *clkDev);
/*
 * @brief Deinitialize the RCC peripheral.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRccPll_Deinit(whal_Clock *clkDev);
/*
 * @brief Initialize the RCC peripheral.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRccMsi_Init(whal_Clock *clkDev);
/*
 * @brief Deinitialize the RCC peripheral.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRccMsi_Deinit(whal_Clock *clkDev);
/*
 * @brief Enable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32wbRcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock enabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRcc_Enable(whal_Clock *clkDev, const void *clk);
/*
 * @brief Disable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32wbRcc_Clk descriptor.
 *
 * @retval WHAL_SUCCESS Clock disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRcc_Disable(whal_Clock *clkDev, const void *clk);
/*
 * @brief Compute the current system clock rate.
 *
 * @param clkDev  Clock device instance.
 * @param rateOut Output for the computed rate in Hz.
 *
 * @retval WHAL_SUCCESS Rate computed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRccPll_GetRate(whal_Clock *clkDev, size_t *rateOut);
/*
 * @brief Compute the current system clock rate.
 *
 * @param clkDev  Clock device instance.
 * @param rateOut Output for the computed rate in Hz.
 *
 * @retval WHAL_SUCCESS Rate computed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRccMsi_GetRate(whal_Clock *clkDev, size_t *rateOut);
/*
 * @brief Enable or disable the HSI48 oscillator required by the RNG peripheral.
 *
 * @param clkDev Clock controller instance.
 * @param enable 1 to enable, 0 to disable.
 *
 * @retval WHAL_SUCCESS HSI48 enabled and ready, or disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRcc_Ext_EnableHsi48(whal_Clock *clkDev, uint8_t enable);

#endif /* WHAL_STM32WB_RCC_H */
