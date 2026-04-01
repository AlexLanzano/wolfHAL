#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32n6_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32N6 RCC Register Definitions (from RM0486)
 *
 * The RCC peripheral manages:
 * - Clock source selection (HSI, MSI, HSE) and PLL1-4 configuration
 * - CPU clock (sys_cpu_ck) and system bus clock (sys_bus_ck) routing
 * - Bus clock prescalers (AHB, APB1-5)
 * - Peripheral clock gating
 * - IC1-IC20 intermediate clock dividers
 *
 * RCC base address: 0x46028000
 */

/* Clock Control Register */
#define RCC_CR_REG 0x000
#define RCC_CR_HSION_Pos 3
#define RCC_CR_HSION_Msk (1UL << RCC_CR_HSION_Pos)

#define RCC_CR_HSEON_Pos 4
#define RCC_CR_HSEON_Msk (1UL << RCC_CR_HSEON_Pos)

#define RCC_CR_PLL1ON_Pos 8
#define RCC_CR_PLL1ON_Msk (1UL << RCC_CR_PLL1ON_Pos)

/* Clock Status Register */
#define RCC_SR_REG 0x004
#define RCC_SR_HSIRDY_Pos 3
#define RCC_SR_HSIRDY_Msk (1UL << RCC_SR_HSIRDY_Pos)

#define RCC_SR_HSERDY_Pos 4
#define RCC_SR_HSERDY_Msk (1UL << RCC_SR_HSERDY_Pos)

#define RCC_SR_PLL1RDY_Pos 8
#define RCC_SR_PLL1RDY_Msk (1UL << RCC_SR_PLL1RDY_Pos)

/* Clock Configuration Register 1 */
#define RCC_CFGR1_REG 0x020
#define RCC_CFGR1_CPUSW_Pos 16
#define RCC_CFGR1_CPUSW_Msk (WHAL_BITMASK(2) << RCC_CFGR1_CPUSW_Pos)

#define RCC_CFGR1_CPUSWS_Pos 20
#define RCC_CFGR1_CPUSWS_Msk (WHAL_BITMASK(2) << RCC_CFGR1_CPUSWS_Pos)

#define RCC_CFGR1_SYSSW_Pos 24
#define RCC_CFGR1_SYSSW_Msk (WHAL_BITMASK(2) << RCC_CFGR1_SYSSW_Pos)

#define RCC_CFGR1_SYSSWS_Pos 28
#define RCC_CFGR1_SYSSWS_Msk (WHAL_BITMASK(2) << RCC_CFGR1_SYSSWS_Pos)

/* HSI Configuration Register */
#define RCC_HSICFGR_REG 0x048
#define RCC_HSICFGR_HSIDIV_Pos 7
#define RCC_HSICFGR_HSIDIV_Msk (WHAL_BITMASK(2) << RCC_HSICFGR_HSIDIV_Pos)

/* PLL1 Configuration Register 1 */
#define RCC_PLL1CFGR1_REG 0x080
#define RCC_PLL1CFGR1_PLL1DIVN_Pos 8
#define RCC_PLL1CFGR1_PLL1DIVN_Msk (WHAL_BITMASK(12) << RCC_PLL1CFGR1_PLL1DIVN_Pos)

#define RCC_PLL1CFGR1_PLL1DIVM_Pos 20
#define RCC_PLL1CFGR1_PLL1DIVM_Msk (WHAL_BITMASK(6) << RCC_PLL1CFGR1_PLL1DIVM_Pos)

#define RCC_PLL1CFGR1_PLL1BYP_Pos 27
#define RCC_PLL1CFGR1_PLL1BYP_Msk (1UL << RCC_PLL1CFGR1_PLL1BYP_Pos)

#define RCC_PLL1CFGR1_PLL1SEL_Pos 28
#define RCC_PLL1CFGR1_PLL1SEL_Msk (WHAL_BITMASK(3) << RCC_PLL1CFGR1_PLL1SEL_Pos)

/* PLL1 Configuration Register 3 - post dividers */
#define RCC_PLL1CFGR3_REG 0x088
#define RCC_PLL1CFGR3_PLL1PDIV2_Pos 24
#define RCC_PLL1CFGR3_PLL1PDIV2_Msk (WHAL_BITMASK(3) << RCC_PLL1CFGR3_PLL1PDIV2_Pos)

#define RCC_PLL1CFGR3_PLL1PDIV1_Pos 27
#define RCC_PLL1CFGR3_PLL1PDIV1_Msk (WHAL_BITMASK(3) << RCC_PLL1CFGR3_PLL1PDIV1_Pos)

#define RCC_PLL1CFGR3_PLL1PDIVEN_Pos 30
#define RCC_PLL1CFGR3_PLL1PDIVEN_Msk (1UL << RCC_PLL1CFGR3_PLL1PDIVEN_Pos)

/* IC1 Configuration Register */
#define RCC_IC1CFGR_REG 0x0C8
#define RCC_IC1CFGR_IC1INT_Pos 16
#define RCC_IC1CFGR_IC1INT_Msk (WHAL_BITMASK(8) << RCC_IC1CFGR_IC1INT_Pos)

#define RCC_IC1CFGR_IC1SEL_Pos 28
#define RCC_IC1CFGR_IC1SEL_Msk (WHAL_BITMASK(2) << RCC_IC1CFGR_IC1SEL_Pos)

/* IC Dividers Enable Register */
#define RCC_DIVENR_REG 0x240
#define RCC_DIVENR_IC1EN_Pos 0
#define RCC_DIVENR_IC1EN_Msk (1UL << RCC_DIVENR_IC1EN_Pos)

/* Embedded Buses Enable Register */
#define RCC_BUSENR_REG 0x244

/* HSI base frequency */
#define HSI_FREQ 64000000UL

whal_Error whal_Stm32n6RccHsi_Init(whal_Clock *clkDev)
{
    whal_Stm32n6Rcc_Cfg *cfg;
    whal_Stm32n6Rcc_HsiClkCfg *hsiCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32n6Rcc_Cfg *)clkDev->cfg;
    hsiCfg = (whal_Stm32n6Rcc_HsiClkCfg *)cfg->sysClkCfg;
    base = clkDev->regmap.base;

    /* Enable HSI */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSION_Msk,
                    whal_SetBits(RCC_CR_HSION_Msk, RCC_CR_HSION_Pos, 1));

    /* Wait for HSI to be ready */
    while (1) {
        size_t rdy;
        whal_Reg_Get(base, RCC_SR_REG, RCC_SR_HSIRDY_Msk,
                     RCC_SR_HSIRDY_Pos, &rdy);
        if (rdy)
            break;
    }

    /* Set HSI divider */
    whal_Reg_Update(base, RCC_HSICFGR_REG, RCC_HSICFGR_HSIDIV_Msk,
                    whal_SetBits(RCC_HSICFGR_HSIDIV_Msk, RCC_HSICFGR_HSIDIV_Pos,
                                 hsiCfg->div));

    /* Select HSI as CPU clock source */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_CPUSW_Msk,
                    whal_SetBits(RCC_CFGR1_CPUSW_Msk, RCC_CFGR1_CPUSW_Pos,
                                 WHAL_STM32N6_RCC_CPUCLK_SRC_HSI));

    /* Select HSI as system bus clock source */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SYSSW_Msk,
                    whal_SetBits(RCC_CFGR1_SYSSW_Msk, RCC_CFGR1_SYSSW_Pos,
                                 WHAL_STM32N6_RCC_SYSCLK_SRC_HSI));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6RccHsi_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Switch back to HSI */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_CPUSW_Msk,
                    whal_SetBits(RCC_CFGR1_CPUSW_Msk, RCC_CFGR1_CPUSW_Pos,
                                 WHAL_STM32N6_RCC_CPUCLK_SRC_HSI));

    /* Reset HSI divider to default (div1) */
    whal_Reg_Update(base, RCC_HSICFGR_REG, RCC_HSICFGR_HSIDIV_Msk,
                    whal_SetBits(RCC_HSICFGR_HSIDIV_Msk, RCC_HSICFGR_HSIDIV_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6RccPll_Init(whal_Clock *clkDev)
{
    whal_Stm32n6Rcc_Cfg *cfg;
    whal_Stm32n6Rcc_PllClkCfg *pllCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32n6Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32n6Rcc_PllClkCfg *)cfg->sysClkCfg;
    base = clkDev->regmap.base;

    /* Enable HSE if selected as PLL source */
    if (pllCfg->clkSrc == WHAL_STM32N6_RCC_PLLCLK_SRC_HSE) {
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk,
                        whal_SetBits(RCC_CR_HSEON_Msk, RCC_CR_HSEON_Pos, 1));
        while (1) {
            size_t rdy;
            whal_Reg_Get(base, RCC_SR_REG, RCC_SR_HSERDY_Msk,
                         RCC_SR_HSERDY_Pos, &rdy);
            if (rdy)
                break;
        }
    }

    /* Ensure PLL1 is off before configuring */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    whal_SetBits(RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Pos, 0));

    /* Configure PLL1: source, DIVM, DIVN */
    whal_Reg_Update(base, RCC_PLL1CFGR1_REG,
                    RCC_PLL1CFGR1_PLL1SEL_Msk | RCC_PLL1CFGR1_PLL1DIVM_Msk |
                    RCC_PLL1CFGR1_PLL1DIVN_Msk | RCC_PLL1CFGR1_PLL1BYP_Msk,
                    whal_SetBits(RCC_PLL1CFGR1_PLL1SEL_Msk, RCC_PLL1CFGR1_PLL1SEL_Pos,
                                 pllCfg->clkSrc) |
                    whal_SetBits(RCC_PLL1CFGR1_PLL1DIVM_Msk, RCC_PLL1CFGR1_PLL1DIVM_Pos,
                                 pllCfg->m) |
                    whal_SetBits(RCC_PLL1CFGR1_PLL1DIVN_Msk, RCC_PLL1CFGR1_PLL1DIVN_Pos,
                                 pllCfg->n) |
                    whal_SetBits(RCC_PLL1CFGR1_PLL1BYP_Msk, RCC_PLL1CFGR1_PLL1BYP_Pos, 0));

    /* Configure PLL1 post dividers and enable them */
    whal_Reg_Update(base, RCC_PLL1CFGR3_REG,
                    RCC_PLL1CFGR3_PLL1PDIV1_Msk | RCC_PLL1CFGR3_PLL1PDIV2_Msk |
                    RCC_PLL1CFGR3_PLL1PDIVEN_Msk,
                    whal_SetBits(RCC_PLL1CFGR3_PLL1PDIV1_Msk, RCC_PLL1CFGR3_PLL1PDIV1_Pos,
                                 pllCfg->p1) |
                    whal_SetBits(RCC_PLL1CFGR3_PLL1PDIV2_Msk, RCC_PLL1CFGR3_PLL1PDIV2_Pos,
                                 pllCfg->p2) |
                    whal_SetBits(RCC_PLL1CFGR3_PLL1PDIVEN_Msk, RCC_PLL1CFGR3_PLL1PDIVEN_Pos,
                                 1));

    /* Enable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    whal_SetBits(RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Pos, 1));

    /* Wait for PLL1 lock */
    while (1) {
        size_t rdy;
        whal_Reg_Get(base, RCC_SR_REG, RCC_SR_PLL1RDY_Msk,
                     RCC_SR_PLL1RDY_Pos, &rdy);
        if (rdy)
            break;
    }

    /* Configure IC1 divider: select PLL1, set divider */
    whal_Reg_Update(base, RCC_IC1CFGR_REG,
                    RCC_IC1CFGR_IC1SEL_Msk | RCC_IC1CFGR_IC1INT_Msk,
                    whal_SetBits(RCC_IC1CFGR_IC1SEL_Msk, RCC_IC1CFGR_IC1SEL_Pos, 0) |
                    whal_SetBits(RCC_IC1CFGR_IC1INT_Msk, RCC_IC1CFGR_IC1INT_Pos,
                                 pllCfg->ic1div));

    /* Enable IC1 divider */
    whal_Reg_Update(base, RCC_DIVENR_REG, RCC_DIVENR_IC1EN_Msk,
                    whal_SetBits(RCC_DIVENR_IC1EN_Msk, RCC_DIVENR_IC1EN_Pos, 1));

    /* Switch CPU clock to IC1 */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_CPUSW_Msk,
                    whal_SetBits(RCC_CFGR1_CPUSW_Msk, RCC_CFGR1_CPUSW_Pos,
                                 WHAL_STM32N6_RCC_CPUCLK_SRC_IC1));

    /* Wait for CPU clock switch to complete */
    while (1) {
        size_t sws;
        whal_Reg_Get(base, RCC_CFGR1_REG, RCC_CFGR1_CPUSWS_Msk,
                     RCC_CFGR1_CPUSWS_Pos, &sws);
        if (sws == WHAL_STM32N6_RCC_CPUCLK_SRC_IC1)
            break;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6RccPll_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Switch CPU clock back to HSI */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_CPUSW_Msk,
                    whal_SetBits(RCC_CFGR1_CPUSW_Msk, RCC_CFGR1_CPUSW_Pos,
                                 WHAL_STM32N6_RCC_CPUCLK_SRC_HSI));

    /* Wait for switch */
    while (1) {
        size_t sws;
        whal_Reg_Get(base, RCC_CFGR1_REG, RCC_CFGR1_CPUSWS_Msk,
                     RCC_CFGR1_CPUSWS_Pos, &sws);
        if (sws == WHAL_STM32N6_RCC_CPUCLK_SRC_HSI)
            break;
    }

    /* Disable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    whal_SetBits(RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6Rcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32n6Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32n6Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6Rcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32n6Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (whal_Stm32n6Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6RccPll_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    whal_Stm32n6Rcc_Cfg *cfg;
    whal_Stm32n6Rcc_PllClkCfg *pllCfg;
    size_t srcFreq;
    size_t pllm, plln, p1, p2, ic1div;

    if (!clkDev || !clkDev->cfg || !rateOut)
        return WHAL_EINVAL;

    cfg = (whal_Stm32n6Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32n6Rcc_PllClkCfg *)cfg->sysClkCfg;

    switch (pllCfg->clkSrc) {
    case WHAL_STM32N6_RCC_PLLCLK_SRC_HSI:
        srcFreq = HSI_FREQ;
        break;
    case WHAL_STM32N6_RCC_PLLCLK_SRC_HSE:
        srcFreq = 24000000; /* TODO: Make configurable per board */
        break;
    default:
        return WHAL_EINVAL;
    }

    pllm = pllCfg->m;
    plln = pllCfg->n;
    p1 = pllCfg->p1;
    p2 = pllCfg->p2;
    ic1div = pllCfg->ic1div;

    /*
     * PLL1 output:
     *   f_vco = (f_src / m) * n
     *   f_postdiv = f_vco / p1 / p2
     *   f_ic1 = f_postdiv / (ic1div + 1)
     */
    size_t rate = ((srcFreq / pllm) * plln) / p1 / p2;
    if (ic1div > 0)
        rate = rate / (ic1div + 1);

    *rateOut = rate;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6RccHsi_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    whal_Stm32n6Rcc_Cfg *cfg;
    whal_Stm32n6Rcc_HsiClkCfg *hsiCfg;

    if (!clkDev || !clkDev->cfg || !rateOut)
        return WHAL_EINVAL;

    cfg = (whal_Stm32n6Rcc_Cfg *)clkDev->cfg;
    hsiCfg = (whal_Stm32n6Rcc_HsiClkCfg *)cfg->sysClkCfg;

    /* HSI divider: 0=div1(64MHz), 1=div2(32MHz), 2=div4(16MHz), 3=div8(8MHz) */
    *rateOut = HSI_FREQ >> hsiCfg->div;
    return WHAL_SUCCESS;
}

const whal_ClockDriver whal_Stm32n6RccPll_Driver = {
    .Init = whal_Stm32n6RccPll_Init,
    .Deinit = whal_Stm32n6RccPll_Deinit,
    .Enable = whal_Stm32n6Rcc_Enable,
    .Disable = whal_Stm32n6Rcc_Disable,
    .GetRate = whal_Stm32n6RccPll_GetRate,
};

const whal_ClockDriver whal_Stm32n6RccHsi_Driver = {
    .Init = whal_Stm32n6RccHsi_Init,
    .Deinit = whal_Stm32n6RccHsi_Deinit,
    .Enable = whal_Stm32n6Rcc_Enable,
    .Disable = whal_Stm32n6Rcc_Disable,
    .GetRate = whal_Stm32n6RccHsi_GetRate,
};
