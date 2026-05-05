#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32f0_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define RCC_CR_REG          0x00
#define RCC_CR_PLLON_Msk    (1UL << 24)
#define RCC_CR_PLLRDY_Msk   (1UL << 25)
#define RCC_CR_PLLRDY_Pos   25

#define RCC_CFGR_REG        0x04
#define RCC_CFGR_SW_Pos     0
#define RCC_CFGR_SW_Msk     (WHAL_BITMASK(2) << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos    2
#define RCC_CFGR_SWS_Msk    (WHAL_BITMASK(2) << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_PLLSRC_Pos 16
#define RCC_CFGR_PLLSRC_Msk (1UL << RCC_CFGR_PLLSRC_Pos)
#define RCC_CFGR_PLLMUL_Pos 18
#define RCC_CFGR_PLLMUL_Msk (WHAL_BITMASK(4) << RCC_CFGR_PLLMUL_Pos)

#define RCC_CFGR2_REG        0x2C
#define RCC_CFGR2_PREDIV_Pos 0
#define RCC_CFGR2_PREDIV_Msk (WHAL_BITMASK(4) << RCC_CFGR2_PREDIV_Pos)

whal_Error whal_Stm32f0_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32f0_Rcc_OscCfg *cfg)
{
    size_t rdy;

    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->base, cfg->onReg, cfg->onMsk, cfg->onMsk);
    do {
        whal_Reg_Get(clkDev->base, cfg->rdyReg, cfg->rdyMsk,
                     cfg->rdyPos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f0_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32f0_Rcc_OscCfg *cfg)
{
    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->base, cfg->onReg, cfg->onMsk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f0_Rcc_EnablePll(whal_Clock *clkDev,
                                     const whal_Stm32f0_Rcc_PllCfg *cfg)
{
    size_t rdy;
    uint32_t pllsrc;

    if (!clkDev || !cfg ||
        cfg->prediv < 1 || cfg->prediv > 16 ||
        cfg->pllmul < 2 || cfg->pllmul > 16)
        return WHAL_EINVAL;

    /* Disable PLL before reconfiguring. */
    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_PLLON_Msk, 0);

    whal_Reg_Update(clkDev->base, RCC_CFGR2_REG, RCC_CFGR2_PREDIV_Msk,
                    whal_SetBits(RCC_CFGR2_PREDIV_Msk, RCC_CFGR2_PREDIV_Pos,
                                 cfg->prediv - 1));
    whal_Reg_Update(clkDev->base, RCC_CFGR_REG, RCC_CFGR_PLLMUL_Msk,
                    whal_SetBits(RCC_CFGR_PLLMUL_Msk, RCC_CFGR_PLLMUL_Pos,
                                 cfg->pllmul - 2));

    pllsrc = (cfg->clkSrc == WHAL_STM32F0_RCC_PLLSRC_HSE_PREDIV ||
              cfg->clkSrc == WHAL_STM32F0_RCC_PLLSRC_HSI48_PREDIV) ? 1 : 0;
    whal_Reg_Update(clkDev->base, RCC_CFGR_REG, RCC_CFGR_PLLSRC_Msk,
                    whal_SetBits(RCC_CFGR_PLLSRC_Msk, RCC_CFGR_PLLSRC_Pos,
                                 pllsrc));

    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    RCC_CR_PLLON_Msk);
    do {
        whal_Reg_Get(clkDev->base, RCC_CR_REG, RCC_CR_PLLRDY_Msk,
                     RCC_CR_PLLRDY_Pos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f0_Rcc_DisablePll(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_PLLON_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f0_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32f0_Rcc_SysClockSrc src)
{
    size_t sws;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->base, RCC_CFGR_REG, RCC_CFGR_SW_Msk,
                    whal_SetBits(RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, src));
    do {
        whal_Reg_Get(clkDev->base, RCC_CFGR_REG, RCC_CFGR_SWS_Msk,
                     RCC_CFGR_SWS_Pos, &sws);
    } while (sws != (size_t)src);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f0_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32f0_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f0_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32f0_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 0));
    return WHAL_SUCCESS;
}
