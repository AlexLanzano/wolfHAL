#ifndef WHAL_STM32F4_RCC_H
#define WHAL_STM32F4_RCC_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <stddef.h>

/*
 * @file stm32f4_rcc.h
 * @brief STM32F4 RCC (Reset and Clock Control) driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init.
 *
 * PLL output:
 *   f_vco  = (f_input / m) * n
 *   f_pll  = f_vco / p   (SYSCLK)
 *   f_pllq = f_vco / q   (USB / SDIO)
 */

/*
 * @brief System clock source selection (RCC_CFGR.SW).
 */
typedef enum {
    WHAL_STM32F4_RCC_SYSCLK_SRC_HSI,
    WHAL_STM32F4_RCC_SYSCLK_SRC_HSE,
    WHAL_STM32F4_RCC_SYSCLK_SRC_PLL,
} whal_Stm32f4_Rcc_SysClockSrc;

/*
 * @brief PLL input selection (RCC_PLLCFGR.PLLSRC).
 */
typedef enum {
    WHAL_STM32F4_RCC_PLLCLK_SRC_HSI,
    WHAL_STM32F4_RCC_PLLCLK_SRC_HSE,
} whal_Stm32f4_Rcc_PllClockSrc;

/*
 * @brief PLL configuration parameters.
 *   m: 2-63   (VCO input 1-2 MHz)
 *   n: 50-432 (VCO output 100-432 MHz)
 *   p: 0-3    (div by 2/4/6/8)
 *   q: 2-15
 */
typedef struct {
    whal_Stm32f4_Rcc_PllClockSrc clkSrc;
    uint16_t n;
    uint8_t m;
    uint8_t p;
    uint8_t q;
} whal_Stm32f4_Rcc_PllCfg;

/*
 * @brief Bus prescaler config (APB1/APB2).
 *   ppre1/ppre2: 0=/1, 4=/2, 5=/4, 6=/8, 7=/16
 */
typedef struct {
    uint8_t ppre1;
    uint8_t ppre2;
} whal_Stm32f4_Rcc_BusCfg;

/*
 * @brief Peripheral clock descriptor.
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32f4_Rcc_PeriphClk;

/*
 * @brief Cfg for EnableOsc/DisableOsc — on bit + ready bit.
 */
typedef struct {
    size_t onReg;
    size_t onMsk;
    size_t rdyReg;
    size_t rdyMsk;
    size_t rdyPos;
} whal_Stm32f4_Rcc_OscCfg;

#define WHAL_STM32F4_RCC_HSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  0),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL <<  1), .rdyPos =  1
#define WHAL_STM32F4_RCC_HSE_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL << 16),                  \
    .rdyReg = 0x000, .rdyMsk = (1UL << 17), .rdyPos = 17

/*
 * @brief Enable an oscillator (HSI/HSE). Blocks until ready.
 */
whal_Error whal_Stm32f4_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32f4_Rcc_OscCfg *cfg);
/*
 * @brief Disable an oscillator.
 */
whal_Error whal_Stm32f4_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32f4_Rcc_OscCfg *cfg);

/*
 * @brief Configure and enable the PLL. Blocks until PLL is ready.
 */
whal_Error whal_Stm32f4_Rcc_EnablePll(whal_Clock *clkDev,
                                     const whal_Stm32f4_Rcc_PllCfg *cfg);
/*
 * @brief Disable the PLL.
 */
whal_Error whal_Stm32f4_Rcc_DisablePll(whal_Clock *clkDev);

/*
 * @brief Set APB1 / APB2 bus prescalers.
 */
whal_Error whal_Stm32f4_Rcc_SetBusPrescalers(whal_Clock *clkDev,
                                            const whal_Stm32f4_Rcc_BusCfg *cfg);

/*
 * @brief Switch SYSCLK to the given source. Blocks until SWS confirms.
 */
whal_Error whal_Stm32f4_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32f4_Rcc_SysClockSrc src);

/*
 * @brief Enable a peripheral clock.
 */
whal_Error whal_Stm32f4_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32f4_Rcc_PeriphClk *clk);
/*
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Stm32f4_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32f4_Rcc_PeriphClk *clk);

#endif /* WHAL_STM32F4_RCC_H */
