#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32wba_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define RCC_CR_REG          0x000
#define RCC_CR_PLL1ON_Msk   (1UL << 24)
#define RCC_CR_PLL1RDY_Msk  (1UL << 25)
#define RCC_CR_PLL1RDY_Pos  25

#define RCC_CFGR1_REG       0x01C
#define RCC_CFGR1_SW_Pos    0
#define RCC_CFGR1_SW_Msk    (3UL << RCC_CFGR1_SW_Pos)
#define RCC_CFGR1_SWS_Pos   2
#define RCC_CFGR1_SWS_Msk   (3UL << RCC_CFGR1_SWS_Pos)

#define RCC_PLL1CFGR_REG     0x028
#define RCC_PLL1CFGR_SRC_Pos 0
#define RCC_PLL1CFGR_SRC_Msk (3UL << RCC_PLL1CFGR_SRC_Pos)
#define RCC_PLL1CFGR_RGE_Pos 2
#define RCC_PLL1CFGR_RGE_Msk (3UL << RCC_PLL1CFGR_RGE_Pos)
#define RCC_PLL1CFGR_M_Pos   8
#define RCC_PLL1CFGR_M_Msk   (7UL << RCC_PLL1CFGR_M_Pos)
#define RCC_PLL1CFGR_REN_Pos 18
#define RCC_PLL1CFGR_REN_Msk (1UL << RCC_PLL1CFGR_REN_Pos)

#define RCC_PLL1DIVR_REG    0x034
#define RCC_PLL1DIVR_N_Pos  0
#define RCC_PLL1DIVR_N_Msk  (0x1FFUL << RCC_PLL1DIVR_N_Pos)
#define RCC_PLL1DIVR_P_Pos  9
#define RCC_PLL1DIVR_P_Msk  (0x7FUL << RCC_PLL1DIVR_P_Pos)
#define RCC_PLL1DIVR_Q_Pos  16
#define RCC_PLL1DIVR_Q_Msk  (0x7FUL << RCC_PLL1DIVR_Q_Pos)
#define RCC_PLL1DIVR_R_Pos  24
#define RCC_PLL1DIVR_R_Msk  (0x7FUL << RCC_PLL1DIVR_R_Pos)

#define RCC_BDCR1_REG         0x0C8
#define RCC_BDCR1_LSI1ON_Msk  (1UL << 2)
#define RCC_BDCR1_LSI1RDY_Msk (1UL << 3)
#define RCC_BDCR1_LSI1RDY_Pos 3

#define RCC_CFGR4_REG       0x080
#define RCC_CFGR4_HPRE5_Pos 0
#define RCC_CFGR4_HPRE5_Msk (7UL << RCC_CFGR4_HPRE5_Pos)

#define RCC_CCIPR2_REG        0x0E4
#define RCC_CCIPR2_RNGSEL_Pos 12
#define RCC_CCIPR2_RNGSEL_Msk (3UL << RCC_CCIPR2_RNGSEL_Pos)

whal_Error whal_Stm32wba_Rcc_EnableOsc(whal_Clock *clkDev,
                                      const whal_Stm32wba_Rcc_OscCfg *cfg)
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

whal_Error whal_Stm32wba_Rcc_DisableOsc(whal_Clock *clkDev,
                                       const whal_Stm32wba_Rcc_OscCfg *cfg)
{
    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->base, cfg->onReg, cfg->onMsk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wba_Rcc_EnablePll1(whal_Clock *clkDev,
                                       const whal_Stm32wba_Rcc_Pll1Cfg *cfg)
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

    whal_Reg_Write(clkDev->base, RCC_PLL1CFGR_REG,
                   whal_SetBits(RCC_PLL1CFGR_SRC_Msk, RCC_PLL1CFGR_SRC_Pos, cfg->clkSrc) |
                   whal_SetBits(RCC_PLL1CFGR_RGE_Msk, RCC_PLL1CFGR_RGE_Pos, cfg->rge) |
                   whal_SetBits(RCC_PLL1CFGR_M_Msk,   RCC_PLL1CFGR_M_Pos,   cfg->m) |
                   whal_SetBits(RCC_PLL1CFGR_REN_Msk, RCC_PLL1CFGR_REN_Pos, 1));
    whal_Reg_Write(clkDev->base, RCC_PLL1DIVR_REG,
                   whal_SetBits(RCC_PLL1DIVR_N_Msk, RCC_PLL1DIVR_N_Pos, cfg->n) |
                   whal_SetBits(RCC_PLL1DIVR_P_Msk, RCC_PLL1DIVR_P_Pos, cfg->p) |
                   whal_SetBits(RCC_PLL1DIVR_Q_Msk, RCC_PLL1DIVR_Q_Pos, cfg->q) |
                   whal_SetBits(RCC_PLL1DIVR_R_Msk, RCC_PLL1DIVR_R_Pos, cfg->r));

    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    RCC_CR_PLL1ON_Msk);
    do {
        whal_Reg_Get(clkDev->base, RCC_CR_REG, RCC_CR_PLL1RDY_Msk,
                     RCC_CR_PLL1RDY_Pos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wba_Rcc_DisablePll1(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wba_Rcc_EnableLsi(whal_Clock *clkDev)
{
    size_t rdy;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->base, RCC_BDCR1_REG, RCC_BDCR1_LSI1ON_Msk,
                    RCC_BDCR1_LSI1ON_Msk);
    do {
        whal_Reg_Get(clkDev->base, RCC_BDCR1_REG, RCC_BDCR1_LSI1RDY_Msk,
                     RCC_BDCR1_LSI1RDY_Pos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wba_Rcc_DisableLsi(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, RCC_BDCR1_REG, RCC_BDCR1_LSI1ON_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wba_Rcc_SetSysClock(whal_Clock *clkDev,
                                        whal_Stm32wba_Rcc_SysClockSrc src)
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

whal_Error whal_Stm32wba_Rcc_SetRngClockSrc(whal_Clock *clkDev,
                                           whal_Stm32wba_Rcc_RngSrc src)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, RCC_CCIPR2_REG, RCC_CCIPR2_RNGSEL_Msk,
                    whal_SetBits(RCC_CCIPR2_RNGSEL_Msk, RCC_CCIPR2_RNGSEL_Pos,
                                 src));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wba_Rcc_SetHpre5(whal_Clock *clkDev, uint8_t hpre5)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, RCC_CFGR4_REG, RCC_CFGR4_HPRE5_Msk,
                    whal_SetBits(RCC_CFGR4_HPRE5_Msk, RCC_CFGR4_HPRE5_Pos, hpre5));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wba_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32wba_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wba_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                             const whal_Stm32wba_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 0));
    return WHAL_SUCCESS;
}
