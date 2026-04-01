#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32h7_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32H7 RCC Register Definitions (RM0433)
 *
 * The RCC peripheral manages:
 * - Clock source selection and PLL1/PLL2/PLL3 configuration
 * - System clock (SYSCLK) routing
 * - D1/D2/D3 clock domain prescalers
 * - Peripheral clock gating (AHB1-4, APB1-4 buses)
 *
 * RCC base address: 0x58024400
 */

/* Clock Control Register (offset 0x000) */
#define RCC_CR_REG 0x000
#define RCC_CR_HSION_Pos 0
#define RCC_CR_HSION_Msk (1UL << RCC_CR_HSION_Pos)

#define RCC_CR_HSIRDY_Pos 2
#define RCC_CR_HSIRDY_Msk (1UL << RCC_CR_HSIRDY_Pos)

#define RCC_CR_HSIDIV_Pos 3
#define RCC_CR_HSIDIV_Msk (WHAL_BITMASK(2) << RCC_CR_HSIDIV_Pos)

#define RCC_CR_HSIDIVF_Pos 5
#define RCC_CR_HSIDIVF_Msk (1UL << RCC_CR_HSIDIVF_Pos)

#define RCC_CR_CSION_Pos 7
#define RCC_CR_CSION_Msk (1UL << RCC_CR_CSION_Pos)

#define RCC_CR_CSIRDY_Pos 8
#define RCC_CR_CSIRDY_Msk (1UL << RCC_CR_CSIRDY_Pos)

#define RCC_CR_HSI48ON_Pos 12
#define RCC_CR_HSI48ON_Msk (1UL << RCC_CR_HSI48ON_Pos)

#define RCC_CR_HSI48RDY_Pos 13
#define RCC_CR_HSI48RDY_Msk (1UL << RCC_CR_HSI48RDY_Pos)

#define RCC_CR_HSEON_Pos 16
#define RCC_CR_HSEON_Msk (1UL << RCC_CR_HSEON_Pos)

#define RCC_CR_HSERDY_Pos 17
#define RCC_CR_HSERDY_Msk (1UL << RCC_CR_HSERDY_Pos)

#define RCC_CR_PLL1ON_Pos 24
#define RCC_CR_PLL1ON_Msk (1UL << RCC_CR_PLL1ON_Pos)

#define RCC_CR_PLL1RDY_Pos 25
#define RCC_CR_PLL1RDY_Msk (1UL << RCC_CR_PLL1RDY_Pos)

/* Clock Configuration Register (offset 0x010) */
#define RCC_CFGR_REG 0x010
#define RCC_CFGR_SW_Pos 0
#define RCC_CFGR_SW_Msk (WHAL_BITMASK(3) << RCC_CFGR_SW_Pos)

#define RCC_CFGR_SWS_Pos 3
#define RCC_CFGR_SWS_Msk (WHAL_BITMASK(3) << RCC_CFGR_SWS_Pos)

/* D1 Clock Configuration Register (offset 0x018) */
#define RCC_D1CFGR_REG 0x018
#define RCC_D1CFGR_HPRE_Pos 0
#define RCC_D1CFGR_HPRE_Msk (WHAL_BITMASK(4) << RCC_D1CFGR_HPRE_Pos)

#define RCC_D1CFGR_D1PPRE_Pos 4
#define RCC_D1CFGR_D1PPRE_Msk (WHAL_BITMASK(3) << RCC_D1CFGR_D1PPRE_Pos)

#define RCC_D1CFGR_D1CPRE_Pos 8
#define RCC_D1CFGR_D1CPRE_Msk (WHAL_BITMASK(4) << RCC_D1CFGR_D1CPRE_Pos)

/* PLL Clock Source Selection Register (offset 0x028) */
#define RCC_PLLCKSELR_REG 0x028
#define RCC_PLLCKSELR_PLLSRC_Pos 0
#define RCC_PLLCKSELR_PLLSRC_Msk (WHAL_BITMASK(2) << RCC_PLLCKSELR_PLLSRC_Pos)

#define RCC_PLLCKSELR_DIVM1_Pos 4
#define RCC_PLLCKSELR_DIVM1_Msk (WHAL_BITMASK(6) << RCC_PLLCKSELR_DIVM1_Pos)

/* PLL Configuration Register (offset 0x02C) */
#define RCC_PLLCFGR_REG 0x02C
#define RCC_PLLCFGR_PLL1RGE_Pos 2
#define RCC_PLLCFGR_PLL1RGE_Msk (WHAL_BITMASK(2) << RCC_PLLCFGR_PLL1RGE_Pos)

#define RCC_PLLCFGR_PLL1VCOSEL_Pos 1
#define RCC_PLLCFGR_PLL1VCOSEL_Msk (1UL << RCC_PLLCFGR_PLL1VCOSEL_Pos)

#define RCC_PLLCFGR_DIVP1EN_Pos 16
#define RCC_PLLCFGR_DIVP1EN_Msk (1UL << RCC_PLLCFGR_DIVP1EN_Pos)

#define RCC_PLLCFGR_DIVQ1EN_Pos 17
#define RCC_PLLCFGR_DIVQ1EN_Msk (1UL << RCC_PLLCFGR_DIVQ1EN_Pos)

#define RCC_PLLCFGR_DIVR1EN_Pos 18
#define RCC_PLLCFGR_DIVR1EN_Msk (1UL << RCC_PLLCFGR_DIVR1EN_Pos)

/* PLL1 Dividers Register (offset 0x030) */
#define RCC_PLL1DIVR_REG 0x030
#define RCC_PLL1DIVR_DIVN1_Pos 0
#define RCC_PLL1DIVR_DIVN1_Msk (WHAL_BITMASK(9) << RCC_PLL1DIVR_DIVN1_Pos)

#define RCC_PLL1DIVR_DIVP1_Pos 9
#define RCC_PLL1DIVR_DIVP1_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_DIVP1_Pos)

#define RCC_PLL1DIVR_DIVQ1_Pos 16
#define RCC_PLL1DIVR_DIVQ1_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_DIVQ1_Pos)

#define RCC_PLL1DIVR_DIVR1_Pos 24
#define RCC_PLL1DIVR_DIVR1_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_DIVR1_Pos)

/* HSI base frequency */
#define HSI_FREQ 64000000UL

/*
 * Compute the input reference frequency for PLL source selection.
 * Returns the HSI frequency accounting for the divider if HSI is selected.
 */
static size_t Stm32h7Rcc_GetPllInputFreq(size_t rccBase,
                                          whal_Stm32h7Rcc_PllClockSrc src)
{
    size_t hsidiv;

    switch (src) {
    case WHAL_STM32H7_RCC_PLLCLK_SRC_HSI:
        whal_Reg_Get(rccBase, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                     RCC_CR_HSIDIV_Pos, &hsidiv);
        return HSI_FREQ >> hsidiv;
    case WHAL_STM32H7_RCC_PLLCLK_SRC_CSI:
        return 4000000;
    case WHAL_STM32H7_RCC_PLLCLK_SRC_HSE:
        return 8000000; /* TODO: Make configurable per board */
    default:
        return 0;
    }
}

whal_Error whal_Stm32h7RccPll_Init(whal_Clock *clkDev)
{
    whal_Stm32h7Rcc_Cfg *cfg;
    whal_Stm32h7Rcc_PllClkCfg *pllCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h7Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32h7Rcc_PllClkCfg *)cfg->sysClkCfg;
    base = clkDev->regmap.base;

    /* Set HSI to div1 (64 MHz) if selected as PLL source */
    if (pllCfg->clkSrc == WHAL_STM32H7_RCC_PLLCLK_SRC_HSI) {
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                        whal_SetBits(RCC_CR_HSIDIV_Msk, RCC_CR_HSIDIV_Pos, 0));
        /* Wait for divider to stabilize */
        while (1) {
            size_t divf;
            whal_Reg_Get(base, RCC_CR_REG, RCC_CR_HSIDIVF_Msk,
                         RCC_CR_HSIDIVF_Pos, &divf);
            if (divf)
                break;
        }
    }

    /* Enable HSE if selected as PLL source */
    if (pllCfg->clkSrc == WHAL_STM32H7_RCC_PLLCLK_SRC_HSE) {
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

    /* Ensure PLL1 is off before configuring */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    whal_SetBits(RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Pos, 0));

    /* Configure PLL source and DIVM1 prescaler in PLLCKSELR */
    whal_Reg_Update(base, RCC_PLLCKSELR_REG,
                    RCC_PLLCKSELR_PLLSRC_Msk | RCC_PLLCKSELR_DIVM1_Msk,
                    whal_SetBits(RCC_PLLCKSELR_PLLSRC_Msk, RCC_PLLCKSELR_PLLSRC_Pos,
                                 pllCfg->clkSrc) |
                    whal_SetBits(RCC_PLLCKSELR_DIVM1_Msk, RCC_PLLCKSELR_DIVM1_Pos,
                                 pllCfg->m));

    /* Enable PLL1 P, Q, R outputs in PLLCFGR */
    whal_Reg_Update(base, RCC_PLLCFGR_REG,
                    RCC_PLLCFGR_DIVP1EN_Msk | RCC_PLLCFGR_DIVQ1EN_Msk |
                    RCC_PLLCFGR_DIVR1EN_Msk,
                    whal_SetBits(RCC_PLLCFGR_DIVP1EN_Msk, RCC_PLLCFGR_DIVP1EN_Pos, 1) |
                    whal_SetBits(RCC_PLLCFGR_DIVQ1EN_Msk, RCC_PLLCFGR_DIVQ1EN_Pos, 1) |
                    whal_SetBits(RCC_PLLCFGR_DIVR1EN_Msk, RCC_PLLCFGR_DIVR1EN_Pos, 1));

    /* Configure PLL1 multiplier and dividers in PLL1DIVR */
    whal_Reg_Update(base, RCC_PLL1DIVR_REG,
                    RCC_PLL1DIVR_DIVN1_Msk | RCC_PLL1DIVR_DIVP1_Msk |
                    RCC_PLL1DIVR_DIVQ1_Msk | RCC_PLL1DIVR_DIVR1_Msk,
                    whal_SetBits(RCC_PLL1DIVR_DIVN1_Msk, RCC_PLL1DIVR_DIVN1_Pos,
                                 pllCfg->n) |
                    whal_SetBits(RCC_PLL1DIVR_DIVP1_Msk, RCC_PLL1DIVR_DIVP1_Pos,
                                 pllCfg->p) |
                    whal_SetBits(RCC_PLL1DIVR_DIVQ1_Msk, RCC_PLL1DIVR_DIVQ1_Pos,
                                 pllCfg->q) |
                    whal_SetBits(RCC_PLL1DIVR_DIVR1_Msk, RCC_PLL1DIVR_DIVR1_Pos,
                                 pllCfg->r));

    /* Enable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    whal_SetBits(RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Pos, 1));

    /* Wait for PLL1 lock */
    while (1) {
        size_t rdy;
        whal_Reg_Get(base, RCC_CR_REG, RCC_CR_PLL1RDY_Msk,
                     RCC_CR_PLL1RDY_Pos, &rdy);
        if (rdy)
            break;
    }

    /* Switch system clock to PLL1 */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos,
                                 WHAL_STM32H7_RCC_SYSCLK_SRC_PLL1));

    /* Wait for switch to complete */
    while (1) {
        size_t sws;
        whal_Reg_Get(base, RCC_CFGR_REG, RCC_CFGR_SWS_Msk,
                     RCC_CFGR_SWS_Pos, &sws);
        if (sws == WHAL_STM32H7_RCC_SYSCLK_SRC_PLL1)
            break;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7RccPll_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Switch back to HSI before disabling PLL1 */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos,
                                 WHAL_STM32H7_RCC_SYSCLK_SRC_HSI));

    /* Wait for switch to complete */
    while (1) {
        size_t sws;
        whal_Reg_Get(base, RCC_CFGR_REG, RCC_CFGR_SWS_Msk,
                     RCC_CFGR_SWS_Pos, &sws);
        if (sws == WHAL_STM32H7_RCC_SYSCLK_SRC_HSI)
            break;
    }

    /* Disable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    whal_SetBits(RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7RccHsi_Init(whal_Clock *clkDev)
{
    whal_Stm32h7Rcc_Cfg *cfg;
    whal_Stm32h7Rcc_HsiClkCfg *hsiCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h7Rcc_Cfg *)clkDev->cfg;
    hsiCfg = (whal_Stm32h7Rcc_HsiClkCfg *)cfg->sysClkCfg;
    base = clkDev->regmap.base;

    /* Enable HSI */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSION_Msk,
                    whal_SetBits(RCC_CR_HSION_Msk, RCC_CR_HSION_Pos, 1));

    /* Set HSI divider */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                    whal_SetBits(RCC_CR_HSIDIV_Msk, RCC_CR_HSIDIV_Pos, hsiCfg->div));

    /* Wait for HSI divider to stabilize */
    while (1) {
        size_t divf;
        whal_Reg_Get(base, RCC_CR_REG, RCC_CR_HSIDIVF_Msk,
                     RCC_CR_HSIDIVF_Pos, &divf);
        if (divf)
            break;
    }

    /* Select HSI as system clock */
    whal_Reg_Update(base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos,
                                 WHAL_STM32H7_RCC_SYSCLK_SRC_HSI));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7RccHsi_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Reset HSI divider to default (div1 = 64 MHz) */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                    whal_SetBits(RCC_CR_HSIDIV_Msk, RCC_CR_HSIDIV_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Rcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32h7Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32h7Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Rcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32h7Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32h7Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7RccPll_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    whal_Stm32h7Rcc_Cfg *cfg;
    whal_Stm32h7Rcc_PllClkCfg *pllCfg;
    size_t srcFreq;
    size_t pllm;
    size_t plln;
    size_t pllp;

    if (!clkDev || !clkDev->cfg || !rateOut)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h7Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32h7Rcc_PllClkCfg *)cfg->sysClkCfg;

    srcFreq = Stm32h7Rcc_GetPllInputFreq(clkDev->regmap.base, pllCfg->clkSrc);
    if (srcFreq == 0)
        return WHAL_EINVAL;

    /*
     * PLL1 output (pll1_p_ck):
     *   f_vco = (f_src / m) * (n + 1)
     *   f_pllp = f_vco / (p + 1)
     *
     * DIVM1: register value is the divisor directly (1-63, 0=disabled)
     * DIVN1: register value + 1 is the multiplier
     * DIVP1: register value + 1 is the divisor
     */
    pllm = pllCfg->m;
    plln = pllCfg->n + 1;
    pllp = pllCfg->p + 1;

    *rateOut = ((srcFreq / pllm) * plln) / pllp;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7RccHsi_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    size_t hsidiv;

    if (!clkDev || !rateOut)
        return WHAL_EINVAL;

    whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                 RCC_CR_HSIDIV_Pos, &hsidiv);

    /* HSI divider: 0=div1(64MHz), 1=div2(32MHz), 2=div4(16MHz), 3=div8(8MHz) */
    *rateOut = HSI_FREQ >> hsidiv;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Rcc_Ext_EnableHsi48(whal_Clock *clkDev, uint8_t enable)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSI48ON_Msk,
                    whal_SetBits(RCC_CR_HSI48ON_Msk, RCC_CR_HSI48ON_Pos,
                                 enable ? 1 : 0));

    if (enable) {
        while (1) {
            size_t rdy;
            whal_Reg_Get(base, RCC_CR_REG, RCC_CR_HSI48RDY_Msk,
                         RCC_CR_HSI48RDY_Pos, &rdy);
            if (rdy)
                break;
        }
    }

    return WHAL_SUCCESS;
}

const whal_ClockDriver whal_Stm32h7RccPll_Driver = {
    .Init = whal_Stm32h7RccPll_Init,
    .Deinit = whal_Stm32h7RccPll_Deinit,
    .Enable = whal_Stm32h7Rcc_Enable,
    .Disable = whal_Stm32h7Rcc_Disable,
    .GetRate = whal_Stm32h7RccPll_GetRate,
};

const whal_ClockDriver whal_Stm32h7RccHsi_Driver = {
    .Init = whal_Stm32h7RccHsi_Init,
    .Deinit = whal_Stm32h7RccHsi_Deinit,
    .Enable = whal_Stm32h7Rcc_Enable,
    .Disable = whal_Stm32h7Rcc_Disable,
    .GetRate = whal_Stm32h7RccHsi_GetRate,
};
