#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32WB RCC Register Definitions
 *
 * The RCC (Reset and Clock Control) peripheral manages:
 * - Clock source selection and PLL configuration
 * - System clock (SYSCLK) routing
 * - Bus clock prescalers (AHB, APB1, APB2)
 * - Peripheral clock gating
 */

/* Clock Control Register - oscillator enables and status */
#define RCC_CR_REG 0x000
#define RCC_CR_MSIRANGE_Pos 4                                              /* MSI frequency range */
#define RCC_CR_MSIRANGE_Msk (WHAL_BITMASK(4) << RCC_CR_MSIRANGE_Pos)

#define RCC_CR_HSION_Pos 8                                                 /* HSI enable */
#define RCC_CR_HSION_Msk (1UL << RCC_CR_HSION_Pos)

#define RCC_CR_PLLON_Pos 24                                                /* PLL enable */
#define RCC_CR_PLLON_Msk (1UL << RCC_CR_PLLON_Pos)

/* Clock Configuration Register - clock source and prescaler selection */
#define RCC_CFGR_REG 0x008
#define RCC_CFGR_SW_Pos 0                                                  /* System clock switch */
#define RCC_CFGR_SW_Msk (WHAL_BITMASK(2) << RCC_CFGR_SW_Pos)

#define RCC_CFGR_SWS_Pos 2                                                 /* System clock switch status */
#define RCC_CFGR_SWS_Msk (WHAL_BITMASK(2) << RCC_CFGR_SWS_Pos)

#define RCC_CFGR_HPRE_Pos 4                                                /* AHB prescaler */
#define RCC_CFGR_HPRE_Msk (WHAL_BITMASK(4) << RCC_CFGR_HPRE_Pos)

#define RCC_CFGR_PPRE1_Pos 8                                               /* APB1 prescaler */
#define RCC_CFGR_PPRE1_Msk (WHAL_BITMASK(3) << RCC_CFGR_PPRE1_Pos)

#define RCC_CFGR_PPRE2_Pos 11                                              /* APB2 prescaler */
#define RCC_CFGR_PPRE2_Msk (WHAL_BITMASK(3) << RCC_CFGR_PPRE2_Pos)

#define RCC_CFGR_STOPWUCK_Pos 15                                           /* Wakeup clock after stop */
#define RCC_CFGR_STOPWUCK_Msk (1UL << RCC_CFGR_STOPWUCK_Pos)

#define RCC_CFGR_HPREF_Pos 16                                              /* AHB prescaler flag */
#define RCC_CFGR_HPREF_Msk (1UL << RCC_CFGR_HPREF_Pos)

#define RCC_CFGR_PPRE1F_Pos 17                                             /* APB1 prescaler flag */
#define RCC_CFGR_PPRE1F_Msk (1UL << RCC_CFGR_PPRE1F_Pos)

#define RCC_CFGR_PPRE2F_Pos 18                                             /* APB2 prescaler flag */
#define RCC_CFGR_PPRE2F_Msk (1UL << RCC_CFGR_PPRE2F_Pos)

#define RCC_CFGR_MCOSEL_Pos 24                                             /* MCO source selection */
#define RCC_CFGR_MCOSEL_Msk (WHAL_BITMASK(4) << RCC_CFGR_MCOSEL_Pos)

#define RCC_CFGR_MCOPRE_Pos 28                                             /* MCO prescaler */
#define RCC_CFGR_MCOPRE_Msk (WHAL_BITMASK(3) << RCC_CFGR_MCOPRE_Pos)

/* PLL Configuration Register */
#define RCC_PLLCFGR_REG 0x00C
#define RCC_PLLCFGR_PLLSRC_Pos 0                                              /* PLL input source */
#define RCC_PLLCFGR_PLLSRC_Msk (WHAL_BITMASK(2) << RCC_PLLCFGR_PLLSRC_Pos)

#define RCC_PLLCFGR_PLLM_Pos 4                                                /* PLL input divider */
#define RCC_PLLCFGR_PLLM_Msk (WHAL_BITMASK(3) << RCC_PLLCFGR_PLLM_Pos)

#define RCC_PLLCFGR_PLLN_Pos 8                                                /* PLL VCO multiplier */
#define RCC_PLLCFGR_PLLN_Msk (WHAL_BITMASK(7) << RCC_PLLCFGR_PLLN_Pos)

#define RCC_PLLCFGR_PLLP_Pos 17                                               /* PLLP output divider */
#define RCC_PLLCFGR_PLLP_Msk (WHAL_BITMASK(5) << RCC_PLLCFGR_PLLP_Pos)

#define RCC_PLLCFGR_PLLQ_Pos 25                                               /* PLLQ output divider */
#define RCC_PLLCFGR_PLLQ_Msk (WHAL_BITMASK(3) << RCC_PLLCFGR_PLLQ_Pos)

#define RCC_PLLCFGR_PLLREN_Pos 28                                             /* PLLR output enable */
#define RCC_PLLCFGR_PLLREN_Msk (1UL << RCC_PLLCFGR_PLLREN_Pos)

#define RCC_PLLCFGR_PLLR_Pos 29                                               /* PLLR output divider */
#define RCC_PLLCFGR_PLLR_Msk (WHAL_BITMASK(3) << RCC_PLLCFGR_PLLR_Pos)

#define RCC_PLLCFGR_Msk \
        (RCC_PLLCFGR_PLLSRC_Msk | \
        RCC_PLLCFGR_PLLM_Msk | \
        RCC_PLLCFGR_PLLN_Msk | \
        RCC_PLLCFGR_PLLP_Msk | \
        RCC_PLLCFGR_PLLQ_Msk | \
        RCC_PLLCFGR_PLLREN_Msk | \
        RCC_PLLCFGR_PLLR_Msk)

/* AHB2 Peripheral Clock Enable Register */
#define RCC_AHB2ENR_REG 0x04C
#define RCC_AHB2ENR_GPIOAEN_Pos 0                                                 /* GPIOA clock enable */
#define RCC_AHB2ENR_GPIOAEN_Msk (1UL << RCC_AHB2ENR_GPIOAEN_Pos)

#define RCC_AHB2ENR_GPIOBEN_Pos 1                                                 /* GPIOB clock enable */
#define RCC_AHB2ENR_GPIOBEN_Msk (1UL << RCC_AHB2ENR_GPIOBEN_Pos)

#define RCC_AHB2ENR_GPIOCEN_Pos 2                                                 /* GPIOC clock enable */
#define RCC_AHB2ENR_GPIOCEN_Msk (1UL << RCC_AHB2ENR_GPIOCEN_Pos)

#define RCC_AHB2ENR_GPIODEN_Pos 3                                                 /* GPIOD clock enable */
#define RCC_AHB2ENR_GPIODEN_Msk (1UL << RCC_AHB2ENR_GPIODEN_Pos)

#define RCC_AHB2ENR_GPIOEEN_Pos 4                                                 /* GPIOE clock enable */
#define RCC_AHB2ENR_GPIOEEN_Msk (1UL << RCC_AHB2ENR_GPIOEEN_Pos)

#define RCC_AHB2ENR_GPIOHEN_Pos 7                                                 /* GPIOH clock enable */
#define RCC_AHB2ENR_GPIOHEN_Msk (1UL << RCC_AHB2ENR_GPIOHEN_Pos)

#define RCC_AHB2ENR_ADCEN_Pos 13                                                  /* ADC clock enable */
#define RCC_AHB2ENR_ADCEN_Msk (1UL << RCC_AHB2ENR_ADCEN_Pos)

#define RCC_AHB2ENR_AES1EN_Pos 16                                                 /* AES1 clock enable */
#define RCC_AHB2ENR_AES1EN_Msk (1UL << RCC_AHB2ENR_AES1EN_Pos)

/* AHB3 Peripheral Clock Enable Register */
#define RCC_AHB3ENR_REG 0x50
#define RCC_AHB3ENR_FLASHEN_Pos 25                                                /* Flash interface clock enable */
#define RCC_AHB3ENR_FLASHEN_Msk (1UL << RCC_AHB3ENR_FLASHEN_Pos)

/* APB1 Peripheral Clock Enable Register 2 */
#define RCC_APB1ENR2_REG 0x05C
#define RCC_APB1ENR2_LPUART1EN_Pos 0                                                  /* LPUART1 clock enable */
#define RCC_APB1ENR2_LPUART1EN_Msk (1UL << RCC_APB1ENR2_LPUART1EN_Pos)

#define RCC_APB1ENR2_LPTIM2EN_Pos 5                                                   /* LPTIM2 clock enable */
#define RCC_APB1ENR2_LPTIM2EN_Msk (1UL << RCC_APB1ENR2_LPTIM2EN_Pos)

/* Clock Recovery RC Register - HSI48 oscillator control */
#define RCC_CRRCR_REG 0x098
#define RCC_CRRCR_HSI48ON_Pos 0                                                   /* HSI48 oscillator enable */
#define RCC_CRRCR_HSI48ON_Msk (1UL << RCC_CRRCR_HSI48ON_Pos)

#define RCC_CRRCR_HSI48RDY_Pos 1                                                  /* HSI48 oscillator ready */
#define RCC_CRRCR_HSI48RDY_Msk (1UL << RCC_CRRCR_HSI48RDY_Pos)

whal_Error whal_Stm32wbRccPll_Init(whal_Clock *clkDev)
{
    whal_Stm32wbRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32wbRcc_Cfg *)clkDev->cfg;
    whal_Stm32wbRcc_PllClkCfg *pllCfg = cfg->sysClkCfg;

    /* Select system clock source (PLL in this case) */
    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, cfg->sysClkSrc));

    /* Configure PLL: source, dividers, and multiplier */
    whal_Reg_Update(clkDev->regmap.base,
                    RCC_PLLCFGR_REG, RCC_PLLCFGR_Msk,
                    whal_SetBits(RCC_PLLCFGR_PLLSRC_Msk, RCC_PLLCFGR_PLLSRC_Pos, pllCfg->clkSrc) |
                    whal_SetBits(RCC_PLLCFGR_PLLM_Msk, RCC_PLLCFGR_PLLM_Pos, pllCfg->m) |
                    whal_SetBits(RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_Pos, pllCfg->n) |
                    whal_SetBits(RCC_PLLCFGR_PLLP_Msk, RCC_PLLCFGR_PLLP_Pos, pllCfg->p) |
                    whal_SetBits(RCC_PLLCFGR_PLLQ_Msk, RCC_PLLCFGR_PLLQ_Pos, pllCfg->q) |
                    whal_SetBits(RCC_PLLCFGR_PLLREN_Msk, RCC_PLLCFGR_PLLREN_Pos, 1) |
                    whal_SetBits(RCC_PLLCFGR_PLLR_Msk, RCC_PLLCFGR_PLLR_Pos, pllCfg->r));

    /* Enable the PLL */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG,
                    RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccMsi_Init(whal_Clock *clkDev)
{
    whal_Stm32wbRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32wbRcc_Cfg *)clkDev->cfg;
    whal_Stm32wbRcc_MsiClkCfg *msiCfg = cfg->sysClkCfg;

    /* Select MSI as system clock source */
    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, WHAL_STM32WB_RCC_SYSCLK_SRC_MSI));

    /* Set MSI frequency range */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_MSIRANGE_Msk,
                    whal_SetBits(RCC_CR_MSIRANGE_Msk, RCC_CR_MSIRANGE_Pos, msiCfg->freq));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccPll_Deinit(whal_Clock *clkDev)
{
    if (!clkDev) {
        return WHAL_EINVAL;
    }

    /* Switch back to MSI before disabling PLL */
    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, WHAL_STM32WB_RCC_SYSCLK_SRC_MSI));

    /* Reset MSI to default 4 MHz */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_MSIRANGE_Msk,
                    whal_SetBits(RCC_CR_MSIRANGE_Msk, RCC_CR_MSIRANGE_Pos, WHAL_STM32WB_RCC_MSIRANGE_4MHz));

    /* Disable the PLL */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG,
                    RCC_CR_PLLON_Msk,
                    whal_SetBits(RCC_CR_PLLON_Msk, RCC_CR_PLLON_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccMsi_Deinit(whal_Clock *clkDev)
{
    if (!clkDev) {
        return WHAL_EINVAL;
    }

    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, WHAL_STM32WB_RCC_SYSCLK_SRC_MSI));

    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_MSIRANGE_Msk,
                    whal_SetBits(RCC_CR_MSIRANGE_Msk, RCC_CR_MSIRANGE_Pos, WHAL_STM32WB_RCC_MSIRANGE_4MHz));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32wbRcc_Clk *stClk = (whal_Stm32wbRcc_Clk *)clk;

    /* Set the peripheral's enable bit in the appropriate RCC enable register */
    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32wbRcc_Clk *stClk = (whal_Stm32wbRcc_Clk *)clk;

    /* Clear the peripheral's enable bit to gate its clock */
    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccPll_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    whal_Stm32wbRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32wbRcc_Cfg *)clkDev->cfg;
    whal_Stm32wbRcc_PllClkCfg *pllClkCfg = cfg->sysClkCfg;

    /*
     * Calculate PLL output frequency:
     *   f_pllr = ((f_src / (m + 1)) * n) / (r + 1)
     */
    size_t srcFreq;
    size_t pllm = pllClkCfg->m + 1;
    size_t plln = pllClkCfg->n;
    size_t pllr = pllClkCfg->r + 1;

    /* Determine source frequency based on PLL input selection */
    if (pllClkCfg->clkSrc == WHAL_STM32WB_RCC_PLLCLK_SRC_MSI) {
        srcFreq = 4000000;
    }
    else {
        return WHAL_EINVAL;
    }

    *rateOut = ((srcFreq / pllm) * plln) / pllr;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccMsi_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    size_t msiRange;

    if (!clkDev || !rateOut) {
        return WHAL_EINVAL;
    }

    /* Read current MSI range from hardware */
    whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG, RCC_CR_MSIRANGE_Msk,
                 RCC_CR_MSIRANGE_Pos, &msiRange);

    /* Map range setting to frequency in Hz */
    switch (msiRange) {
    case WHAL_STM32WB_RCC_MSIRANGE_100kHz:
        *rateOut = 100000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_200kHz:
        *rateOut = 200000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_400kHz:
        *rateOut = 400000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_800kHz:
        *rateOut = 800000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_1MHz:
        *rateOut = 1000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_2MHz:
        *rateOut = 2000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_4MHz:
        *rateOut = 4000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_8MHz:
        *rateOut = 8000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_16MHz:
        *rateOut = 16000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_24MHz:
        *rateOut = 24000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_32MHz:
        *rateOut = 32000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_48MHz:
        *rateOut = 48000000;
        break;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRcc_Ext_EnableHsi48(whal_Clock *clkDev, uint8_t enable)
{
    if (!clkDev) {
        return WHAL_EINVAL;
    }

    whal_Reg_Update(clkDev->regmap.base, RCC_CRRCR_REG, RCC_CRRCR_HSI48ON_Msk,
                    whal_SetBits(RCC_CRRCR_HSI48ON_Msk, RCC_CRRCR_HSI48ON_Pos, enable));

    if (enable) {
        size_t rdy;
        do {
            whal_Reg_Get(clkDev->regmap.base, RCC_CRRCR_REG,
                         RCC_CRRCR_HSI48RDY_Msk, RCC_CRRCR_HSI48RDY_Pos, &rdy);
        } while (!rdy);
    }

    return WHAL_SUCCESS;
}

const whal_ClockDriver whal_Stm32wbRccPll_Driver = {
    .Init = whal_Stm32wbRccPll_Init,
    .Deinit = whal_Stm32wbRccPll_Deinit,
    .Enable = whal_Stm32wbRcc_Enable,
    .Disable = whal_Stm32wbRcc_Disable,
    .GetRate = whal_Stm32wbRccPll_GetRate,
};

const whal_ClockDriver whal_Stm32wbRccMsi_Driver = {
    .Init = whal_Stm32wbRccMsi_Init,
    .Deinit = whal_Stm32wbRccMsi_Deinit,
    .Enable = whal_Stm32wbRcc_Enable,
    .Disable = whal_Stm32wbRcc_Disable,
    .GetRate = whal_Stm32wbRccMsi_GetRate,
};
