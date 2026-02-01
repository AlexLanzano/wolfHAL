#ifndef WHAL_STM32WB_RCC_H
#define WHAL_STM32WB_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/flash/stm32wb_flash.h>
#include <stddef.h>

/*
 * @file stm32wb_rcc.h
 * @brief STM32 RCC driver configuration for system and peripheral clocks.
 */

/*
 * @brief Possible system clock sources.
 */
typedef enum {
    WHAL_STM32WB_RCC_SYSCLK_SRC_MSI,
    WHAL_STM32WB_RCC_SYSCLK_SRC_HSI16,
    WHAL_STM32WB_RCC_SYSCLK_SRC_HSE,
    WHAL_STM32WB_RCC_SYSCLK_SRC_PLL,
} whal_Stm32wbRcc_SysClockSrc;

/*
 * @brief PLL input clock sources.
 */
typedef enum {
    WHAL_STM32WB_RCC_PLLCLK_SRC_NONE,
    WHAL_STM32WB_RCC_PLLCLK_SRC_MSI,
    WHAL_STM32WB_RCC_PLLCLK_SRC_HSI16,
    WHAL_STM32WB_RCC_PLLCLK_SRC_HSE,
} whal_Stm32wbRcc_PllClockSrc;

typedef enum whal_Stm32wbRcc_MsiRange {
    WHAL_STM32WB_RCC_MSIRANGE_100kHz,
    WHAL_STM32WB_RCC_MSIRANGE_200kHz,
    WHAL_STM32WB_RCC_MSIRANGE_400kHz,
    WHAL_STM32WB_RCC_MSIRANGE_800kHz,
    WHAL_STM32WB_RCC_MSIRANGE_1MHz,
    WHAL_STM32WB_RCC_MSIRANGE_2MHz,
    WHAL_STM32WB_RCC_MSIRANGE_4MHz,
    WHAL_STM32WB_RCC_MSIRANGE_8MHz,
    WHAL_STM32WB_RCC_MSIRANGE_16MHz,
    WHAL_STM32WB_RCC_MSIRANGE_24MHz,
    WHAL_STM32WB_RCC_MSIRANGE_32MHz,
    WHAL_STM32WB_RCC_MSIRANGE_48MHz,
} whal_Stm32wbRcc_MsiRange;
/*
 * @brief PLL configuration parameters.
 */
typedef struct whal_Stm32wbRcc_PllClkCfg {
    whal_Stm32wbRcc_PllClockSrc clkSrc;
    uint8_t r;
    uint8_t q;
    uint8_t p;
    uint8_t n;
    uint8_t m;
} whal_Stm32wbRcc_PllClkCfg;

/*
 * @brief MSI clock configuration parameters.
 */
typedef struct whal_Stm32wbRcc_MsiClkCfg {
    whal_Stm32wbRcc_MsiRange freq;
} whal_Stm32wbRcc_MsiClkCfg;

/*
 * @brief Register and mask pair for gating peripheral clocks.
 */
typedef struct whal_Stm32wbRcc_Clk {
    size_t regOffset;
    size_t enableMask;
} whal_Stm32wbRcc_Clk;

/*
 * @brief Composite configuration for the STM32 RCC driver.
 */
typedef struct whal_Stm32wbRcc_Cfg {
    whal_Flash *flash;
    whal_Stm32wbFlash_Latency flashLatency;

    whal_Stm32wbRcc_SysClockSrc sysClkSrc;
    void *sysClkCfg;
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

#endif /* WHAL_STM32WB_RCC_H */
