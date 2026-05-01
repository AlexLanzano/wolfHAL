#ifndef WHAL_STM32N6_RCC_H
#define WHAL_STM32N6_RCC_H

/**
 * @file stm32n6_rcc.h
 * @brief STM32N6 RCC (Reset and Clock Control) driver.
 *
 * Boards bring up the clock tree imperatively from Board_Init.
 *
 * Differences from STM32H5/WBA:
 *   - Four PLLs (PLL1-PLL4) with DIVM/DIVN/POSTDIV1/POSTDIV2 scheme
 *   - Separate SYSSW and CPUSW fields in CFGR1
 *   - PLL1 configuration split across PLL1CFGR1/PLL1CFGR2/PLL1CFGR3
 */

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/clock/clock.h>

/**
 * @brief System clock switch selection (RCC_CFGR1.SYSSW).
 */
typedef enum {
    WHAL_STM32N6_RCC_SYSCLK_SRC_HSI = 0,
    WHAL_STM32N6_RCC_SYSCLK_SRC_MSI = 1,
    WHAL_STM32N6_RCC_SYSCLK_SRC_HSE = 2,
    WHAL_STM32N6_RCC_SYSCLK_SRC_IC2 = 3,
} whal_Stm32n6_Rcc_SysClockSrc;

/**
 * @brief CPU clock switch selection (RCC_CFGR1.CPUSW).
 */
typedef enum {
    WHAL_STM32N6_RCC_CPUCLK_SRC_HSI = 0,
    WHAL_STM32N6_RCC_CPUCLK_SRC_MSI = 1,
    WHAL_STM32N6_RCC_CPUCLK_SRC_HSE = 2,
    WHAL_STM32N6_RCC_CPUCLK_SRC_IC1 = 3,
} whal_Stm32n6_Rcc_CpuClockSrc;

/**
 * @brief Ethernet 1 PHY interface selection (RCC_CCIPR2.ETH1SEL).
 */
typedef enum {
    WHAL_STM32N6_RCC_ETH1_IF_MII   = 0,
    WHAL_STM32N6_RCC_ETH1_IF_RGMII = 1,
    WHAL_STM32N6_RCC_ETH1_IF_RMII  = 4,
} whal_Stm32n6_Rcc_Eth1If;

/**
 * @brief PLL1 input clock source (RCC_PLL1CFGR1.PLL1SEL).
 */
typedef enum {
    WHAL_STM32N6_RCC_PLL1SRC_HSI     = 0,
    WHAL_STM32N6_RCC_PLL1SRC_MSI     = 1,
    WHAL_STM32N6_RCC_PLL1SRC_HSE     = 2,
    WHAL_STM32N6_RCC_PLL1SRC_I2SCKIN = 3,
} whal_Stm32n6_Rcc_Pll1Src;

/**
 * @brief PLL1 configuration parameters.
 *   VCO = (input / m) * n
 *   PLL1 output = VCO / pdiv1 / pdiv2
 */
typedef struct {
    whal_Stm32n6_Rcc_Pll1Src clkSrc;
    uint16_t n;
    uint8_t m;
    uint8_t pdiv1;
    uint8_t pdiv2;
} whal_Stm32n6_Rcc_Pll1Cfg;

/**
 * @brief Peripheral clock descriptor.
 */
typedef struct {
    size_t regOffset;
    size_t enableMask;
    size_t enablePos;
} whal_Stm32n6_Rcc_PeriphClk;

/**
 * @brief Cfg for EnableOsc/DisableOsc — on bit + ready bit.
 */
typedef struct {
    size_t onReg;
    size_t onMsk;
    size_t rdyReg;
    size_t rdyMsk;
    size_t rdyPos;
} whal_Stm32n6_Rcc_OscCfg;

#define WHAL_STM32N6_RCC_LSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  0),                  \
    .rdyReg = 0x004, .rdyMsk = (1UL <<  0), .rdyPos =  0
#define WHAL_STM32N6_RCC_LSE_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  1),                  \
    .rdyReg = 0x004, .rdyMsk = (1UL <<  1), .rdyPos =  1
#define WHAL_STM32N6_RCC_MSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  2),                  \
    .rdyReg = 0x004, .rdyMsk = (1UL <<  2), .rdyPos =  2
#define WHAL_STM32N6_RCC_HSI_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  3),                  \
    .rdyReg = 0x004, .rdyMsk = (1UL <<  3), .rdyPos =  3
#define WHAL_STM32N6_RCC_HSE_CFG                             \
    .onReg  = 0x000, .onMsk  = (1UL <<  4),                  \
    .rdyReg = 0x004, .rdyMsk = (1UL <<  4), .rdyPos =  4

/**
 * @brief Enable an oscillator (HSI/MSI/HSE/LSI/LSE). Blocks until ready.
 */
whal_Error whal_Stm32n6_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32n6_Rcc_OscCfg *cfg);
/**
 * @brief Disable an oscillator.
 */
whal_Error whal_Stm32n6_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32n6_Rcc_OscCfg *cfg);

/**
 * @brief Configure and enable PLL1. Caller must have the PLL1 source
 *        oscillator already enabled. Blocks until PLL1 is ready.
 */
whal_Error whal_Stm32n6_Rcc_EnablePll1(whal_Clock *clkDev,
                                      const whal_Stm32n6_Rcc_Pll1Cfg *cfg);
/**
 * @brief Disable PLL1.
 */
whal_Error whal_Stm32n6_Rcc_DisablePll1(whal_Clock *clkDev);

/**
 * @brief Switch SYSCLK to the given source. Blocks until SYSSWS confirms.
 */
whal_Error whal_Stm32n6_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32n6_Rcc_SysClockSrc src);
/**
 * @brief Switch CPU clock to the given source. Blocks until CPUSWS confirms.
 */
whal_Error whal_Stm32n6_Rcc_SetCpuClock(whal_Clock *clkDev,
                                       whal_Stm32n6_Rcc_CpuClockSrc src);

/**
 * @brief Select the ETH1 MAC-PHY interface (RCC_CCIPR2.ETH1SEL).
 *        Must be programmed while ETH1 is in reset and before its clocks
 *        are enabled.
 */
whal_Error whal_Stm32n6_Rcc_SetEth1If(whal_Clock *clkDev,
                                     whal_Stm32n6_Rcc_Eth1If mode);

/**
 * @brief Enable a peripheral clock.
 */
whal_Error whal_Stm32n6_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32n6_Rcc_PeriphClk *clk);
/**
 * @brief Disable a peripheral clock.
 */
whal_Error whal_Stm32n6_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32n6_Rcc_PeriphClk *clk);

#endif /* WHAL_STM32N6_RCC_H */
