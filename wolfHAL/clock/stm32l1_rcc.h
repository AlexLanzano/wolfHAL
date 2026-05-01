#ifndef WHAL_STM32L1_RCC_H
#define WHAL_STM32L1_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32l1_rcc.h
 * @brief STM32L1 RCC (Reset and Clock Control) driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init.
 *
 * Clock sources:
 *   MSI = multispeed internal (default after reset)
 *   HSI = 16 MHz internal RC
 *   HSE = 1-24 MHz external
 *   PLL = HSI or HSE * PLLMUL / PLLDIV (max 32 MHz)
 */

/*
 * @brief System clock source selection (RCC_CFGR.SW).
 */
typedef enum {
    WHAL_STM32L1_RCC_SYSCLK_SRC_MSI,
    WHAL_STM32L1_RCC_SYSCLK_SRC_HSI,
    WHAL_STM32L1_RCC_SYSCLK_SRC_HSE,
    WHAL_STM32L1_RCC_SYSCLK_SRC_PLL,
} whal_Stm32l1_Rcc_SysClockSrc;

/*
 * @brief PLL input selection.
 */
typedef enum {
    WHAL_STM32L1_RCC_PLLSRC_HSI,
    WHAL_STM32L1_RCC_PLLSRC_HSE,
} whal_Stm32l1_Rcc_PllClockSrc;

/*
 * @brief PLL multiplication factor (RCC_CFGR.PLLMUL).
 */
typedef enum {
    WHAL_STM32L1_RCC_PLLMUL_3  = 0,
    WHAL_STM32L1_RCC_PLLMUL_4  = 1,
    WHAL_STM32L1_RCC_PLLMUL_6  = 2,
    WHAL_STM32L1_RCC_PLLMUL_8  = 3,
    WHAL_STM32L1_RCC_PLLMUL_12 = 4,
    WHAL_STM32L1_RCC_PLLMUL_16 = 5,
    WHAL_STM32L1_RCC_PLLMUL_24 = 6,
    WHAL_STM32L1_RCC_PLLMUL_32 = 7,
    WHAL_STM32L1_RCC_PLLMUL_48 = 8,
} whal_Stm32l1_Rcc_PllMul;

/*
 * @brief PLL output division factor (RCC_CFGR.PLLDIV).
 */
typedef enum {
    WHAL_STM32L1_RCC_PLLDIV_2 = 1,
    WHAL_STM32L1_RCC_PLLDIV_3 = 2,
    WHAL_STM32L1_RCC_PLLDIV_4 = 3,
} whal_Stm32l1_Rcc_PllDiv;

/*
 * @brief PLL configuration parameters.
 */
typedef struct {
    whal_Stm32l1_Rcc_PllClockSrc clkSrc;
    whal_Stm32l1_Rcc_PllMul pllmul;
    whal_Stm32l1_Rcc_PllDiv plldiv;
} whal_Stm32l1_Rcc_PllCfg;

/*
 * @brief Peripheral clock descriptor.
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32l1_Rcc_PeriphClk;

/*
 * @brief Cfg for EnableOsc/DisableOsc — on bit + ready bit.
 */
typedef struct {
    size_t onReg;
    size_t onMsk;
    size_t rdyReg;
    size_t rdyMsk;
    size_t rdyPos;
} whal_Stm32l1_Rcc_OscCfg;

#define WHAL_STM32L1_RCC_HSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  0),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL <<  1), .rdyPos =  1
#define WHAL_STM32L1_RCC_HSE_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL << 16),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 17), .rdyPos = 17

/*
 * @brief Enable an oscillator (HSI/HSE). Blocks until ready.
 */
whal_Error whal_Stm32l1_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32l1_Rcc_OscCfg *cfg);
/*
 * @brief Disable an oscillator.
 */
whal_Error whal_Stm32l1_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32l1_Rcc_OscCfg *cfg);

/*
 * @brief Configure and enable the PLL. Caller must have the PLL source
 *        oscillator already enabled. Blocks until PLL is ready.
 */
whal_Error whal_Stm32l1_Rcc_EnablePll(whal_Clock *clkDev,
                                     const whal_Stm32l1_Rcc_PllCfg *cfg);
/*
 * @brief Disable the PLL.
 */
whal_Error whal_Stm32l1_Rcc_DisablePll(whal_Clock *clkDev);

/*
 * @brief Switch SYSCLK to the given source. Blocks until SWS confirms.
 */
whal_Error whal_Stm32l1_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32l1_Rcc_SysClockSrc src);

/*
 * @brief Enable a peripheral clock.
 */
whal_Error whal_Stm32l1_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32l1_Rcc_PeriphClk *clk);
/*
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Stm32l1_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32l1_Rcc_PeriphClk *clk);

#endif /* WHAL_STM32L1_RCC_H */
