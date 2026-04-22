#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32f3_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32F3 RCC register offsets and bit definitions.
 * From RM0365 Section 9.4 (RCC registers).
 *
 * RCC_CR (0x00):
 *   Bit 0  HSION, Bit 1  HSIRDY
 *   Bit 16 HSEON, Bit 17 HSERDY
 *   Bit 24 PLLON, Bit 25 PLLRDY
 *
 * RCC_CFGR (0x04):
 *   Bits 1:0   SW    (system clock switch)
 *   Bits 3:2   SWS   (system clock switch status)
 *   Bits 7:4   HPRE  (AHB prescaler)
 *   Bits 10:8  PPRE1 (APB1 prescaler)
 *   Bits 13:11 PPRE2 (APB2 prescaler)
 *   Bit 16     PLLSRC (0=HSI/2, 1=HSE/PREDIV)
 *   Bits 21:18 PLLMUL (multiplication factor)
 *
 * RCC_CFGR2 (0x2C):
 *   Bits 3:0   PREDIV (HSE divider for PLL)
 */

#define RCC_CR_REG 0x00
#define RCC_CR_HSION_Pos 0
#define RCC_CR_HSION_Msk (1UL << RCC_CR_HSION_Pos)
#define RCC_CR_HSIRDY_Pos 1
#define RCC_CR_HSIRDY_Msk (1UL << RCC_CR_HSIRDY_Pos)
#define RCC_CR_HSEON_Pos 16
#define RCC_CR_HSEON_Msk (1UL << RCC_CR_HSEON_Pos)
#define RCC_CR_HSERDY_Pos 17
#define RCC_CR_HSERDY_Msk (1UL << RCC_CR_HSERDY_Pos)
#define RCC_CR_PLLON_Pos 24
#define RCC_CR_PLLON_Msk (1UL << RCC_CR_PLLON_Pos)
#define RCC_CR_PLLRDY_Pos 25
#define RCC_CR_PLLRDY_Msk (1UL << RCC_CR_PLLRDY_Pos)

#define RCC_CFGR_REG 0x04
#define RCC_CFGR_SW_Pos 0
#define RCC_CFGR_SW_Msk (WHAL_BITMASK(2) << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos 2
#define RCC_CFGR_SWS_Msk (WHAL_BITMASK(2) << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_PLLSRC_Pos 16
#define RCC_CFGR_PLLSRC_Msk (1UL << RCC_CFGR_PLLSRC_Pos)
#define RCC_CFGR_PLLMUL_Pos 18
#define RCC_CFGR_PLLMUL_Msk (WHAL_BITMASK(4) << RCC_CFGR_PLLMUL_Pos)

#define RCC_CFGR_SW_HSI 0
#define RCC_CFGR_SW_HSE 1
#define RCC_CFGR_SW_PLL 2

#define RCC_CFGR2_REG 0x2C
#define RCC_CFGR2_PREDIV_Pos 0
#define RCC_CFGR2_PREDIV_Msk (WHAL_BITMASK(4) << RCC_CFGR2_PREDIV_Pos)

#define RCC_CFGR3_REG 0x30
#define RCC_CFGR3_I2C1SW_Pos 4
#define RCC_CFGR3_I2C1SW_Msk (1UL << RCC_CFGR3_I2C1SW_Pos)

#ifdef WHAL_CFG_CLOCK_API_MAPPING_STM32F3
#define whal_Stm32f3Rcc_Init    whal_Clock_Init
#define whal_Stm32f3Rcc_Deinit  whal_Clock_Deinit
#define whal_Stm32f3Rcc_Enable  whal_Clock_Enable
#define whal_Stm32f3Rcc_Disable whal_Clock_Disable
#endif /* WHAL_CFG_CLOCK_API_MAPPING_STM32F3 */

static whal_Error Stm32f3Rcc_EnablePllSource(size_t base,
                                              whal_Stm32f3Rcc_PllCfg *pll)
{
    size_t rdy;

    switch (pll->clkSrc) {
    case WHAL_STM32F3_RCC_PLLSRC_HSI_DIV2:
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSION_Msk,
                        whal_SetBits(RCC_CR_HSION_Msk, RCC_CR_HSION_Pos, 1));
        do {
            whal_Reg_Get(base, RCC_CR_REG,
                         RCC_CR_HSIRDY_Msk, RCC_CR_HSIRDY_Pos, &rdy);
        } while (!rdy);
        break;

    case WHAL_STM32F3_RCC_PLLSRC_HSE_PREDIV:
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk,
                        whal_SetBits(RCC_CR_HSEON_Msk, RCC_CR_HSEON_Pos, 1));
        do {
            whal_Reg_Get(base, RCC_CR_REG,
                         RCC_CR_HSERDY_Msk, RCC_CR_HSERDY_Pos, &rdy);
        } while (!rdy);
        break;
    }

    return WHAL_SUCCESS;
}

static void Stm32f3Rcc_ConfigurePll(size_t base, whal_Stm32f3Rcc_PllCfg *pll)
{
    uint32_t pllsrc;

    /* Disable PLL before reconfiguring */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 0));

    /* Set PREDIV (only meaningful for HSE source, but safe to set always) */
    whal_Reg_Update(base, RCC_CFGR2_REG, RCC_CFGR2_PREDIV_Msk,
                    whal_SetBits(RCC_CFGR2_PREDIV_Msk, RCC_CFGR2_PREDIV_Pos,
                                 pll->prediv - 1));

    /* Set PLLMUL */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_PLLMUL_Msk,
                    whal_SetBits(RCC_CFGR_PLLMUL_Msk, RCC_CFGR_PLLMUL_Pos,
                                 pll->pllmul - 2));

    /* Set PLL source: 0 = HSI/2, 1 = HSE/PREDIV */
    pllsrc = (pll->clkSrc == WHAL_STM32F3_RCC_PLLSRC_HSE_PREDIV) ? 1 : 0;
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_PLLSRC_Msk,
                    whal_SetBits(RCC_CFGR_PLLSRC_Msk, RCC_CFGR_PLLSRC_Pos,
                                 pllsrc));

    /* Enable PLL and wait for lock */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 1));

    size_t rdy;
    do {
        whal_Reg_Get(base, RCC_CR_REG,
                     RCC_CR_PLLRDY_Msk, RCC_CR_PLLRDY_Pos, &rdy);
    } while (!rdy);
}

whal_Error whal_Stm32f3Rcc_Init(whal_Clock *clkDev)
{
    whal_Stm32f3Rcc_Cfg *cfg;
    size_t base;
    uint32_t sw;
    size_t sws;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32f3Rcc_Cfg *)clkDev->cfg;
    base = clkDev->regmap.base;

    switch (cfg->sysClkSrc) {
    case WHAL_STM32F3_RCC_SYSCLK_SRC_HSI: {
        size_t rdy;
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSION_Msk,
                        whal_SetBits(RCC_CR_HSION_Msk, RCC_CR_HSION_Pos, 1));
        do {
            whal_Reg_Get(base, RCC_CR_REG,
                         RCC_CR_HSIRDY_Msk, RCC_CR_HSIRDY_Pos, &rdy);
        } while (!rdy);
        sw = RCC_CFGR_SW_HSI;
        break;
    }

    case WHAL_STM32F3_RCC_SYSCLK_SRC_HSE: {
        size_t rdy;
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk,
                        whal_SetBits(RCC_CR_HSEON_Msk, RCC_CR_HSEON_Pos, 1));
        do {
            whal_Reg_Get(base, RCC_CR_REG,
                         RCC_CR_HSERDY_Msk, RCC_CR_HSERDY_Pos, &rdy);
        } while (!rdy);
        sw = RCC_CFGR_SW_HSE;
        break;
    }

    case WHAL_STM32F3_RCC_SYSCLK_SRC_PLL:
        if (!cfg->pllCfg ||
            cfg->pllCfg->prediv < 1 || cfg->pllCfg->prediv > 16 ||
            cfg->pllCfg->pllmul < 2 || cfg->pllCfg->pllmul > 16)
            return WHAL_EINVAL;

        Stm32f3Rcc_EnablePllSource(base, cfg->pllCfg);
        Stm32f3Rcc_ConfigurePll(base, cfg->pllCfg);
        sw = RCC_CFGR_SW_PLL;
        break;

    default:
        return WHAL_EINVAL;
    }

    /* Switch system clock */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, sw));

    /* Wait for switch status to confirm */
    do {
        whal_Reg_Get(base, RCC_CFGR_REG,
                     RCC_CFGR_SWS_Msk, RCC_CFGR_SWS_Pos, &sws);
    } while (sws != sw);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f3Rcc_Deinit(whal_Clock *clkDev)
{
    size_t sws;

    if (!clkDev)
        return WHAL_EINVAL;

    /* Switch back to HSI */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_HSION_Msk,
                    whal_SetBits(RCC_CR_HSION_Msk, RCC_CR_HSION_Pos, 1));

    size_t rdy;
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG,
                     RCC_CR_HSIRDY_Msk, RCC_CR_HSIRDY_Pos, &rdy);
    } while (!rdy);

    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos,
                                 RCC_CFGR_SW_HSI));

    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CFGR_REG,
                     RCC_CFGR_SWS_Msk, RCC_CFGR_SWS_Pos, &sws);
    } while (sws != RCC_CFGR_SW_HSI);

    /* Disable PLL */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f3Rcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32f3Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32f3Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f3Rcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32f3Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32f3Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f3Rcc_Ext_SetI2c1ClkSrc(whal_Clock *clkDev,
                                              whal_Stm32f3Rcc_I2c1ClkSrc src)
{
    if (!clkDev) {
        return WHAL_EINVAL;
    }

    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR3_REG, RCC_CFGR3_I2C1SW_Msk,
                    whal_SetBits(RCC_CFGR3_I2C1SW_Msk,
                                  RCC_CFGR3_I2C1SW_Pos, src));

    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_CLOCK_API_MAPPING_STM32F3
const whal_ClockDriver whal_Stm32f3Rcc_Driver = {
    .Init = whal_Stm32f3Rcc_Init,
    .Deinit = whal_Stm32f3Rcc_Deinit,
    .Enable = whal_Stm32f3Rcc_Enable,
    .Disable = whal_Stm32f3Rcc_Disable,
};
#endif /* !WHAL_CFG_CLOCK_API_MAPPING_STM32F3 */
