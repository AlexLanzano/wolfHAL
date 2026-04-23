#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32l1_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32L1 RCC register offsets and bit definitions.
 * From RM0038 Section 6.3 (RCC registers).
 *
 * RCC_CR (0x00):
 *   Bit 0  HSION, Bit 1  HSIRDY
 *   Bit 8  MSION, Bit 9  MSIRDY
 *   Bit 16 HSEON, Bit 17 HSERDY
 *   Bit 24 PLLON, Bit 25 PLLRDY
 *
 * RCC_CFGR (0x08):
 *   Bits 1:0   SW     (system clock switch)
 *   Bits 3:2   SWS    (system clock switch status)
 *   Bits 7:4   HPRE   (AHB prescaler)
 *   Bits 10:8  PPRE1  (APB1 prescaler)
 *   Bits 13:11 PPRE2  (APB2 prescaler)
 *   Bit 16     PLLSRC (0=HSI, 1=HSE)
 *   Bits 21:18 PLLMUL
 *   Bits 23:22 PLLDIV
 */

#define RCC_CR_REG 0x00
#define RCC_CR_HSION_Pos 0
#define RCC_CR_HSION_Msk (1UL << RCC_CR_HSION_Pos)
#define RCC_CR_HSIRDY_Pos 1
#define RCC_CR_HSIRDY_Msk (1UL << RCC_CR_HSIRDY_Pos)
#define RCC_CR_MSION_Pos 8
#define RCC_CR_MSION_Msk (1UL << RCC_CR_MSION_Pos)
#define RCC_CR_MSIRDY_Pos 9
#define RCC_CR_MSIRDY_Msk (1UL << RCC_CR_MSIRDY_Pos)
#define RCC_CR_HSEON_Pos 16
#define RCC_CR_HSEON_Msk (1UL << RCC_CR_HSEON_Pos)
#define RCC_CR_HSERDY_Pos 17
#define RCC_CR_HSERDY_Msk (1UL << RCC_CR_HSERDY_Pos)
#define RCC_CR_PLLON_Pos 24
#define RCC_CR_PLLON_Msk (1UL << RCC_CR_PLLON_Pos)
#define RCC_CR_PLLRDY_Pos 25
#define RCC_CR_PLLRDY_Msk (1UL << RCC_CR_PLLRDY_Pos)

#define RCC_CFGR_REG 0x08
#define RCC_CFGR_SW_Pos 0
#define RCC_CFGR_SW_Msk (WHAL_BITMASK(2) << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos 2
#define RCC_CFGR_SWS_Msk (WHAL_BITMASK(2) << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_PLLSRC_Pos 16
#define RCC_CFGR_PLLSRC_Msk (1UL << RCC_CFGR_PLLSRC_Pos)
#define RCC_CFGR_PLLMUL_Pos 18
#define RCC_CFGR_PLLMUL_Msk (WHAL_BITMASK(4) << RCC_CFGR_PLLMUL_Pos)
#define RCC_CFGR_PLLDIV_Pos 22
#define RCC_CFGR_PLLDIV_Msk (WHAL_BITMASK(2) << RCC_CFGR_PLLDIV_Pos)

#define RCC_CFGR_SW_MSI 0
#define RCC_CFGR_SW_HSI 1
#define RCC_CFGR_SW_HSE 2
#define RCC_CFGR_SW_PLL 3

#ifdef WHAL_CFG_CLOCK_API_MAPPING_STM32L1
#define whal_Stm32l1Rcc_Init    whal_Clock_Init
#define whal_Stm32l1Rcc_Deinit  whal_Clock_Deinit
#define whal_Stm32l1Rcc_Enable  whal_Clock_Enable
#define whal_Stm32l1Rcc_Disable whal_Clock_Disable
#endif /* WHAL_CFG_CLOCK_API_MAPPING_STM32L1 */

static whal_Error Stm32l1Rcc_EnablePllSource(size_t base,
                                              whal_Stm32l1Rcc_PllCfg *pll)
{
    size_t rdy;

    switch (pll->clkSrc) {
    case WHAL_STM32L1_RCC_PLLSRC_HSI:
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSION_Msk,
                        whal_SetBits(RCC_CR_HSION_Msk, RCC_CR_HSION_Pos, 1));
        do {
            whal_Reg_Get(base, RCC_CR_REG,
                         RCC_CR_HSIRDY_Msk, RCC_CR_HSIRDY_Pos, &rdy);
        } while (!rdy);
        break;

    case WHAL_STM32L1_RCC_PLLSRC_HSE:
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

static void Stm32l1Rcc_ConfigurePll(size_t base, whal_Stm32l1Rcc_PllCfg *pll)
{
    uint32_t pllsrc;

    /* Disable PLL before reconfiguring */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 0));

    /* Wait for PLL to stop */
    size_t rdy;
    do {
        whal_Reg_Get(base, RCC_CR_REG,
                     RCC_CR_PLLRDY_Msk, RCC_CR_PLLRDY_Pos, &rdy);
    } while (rdy);

    /* Set PLL source: 0 = HSI, 1 = HSE */
    pllsrc = (pll->clkSrc == WHAL_STM32L1_RCC_PLLSRC_HSE) ? 1 : 0;
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_PLLSRC_Msk,
                    whal_SetBits(RCC_CFGR_PLLSRC_Msk, RCC_CFGR_PLLSRC_Pos,
                                 pllsrc));

    /* Set PLLMUL */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_PLLMUL_Msk,
                    whal_SetBits(RCC_CFGR_PLLMUL_Msk, RCC_CFGR_PLLMUL_Pos,
                                 pll->pllmul));

    /* Set PLLDIV */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_PLLDIV_Msk,
                    whal_SetBits(RCC_CFGR_PLLDIV_Msk, RCC_CFGR_PLLDIV_Pos,
                                 pll->plldiv));

    /* Enable PLL and wait for lock */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 1));

    do {
        whal_Reg_Get(base, RCC_CR_REG,
                     RCC_CR_PLLRDY_Msk, RCC_CR_PLLRDY_Pos, &rdy);
    } while (!rdy);
}

whal_Error whal_Stm32l1Rcc_Init(whal_Clock *clkDev)
{
    whal_Stm32l1Rcc_Cfg *cfg;
    size_t base;
    uint32_t sw;
    size_t sws;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32l1Rcc_Cfg *)clkDev->cfg;
    base = clkDev->regmap.base;

    switch (cfg->sysClkSrc) {
    case WHAL_STM32L1_RCC_SYSCLK_SRC_MSI: {
        size_t rdy;
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_MSION_Msk,
                        whal_SetBits(RCC_CR_MSION_Msk, RCC_CR_MSION_Pos, 1));
        do {
            whal_Reg_Get(base, RCC_CR_REG,
                         RCC_CR_MSIRDY_Msk, RCC_CR_MSIRDY_Pos, &rdy);
        } while (!rdy);
        sw = RCC_CFGR_SW_MSI;
        break;
    }

    case WHAL_STM32L1_RCC_SYSCLK_SRC_HSI: {
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

    case WHAL_STM32L1_RCC_SYSCLK_SRC_HSE: {
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

    case WHAL_STM32L1_RCC_SYSCLK_SRC_PLL:
        if (!cfg->pllCfg)
            return WHAL_EINVAL;

        Stm32l1Rcc_EnablePllSource(base, cfg->pllCfg);
        Stm32l1Rcc_ConfigurePll(base, cfg->pllCfg);
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

whal_Error whal_Stm32l1Rcc_Deinit(whal_Clock *clkDev)
{
    size_t sws;

    if (!clkDev)
        return WHAL_EINVAL;

    /* Switch back to MSI (default after reset) */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_MSION_Msk,
                    whal_SetBits(RCC_CR_MSION_Msk, RCC_CR_MSION_Pos, 1));

    size_t rdy;
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG,
                     RCC_CR_MSIRDY_Msk, RCC_CR_MSIRDY_Pos, &rdy);
    } while (!rdy);

    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos,
                                 RCC_CFGR_SW_MSI));

    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CFGR_REG,
                     RCC_CFGR_SWS_Msk, RCC_CFGR_SWS_Pos, &sws);
    } while (sws != RCC_CFGR_SW_MSI);

    /* Disable PLL */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1Rcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32l1Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32l1Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1Rcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32l1Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32l1Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_CLOCK_API_MAPPING_STM32L1
const whal_ClockDriver whal_Stm32l1Rcc_Driver = {
    .Init = whal_Stm32l1Rcc_Init,
    .Deinit = whal_Stm32l1Rcc_Deinit,
    .Enable = whal_Stm32l1Rcc_Enable,
    .Disable = whal_Stm32l1Rcc_Disable,
};
#endif /* !WHAL_CFG_CLOCK_API_MAPPING_STM32L1 */
