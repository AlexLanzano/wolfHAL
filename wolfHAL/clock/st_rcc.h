#ifndef WHAL_ST_RCC_H
#define WHAL_ST_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file st_rcc.h
 * @brief STM32 RCC driver configuration for system and peripheral clocks.
 */

/*
 * @brief Possible system clock sources.
 */
typedef enum {
    WHAL_ST_RCC_SYSCLK_SRC_MSI,
    WHAL_ST_RCC_SYSCLK_SRC_HSI16,
    WHAL_ST_RCC_SYSCLK_SRC_HSE,
    WHAL_ST_RCC_SYSCLK_SRC_PLL,
} whal_StRcc_SysClockSrc;

/*
 * @brief PLL input clock sources.
 */
typedef enum {
    WHAL_ST_RCC_PLLCLK_SRC_NONE,
    WHAL_ST_RCC_PLLCLK_SRC_MSI,
    WHAL_ST_RCC_PLLCLK_SRC_HSI16,
    WHAL_ST_RCC_PLLCLK_SRC_HSE,
} whal_StRcc_PllClockSrc;

/*
 * @brief Peripherals that can be clock-gated by this driver.
 */
typedef enum whal_StRcc_PeriphClk {
    WHAL_ST_RCC_PERIPH_GPIOA,
    WHAL_ST_RCC_PERIPH_GPIOB,
    WHAL_ST_RCC_PERIPH_LPUART1,
    WHAL_ST_RCC_PERIPH_FLASH,
} whal_StRcc_PeriphClk;

typedef enum whal_StRcc_MsiRange {
    WHAL_ST_RCC_MSIRANGE_100kHz,
    WHAL_ST_RCC_MSIRANGE_200kHz,
    WHAL_ST_RCC_MSIRANGE_400kHz,
    WHAL_ST_RCC_MSIRANGE_800kHz,
    WHAL_ST_RCC_MSIRANGE_1MHz,
    WHAL_ST_RCC_MSIRANGE_2MHz,
    WHAL_ST_RCC_MSIRANGE_4MHz,
    WHAL_ST_RCC_MSIRANGE_8MHz,
    WHAL_ST_RCC_MSIRANGE_16MHz,
    WHAL_ST_RCC_MSIRANGE_24MHz,
    WHAL_ST_RCC_MSIRANGE_32MHz,
    WHAL_ST_RCC_MSIRANGE_48MHz,
} whal_StRcc_MsiRange;
/*
 * @brief PLL configuration parameters.
 */
typedef struct whal_StRcc_PllClkCfg {
    whal_StRcc_PllClockSrc clkSrc;
    uint8_t r;
    uint8_t q;
    uint8_t p;
    uint8_t n;
    uint8_t m;
} whal_StRcc_PllClkCfg;

/*
 * @brief MSI clock configuration parameters.
 */
typedef struct whal_StRcc_MsiClkCfg {
    size_t freq;
} whal_StRcc_MsiClkCfg;

/*
 * @brief Composite configuration for the STM32 RCC driver.
 */
typedef struct whal_StRcc_Cfg {
    whal_StRcc_SysClockSrc sysClkSrc;

    union {
        whal_StRcc_PllClkCfg pll;
        whal_StRcc_MsiClkCfg msi;
    } sysClkCfg;

    whal_StRcc_PeriphClk *periphClkEn;
    uint8_t periphClkEnCount;
} whal_StRcc_Cfg;

/*
 * @brief Driver instance for the STM32 RCC clock controller.
 */
extern const whal_ClockDriver whal_StRcc_Driver;

/*
 * @brief Initialize the RCC peripheral.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StRcc_Init(whal_Clock *clkDev);
/*
 * @brief Deinitialize the RCC peripheral.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StRcc_Deinit(whal_Clock *clkDev);
/*
 * @brief Enable the configured clocks and peripheral gates.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Clocks enabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StRcc_Enable(whal_Clock *clkDev);
/*
 * @brief Disable the configured clocks and peripheral gates.
 *
 * @param clkDev Clock device instance.
 *
 * @retval WHAL_SUCCESS Clocks disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StRcc_Disable(whal_Clock *clkDev);
/*
 * @brief Compute the current system clock rate.
 *
 * @param clkDev  Clock device instance.
 * @param rateOut Output for the computed rate in Hz.
 *
 * @retval WHAL_SUCCESS Rate computed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StRcc_GetRate(whal_Clock *clkDev, size_t *rateOut);
/*
 * @brief Dispatch a driver-specific RCC command.
 *
 * @param clkDev Clock device instance.
 * @param cmd    Driver-defined command selector.
 * @param args   Optional command arguments.
 *
 * @retval WHAL_SUCCESS Command handled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StRcc_Cmd(whal_Clock *clkDev, size_t cmd, void *args);

#endif /* WHAL_ST_RCC_H */
