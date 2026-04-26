#ifndef WHAL_STM32N6_RCC_H
#define WHAL_STM32N6_RCC_H

/**
 * @file stm32n6_rcc.h
 * @brief STM32N6 RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32N6 RCC peripheral controls:
 * - System clock source selection (HSI, MSI, HSE, PLL1 ic1_ck)
 * - PLL1/PLL2/PLL3/PLL4 configuration
 * - CPU clock source selection (HSI, MSI, HSE, PLL1 ic1_ck)
 * - Peripheral clock gating (AHB1-5, APB1-5 buses)
 * - Bus clock prescalers (HPRE, PPRE1-5, TIMPRE)
 *
 * RCC base address: 0x46028000
 *
 * Two driver variants are provided:
 * - whal_Stm32n6RccPll_Driver: Uses PLL1 as system clock source
 * - whal_Stm32n6RccHsi_Driver: Uses HSI oscillator as system clock source
 */

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/clock/clock.h>

/**
 * @brief System clock switch selection (RCC_CFGR1.SYSSW).
 *
 * Determines which source drives the system bus clocks (sys_bus_ck).
 */
typedef enum {
    WHAL_STM32N6_RCC_SYSCLK_SRC_HSI = 0, /**< 64 MHz HSI (default) */
    WHAL_STM32N6_RCC_SYSCLK_SRC_MSI = 1, /**< 4 MHz MSI */
    WHAL_STM32N6_RCC_SYSCLK_SRC_HSE = 2, /**< HSE crystal */
    WHAL_STM32N6_RCC_SYSCLK_SRC_IC2 = 3, /**< PLL ic2_ck */
} whal_Stm32n6Rcc_SysClockSrc;

/**
 * @brief CPU clock switch selection (RCC_CFGR1.CPUSW).
 *
 * Determines which source drives the CPU clock (sys_cpu_ck).
 */
typedef enum {
    WHAL_STM32N6_RCC_CPUCLK_SRC_HSI = 0, /**< HSI (default) */
    WHAL_STM32N6_RCC_CPUCLK_SRC_MSI = 1, /**< MSI */
    WHAL_STM32N6_RCC_CPUCLK_SRC_HSE = 2, /**< HSE */
    WHAL_STM32N6_RCC_CPUCLK_SRC_IC1 = 3, /**< PLL ic1_ck */
} whal_Stm32n6Rcc_CpuClockSrc;

/**
 * @brief Ethernet 1 PHY interface selection (RCC_CCIPR2.ETH1SEL).
 *
 * Selects the MAC-PHY interface mode. Must be programmed while ETH1 is held
 * in reset and before enabling the ETH1 clocks (RM0486 §14.10.51).
 */
typedef enum {
    WHAL_STM32N6_RCC_ETH1_IF_MII   = 0, /**< MII (default) */
    WHAL_STM32N6_RCC_ETH1_IF_RGMII = 1, /**< RGMII */
    WHAL_STM32N6_RCC_ETH1_IF_RMII  = 4, /**< RMII */
} whal_Stm32n6Rcc_Eth1If;

/**
 * @brief PLL1 input clock source selection (RCC_PLL1CFGR1.PLL1SEL).
 */
typedef enum {
    WHAL_STM32N6_RCC_PLL1SRC_HSI    = 0, /**< HSI as PLL1 reference */
    WHAL_STM32N6_RCC_PLL1SRC_MSI    = 1, /**< MSI as PLL1 reference */
    WHAL_STM32N6_RCC_PLL1SRC_HSE    = 2, /**< HSE as PLL1 reference */
    WHAL_STM32N6_RCC_PLL1SRC_I2SCKIN = 3, /**< I2S_CKIN as PLL1 reference */
} whal_Stm32n6Rcc_Pll1Src;

/**
 * @brief PLL1 configuration parameters.
 *
 * VCO frequency = (f_ref / DIVM) * DIVN
 *   where f_ref is the PLL source clock.
 *
 * PLL1 output = VCO / POSTDIV1 / POSTDIV2
 *
 * Constraints:
 *   - DIVM: 1-63 (0x01-0x3F, 0x00 = PLL disabled)
 *   - DIVN: 16-640 (0x10-0x280)
 *   - POSTDIV1: 1-7 (0=not applicable, 1=div1, ..., 7=div7)
 *   - POSTDIV2: 1-7 (0=not applicable, 1=div1, ..., 7=div7)
 */
typedef struct whal_Stm32n6Rcc_Pll1Cfg {
    whal_Stm32n6Rcc_Pll1Src clkSrc; /**< PLL1 input source */
    uint16_t n;    /**< DIVN multiplier (16-640) */
    uint8_t m;     /**< DIVM divider (1-63) */
    uint8_t pdiv1; /**< POSTDIV1 (1-7) */
    uint8_t pdiv2; /**< POSTDIV2 (1-7) */
} whal_Stm32n6Rcc_Pll1Cfg;

/**
 * @brief Peripheral clock enable descriptor.
 *
 * Describes the register offset and bit mask needed to enable/disable
 * a peripheral's bus clock. Used with whal_Stm32n6Rcc_Enable/Disable.
 */
typedef struct whal_Stm32n6Rcc_Clk {
    size_t regOffset;   /**< Offset from RCC base to enable register */
    size_t enableMask;  /**< Bit mask for the peripheral enable bit */
    size_t enablePos;   /**< Bit position for the peripheral enable bit */
} whal_Stm32n6Rcc_Clk;

/**
 * @brief RCC driver configuration.
 *
 * Contains all parameters needed to configure the system clock.
 */
typedef struct whal_Stm32n6Rcc_Cfg {
    whal_Stm32n6Rcc_SysClockSrc sysClkSrc; /**< System bus clock source */
    whal_Stm32n6Rcc_CpuClockSrc cpuClkSrc; /**< CPU clock source */
    void *sysClkCfg; /**< Pointer to Pll1Cfg when using PLL */
} whal_Stm32n6Rcc_Cfg;

/**
 * @brief Driver instance for PLL1 clock source.
 */
extern const whal_ClockDriver whal_Stm32n6RccPll_Driver;

/**
 * @brief Driver instance for HSI clock source.
 */
extern const whal_ClockDriver whal_Stm32n6RccHsi_Driver;

/**
 * @brief Initialize the RCC peripheral with PLL1 as system clock.
 *
 * @param clkDev Clock device instance.
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6RccPll_Init(whal_Clock *clkDev);

/**
 * @brief Deinitialize the RCC peripheral from PLL1 mode.
 *
 * @param clkDev Clock device instance.
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6RccPll_Deinit(whal_Clock *clkDev);

/**
 * @brief Initialize the RCC peripheral with HSI as system clock.
 *
 * @param clkDev Clock device instance.
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6RccHsi_Init(whal_Clock *clkDev);

/**
 * @brief Deinitialize the RCC peripheral from HSI mode.
 *
 * @param clkDev Clock device instance.
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6RccHsi_Deinit(whal_Clock *clkDev);

/**
 * @brief Enable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32n6Rcc_Clk descriptor.
 * @retval WHAL_SUCCESS Clock enabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6Rcc_Enable(whal_Clock *clkDev, const void *clk);

/**
 * @brief Disable a peripheral clock gate.
 *
 * @param clkDev Clock device instance.
 * @param clk    Pointer to a whal_Stm32n6Rcc_Clk descriptor.
 * @retval WHAL_SUCCESS Clock disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6Rcc_Disable(whal_Clock *clkDev, const void *clk);

/**
 * @brief Select the ETH1 MAC-PHY interface mode (RCC_CCIPR2.ETH1SEL).
 *
 * Must be called while ETH1 is held in reset and before enabling its
 * clocks. The default after reset is MII; boards using RMII or RGMII must
 * call this before bringing up the Ethernet driver.
 *
 * @param clkDev Clock device instance.
 * @param mode   Interface mode (MII / RGMII / RMII).
 * @retval WHAL_SUCCESS Configuration applied.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6Rcc_Ext_SetEth1If(whal_Clock *clkDev,
                                         whal_Stm32n6Rcc_Eth1If mode);

#endif /* WHAL_STM32N6_RCC_H */
