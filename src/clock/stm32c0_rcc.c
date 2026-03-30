#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32c0_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32C0 RCC Register Definitions
 *
 * The RCC (Reset and Clock Control) peripheral manages:
 * - HSI48 oscillator with HSIDIV prescaler
 * - System clock source selection (HSISYS, HSE, LSI, LSE)
 * - Peripheral clock gating (IOP, AHB, APB buses)
 */

/* Clock Control Register - oscillator enables and status */
#define RCC_CR_REG 0x000
#define RCC_CR_HSION_Pos 8                                                 /* HSI enable */
#define RCC_CR_HSION_Msk (1UL << RCC_CR_HSION_Pos)

#define RCC_CR_HSIRDY_Pos 10                                               /* HSI ready */
#define RCC_CR_HSIRDY_Msk (1UL << RCC_CR_HSIRDY_Pos)

#define RCC_CR_HSIDIV_Pos 11                                               /* HSI divider */
#define RCC_CR_HSIDIV_Msk (WHAL_BITMASK(3) << RCC_CR_HSIDIV_Pos)

/* Clock Configuration Register - clock source and prescaler selection */
#define RCC_CFGR_REG 0x008
#define RCC_CFGR_SW_Pos 0                                                  /* System clock switch */
#define RCC_CFGR_SW_Msk (WHAL_BITMASK(3) << RCC_CFGR_SW_Pos)

#define RCC_CFGR_SWS_Pos 3                                                 /* System clock switch status */
#define RCC_CFGR_SWS_Msk (WHAL_BITMASK(3) << RCC_CFGR_SWS_Pos)

/* HSISYS clock source selection value */
#define RCC_CFGR_SW_HSISYS 0

whal_Error whal_Stm32c0Rcc_Init(whal_Clock *clkDev)
{
    whal_Stm32c0Rcc_Cfg *cfg;
    size_t rdy;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32c0Rcc_Cfg *)clkDev->cfg;

    /* Set HSIDIV to configured divider */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                    whal_SetBits(RCC_CR_HSIDIV_Msk, RCC_CR_HSIDIV_Pos, cfg->hsidiv));

    /* Ensure HSI is enabled */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_HSION_Msk,
                    whal_SetBits(RCC_CR_HSION_Msk, RCC_CR_HSION_Pos, 1));

    /* Wait for HSI to be ready */
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG,
                     RCC_CR_HSIRDY_Msk, RCC_CR_HSIRDY_Pos, &rdy);
    } while (!rdy);

    /* Select HSISYS as system clock source */
    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, RCC_CFGR_SW_HSISYS));

    /* Wait for system clock switch status to confirm HSISYS */
    size_t sws;
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CFGR_REG,
                     RCC_CFGR_SWS_Msk, RCC_CFGR_SWS_Pos, &sws);
    } while (sws != RCC_CFGR_SW_HSISYS);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32c0Rcc_Deinit(whal_Clock *clkDev)
{
    if (!clkDev) {
        return WHAL_EINVAL;
    }

    /* Reset HSIDIV to default (div1 = 48 MHz) */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                    whal_SetBits(RCC_CR_HSIDIV_Msk, RCC_CR_HSIDIV_Pos,
                                 WHAL_STM32C0_RCC_HSIDIV_1));

    /* Select HSISYS as system clock source */
    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, RCC_CFGR_SW_HSISYS));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32c0Rcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32c0Rcc_Clk *stClk = (whal_Stm32c0Rcc_Clk *)clk;

    /* Set the peripheral's enable bit in the appropriate RCC enable register */
    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32c0Rcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32c0Rcc_Clk *stClk = (whal_Stm32c0Rcc_Clk *)clk;

    /* Clear the peripheral's enable bit to gate its clock */
    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32c0Rcc_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    whal_Stm32c0Rcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg || !rateOut) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32c0Rcc_Cfg *)clkDev->cfg;

    /* HSI48 base frequency divided by HSIDIV (1 << hsidiv) */
    *rateOut = 48000000 / (1 << cfg->hsidiv);

    return WHAL_SUCCESS;
}

const whal_ClockDriver whal_Stm32c0Rcc_Driver = {
    .Init = whal_Stm32c0Rcc_Init,
    .Deinit = whal_Stm32c0Rcc_Deinit,
    .Enable = whal_Stm32c0Rcc_Enable,
    .Disable = whal_Stm32c0Rcc_Disable,
    .GetRate = whal_Stm32c0Rcc_GetRate,
};
