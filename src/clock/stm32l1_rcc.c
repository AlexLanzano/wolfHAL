#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32l1_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define RCC_CR_REG          0x00
#define RCC_CR_PLLON_Msk    (1UL << 24)
#define RCC_CR_PLLRDY_Msk   (1UL << 25)
#define RCC_CR_PLLRDY_Pos   25

#define RCC_CFGR_REG        0x08
#define RCC_CFGR_SW_Pos     0
#define RCC_CFGR_SW_Msk     (WHAL_BITMASK(2) << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos    2
#define RCC_CFGR_SWS_Msk    (WHAL_BITMASK(2) << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_PLLSRC_Pos 16
#define RCC_CFGR_PLLSRC_Msk (1UL << RCC_CFGR_PLLSRC_Pos)
#define RCC_CFGR_PLLMUL_Pos 18
#define RCC_CFGR_PLLMUL_Msk (WHAL_BITMASK(4) << RCC_CFGR_PLLMUL_Pos)
#define RCC_CFGR_PLLDIV_Pos 22
#define RCC_CFGR_PLLDIV_Msk (WHAL_BITMASK(2) << RCC_CFGR_PLLDIV_Pos)

whal_Error whal_Stm32l1_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32l1_Rcc_OscCfg *cfg)
{
    size_t rdy;

    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, cfg->onReg, cfg->onMsk, cfg->onMsk);
    do {
        whal_Reg_Get(clkDev->regmap.base, cfg->rdyReg, cfg->rdyMsk,
                     cfg->rdyPos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32l1_Rcc_OscCfg *cfg)
{
    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, cfg->onReg, cfg->onMsk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1_Rcc_EnablePll(whal_Clock *clkDev,
                                     const whal_Stm32l1_Rcc_PllCfg *cfg)
{
    size_t rdy;
    uint32_t pllsrc;

    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    /* Disable PLL before reconfiguring; wait until it's actually off. */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLON_Msk, 0);
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLRDY_Msk,
                     RCC_CR_PLLRDY_Pos, &rdy);
    } while (rdy);

    pllsrc = (cfg->clkSrc == WHAL_STM32L1_RCC_PLLSRC_HSE) ? 1 : 0;
    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_PLLSRC_Msk,
                    whal_SetBits(RCC_CFGR_PLLSRC_Msk, RCC_CFGR_PLLSRC_Pos,
                                 pllsrc));
    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_PLLMUL_Msk,
                    whal_SetBits(RCC_CFGR_PLLMUL_Msk, RCC_CFGR_PLLMUL_Pos,
                                 cfg->pllmul));
    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_PLLDIV_Msk,
                    whal_SetBits(RCC_CFGR_PLLDIV_Msk, RCC_CFGR_PLLDIV_Pos,
                                 cfg->plldiv));

    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    RCC_CR_PLLON_Msk);
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLRDY_Msk,
                     RCC_CR_PLLRDY_Pos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1_Rcc_DisablePll(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLON_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32l1_Rcc_SysClockSrc src)
{
    size_t sws;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, src));
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CFGR_REG, RCC_CFGR_SWS_Msk,
                     RCC_CFGR_SWS_Pos, &sws);
    } while (sws != (size_t)src);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32l1_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->regmap.base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32l1_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->regmap.base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 0));
    return WHAL_SUCCESS;
}
