#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32h5_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32H5 RCC Register Definitions
 *
 * The RCC peripheral manages:
 * - Clock source selection and PLL1/PLL2/PLL3 configuration
 * - System clock (SYSCLK) routing
 * - Bus clock prescalers (AHB, APB1, APB2, APB3)
 * - Peripheral clock gating
 *
 * RCC base address: 0x44020C00
 */

/* Clock Control Register */
#define RCC_CR_REG 0x000
#define RCC_CR_HSION_Pos 0
#define RCC_CR_HSION_Msk (1UL << RCC_CR_HSION_Pos)

#define RCC_CR_HSIRDY_Pos 1
#define RCC_CR_HSIRDY_Msk (1UL << RCC_CR_HSIRDY_Pos)

#define RCC_CR_HSIDIV_Pos 3
#define RCC_CR_HSIDIV_Msk (WHAL_BITMASK(2) << RCC_CR_HSIDIV_Pos)

#define RCC_CR_HSIDIVF_Pos 5
#define RCC_CR_HSIDIVF_Msk (1UL << RCC_CR_HSIDIVF_Pos)

#define RCC_CR_CSION_Pos 8
#define RCC_CR_CSION_Msk (1UL << RCC_CR_CSION_Pos)

#define RCC_CR_CSIRDY_Pos 9
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

/* Clock Configuration Register 1 */
#define RCC_CFGR1_REG 0x01C
#define RCC_CFGR1_SW_Pos 0
#define RCC_CFGR1_SW_Msk (WHAL_BITMASK(2) << RCC_CFGR1_SW_Pos)

#define RCC_CFGR1_SWS_Pos 3
#define RCC_CFGR1_SWS_Msk (WHAL_BITMASK(2) << RCC_CFGR1_SWS_Pos)

/* PLL1 Clock Source Selection Register */
#define RCC_PLL1CFGR_REG 0x028
#define RCC_PLL1CFGR_PLL1SRC_Pos 0
#define RCC_PLL1CFGR_PLL1SRC_Msk (WHAL_BITMASK(2) << RCC_PLL1CFGR_PLL1SRC_Pos)

#define RCC_PLL1CFGR_PLL1RGE_Pos 2
#define RCC_PLL1CFGR_PLL1RGE_Msk (WHAL_BITMASK(2) << RCC_PLL1CFGR_PLL1RGE_Pos)

#define RCC_PLL1CFGR_PLL1VCOSEL_Pos 5
#define RCC_PLL1CFGR_PLL1VCOSEL_Msk (1UL << RCC_PLL1CFGR_PLL1VCOSEL_Pos)

#define RCC_PLL1CFGR_PLL1M_Pos 8
#define RCC_PLL1CFGR_PLL1M_Msk (WHAL_BITMASK(6) << RCC_PLL1CFGR_PLL1M_Pos)

#define RCC_PLL1CFGR_PLL1PEN_Pos 16
#define RCC_PLL1CFGR_PLL1PEN_Msk (1UL << RCC_PLL1CFGR_PLL1PEN_Pos)

#define RCC_PLL1CFGR_PLL1QEN_Pos 17
#define RCC_PLL1CFGR_PLL1QEN_Msk (1UL << RCC_PLL1CFGR_PLL1QEN_Pos)

#define RCC_PLL1CFGR_PLL1REN_Pos 18
#define RCC_PLL1CFGR_PLL1REN_Msk (1UL << RCC_PLL1CFGR_PLL1REN_Pos)

/* PLL1 Dividers Register */
#define RCC_PLL1DIVR_REG 0x034
#define RCC_PLL1DIVR_PLL1N_Pos 0
#define RCC_PLL1DIVR_PLL1N_Msk (WHAL_BITMASK(9) << RCC_PLL1DIVR_PLL1N_Pos)

#define RCC_PLL1DIVR_PLL1P_Pos 9
#define RCC_PLL1DIVR_PLL1P_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_PLL1P_Pos)

#define RCC_PLL1DIVR_PLL1Q_Pos 16
#define RCC_PLL1DIVR_PLL1Q_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_PLL1Q_Pos)

#define RCC_PLL1DIVR_PLL1R_Pos 24
#define RCC_PLL1DIVR_PLL1R_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_PLL1R_Pos)

/* HSI base frequency */
#define HSI_FREQ 64000000UL

/*
 * Compute the input reference frequency for PLL source selection.
 * Returns the HSI frequency accounting for the divider if HSI is selected.
 */
static size_t Stm32h5Rcc_GetPllInputFreq(size_t rccBase,
                                          whal_Stm32h5Rcc_PllClockSrc src)
{
    size_t hsidiv;

    switch (src) {
    case WHAL_STM32H5_RCC_PLLCLK_SRC_HSI:
        whal_Reg_Get(rccBase, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                     RCC_CR_HSIDIV_Pos, &hsidiv);
        return HSI_FREQ >> hsidiv;
    case WHAL_STM32H5_RCC_PLLCLK_SRC_CSI:
        return 4000000;
    case WHAL_STM32H5_RCC_PLLCLK_SRC_HSE:
        return 8000000; /* TODO: Make configurable per board */
    default:
        return 0;
    }
}

whal_Error whal_Stm32h5RccPll_Init(whal_Clock *clkDev)
{
    whal_Stm32h5Rcc_Cfg *cfg;
    whal_Stm32h5Rcc_PllClkCfg *pllCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32h5Rcc_PllClkCfg *)cfg->sysClkCfg;
    base = clkDev->regmap.base;

    /* Set HSI to div1 (64 MHz) if selected as PLL source */
    if (pllCfg->clkSrc == WHAL_STM32H5_RCC_PLLCLK_SRC_HSI) {
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
    if (pllCfg->clkSrc == WHAL_STM32H5_RCC_PLLCLK_SRC_HSE) {
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

    /* Configure PLL1 source and prescaler */
    whal_Reg_Update(base, RCC_PLL1CFGR_REG,
                    RCC_PLL1CFGR_PLL1SRC_Msk | RCC_PLL1CFGR_PLL1M_Msk |
                    RCC_PLL1CFGR_PLL1PEN_Msk | RCC_PLL1CFGR_PLL1REN_Msk,
                    whal_SetBits(RCC_PLL1CFGR_PLL1SRC_Msk, RCC_PLL1CFGR_PLL1SRC_Pos, pllCfg->clkSrc) |
                    whal_SetBits(RCC_PLL1CFGR_PLL1M_Msk, RCC_PLL1CFGR_PLL1M_Pos, pllCfg->m) |
                    whal_SetBits(RCC_PLL1CFGR_PLL1PEN_Msk, RCC_PLL1CFGR_PLL1PEN_Pos, 1) |
                    whal_SetBits(RCC_PLL1CFGR_PLL1REN_Msk, RCC_PLL1CFGR_PLL1REN_Pos, 1));

    /* Configure PLL1 multiplier and dividers */
    whal_Reg_Update(base, RCC_PLL1DIVR_REG,
                    RCC_PLL1DIVR_PLL1N_Msk | RCC_PLL1DIVR_PLL1P_Msk |
                    RCC_PLL1DIVR_PLL1Q_Msk | RCC_PLL1DIVR_PLL1R_Msk,
                    whal_SetBits(RCC_PLL1DIVR_PLL1N_Msk, RCC_PLL1DIVR_PLL1N_Pos, pllCfg->n) |
                    whal_SetBits(RCC_PLL1DIVR_PLL1P_Msk, RCC_PLL1DIVR_PLL1P_Pos, pllCfg->p) |
                    whal_SetBits(RCC_PLL1DIVR_PLL1Q_Msk, RCC_PLL1DIVR_PLL1Q_Pos, pllCfg->q) |
                    whal_SetBits(RCC_PLL1DIVR_PLL1R_Msk, RCC_PLL1DIVR_PLL1R_Pos, pllCfg->r));

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
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SW_Msk,
                    whal_SetBits(RCC_CFGR1_SW_Msk, RCC_CFGR1_SW_Pos,
                                 WHAL_STM32H5_RCC_SYSCLK_SRC_PLL1));

    /* Wait for switch to complete */
    while (1) {
        size_t sws;
        whal_Reg_Get(base, RCC_CFGR1_REG, RCC_CFGR1_SWS_Msk,
                     RCC_CFGR1_SWS_Pos, &sws);
        if (sws == WHAL_STM32H5_RCC_SYSCLK_SRC_PLL1)
            break;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5RccPll_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Switch back to HSI before disabling PLL1 */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SW_Msk,
                    whal_SetBits(RCC_CFGR1_SW_Msk, RCC_CFGR1_SW_Pos,
                                 WHAL_STM32H5_RCC_SYSCLK_SRC_HSI));

    /* Wait for switch to complete */
    while (1) {
        size_t sws;
        whal_Reg_Get(base, RCC_CFGR1_REG, RCC_CFGR1_SWS_Msk,
                     RCC_CFGR1_SWS_Pos, &sws);
        if (sws == WHAL_STM32H5_RCC_SYSCLK_SRC_HSI)
            break;
    }

    /* Disable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    whal_SetBits(RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5RccHsi_Init(whal_Clock *clkDev)
{
    whal_Stm32h5Rcc_Cfg *cfg;
    whal_Stm32h5Rcc_HsiClkCfg *hsiCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Rcc_Cfg *)clkDev->cfg;
    hsiCfg = (whal_Stm32h5Rcc_HsiClkCfg *)cfg->sysClkCfg;
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
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SW_Msk,
                    whal_SetBits(RCC_CFGR1_SW_Msk, RCC_CFGR1_SW_Pos,
                                 WHAL_STM32H5_RCC_SYSCLK_SRC_HSI));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5RccHsi_Deinit(whal_Clock *clkDev)
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

whal_Error whal_Stm32h5Rcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32h5Rcc_Clk *stClk = (whal_Stm32h5Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Rcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32h5Rcc_Clk *stClk = (whal_Stm32h5Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5RccPll_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    whal_Stm32h5Rcc_Cfg *cfg;
    whal_Stm32h5Rcc_PllClkCfg *pllCfg;
    size_t srcFreq;
    size_t pllm;
    size_t plln;
    size_t pllp;

    if (!clkDev || !clkDev->cfg || !rateOut)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32h5Rcc_PllClkCfg *)cfg->sysClkCfg;

    srcFreq = Stm32h5Rcc_GetPllInputFreq(clkDev->regmap.base, pllCfg->clkSrc);
    if (srcFreq == 0)
        return WHAL_EINVAL;

    /*
     * PLL1 output (pll1_p_ck):
     *   f_vco = (f_src / m) * (n + 1)
     *   f_pllp = f_vco / (p + 1)
     *
     * PLL1M: register value is the divisor directly (1-63, 0=disabled)
     * PLL1N: register value + 1 is the multiplier (3=4x, 4=5x, ...)
     * PLL1P: register value + 1 is the divisor (1=/2, 2=/3, ...)
     */
    pllm = pllCfg->m;
    plln = pllCfg->n + 1;
    pllp = pllCfg->p + 1;

    *rateOut = ((srcFreq / pllm) * plln) / pllp;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5RccHsi_GetRate(whal_Clock *clkDev, size_t *rateOut)
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

const whal_ClockDriver whal_Stm32h5RccPll_Driver = {
    .Init = whal_Stm32h5RccPll_Init,
    .Deinit = whal_Stm32h5RccPll_Deinit,
    .Enable = whal_Stm32h5Rcc_Enable,
    .Disable = whal_Stm32h5Rcc_Disable,
    .GetRate = whal_Stm32h5RccPll_GetRate,
};

const whal_ClockDriver whal_Stm32h5RccHsi_Driver = {
    .Init = whal_Stm32h5RccHsi_Init,
    .Deinit = whal_Stm32h5RccHsi_Deinit,
    .Enable = whal_Stm32h5Rcc_Enable,
    .Disable = whal_Stm32h5Rcc_Disable,
    .GetRate = whal_Stm32h5RccHsi_GetRate,
};
