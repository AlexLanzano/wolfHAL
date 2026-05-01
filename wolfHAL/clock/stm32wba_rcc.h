#ifndef WHAL_STM32WBA_RCC_H
#define WHAL_STM32WBA_RCC_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/clock/clock.h>

/*
 * @file stm32wba_rcc.h
 * @brief STM32WBA RCC (Reset and Clock Control) driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init.
 *
 * Differences from STM32WB:
 *   - No MSI oscillator
 *   - HSE = 32 MHz (HSE32)
 *   - PLL1 split across RCC_PLL1CFGR and RCC_PLL1DIVR
 *   - HPRE5 prescaler for AHB5 domain
 */

/*
 * @brief System clock source selection (RCC_CFGR1.SW).
 */
typedef enum {
    WHAL_STM32WBA_RCC_SYSCLK_SRC_HSI16 = 0,
    WHAL_STM32WBA_RCC_SYSCLK_SRC_HSE32 = 2,
    WHAL_STM32WBA_RCC_SYSCLK_SRC_PLL1  = 3,
} whal_Stm32wba_Rcc_SysClockSrc;

/*
 * @brief PLL1 input clock source selection.
 */
typedef enum {
    WHAL_STM32WBA_RCC_PLL1SRC_NONE  = 0,
    WHAL_STM32WBA_RCC_PLL1SRC_HSI16 = 2,
    WHAL_STM32WBA_RCC_PLL1SRC_HSE32 = 3,
} whal_Stm32wba_Rcc_Pll1Src;

/*
 * @brief PLL1 input reference frequency range.
 */
typedef enum {
    WHAL_STM32WBA_RCC_PLL1RGE_4_8  = 2,
    WHAL_STM32WBA_RCC_PLL1RGE_8_16 = 3,
} whal_Stm32wba_Rcc_Pll1Rge;

/*
 * @brief PLL1 configuration parameters.
 *   f_ref = f_input / (m + 1)    [4-16 MHz]
 *   f_vco = f_ref * n            [128-544 MHz]
 *   f_pll1r = f_vco / (r + 1)    [SYSCLK output, max 100 MHz]
 */
typedef struct {
    whal_Stm32wba_Rcc_Pll1Src clkSrc;
    whal_Stm32wba_Rcc_Pll1Rge rge;
    uint8_t m;
    uint16_t n;
    uint8_t r;
    uint8_t q;
    uint8_t p;
} whal_Stm32wba_Rcc_Pll1Cfg;

/*
 * @brief RNG kernel clock source selection (RCC_CCIPR2.RNGSEL).
 */
typedef enum {
    WHAL_STM32WBA_RCC_RNGSEL_LSE      = 0,
    WHAL_STM32WBA_RCC_RNGSEL_LSI      = 1,
    WHAL_STM32WBA_RCC_RNGSEL_HSI16    = 2,
    WHAL_STM32WBA_RCC_RNGSEL_PLL1Q_D2 = 3,
} whal_Stm32wba_Rcc_RngSrc;

/*
 * @brief Peripheral clock descriptor.
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32wba_Rcc_PeriphClk;

/*
 * @brief Cfg for EnableOsc/DisableOsc — on bit + ready bit.
 */
typedef struct {
    size_t onReg;
    size_t onMsk;
    size_t rdyReg;
    size_t rdyMsk;
    size_t rdyPos;
} whal_Stm32wba_Rcc_OscCfg;

#define WHAL_STM32WBA_RCC_HSI16_CFG                          \
    .onReg  = 0x000, .onMsk  = (1UL <<  8),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 10), .rdyPos = 10
#define WHAL_STM32WBA_RCC_HSE32_CFG                          \
    .onReg  = 0x000, .onMsk  = (1UL << 16),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 17), .rdyPos = 17

/*
 * @brief Enable an oscillator (HSI16/HSE32). Blocks until ready.
 */
whal_Error whal_Stm32wba_Rcc_EnableOsc(whal_Clock *clkDev,
                                      const whal_Stm32wba_Rcc_OscCfg *cfg);
/*
 * @brief Disable an oscillator.
 */
whal_Error whal_Stm32wba_Rcc_DisableOsc(whal_Clock *clkDev,
                                       const whal_Stm32wba_Rcc_OscCfg *cfg);

/*
 * @brief Configure and enable PLL1. Caller must have the PLL1 source
 *        oscillator already enabled. Blocks until PLL1 is ready.
 */
whal_Error whal_Stm32wba_Rcc_EnablePll1(whal_Clock *clkDev,
                                       const whal_Stm32wba_Rcc_Pll1Cfg *cfg);
/*
 * @brief Disable PLL1.
 */
whal_Error whal_Stm32wba_Rcc_DisablePll1(whal_Clock *clkDev);

/*
 * @brief Enable LSI1 oscillator (required by IWDG). Blocks until ready.
 */
whal_Error whal_Stm32wba_Rcc_EnableLsi(whal_Clock *clkDev);
/*
 * @brief Disable LSI1.
 */
whal_Error whal_Stm32wba_Rcc_DisableLsi(whal_Clock *clkDev);

/*
 * @brief Switch SYSCLK to the given source. Blocks until SWS confirms.
 */
whal_Error whal_Stm32wba_Rcc_SetSysClock(whal_Clock *clkDev,
                                        whal_Stm32wba_Rcc_SysClockSrc src);

/*
 * @brief Select the RNG kernel clock source (RCC_CCIPR2.RNGSEL).
 */
whal_Error whal_Stm32wba_Rcc_SetRngClockSrc(whal_Clock *clkDev,
                                           whal_Stm32wba_Rcc_RngSrc src);

/*
 * @brief Set the HPRE5 prescaler for the AHB5 domain (max 32 MHz).
 *   0xx: not divided
 *   100: div 2
 *   101: div 4
 *   110: div 8
 *   111: div 16
 */
whal_Error whal_Stm32wba_Rcc_SetHpre5(whal_Clock *clkDev, uint8_t hpre5);

/*
 * @brief Enable a peripheral clock.
 */
whal_Error whal_Stm32wba_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32wba_Rcc_PeriphClk *clk);
/*
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Stm32wba_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                             const whal_Stm32wba_Rcc_PeriphClk *clk);

#endif /* WHAL_STM32WBA_RCC_H */
