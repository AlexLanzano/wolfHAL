#ifndef WHAL_STM32N6_RCC_H
#define WHAL_STM32N6_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32n6_rcc.h
 * @brief STM32N6 RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32N6 RCC peripheral controls:
 * - System clock source selection (HSI, MSI, HSE, IC1)
 * - CPU clock source selection (HSI, MSI, HSE, IC1)
 * - PLL1/PLL2/PLL3/PLL4 configuration
 * - Peripheral clock gating (AHB1-5, APB1-5 buses)
 * - Bus clock prescalers
 *
 * Two driver variants are provided:
 * - whal_Stm32n6RccPll_Driver: Uses PLL1 as CPU clock source via IC1
 * - whal_Stm32n6RccHsi_Driver: Uses HSI oscillator as system clock source
 */

/*
 * @brief CPU clock source selection.
 *
 * Determines which oscillator/IC drives the CPU clock (sys_cpu_ck).
 */
typedef enum {
    WHAL_STM32N6_RCC_CPUCLK_SRC_HSI,  /* 64 MHz High-Speed Internal */
    WHAL_STM32N6_RCC_CPUCLK_SRC_MSI,  /* 4 MHz Low-Power Internal */
    WHAL_STM32N6_RCC_CPUCLK_SRC_HSE,  /* High-Speed External crystal */
    WHAL_STM32N6_RCC_CPUCLK_SRC_IC1,  /* IC1 divider output */
} whal_Stm32n6Rcc_CpuClockSrc;

/*
 * @brief System bus clock source selection.
 *
 * Determines which oscillator/IC drives the system bus clocks.
 */
typedef enum {
    WHAL_STM32N6_RCC_SYSCLK_SRC_HSI,  /* 64 MHz High-Speed Internal */
    WHAL_STM32N6_RCC_SYSCLK_SRC_MSI,  /* 4/16 MHz Low-Power Internal */
    WHAL_STM32N6_RCC_SYSCLK_SRC_HSE,  /* High-Speed External crystal */
    WHAL_STM32N6_RCC_SYSCLK_SRC_IC2,  /* IC2 divider output (from PLL) */
} whal_Stm32n6Rcc_SysClockSrc;

/*
 * @brief PLL input clock source selection.
 *
 * Determines which oscillator feeds PLL1/PLL2/PLL3/PLL4.
 */
typedef enum {
    WHAL_STM32N6_RCC_PLLCLK_SRC_HSI,     /* HSI as PLL input */
    WHAL_STM32N6_RCC_PLLCLK_SRC_MSI,     /* MSI as PLL input */
    WHAL_STM32N6_RCC_PLLCLK_SRC_HSE,     /* HSE as PLL input */
    WHAL_STM32N6_RCC_PLLCLK_SRC_I2S,     /* I2S_CKIN as PLL input */
} whal_Stm32n6Rcc_PllClockSrc;

/*
 * @brief PLL1 configuration parameters.
 *
 * The PLL1 output frequency is calculated as:
 *   f_vco = (f_input / m) * n
 *   f_postdiv1 = f_vco / p1
 *   f_postdiv2 = f_postdiv1 / p2
 *
 * Constraints:
 *   - PLL input (f_input / m) should be close to f_vco / 16
 *   - VCO frequency range depends on configuration
 *   - m: 1-63 (DIVM field)
 *   - n: 16-640 (DIVN field, integer mode)
 *   - p1, p2: 1-7 (PDIV1, PDIV2 fields)
 */
typedef struct whal_Stm32n6Rcc_PllClkCfg {
    whal_Stm32n6Rcc_PllClockSrc clkSrc; /* PLL input source */
    uint16_t n;  /* PLLN multiplier (16-640) */
    uint8_t m;   /* PLLM divider (1-63) */
    uint8_t p1;  /* PLLP1 divider (1-7) */
    uint8_t p2;  /* PLLP2 divider (1-7) */
    uint8_t ic1div; /* IC1 divider (0=bypass, 1-255 divides by value+1) */
} whal_Stm32n6Rcc_PllClkCfg;

/*
 * @brief HSI clock configuration parameters.
 *
 * The HSI is a 64 MHz internal RC oscillator with a configurable divider.
 * Default after reset is 64 MHz (div1).
 */
typedef struct whal_Stm32n6Rcc_HsiClkCfg {
    uint8_t div; /* HSI divider (0=div1, 1=div2, 2=div4, 3=div8) */
} whal_Stm32n6Rcc_HsiClkCfg;

/*
 * @brief Peripheral clock enable descriptor.
 *
 * Describes the register offset and bit mask needed to enable/disable
 * a peripheral's bus clock. Used with whal_Stm32n6Rcc_Enable/Disable.
 */
typedef struct whal_Stm32n6Rcc_Clk {
    size_t regOffset;   /* Offset from RCC base to enable register */
    size_t enableMask;  /* Bit mask for the peripheral enable bit */
    size_t enablePos;   /* Bit position for the peripheral enable bit */
} whal_Stm32n6Rcc_Clk;

/*
 * @brief RCC driver configuration.
 *
 * Contains all parameters needed to configure the system clock.
 */
typedef struct whal_Stm32n6Rcc_Cfg {
    void *sysClkCfg; /* Pointer to PllClkCfg or HsiClkCfg based on driver */
} whal_Stm32n6Rcc_Cfg;

/*
 * @brief Driver instance for PLL1 clock source (via IC1 to CPU).
 */
extern const whal_ClockDriver whal_Stm32n6RccPll_Driver;

/*
 * @brief Driver instance for HSI clock source.
 */
extern const whal_ClockDriver whal_Stm32n6RccHsi_Driver;

whal_Error whal_Stm32n6RccPll_Init(whal_Clock *clkDev);
whal_Error whal_Stm32n6RccPll_Deinit(whal_Clock *clkDev);
whal_Error whal_Stm32n6RccHsi_Init(whal_Clock *clkDev);
whal_Error whal_Stm32n6RccHsi_Deinit(whal_Clock *clkDev);
whal_Error whal_Stm32n6Rcc_Enable(whal_Clock *clkDev, const void *clk);
whal_Error whal_Stm32n6Rcc_Disable(whal_Clock *clkDev, const void *clk);
whal_Error whal_Stm32n6RccPll_GetRate(whal_Clock *clkDev, size_t *rateOut);
whal_Error whal_Stm32n6RccHsi_GetRate(whal_Clock *clkDev, size_t *rateOut);

#endif /* WHAL_STM32N6_RCC_H */
