#ifndef WHAL_STM32F0_RCC_H
#define WHAL_STM32F0_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32f0_rcc.h
 * @brief STM32F0 RCC (Reset and Clock Control) driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init.
 *
 * Clock sources:
 *   HSI   = 8 MHz internal RC
 *   HSE   = 4-32 MHz external
 *   HSI48 = 48 MHz internal (F04x/F07x/F09x only)
 *   PLL   = source/PREDIV * PLLMUL
 */

/*
 * @brief System clock source selection (RCC_CFGR.SW).
 */
typedef enum {
    WHAL_STM32F0_RCC_SYSCLK_SRC_HSI,
    WHAL_STM32F0_RCC_SYSCLK_SRC_HSE,
    WHAL_STM32F0_RCC_SYSCLK_SRC_PLL,
    WHAL_STM32F0_RCC_SYSCLK_SRC_HSI48,
} whal_Stm32f0_Rcc_SysClockSrc;

/*
 * @brief PLL input selection.
 */
typedef enum {
    WHAL_STM32F0_RCC_PLLSRC_HSI_DIV2,
    WHAL_STM32F0_RCC_PLLSRC_HSI_PREDIV,
    WHAL_STM32F0_RCC_PLLSRC_HSE_PREDIV,
    WHAL_STM32F0_RCC_PLLSRC_HSI48_PREDIV,
} whal_Stm32f0_Rcc_PllClockSrc;

/*
 * @brief PLL configuration parameters.
 *   prediv: 1-16 (encoded as value-1)
 *   pllmul: 2-16 (encoded as value-2)
 */
typedef struct {
    whal_Stm32f0_Rcc_PllClockSrc clkSrc;
    uint8_t prediv;
    uint8_t pllmul;
} whal_Stm32f0_Rcc_PllCfg;

/*
 * @brief Peripheral clock descriptor.
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32f0_Rcc_PeriphClk;

/*
 * @brief Cfg for EnableOsc/DisableOsc — on bit + ready bit.
 */
typedef struct {
    size_t onReg;
    size_t onMsk;
    size_t rdyReg;
    size_t rdyMsk;
    size_t rdyPos;
} whal_Stm32f0_Rcc_OscCfg;

#define WHAL_STM32F0_RCC_HSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  0),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL <<  1), .rdyPos =  1
#define WHAL_STM32F0_RCC_HSE_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL << 16),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 17), .rdyPos = 17
#define WHAL_STM32F0_RCC_HSI48_CFG                           \
    .onReg  = 0x034, .onMsk  = (1UL << 16),                  \
    .rdyReg = 0x034, .rdyMsk = (1UL << 17), .rdyPos = 17

/*
 * @brief Enable an oscillator (HSI/HSE/HSI48). Blocks until ready.
 */
whal_Error whal_Stm32f0_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32f0_Rcc_OscCfg *cfg);
/*
 * @brief Disable an oscillator.
 */
whal_Error whal_Stm32f0_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32f0_Rcc_OscCfg *cfg);

/*
 * @brief Configure the PLL (PREDIV, PLLMUL, source) and enable it.
 *        Caller must have the PLL source oscillator already enabled.
 *        Blocks until PLL is ready.
 */
whal_Error whal_Stm32f0_Rcc_EnablePll(whal_Clock *clkDev,
                                     const whal_Stm32f0_Rcc_PllCfg *cfg);
/*
 * @brief Disable the PLL.
 */
whal_Error whal_Stm32f0_Rcc_DisablePll(whal_Clock *clkDev);

/*
 * @brief Switch SYSCLK to the given source. Blocks until SWS confirms.
 */
whal_Error whal_Stm32f0_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32f0_Rcc_SysClockSrc src);

/*
 * @brief Enable a peripheral clock.
 */
whal_Error whal_Stm32f0_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32f0_Rcc_PeriphClk *clk);
/*
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Stm32f0_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32f0_Rcc_PeriphClk *clk);

#endif /* WHAL_STM32F0_RCC_H */
