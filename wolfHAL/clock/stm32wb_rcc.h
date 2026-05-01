#ifndef WHAL_STM32WB_RCC_H
#define WHAL_STM32WB_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32wb_rcc.h
 * @brief STM32WB RCC (Reset and Clock Control) driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init by calling
 * the helpers below in order:
 *
 *   whal_Stm32wb_Rcc_EnableMsi(...);
 *   whal_Stm32wb_Rcc_EnablePll(..., &pllCfg);
 *   whal_Stm32wb_Rcc_EnableOsc(..., &hsi48Cfg);
 *   whal_Stm32wb_Rcc_SetSysClock(..., WHAL_STM32WB_RCC_SYSCLK_SRC_PLL);
 *   for each peripheral clock: whal_Stm32wb_Rcc_EnablePeriphClk(...);
 *
 * The driver does not provide a declarative tree or an Init/Deinit
 * walker — boards know their own bring-up order.
 */

/*
 * @brief System clock source selection (RCC_CFGR.SW).
 */
typedef enum {
    WHAL_STM32WB_RCC_SYSCLK_SRC_MSI,
    WHAL_STM32WB_RCC_SYSCLK_SRC_HSI16,
    WHAL_STM32WB_RCC_SYSCLK_SRC_HSE,
    WHAL_STM32WB_RCC_SYSCLK_SRC_PLL,
} whal_Stm32wb_Rcc_SysClockSrc;

/*
 * @brief PLL input clock source selection (RCC_PLLCFGR.PLLSRC).
 */
typedef enum {
    WHAL_STM32WB_RCC_PLLCLK_SRC_NONE,
    WHAL_STM32WB_RCC_PLLCLK_SRC_MSI,
    WHAL_STM32WB_RCC_PLLCLK_SRC_HSI16,
    WHAL_STM32WB_RCC_PLLCLK_SRC_HSE,
} whal_Stm32wb_Rcc_PllClockSrc;

/*
 * @brief MSI oscillator frequency range (RCC_CR.MSIRANGE).
 */
typedef enum {
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
} whal_Stm32wb_Rcc_MsiRange;

/*
 * @brief Peripheral clock descriptor (RCC *ENR enable bit).
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32wb_Rcc_PeriphClk;

/*
 * @brief Cfg for EnableOsc/DisableOsc — on bit + ready bit. Boards
 *        construct one with the WHAL_STM32WB_RCC_*_CFG macros below.
 *        NOTE: LSE assumes PWR_CR1.DBP has been set by the caller.
 */
typedef struct {
    size_t onReg;
    size_t onMsk;
    size_t rdyReg;
    size_t rdyMsk;
    size_t rdyPos;
} whal_Stm32wb_Rcc_OscCfg;

#define WHAL_STM32WB_RCC_HSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  8),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 10), .rdyPos = 10
#define WHAL_STM32WB_RCC_HSE_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL << 16),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 17), .rdyPos = 17
#define WHAL_STM32WB_RCC_HSI48_CFG                           \
    .onReg  = 0x098, .onMsk  = (1UL <<  0),                  \
    .rdyReg = 0x098, .rdyMsk = (1UL <<  1), .rdyPos =  1
#define WHAL_STM32WB_RCC_LSI_CFG                             \
    .onReg  = 0x094, .onMsk  = (1UL <<  0),                  \
    .rdyReg = 0x094, .rdyMsk = (1UL <<  1), .rdyPos =  1
#define WHAL_STM32WB_RCC_LSE_CFG                             \
    .onReg  = 0x090, .onMsk  = (1UL <<  0),                  \
    .rdyReg = 0x090, .rdyMsk = (1UL <<  1), .rdyPos =  1

/*
 * @brief PLL configuration parameters.
 *
 * The PLL output frequency is calculated as:
 *   f_vco  = (f_input / m) * n
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
typedef struct {
    whal_Stm32wb_Rcc_PllClockSrc clkSrc;
    uint8_t r;
    uint8_t q;
    uint8_t p;
    uint8_t n;
    uint8_t m;
} whal_Stm32wb_Rcc_PllCfg;

/*
 * @brief Enable an oscillator (HSI/HSE/HSI48/LSI/LSE) and block until
 *        the ready bit is set.
 */
whal_Error whal_Stm32wb_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32wb_Rcc_OscCfg *cfg);
/*
 * @brief Disable an oscillator.
 */
whal_Error whal_Stm32wb_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32wb_Rcc_OscCfg *cfg);

/*
 * @brief Enable the MSI oscillator at the given range. Blocks until ready.
 */
whal_Error whal_Stm32wb_Rcc_EnableMsi(whal_Clock *clkDev,
                                     whal_Stm32wb_Rcc_MsiRange range);

/*
 * @brief Configure the PLL dividers/source and enable it. Blocks until
 *        the PLL ready bit is set.
 */
whal_Error whal_Stm32wb_Rcc_EnablePll(whal_Clock *clkDev,
                                     const whal_Stm32wb_Rcc_PllCfg *cfg);
/*
 * @brief Disable the PLL.
 */
whal_Error whal_Stm32wb_Rcc_DisablePll(whal_Clock *clkDev);

/*
 * @brief Switch SYSCLK to the given source. Blocks until RCC_CFGR.SWS
 *        reflects the new source.
 */
whal_Error whal_Stm32wb_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32wb_Rcc_SysClockSrc src);

/*
 * @brief Enable a peripheral clock.
 */
whal_Error whal_Stm32wb_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32wb_Rcc_PeriphClk *clk);
/*
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Stm32wb_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32wb_Rcc_PeriphClk *clk);

#endif /* WHAL_STM32WB_RCC_H */
