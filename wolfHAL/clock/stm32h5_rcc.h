#ifndef WHAL_STM32H5_RCC_H
#define WHAL_STM32H5_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32h5_rcc.h
 * @brief STM32H5 RCC (Reset and Clock Control) driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init.
 *
 * Clock sources:
 *   HSI   = 64 MHz internal RC (with HSIDIV)
 *   CSI   = 4 MHz low-power internal
 *   HSE   = external crystal/oscillator
 *   HSI48 = 48 MHz (for RNG/USB)
 *   PLL1  = source/m * (n+1) / (p+1) (SYSCLK)
 */

/*
 * @brief System clock source selection (RCC_CFGR1.SW).
 */
typedef enum {
    WHAL_STM32H5_RCC_SYSCLK_SRC_HSI,
    WHAL_STM32H5_RCC_SYSCLK_SRC_CSI,
    WHAL_STM32H5_RCC_SYSCLK_SRC_HSE,
    WHAL_STM32H5_RCC_SYSCLK_SRC_PLL1,
} whal_Stm32h5_Rcc_SysClockSrc;

/*
 * @brief PLL input clock source selection.
 */
typedef enum {
    WHAL_STM32H5_RCC_PLLCLK_SRC_NONE,
    WHAL_STM32H5_RCC_PLLCLK_SRC_HSI,
    WHAL_STM32H5_RCC_PLLCLK_SRC_CSI,
    WHAL_STM32H5_RCC_PLLCLK_SRC_HSE,
} whal_Stm32h5_Rcc_PllClockSrc;

/*
 * @brief PLL1 configuration parameters.
 *   m: 1-63 (input / m must be 1-16 MHz)
 *   n: 3-511 (VCO = input * (n+1))
 *   p, q, r: 0-127 (output / (value+1))
 */
typedef struct {
    whal_Stm32h5_Rcc_PllClockSrc clkSrc;
    uint16_t n;
    uint8_t m;
    uint8_t p;
    uint8_t q;
    uint8_t r;
} whal_Stm32h5_Rcc_PllCfg;

/*
 * @brief Peripheral clock descriptor.
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32h5_Rcc_PeriphClk;

/*
 * @brief Cfg for EnableOsc/DisableOsc — on bit + ready bit.
 */
typedef struct {
    size_t onReg;
    size_t onMsk;
    size_t rdyReg;
    size_t rdyMsk;
    size_t rdyPos;
} whal_Stm32h5_Rcc_OscCfg;

#define WHAL_STM32H5_RCC_HSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  0),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL <<  1), .rdyPos =  1
#define WHAL_STM32H5_RCC_CSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  8),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL <<  9), .rdyPos =  9
#define WHAL_STM32H5_RCC_HSI48_CFG                           \
    .onReg  = 0x000, .onMsk  = (1UL << 12),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 13), .rdyPos = 13
#define WHAL_STM32H5_RCC_HSE_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL << 16),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 17), .rdyPos = 17

/*
 * @brief Enable an oscillator (HSI/CSI/HSI48/HSE). Blocks until ready.
 */
whal_Error whal_Stm32h5_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32h5_Rcc_OscCfg *cfg);
/*
 * @brief Disable an oscillator.
 */
whal_Error whal_Stm32h5_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32h5_Rcc_OscCfg *cfg);

/*
 * @brief Configure and enable PLL1. Caller must have the PLL source
 *        oscillator already enabled. Blocks until PLL1 is ready.
 */
whal_Error whal_Stm32h5_Rcc_EnablePll1(whal_Clock *clkDev,
                                      const whal_Stm32h5_Rcc_PllCfg *cfg);
/*
 * @brief Disable PLL1.
 */
whal_Error whal_Stm32h5_Rcc_DisablePll1(whal_Clock *clkDev);

/*
 * @brief Switch SYSCLK to the given source. Blocks until SWS confirms.
 */
whal_Error whal_Stm32h5_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32h5_Rcc_SysClockSrc src);

/*
 * @brief Set the HSI divider (0=div1, 1=div2, 2=div4, 3=div8).
 */
whal_Error whal_Stm32h5_Rcc_SetHsiDiv(whal_Clock *clkDev, uint8_t div);

/*
 * @brief Enable a peripheral clock.
 */
whal_Error whal_Stm32h5_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32h5_Rcc_PeriphClk *clk);
/*
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Stm32h5_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32h5_Rcc_PeriphClk *clk);

#endif /* WHAL_STM32H5_RCC_H */
