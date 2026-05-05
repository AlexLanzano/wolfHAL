#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32h5_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define RCC_CR_REG          0x000
#define RCC_CR_HSIDIV_Pos   3
#define RCC_CR_HSIDIV_Msk   (WHAL_BITMASK(2) << RCC_CR_HSIDIV_Pos)
#define RCC_CR_PLL1ON_Msk   (1UL << 24)
#define RCC_CR_PLL1RDY_Msk  (1UL << 25)
#define RCC_CR_PLL1RDY_Pos  25

#define RCC_CFGR1_REG       0x01C
#define RCC_CFGR1_SW_Pos    0
#define RCC_CFGR1_SW_Msk    (WHAL_BITMASK(2) << RCC_CFGR1_SW_Pos)
#define RCC_CFGR1_SWS_Pos   3
#define RCC_CFGR1_SWS_Msk   (WHAL_BITMASK(2) << RCC_CFGR1_SWS_Pos)

#define RCC_PLL1CFGR_REG          0x028
#define RCC_PLL1CFGR_PLL1SRC_Pos  0
#define RCC_PLL1CFGR_PLL1SRC_Msk  (WHAL_BITMASK(2) << RCC_PLL1CFGR_PLL1SRC_Pos)
#define RCC_PLL1CFGR_PLL1RGE_Pos  2
#define RCC_PLL1CFGR_PLL1RGE_Msk  (WHAL_BITMASK(2) << RCC_PLL1CFGR_PLL1RGE_Pos)
#define RCC_PLL1CFGR_PLL1M_Pos    8
#define RCC_PLL1CFGR_PLL1M_Msk    (WHAL_BITMASK(6) << RCC_PLL1CFGR_PLL1M_Pos)
#define RCC_PLL1CFGR_PLL1PEN_Pos  16
#define RCC_PLL1CFGR_PLL1PEN_Msk  (1UL << RCC_PLL1CFGR_PLL1PEN_Pos)
#define RCC_PLL1CFGR_PLL1QEN_Pos  17
#define RCC_PLL1CFGR_PLL1QEN_Msk  (1UL << RCC_PLL1CFGR_PLL1QEN_Pos)
#define RCC_PLL1CFGR_PLL1REN_Pos  18
#define RCC_PLL1CFGR_PLL1REN_Msk  (1UL << RCC_PLL1CFGR_PLL1REN_Pos)

#define RCC_PLL1DIVR_REG       0x034
#define RCC_PLL1DIVR_PLL1N_Pos 0
#define RCC_PLL1DIVR_PLL1N_Msk (WHAL_BITMASK(9) << RCC_PLL1DIVR_PLL1N_Pos)
#define RCC_PLL1DIVR_PLL1P_Pos 9
#define RCC_PLL1DIVR_PLL1P_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_PLL1P_Pos)
#define RCC_PLL1DIVR_PLL1Q_Pos 16
#define RCC_PLL1DIVR_PLL1Q_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_PLL1Q_Pos)
#define RCC_PLL1DIVR_PLL1R_Pos 24
#define RCC_PLL1DIVR_PLL1R_Msk (WHAL_BITMASK(7) << RCC_PLL1DIVR_PLL1R_Pos)

whal_Error whal_Stm32h5_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32h5_Rcc_OscCfg *cfg)
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

whal_Error whal_Stm32h5_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32h5_Rcc_OscCfg *cfg)
{
    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->base, cfg->onReg, cfg->onMsk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5_Rcc_EnablePll1(whal_Clock *clkDev,
                                      const whal_Stm32h5_Rcc_PllCfg *cfg)
{
    size_t rdy;

    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    /* Disable PLL1 before reconfiguring; wait until off. */
    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);
    do {
        whal_Reg_Get(clkDev->base, RCC_CR_REG, RCC_CR_PLL1RDY_Msk,
                     RCC_CR_PLL1RDY_Pos, &rdy);
    } while (rdy);

    whal_Reg_Update(clkDev->base, RCC_PLL1CFGR_REG,
                    RCC_PLL1CFGR_PLL1SRC_Msk | RCC_PLL1CFGR_PLL1M_Msk |
                    RCC_PLL1CFGR_PLL1PEN_Msk | RCC_PLL1CFGR_PLL1QEN_Msk |
                    RCC_PLL1CFGR_PLL1REN_Msk,
                    whal_SetBits(RCC_PLL1CFGR_PLL1SRC_Msk, RCC_PLL1CFGR_PLL1SRC_Pos, cfg->clkSrc) |
                    whal_SetBits(RCC_PLL1CFGR_PLL1M_Msk,   RCC_PLL1CFGR_PLL1M_Pos,   cfg->m) |
                    whal_SetBits(RCC_PLL1CFGR_PLL1PEN_Msk, RCC_PLL1CFGR_PLL1PEN_Pos, 1) |
                    whal_SetBits(RCC_PLL1CFGR_PLL1QEN_Msk, RCC_PLL1CFGR_PLL1QEN_Pos, 1) |
                    whal_SetBits(RCC_PLL1CFGR_PLL1REN_Msk, RCC_PLL1CFGR_PLL1REN_Pos, 1));
    whal_Reg_Update(clkDev->base, RCC_PLL1DIVR_REG,
                    RCC_PLL1DIVR_PLL1N_Msk | RCC_PLL1DIVR_PLL1P_Msk |
                    RCC_PLL1DIVR_PLL1Q_Msk | RCC_PLL1DIVR_PLL1R_Msk,
                    whal_SetBits(RCC_PLL1DIVR_PLL1N_Msk, RCC_PLL1DIVR_PLL1N_Pos, cfg->n) |
                    whal_SetBits(RCC_PLL1DIVR_PLL1P_Msk, RCC_PLL1DIVR_PLL1P_Pos, cfg->p) |
                    whal_SetBits(RCC_PLL1DIVR_PLL1Q_Msk, RCC_PLL1DIVR_PLL1Q_Pos, cfg->q) |
                    whal_SetBits(RCC_PLL1DIVR_PLL1R_Msk, RCC_PLL1DIVR_PLL1R_Pos, cfg->r));

    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    RCC_CR_PLL1ON_Msk);
    do {
        whal_Reg_Get(clkDev->base, RCC_CR_REG, RCC_CR_PLL1RDY_Msk,
                     RCC_CR_PLL1RDY_Pos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5_Rcc_DisablePll1(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32h5_Rcc_SysClockSrc src)
{
    size_t sws;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->base, RCC_CFGR1_REG, RCC_CFGR1_SW_Msk,
                    whal_SetBits(RCC_CFGR1_SW_Msk, RCC_CFGR1_SW_Pos, src));
    do {
        whal_Reg_Get(clkDev->base, RCC_CFGR1_REG, RCC_CFGR1_SWS_Msk,
                     RCC_CFGR1_SWS_Pos, &sws);
    } while (sws != (size_t)src);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5_Rcc_SetHsiDiv(whal_Clock *clkDev, uint8_t div)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                    whal_SetBits(RCC_CR_HSIDIV_Msk, RCC_CR_HSIDIV_Pos, div));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32h5_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32h5_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 0));
    return WHAL_SUCCESS;
}
