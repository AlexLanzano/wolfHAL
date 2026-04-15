#ifndef WHAL_STM32WBA_RCC_H
#define WHAL_STM32WBA_RCC_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/clock/clock.h>

/*
 * @file stm32wba_rcc.h
 * @brief STM32WBA RCC (Reset and Clock Control) driver configuration.
 *
 * The STM32WBA RCC peripheral controls:
 * - System clock source selection (HSI16, HSE32, PLL1)
 * - PLL1 configuration for up to 100 MHz operation
 * - Peripheral clock gating (AHB1-5, APB1-2, APB7 buses)
 * - Bus clock prescalers (HPRE, PPRE1, PPRE2, PPRE7, HPRE5)
 *
 * Key differences from STM32WB RCC:
 * - No MSI oscillator
 * - HSE is 32 MHz (HSE32)
 * - PLL configuration split across RCC_PLL1CFGR and RCC_PLL1DIVR
 * - HPRE5 prescaler for AHB5 domain (max 32 MHz)
 * - Different register offsets throughout
 */

/*
 * @brief System clock source selection.
 */
typedef enum {
    WHAL_STM32WBA_RCC_SYSCLK_SRC_HSI16 = 0, /* 16 MHz High-Speed Internal */
    WHAL_STM32WBA_RCC_SYSCLK_SRC_HSE32 = 2, /* 32 MHz High-Speed External */
    WHAL_STM32WBA_RCC_SYSCLK_SRC_PLL1  = 3, /* PLL1 output */
} whal_Stm32wbaRcc_SysClockSrc;

/*
 * @brief PLL1 input clock source selection.
 */
typedef enum {
    WHAL_STM32WBA_RCC_PLL1SRC_NONE  = 0, /* No clock (PLL disabled) */
    WHAL_STM32WBA_RCC_PLL1SRC_HSI16 = 2, /* HSI16 as PLL input */
    WHAL_STM32WBA_RCC_PLL1SRC_HSE32 = 3, /* HSE32 as PLL input */
} whal_Stm32wbaRcc_Pll1Src;

/*
 * @brief PLL1 input reference frequency range.
 */
typedef enum {
    WHAL_STM32WBA_RCC_PLL1RGE_4_8   = 2, /* 4-8 MHz */
    WHAL_STM32WBA_RCC_PLL1RGE_8_16  = 3, /* 8-16 MHz */
} whal_Stm32wbaRcc_Pll1Rge;

/*
 * @brief PLL1 configuration parameters.
 *
 * The PLL1 output frequency is calculated as:
 *   f_ref = f_input / (PLL1M + 1)   [must be 4-16 MHz]
 *   f_vco = f_ref * PLL1N           [must be 128-544 MHz]
 *   f_pll1r = f_vco / (PLL1R + 1)   [SYSCLK output, max 100 MHz]
 *
 * For 100 MHz from 32 MHz HSE:
 *   PLL1M = 1 (div by 2, ref = 16 MHz)
 *   PLL1N = 25 (VCO = 400 MHz)
 *   PLL1R = 3 (div by 4, out = 100 MHz)
 *   PLL1RGE = 3 (8-16 MHz range)
 */
typedef struct whal_Stm32wbaRcc_Pll1Cfg {
    whal_Stm32wbaRcc_Pll1Src clkSrc; /* PLL1 input source */
    whal_Stm32wbaRcc_Pll1Rge rge;    /* PLL1 input frequency range */
    uint8_t m;  /* PLL1M prescaler (0-7 maps to div 1-8) */
    uint16_t n; /* PLL1N multiplier (4-512, written to PLL1N[8:0]) */
    uint8_t r;  /* PLL1R divider (0=VCO, 1=div2, 2=div3, 3=div4, ...) */
    uint8_t q;  /* PLL1Q divider (0=VCO, 1=div2, ...) */
    uint8_t p;  /* PLL1P divider (0=VCO, 1=div2, ...) */
} whal_Stm32wbaRcc_Pll1Cfg;

/*
 * @brief Peripheral clock enable descriptor.
 *
 * Same structure as STM32WB -- describes register offset and bit to toggle.
 */
typedef struct whal_Stm32wbaRcc_Clk {
    size_t regOffset;   /* Offset from RCC base to enable register */
    size_t enableMask;  /* Bit mask for the peripheral enable bit */
    size_t enablePos;   /* Bit position for the peripheral enable bit */
} whal_Stm32wbaRcc_Clk;

/*
 * @brief RCC driver configuration.
 */
typedef struct whal_Stm32wbaRcc_Cfg {
    whal_Stm32wbaRcc_SysClockSrc sysClkSrc;
    void *sysClkCfg; /* Pointer to Pll1Cfg when sysClkSrc is PLL1 */
} whal_Stm32wbaRcc_Cfg;

extern const whal_ClockDriver whal_Stm32wbaRccPll_Driver;
extern const whal_ClockDriver whal_Stm32wbaRccHse_Driver;

whal_Error whal_Stm32wbaRccPll_Init(whal_Clock *clkDev);
whal_Error whal_Stm32wbaRccPll_Deinit(whal_Clock *clkDev);
whal_Error whal_Stm32wbaRccHse_Init(whal_Clock *clkDev);
whal_Error whal_Stm32wbaRccHse_Deinit(whal_Clock *clkDev);
whal_Error whal_Stm32wbaRcc_Enable(whal_Clock *clkDev, const void *clk);
whal_Error whal_Stm32wbaRcc_Disable(whal_Clock *clkDev, const void *clk);

/*
 * @brief RNG kernel clock source selection (RCC_CCIPR2.RNGSEL).
 */
typedef enum {
    WHAL_STM32WBA_RCC_RNGSEL_LSE      = 0,
    WHAL_STM32WBA_RCC_RNGSEL_LSI      = 1,
    WHAL_STM32WBA_RCC_RNGSEL_HSI16    = 2,
    WHAL_STM32WBA_RCC_RNGSEL_PLL1Q_D2 = 3,
} whal_Stm32wbaRcc_RngSrc;

/*
 * @brief Enable or disable the LSI1 oscillator required by the IWDG.
 */
whal_Error whal_Stm32wbaRcc_Ext_EnableLsi(whal_Clock *clkDev, uint8_t enable);

/*
 * @brief Select the RNG kernel clock source (RCC_CCIPR2.RNGSEL).
 *
 * Default after reset is LSE, which is typically not enabled. HSI16 is the
 * usual choice since it is always running.
 */
whal_Error whal_Stm32wbaRcc_Ext_SetRngClockSrc(whal_Clock *clkDev,
                                                whal_Stm32wbaRcc_RngSrc src);

/*
 * @brief Set the HPRE5 prescaler for the AHB5 domain.
 *
 * The AHB5 domain (radio, PTACONV) must not exceed 32 MHz.
 * @param hpre5 HPRE5 value in RCC_CFGR3 bits[6:4]:
 *   0xx: not divided
 *   100: div 2
 *   101: div 4
 *   110: div 8
 *   111: div 16
 */
whal_Error whal_Stm32wbaRcc_Ext_SetHpre5(whal_Clock *clkDev, uint8_t hpre5);

#endif /* WHAL_STM32WBA_RCC_H */
