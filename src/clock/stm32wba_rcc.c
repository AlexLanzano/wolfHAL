#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32wba_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32WBA RCC Register Definitions (RM0493)
 *
 * RCC base: 0x46020C00
 */

/* RCC_CR (offset 0x000) - Clock control */
#define RCC_CR_REG          0x000
#define RCC_CR_HSEON_Pos    16
#define RCC_CR_HSEON_Msk    (1UL << RCC_CR_HSEON_Pos)
#define RCC_CR_HSERDY_Pos   17
#define RCC_CR_HSERDY_Msk   (1UL << RCC_CR_HSERDY_Pos)
#define RCC_CR_HSEPRE_Pos   20
#define RCC_CR_HSEPRE_Msk   (1UL << RCC_CR_HSEPRE_Pos)
#define RCC_CR_PLL1ON_Pos   24
#define RCC_CR_PLL1ON_Msk   (1UL << RCC_CR_PLL1ON_Pos)
#define RCC_CR_PLL1RDY_Pos  25
#define RCC_CR_PLL1RDY_Msk  (1UL << RCC_CR_PLL1RDY_Pos)
#define RCC_CR_HSION_Pos    8
#define RCC_CR_HSION_Msk    (1UL << RCC_CR_HSION_Pos)
#define RCC_CR_HSIRDY_Pos   10
#define RCC_CR_HSIRDY_Msk   (1UL << RCC_CR_HSIRDY_Pos)

/* RCC_CFGR1 (offset 0x01C) - Clock configuration 1 */
#define RCC_CFGR1_REG       0x01C
#define RCC_CFGR1_SW_Pos    0
#define RCC_CFGR1_SW_Msk    (3UL << RCC_CFGR1_SW_Pos)
#define RCC_CFGR1_SWS_Pos   2
#define RCC_CFGR1_SWS_Msk   (3UL << RCC_CFGR1_SWS_Pos)

/* RCC_CFGR2 (offset 0x020) - Clock configuration 2 (prescalers) */
#define RCC_CFGR2_REG       0x020
#define RCC_CFGR2_HPRE_Pos  0
#define RCC_CFGR2_HPRE_Msk  (7UL << RCC_CFGR2_HPRE_Pos)
#define RCC_CFGR2_PPRE1_Pos 4
#define RCC_CFGR2_PPRE1_Msk (7UL << RCC_CFGR2_PPRE1_Pos)
#define RCC_CFGR2_PPRE2_Pos 8
#define RCC_CFGR2_PPRE2_Msk (7UL << RCC_CFGR2_PPRE2_Pos)

/* RCC_CFGR3 (offset 0x024) - Clock configuration 3 (APB7, HPRE5) */
#define RCC_CFGR3_REG       0x024
#define RCC_CFGR3_PPRE7_Pos 4
#define RCC_CFGR3_PPRE7_Msk (7UL << RCC_CFGR3_PPRE7_Pos)

/* RCC_PLL1CFGR (offset 0x028) - PLL1 configuration */
#define RCC_PLL1CFGR_REG        0x028
#define RCC_PLL1CFGR_SRC_Pos    0
#define RCC_PLL1CFGR_SRC_Msk    (3UL << RCC_PLL1CFGR_SRC_Pos)
#define RCC_PLL1CFGR_RGE_Pos    2
#define RCC_PLL1CFGR_RGE_Msk    (3UL << RCC_PLL1CFGR_RGE_Pos)
#define RCC_PLL1CFGR_FRACEN_Pos 4
#define RCC_PLL1CFGR_FRACEN_Msk (1UL << RCC_PLL1CFGR_FRACEN_Pos)
#define RCC_PLL1CFGR_M_Pos      8
#define RCC_PLL1CFGR_M_Msk      (7UL << RCC_PLL1CFGR_M_Pos)
#define RCC_PLL1CFGR_PEN_Pos    16
#define RCC_PLL1CFGR_PEN_Msk    (1UL << RCC_PLL1CFGR_PEN_Pos)
#define RCC_PLL1CFGR_QEN_Pos    17
#define RCC_PLL1CFGR_QEN_Msk    (1UL << RCC_PLL1CFGR_QEN_Pos)
#define RCC_PLL1CFGR_REN_Pos    18
#define RCC_PLL1CFGR_REN_Msk    (1UL << RCC_PLL1CFGR_REN_Pos)

/* RCC_PLL1DIVR (offset 0x034) - PLL1 dividers */
#define RCC_PLL1DIVR_REG    0x034
#define RCC_PLL1DIVR_N_Pos  0
#define RCC_PLL1DIVR_N_Msk  (0x1FFUL << RCC_PLL1DIVR_N_Pos)
#define RCC_PLL1DIVR_P_Pos  9
#define RCC_PLL1DIVR_P_Msk  (0x7FUL << RCC_PLL1DIVR_P_Pos)
#define RCC_PLL1DIVR_Q_Pos  16
#define RCC_PLL1DIVR_Q_Msk  (0x7FUL << RCC_PLL1DIVR_Q_Pos)
#define RCC_PLL1DIVR_R_Pos  24
#define RCC_PLL1DIVR_R_Msk  (0x7FUL << RCC_PLL1DIVR_R_Pos)

/* RCC_BDCR1 (offset 0x0C8) - Backup domain control 1 */
#define RCC_BDCR1_REG       0x0C8
#define RCC_BDCR1_LSI1ON_Pos 2
#define RCC_BDCR1_LSI1ON_Msk (1UL << RCC_BDCR1_LSI1ON_Pos)
#define RCC_BDCR1_LSI1RDY_Pos 3
#define RCC_BDCR1_LSI1RDY_Msk (1UL << RCC_BDCR1_LSI1RDY_Pos)

/* RCC_CFGR4 (offset 0x080) - HPRE5 prescaler */
#define RCC_CFGR4_REG       0x080
#define RCC_CFGR4_HPRE5_Pos 0
#define RCC_CFGR4_HPRE5_Msk (7UL << RCC_CFGR4_HPRE5_Pos)

/* RCC_CCIPR2 (offset 0x0E4) - Peripheral independent clock config 2 */
#define RCC_CCIPR2_REG        0x0E4
#define RCC_CCIPR2_RNGSEL_Pos 12
#define RCC_CCIPR2_RNGSEL_Msk (3UL << RCC_CCIPR2_RNGSEL_Pos)

whal_Error whal_Stm32wbaRccPll_Init(whal_Clock *clkDev)
{
    whal_Stm32wbaRcc_Cfg *cfg;
    whal_Stm32wbaRcc_Pll1Cfg *pllCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32wbaRcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32wbaRcc_Pll1Cfg *)cfg->sysClkCfg;
    base = clkDev->regmap.base;

    /* Enable HSE32 oscillator */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk,
                    whal_SetBits(RCC_CR_HSEON_Msk, RCC_CR_HSEON_Pos, 1));

    /* Wait for HSE32 ready */
    while (!(whal_Reg_Read(base, RCC_CR_REG) & RCC_CR_HSERDY_Msk))
        ;

    /* Ensure PLL1 is off before configuring */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);
    while (whal_Reg_Read(base, RCC_CR_REG) & RCC_CR_PLL1RDY_Msk)
        ;

    /* Configure PLL1CFGR: source, M divider, RGE, enable R output */
    whal_Reg_Write(base, RCC_PLL1CFGR_REG,
                   whal_SetBits(RCC_PLL1CFGR_SRC_Msk, RCC_PLL1CFGR_SRC_Pos, pllCfg->clkSrc) |
                   whal_SetBits(RCC_PLL1CFGR_RGE_Msk, RCC_PLL1CFGR_RGE_Pos, pllCfg->rge) |
                   whal_SetBits(RCC_PLL1CFGR_M_Msk, RCC_PLL1CFGR_M_Pos, pllCfg->m) |
                   whal_SetBits(RCC_PLL1CFGR_REN_Msk, RCC_PLL1CFGR_REN_Pos, 1));

    /* Configure PLL1DIVR: N, P, Q, R dividers */
    whal_Reg_Write(base, RCC_PLL1DIVR_REG,
                   whal_SetBits(RCC_PLL1DIVR_N_Msk, RCC_PLL1DIVR_N_Pos, pllCfg->n) |
                   whal_SetBits(RCC_PLL1DIVR_P_Msk, RCC_PLL1DIVR_P_Pos, pllCfg->p) |
                   whal_SetBits(RCC_PLL1DIVR_Q_Msk, RCC_PLL1DIVR_Q_Pos, pllCfg->q) |
                   whal_SetBits(RCC_PLL1DIVR_R_Msk, RCC_PLL1DIVR_R_Pos, pllCfg->r));

    /* Enable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    whal_SetBits(RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Pos, 1));

    /* Wait for PLL1 lock */
    while (!(whal_Reg_Read(base, RCC_CR_REG) & RCC_CR_PLL1RDY_Msk))
        ;

    /* Switch system clock to PLL1 */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SW_Msk,
                    whal_SetBits(RCC_CFGR1_SW_Msk, RCC_CFGR1_SW_Pos, cfg->sysClkSrc));

    /* Wait for switch to complete */
    while ((whal_Reg_Read(base, RCC_CFGR1_REG) & RCC_CFGR1_SWS_Msk) !=
           whal_SetBits(RCC_CFGR1_SWS_Msk, RCC_CFGR1_SWS_Pos, cfg->sysClkSrc))
        ;

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRccPll_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Switch back to HSI16 */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SW_Msk,
                    whal_SetBits(RCC_CFGR1_SW_Msk, RCC_CFGR1_SW_Pos,
                                 WHAL_STM32WBA_RCC_SYSCLK_SRC_HSI16));

    while ((whal_Reg_Read(base, RCC_CFGR1_REG) & RCC_CFGR1_SWS_Msk) !=
           whal_SetBits(RCC_CFGR1_SWS_Msk, RCC_CFGR1_SWS_Pos,
                        WHAL_STM32WBA_RCC_SYSCLK_SRC_HSI16))
        ;

    /* Disable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);

    /* Disable HSE32 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRccHse_Init(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Enable HSE32 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk,
                    whal_SetBits(RCC_CR_HSEON_Msk, RCC_CR_HSEON_Pos, 1));

    while (!(whal_Reg_Read(base, RCC_CR_REG) & RCC_CR_HSERDY_Msk))
        ;

    /* Switch to HSE32 */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SW_Msk,
                    whal_SetBits(RCC_CFGR1_SW_Msk, RCC_CFGR1_SW_Pos,
                                 WHAL_STM32WBA_RCC_SYSCLK_SRC_HSE32));

    while ((whal_Reg_Read(base, RCC_CFGR1_REG) & RCC_CFGR1_SWS_Msk) !=
           whal_SetBits(RCC_CFGR1_SWS_Msk, RCC_CFGR1_SWS_Pos,
                        WHAL_STM32WBA_RCC_SYSCLK_SRC_HSE32))
        ;

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRccHse_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Switch back to HSI16 */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SW_Msk,
                    whal_SetBits(RCC_CFGR1_SW_Msk, RCC_CFGR1_SW_Pos,
                                 WHAL_STM32WBA_RCC_SYSCLK_SRC_HSI16));

    while ((whal_Reg_Read(base, RCC_CFGR1_REG) & RCC_CFGR1_SWS_Msk) !=
           whal_SetBits(RCC_CFGR1_SWS_Msk, RCC_CFGR1_SWS_Pos,
                        WHAL_STM32WBA_RCC_SYSCLK_SRC_HSI16))
        ;

    /* Disable HSE32 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRcc_Enable(whal_Clock *clkDev, const void *clk)
{
    const whal_Stm32wbaRcc_Clk *stClk = (const whal_Stm32wbaRcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRcc_Disable(whal_Clock *clkDev, const void *clk)
{
    const whal_Stm32wbaRcc_Clk *stClk = (const whal_Stm32wbaRcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRcc_Ext_EnableLsi(whal_Clock *clkDev, uint8_t enable)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    whal_Reg_Update(base, RCC_BDCR1_REG, RCC_BDCR1_LSI1ON_Msk,
                    whal_SetBits(RCC_BDCR1_LSI1ON_Msk, RCC_BDCR1_LSI1ON_Pos, enable));

    if (enable) {
        while (!(whal_Reg_Read(base, RCC_BDCR1_REG) & RCC_BDCR1_LSI1RDY_Msk))
            ;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRcc_Ext_SetRngClockSrc(whal_Clock *clkDev,
                                                whal_Stm32wbaRcc_RngSrc src)
{
    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CCIPR2_REG,
                    RCC_CCIPR2_RNGSEL_Msk,
                    whal_SetBits(RCC_CCIPR2_RNGSEL_Msk,
                                 RCC_CCIPR2_RNGSEL_Pos, src));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRcc_Ext_SetHpre5(whal_Clock *clkDev, uint8_t hpre5)
{
    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR4_REG, RCC_CFGR4_HPRE5_Msk,
                    whal_SetBits(RCC_CFGR4_HPRE5_Msk, RCC_CFGR4_HPRE5_Pos, hpre5));

    return WHAL_SUCCESS;
}

const whal_ClockDriver whal_Stm32wbaRccPll_Driver = {
    .Init = whal_Stm32wbaRccPll_Init,
    .Deinit = whal_Stm32wbaRccPll_Deinit,
    .Enable = whal_Stm32wbaRcc_Enable,
    .Disable = whal_Stm32wbaRcc_Disable,
};

const whal_ClockDriver whal_Stm32wbaRccHse_Driver = {
    .Init = whal_Stm32wbaRccHse_Init,
    .Deinit = whal_Stm32wbaRccHse_Deinit,
    .Enable = whal_Stm32wbaRcc_Enable,
    .Disable = whal_Stm32wbaRcc_Disable,
};
