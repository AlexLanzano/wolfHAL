#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32c0_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32C0 RCC Register Definitions
 */

/* Clock Control Register */
#define RCC_CR_REG        0x000
#define RCC_CR_HSION_Pos  8
#define RCC_CR_HSION_Msk  (1UL << RCC_CR_HSION_Pos)
#define RCC_CR_HSIRDY_Pos 10
#define RCC_CR_HSIRDY_Msk (1UL << RCC_CR_HSIRDY_Pos)
#define RCC_CR_HSIDIV_Pos 11
#define RCC_CR_HSIDIV_Msk (WHAL_BITMASK(3) << RCC_CR_HSIDIV_Pos)

/* Clock Configuration Register */
#define RCC_CFGR_REG     0x008
#define RCC_CFGR_SW_Pos  0
#define RCC_CFGR_SW_Msk  (WHAL_BITMASK(3) << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos 3
#define RCC_CFGR_SWS_Msk (WHAL_BITMASK(3) << RCC_CFGR_SWS_Pos)

whal_Error whal_Stm32c0_Rcc_EnableHsi(whal_Clock *clkDev,
                                     whal_Stm32c0_Rcc_HsiDiv hsidiv)
{
    size_t rdy;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_HSION_Msk,
                    RCC_CR_HSION_Msk);
    do {
        whal_Reg_Get(clkDev->regmap.base, RCC_CR_REG, RCC_CR_HSIRDY_Msk,
                     RCC_CR_HSIRDY_Pos, &rdy);
    } while (!rdy);
    whal_Reg_Update(clkDev->regmap.base, RCC_CR_REG, RCC_CR_HSIDIV_Msk,
                    whal_SetBits(RCC_CR_HSIDIV_Msk, RCC_CR_HSIDIV_Pos, hsidiv));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32c0_Rcc_SetSysClock(whal_Clock *clkDev,
                                       whal_Stm32c0_Rcc_SysClockSrc src)
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

whal_Error whal_Stm32c0_Rcc_EnablePeriphClk(whal_Clock *clkDev,
                                           const whal_Stm32c0_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32c0_Rcc_DisablePeriphClk(whal_Clock *clkDev,
                                            const whal_Stm32c0_Rcc_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, clk->regOffset, clk->enableMask,
                    whal_SetBits(clk->enableMask, clk->enablePos, 0));
    return WHAL_SUCCESS;
}
