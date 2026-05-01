#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32WB RCC Register Definitions
 */

/* Clock Control Register */
#define RCC_CR_REG          0x000
#define RCC_CR_MSION_Msk    (1UL <<  0)
#define RCC_CR_MSIRDY_Msk   (1UL <<  1)
#define RCC_CR_MSIRDY_Pos   1
#define RCC_CR_MSIRANGE_Pos 4
#define RCC_CR_MSIRANGE_Msk (WHAL_BITMASK(4) << RCC_CR_MSIRANGE_Pos)
#define RCC_CR_PLLON_Msk    (1UL << 24)
#define RCC_CR_PLLRDY_Msk   (1UL << 25)
#define RCC_CR_PLLRDY_Pos   25

/* Clock Configuration Register */
#define RCC_CFGR_REG     0x008
#define RCC_CFGR_SW_Pos  0
#define RCC_CFGR_SW_Msk  (WHAL_BITMASK(2) << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos 2
#define RCC_CFGR_SWS_Msk (WHAL_BITMASK(2) << RCC_CFGR_SWS_Pos)

/* PLL Configuration Register */
#define RCC_PLLCFGR_REG         0x00C
#define RCC_PLLCFGR_PLLSRC_Pos  0
#define RCC_PLLCFGR_PLLSRC_Msk  (WHAL_BITMASK(2) << RCC_PLLCFGR_PLLSRC_Pos)
#define RCC_PLLCFGR_PLLM_Pos    4
#define RCC_PLLCFGR_PLLM_Msk    (WHAL_BITMASK(3) << RCC_PLLCFGR_PLLM_Pos)
#define RCC_PLLCFGR_PLLN_Pos    8
#define RCC_PLLCFGR_PLLN_Msk    (WHAL_BITMASK(7) << RCC_PLLCFGR_PLLN_Pos)
#define RCC_PLLCFGR_PLLP_Pos    17
#define RCC_PLLCFGR_PLLP_Msk    (WHAL_BITMASK(5) << RCC_PLLCFGR_PLLP_Pos)
#define RCC_PLLCFGR_PLLQ_Pos    25
#define RCC_PLLCFGR_PLLQ_Msk    (WHAL_BITMASK(3) << RCC_PLLCFGR_PLLQ_Pos)
#define RCC_PLLCFGR_PLLREN_Pos  28
#define RCC_PLLCFGR_PLLREN_Msk  (1UL << RCC_PLLCFGR_PLLREN_Pos)
#define RCC_PLLCFGR_PLLR_Pos    29
#define RCC_PLLCFGR_PLLR_Msk    (WHAL_BITMASK(3) << RCC_PLLCFGR_PLLR_Pos)
#define RCC_PLLCFGR_Msk \
        (RCC_PLLCFGR_PLLSRC_Msk | RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk | \
         RCC_PLLCFGR_PLLP_Msk   | RCC_PLLCFGR_PLLQ_Msk | RCC_PLLCFGR_PLLREN_Msk | \
         RCC_PLLCFGR_PLLR_Msk)

whal_Error whal_Stm32wb_Rcc_EnableOsc(whal_Clock *clkDev,
                                     const whal_Stm32wb_Rcc_OscCfg *cfg)
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

whal_Error whal_Stm32wb_Rcc_DisableOsc(whal_Clock *clkDev,
                                      const whal_Stm32wb_Rcc_OscCfg *cfg)
{
    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, cfg->onReg, cfg->onMsk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Rcc_EnableMsi(whal_Clock *clkDev,
                                     whal_Stm32wb_Rcc_MsiRange range)
{
    size_t rdy;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_MSION_Msk,
                    RCC_CR_MSION_Msk);
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG, RCC_CR_MSIRDY_Msk,
                     RCC_CR_MSIRDY_Pos, &rdy);
    } while (!rdy);
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_MSIRANGE_Msk,
                    whal_SetBits(RCC_CR_MSIRANGE_Msk, RCC_CR_MSIRANGE_Pos,
                                 range));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Rcc_EnablePll(whal_Clock *clkDev,
                                     const whal_Stm32wb_Rcc_PllCfg *cfg)
{
    size_t rdy;

    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_PLLCFGR_REG, RCC_PLLCFGR_Msk,
                    whal_SetBits(RCC_PLLCFGR_PLLSRC_Msk, RCC_PLLCFGR_PLLSRC_Pos, cfg->clkSrc) |
                    whal_SetBits(RCC_PLLCFGR_PLLM_Msk,   RCC_PLLCFGR_PLLM_Pos,   cfg->m) |
                    whal_SetBits(RCC_PLLCFGR_PLLN_Msk,   RCC_PLLCFGR_PLLN_Pos,   cfg->n) |
                    whal_SetBits(RCC_PLLCFGR_PLLP_Msk,   RCC_PLLCFGR_PLLP_Pos,   cfg->p) |
                    whal_SetBits(RCC_PLLCFGR_PLLQ_Msk,   RCC_PLLCFGR_PLLQ_Pos,   cfg->q) |
                    whal_SetBits(RCC_PLLCFGR_PLLREN_Msk, RCC_PLLCFGR_PLLREN_Pos, 1) |
                    whal_SetBits(RCC_PLLCFGR_PLLR_Msk,   RCC_PLLCFGR_PLLR_Pos,   cfg->r));
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLON_Msk,
                    RCC_CR_PLLON_Msk);
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLRDY_Msk,
                     RCC_CR_PLLRDY_Pos, &rdy);
    } while (!rdy);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Rcc_DisablePll(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_PLLON_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32wb_Rcc_SysClockSrc src)
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

whal_Error whal_Stm32wb_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32wb_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32wb_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 0));
    return WHAL_SUCCESS;
}
