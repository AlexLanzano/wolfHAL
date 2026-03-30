#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32f4_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32F4 RCC Register Definitions
 *
 * The RCC peripheral manages:
 * - Clock source selection and PLL configuration
 * - System clock (SYSCLK) routing
 * - Bus clock prescalers (AHB, APB1, APB2)
 * - Peripheral clock gating
 *
 * RCC base address: 0x40023800
 */

/* Clock Control Register */
#define RCC_CR_REG 0x000
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

/* PLL Configuration Register */
#define RCC_PLLCFGR_REG 0x004
#define RCC_PLLCFGR_PLLM_Pos 0
#define RCC_PLLCFGR_PLLM_Msk (WHAL_BITMASK(6) << RCC_PLLCFGR_PLLM_Pos)

#define RCC_PLLCFGR_PLLN_Pos 6
#define RCC_PLLCFGR_PLLN_Msk (WHAL_BITMASK(9) << RCC_PLLCFGR_PLLN_Pos)

#define RCC_PLLCFGR_PLLP_Pos 16
#define RCC_PLLCFGR_PLLP_Msk (WHAL_BITMASK(2) << RCC_PLLCFGR_PLLP_Pos)

#define RCC_PLLCFGR_PLLSRC_Pos 22
#define RCC_PLLCFGR_PLLSRC_Msk (1UL << RCC_PLLCFGR_PLLSRC_Pos)

#define RCC_PLLCFGR_PLLQ_Pos 24
#define RCC_PLLCFGR_PLLQ_Msk (WHAL_BITMASK(4) << RCC_PLLCFGR_PLLQ_Pos)

#define RCC_PLLCFGR_Msk \
    (RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk | \
     RCC_PLLCFGR_PLLP_Msk | RCC_PLLCFGR_PLLSRC_Msk | \
     RCC_PLLCFGR_PLLQ_Msk)

/* Clock Configuration Register */
#define RCC_CFGR_REG 0x008
#define RCC_CFGR_SW_Pos 0
#define RCC_CFGR_SW_Msk (WHAL_BITMASK(2) << RCC_CFGR_SW_Pos)

#define RCC_CFGR_SWS_Pos 2
#define RCC_CFGR_SWS_Msk (WHAL_BITMASK(2) << RCC_CFGR_SWS_Pos)

#define RCC_CFGR_PPRE1_Pos 10
#define RCC_CFGR_PPRE1_Msk (WHAL_BITMASK(3) << RCC_CFGR_PPRE1_Pos)

#define RCC_CFGR_PPRE2_Pos 13
#define RCC_CFGR_PPRE2_Msk (WHAL_BITMASK(3) << RCC_CFGR_PPRE2_Pos)

/* PLLP register value to actual divider: 0->2, 1->4, 2->6, 3->8 */
static const uint8_t pllp_div[] = { 2, 4, 6, 8 };

whal_Error whal_Stm32f4RccPll_Init(whal_Clock *clkDev)
{
    whal_Stm32f4Rcc_Cfg *cfg;
    whal_Stm32f4Rcc_PllClkCfg *pllCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32f4Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32f4Rcc_PllClkCfg *)cfg->sysClkCfg;
    base = clkDev->regmap.base;

    /* Enable HSE if selected as PLL source */
    if (pllCfg->clkSrc == WHAL_STM32F4_RCC_PLLCLK_SRC_HSE) {
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk,
                        whal_SetBits(RCC_CR_HSEON_Msk, RCC_CR_HSEON_Pos, 1));
        while (1) {
            size_t rdy;
            whal_Reg_Get(base, RCC_CR_REG, RCC_CR_HSERDY_Msk,
                         RCC_CR_HSERDY_Pos, &rdy);
            if (rdy)
                break;
        }
    }

    /* Ensure PLL is off before configuring */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 0));

    /* Configure PLL: source, M, N, P, Q */
    whal_Reg_Update(base, RCC_PLLCFGR_REG, RCC_PLLCFGR_Msk,
                    whal_SetBits(RCC_PLLCFGR_PLLM_Msk, RCC_PLLCFGR_PLLM_Pos, pllCfg->m) |
                    whal_SetBits(RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_Pos, pllCfg->n) |
                    whal_SetBits(RCC_PLLCFGR_PLLP_Msk, RCC_PLLCFGR_PLLP_Pos, pllCfg->p) |
                    whal_SetBits(RCC_PLLCFGR_PLLSRC_Msk, RCC_PLLCFGR_PLLSRC_Pos, pllCfg->clkSrc) |
                    whal_SetBits(RCC_PLLCFGR_PLLQ_Msk, RCC_PLLCFGR_PLLQ_Pos, pllCfg->q));

    /* Enable PLL */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 1));

    /* Wait for PLL lock */
    while (1) {
        size_t rdy;
        whal_Reg_Get(base, RCC_CR_REG, RCC_CR_PLLRDY_Msk,
                     RCC_CR_PLLRDY_Pos, &rdy);
        if (rdy)
            break;
    }

    /* Set APB1 and APB2 prescalers before switching to higher speed */
    whal_Reg_Update(base, RCC_CFGR_REG,
                    RCC_CFGR_PPRE1_Msk | RCC_CFGR_PPRE2_Msk,
                    whal_SetBits(RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_Pos,
                                 cfg->ppre1) |
                    whal_SetBits(RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_Pos,
                                 cfg->ppre2));

    /* Switch system clock to PLL */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos,
                                 WHAL_STM32F4_RCC_SYSCLK_SRC_PLL));

    /* Wait for switch to complete */
    while (1) {
        size_t sws;
        whal_Reg_Get(base, RCC_CFGR_REG, RCC_CFGR_SWS_Msk,
                     RCC_CFGR_SWS_Pos, &sws);
        if (sws == WHAL_STM32F4_RCC_SYSCLK_SRC_PLL)
            break;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4RccPll_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Switch back to HSI before disabling PLL */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos,
                                 WHAL_STM32F4_RCC_SYSCLK_SRC_HSI));

    /* Wait for switch to complete */
    while (1) {
        size_t sws;
        whal_Reg_Get(base, RCC_CFGR_REG, RCC_CFGR_SWS_Msk,
                     RCC_CFGR_SWS_Pos, &sws);
        if (sws == WHAL_STM32F4_RCC_SYSCLK_SRC_HSI)
            break;
    }

    /* Disable PLL */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4Rcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32f4Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32f4Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4Rcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32f4Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32f4Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4RccPll_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    whal_Stm32f4Rcc_Cfg *cfg;
    whal_Stm32f4Rcc_PllClkCfg *pllCfg;
    size_t srcFreq;
    size_t pllm;
    size_t plln;
    size_t pllp;

    if (!clkDev || !clkDev->cfg || !rateOut)
        return WHAL_EINVAL;

    cfg = (whal_Stm32f4Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32f4Rcc_PllClkCfg *)cfg->sysClkCfg;

    /* Determine source frequency */
    if (pllCfg->clkSrc == WHAL_STM32F4_RCC_PLLCLK_SRC_HSI)
        srcFreq = 16000000;
    else
        srcFreq = 25000000;

    pllm = pllCfg->m;
    plln = pllCfg->n;
    pllp = pllp_div[pllCfg->p & 0x3];

    *rateOut = ((srcFreq / pllm) * plln) / pllp;
    return WHAL_SUCCESS;
}

const whal_ClockDriver whal_Stm32f4RccPll_Driver = {
    .Init = whal_Stm32f4RccPll_Init,
    .Deinit = whal_Stm32f4RccPll_Deinit,
    .Enable = whal_Stm32f4Rcc_Enable,
    .Disable = whal_Stm32f4Rcc_Disable,
    .GetRate = whal_Stm32f4RccPll_GetRate,
};
