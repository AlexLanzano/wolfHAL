#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32n6_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define RCC_CR_REG          0x000
#define RCC_CR_PLL1ON_Msk   (1UL << 8)

#define RCC_SR_REG          0x004
#define RCC_SR_PLL1RDY_Msk  (1UL << 8)
#define RCC_SR_PLL1RDY_Pos  8

#define RCC_CFGR1_REG        0x020
#define RCC_CFGR1_CPUSW_Pos  16
#define RCC_CFGR1_CPUSW_Msk  (3UL << RCC_CFGR1_CPUSW_Pos)
#define RCC_CFGR1_CPUSWS_Pos 20
#define RCC_CFGR1_CPUSWS_Msk (3UL << RCC_CFGR1_CPUSWS_Pos)
#define RCC_CFGR1_SYSSW_Pos  24
#define RCC_CFGR1_SYSSW_Msk  (3UL << RCC_CFGR1_SYSSW_Pos)
#define RCC_CFGR1_SYSSWS_Pos 28
#define RCC_CFGR1_SYSSWS_Msk (3UL << RCC_CFGR1_SYSSWS_Pos)

#define RCC_PLL1CFGR1_REG          0x080
#define RCC_PLL1CFGR1_PLL1SEL_Pos  28
#define RCC_PLL1CFGR1_PLL1SEL_Msk  (7UL << RCC_PLL1CFGR1_PLL1SEL_Pos)
#define RCC_PLL1CFGR1_PLL1DIVM_Pos 20
#define RCC_PLL1CFGR1_PLL1DIVM_Msk (0x3FUL << RCC_PLL1CFGR1_PLL1DIVM_Pos)
#define RCC_PLL1CFGR1_PLL1DIVN_Pos 0
#define RCC_PLL1CFGR1_PLL1DIVN_Msk (0xFFFUL << RCC_PLL1CFGR1_PLL1DIVN_Pos)

#define RCC_PLL1CFGR3_REG            0x088
#define RCC_PLL1CFGR3_PLL1PDIVEN_Msk (1UL << 30)
#define RCC_PLL1CFGR3_PLL1PDIV1_Pos  27
#define RCC_PLL1CFGR3_PLL1PDIV1_Msk  (7UL << RCC_PLL1CFGR3_PLL1PDIV1_Pos)
#define RCC_PLL1CFGR3_PLL1PDIV2_Pos  24
#define RCC_PLL1CFGR3_PLL1PDIV2_Msk  (7UL << RCC_PLL1CFGR3_PLL1PDIV2_Pos)

#define RCC_CCIPR2_REG         0x148
#define RCC_CCIPR2_ETH1SEL_Pos 16
#define RCC_CCIPR2_ETH1SEL_Msk (7UL << RCC_CCIPR2_ETH1SEL_Pos)

whal_Error whal_Stm32n6_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32n6_Rcc_OscCfg *cfg)
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

whal_Error whal_Stm32n6_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32n6_Rcc_OscCfg *cfg)
{
    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, cfg->onReg, cfg->onMsk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6_Rcc_EnablePll1(whal_Clock *clkDev,
                                      const whal_Stm32n6_Rcc_Pll1Cfg *cfg)
{
    size_t rdy;

    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    /* Disable PLL1 before reconfiguring; wait until off. */
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_SR_REG, RCC_SR_PLL1RDY_Msk,
                     RCC_SR_PLL1RDY_Pos, &rdy);
    } while (rdy);

    whal_Reg_Write(clkDev->regmap.base, RCC_PLL1CFGR1_REG,
                   whal_SetBits(RCC_PLL1CFGR1_PLL1SEL_Msk, RCC_PLL1CFGR1_PLL1SEL_Pos, cfg->clkSrc) |
                   whal_SetBits(RCC_PLL1CFGR1_PLL1DIVM_Msk, RCC_PLL1CFGR1_PLL1DIVM_Pos, cfg->m) |
                   whal_SetBits(RCC_PLL1CFGR1_PLL1DIVN_Msk, RCC_PLL1CFGR1_PLL1DIVN_Pos, cfg->n));
    whal_Reg_Update(clkDev->regmap.base, RCC_PLL1CFGR3_REG,
                    RCC_PLL1CFGR3_PLL1PDIVEN_Msk |
                    RCC_PLL1CFGR3_PLL1PDIV1_Msk |
                    RCC_PLL1CFGR3_PLL1PDIV2_Msk,
                    RCC_PLL1CFGR3_PLL1PDIVEN_Msk |
                    whal_SetBits(RCC_PLL1CFGR3_PLL1PDIV1_Msk, RCC_PLL1CFGR3_PLL1PDIV1_Pos, cfg->pdiv1) |
                    whal_SetBits(RCC_PLL1CFGR3_PLL1PDIV2_Msk, RCC_PLL1CFGR3_PLL1PDIV2_Pos, cfg->pdiv2));

    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLL1ON_Msk,
                    RCC_CR_PLL1ON_Msk);
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_SR_REG, RCC_SR_PLL1RDY_Msk,
                     RCC_SR_PLL1RDY_Pos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6_Rcc_DisablePll1(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32n6_Rcc_SysClockSrc src)
{
    size_t sws;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR1_REG, RCC_CFGR1_SYSSW_Msk,
                    whal_SetBits(RCC_CFGR1_SYSSW_Msk, RCC_CFGR1_SYSSW_Pos, src));
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CFGR1_REG, RCC_CFGR1_SYSSWS_Msk,
                     RCC_CFGR1_SYSSWS_Pos, &sws);
    } while (sws != (size_t)src);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6_Rcc_SetCpuClock(whal_Clock *clkDev,
                                       whal_Stm32n6_Rcc_CpuClockSrc src)
{
    size_t sws;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CFGR1_REG, RCC_CFGR1_CPUSW_Msk,
                    whal_SetBits(RCC_CFGR1_CPUSW_Msk, RCC_CFGR1_CPUSW_Pos, src));
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CFGR1_REG, RCC_CFGR1_CPUSWS_Msk,
                     RCC_CFGR1_CPUSWS_Pos, &sws);
    } while (sws != (size_t)src);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6_Rcc_SetEth1If(whal_Clock *clkDev,
                                     whal_Stm32n6_Rcc_Eth1If mode)
{
    if (!clkDev)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->regmap.base, RCC_CCIPR2_REG, RCC_CCIPR2_ETH1SEL_Msk,
                    whal_SetBits(RCC_CCIPR2_ETH1SEL_Msk, RCC_CCIPR2_ETH1SEL_Pos,
                                 mode));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32n6_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->regmap.base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32n6_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;
    whal_Reg_Update(clkDev->regmap.base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 0));
    return WHAL_SUCCESS;
}
