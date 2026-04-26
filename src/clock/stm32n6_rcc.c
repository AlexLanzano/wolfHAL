#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32n6_rcc.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/**
 * @file stm32n6_rcc.c
 * @brief STM32N6 RCC driver implementation.
 *
 * RCC base address: 0x46028000
 *
 * Key differences from STM32H5/WBA:
 * - Four PLLs (PLL1-PLL4) with DIVM/DIVN/POSTDIV1/POSTDIV2 scheme
 * - Separate SYSSW and CPUSW fields in CFGR1
 * - PLL enable in RCC_CR uses PLL1ON[8] (same bit position)
 * - Status register (RCC_SR at 0x004) has ready flags at matching positions
 * - PLL1 configuration split across PLL1CFGR1 (0x080), PLL1CFGR2 (0x084),
 *   PLL1CFGR3 (0x088)
 */

/* RCC_CR (offset 0x000) - Clock control */
#define RCC_CR_REG          0x000
#define RCC_CR_LSION_Pos    0
#define RCC_CR_LSION_Msk    (1UL << RCC_CR_LSION_Pos)
#define RCC_CR_LSEON_Pos    1
#define RCC_CR_LSEON_Msk    (1UL << RCC_CR_LSEON_Pos)
#define RCC_CR_MSION_Pos    2
#define RCC_CR_MSION_Msk    (1UL << RCC_CR_MSION_Pos)
#define RCC_CR_HSION_Pos    3
#define RCC_CR_HSION_Msk    (1UL << RCC_CR_HSION_Pos)
#define RCC_CR_HSEON_Pos    4
#define RCC_CR_HSEON_Msk    (1UL << RCC_CR_HSEON_Pos)
#define RCC_CR_PLL1ON_Pos   8
#define RCC_CR_PLL1ON_Msk   (1UL << RCC_CR_PLL1ON_Pos)

/* RCC_SR (offset 0x004) - Status */
#define RCC_SR_REG          0x004
#define RCC_SR_LSIRDY_Pos   0
#define RCC_SR_LSIRDY_Msk   (1UL << RCC_SR_LSIRDY_Pos)
#define RCC_SR_LSERDY_Pos   1
#define RCC_SR_LSERDY_Msk   (1UL << RCC_SR_LSERDY_Pos)
#define RCC_SR_MSIRDY_Pos   2
#define RCC_SR_MSIRDY_Msk   (1UL << RCC_SR_MSIRDY_Pos)
#define RCC_SR_HSIRDY_Pos   3
#define RCC_SR_HSIRDY_Msk   (1UL << RCC_SR_HSIRDY_Pos)
#define RCC_SR_HSERDY_Pos   4
#define RCC_SR_HSERDY_Msk   (1UL << RCC_SR_HSERDY_Pos)
#define RCC_SR_PLL1RDY_Pos  8
#define RCC_SR_PLL1RDY_Msk  (1UL << RCC_SR_PLL1RDY_Pos)

/* RCC_CFGR1 (offset 0x020) - System/CPU clock switch */
#define RCC_CFGR1_REG       0x020
#define RCC_CFGR1_CPUSW_Pos  16
#define RCC_CFGR1_CPUSW_Msk  (3UL << RCC_CFGR1_CPUSW_Pos)
#define RCC_CFGR1_CPUSWS_Pos 20
#define RCC_CFGR1_CPUSWS_Msk (3UL << RCC_CFGR1_CPUSWS_Pos)
#define RCC_CFGR1_SYSSW_Pos  24
#define RCC_CFGR1_SYSSW_Msk  (3UL << RCC_CFGR1_SYSSW_Pos)
#define RCC_CFGR1_SYSSWS_Pos 28
#define RCC_CFGR1_SYSSWS_Msk (3UL << RCC_CFGR1_SYSSWS_Pos)

/* RCC_PLL1CFGR1 (offset 0x080) - PLL1 source, DIVM, DIVN */
#define RCC_PLL1CFGR1_REG        0x080
#define RCC_PLL1CFGR1_PLL1SEL_Pos 28
#define RCC_PLL1CFGR1_PLL1SEL_Msk (7UL << RCC_PLL1CFGR1_PLL1SEL_Pos)
#define RCC_PLL1CFGR1_PLL1BYP_Pos 27
#define RCC_PLL1CFGR1_PLL1BYP_Msk (1UL << RCC_PLL1CFGR1_PLL1BYP_Pos)
#define RCC_PLL1CFGR1_PLL1DIVM_Pos 20
#define RCC_PLL1CFGR1_PLL1DIVM_Msk (0x3FUL << RCC_PLL1CFGR1_PLL1DIVM_Pos)
#define RCC_PLL1CFGR1_PLL1DIVN_Pos 0
#define RCC_PLL1CFGR1_PLL1DIVN_Msk (0xFFFUL << RCC_PLL1CFGR1_PLL1DIVN_Pos)

/* RCC_PLL1CFGR3 (offset 0x088) - PLL1 post dividers and output enables */
#define RCC_PLL1CFGR3_REG         0x088
#define RCC_PLL1CFGR3_PLL1PDIVEN_Pos  30
#define RCC_PLL1CFGR3_PLL1PDIVEN_Msk  (1UL << RCC_PLL1CFGR3_PLL1PDIVEN_Pos)
#define RCC_PLL1CFGR3_PLL1PDIV1_Pos 27
#define RCC_PLL1CFGR3_PLL1PDIV1_Msk (7UL << RCC_PLL1CFGR3_PLL1PDIV1_Pos)
#define RCC_PLL1CFGR3_PLL1PDIV2_Pos 24
#define RCC_PLL1CFGR3_PLL1PDIV2_Msk (7UL << RCC_PLL1CFGR3_PLL1PDIV2_Pos)

/* RCC_CCIPR2 (offset 0x148) - Independent peripheral clock config 2 */
#define RCC_CCIPR2_REG            0x148
#define RCC_CCIPR2_ETH1SEL_Pos    16
#define RCC_CCIPR2_ETH1SEL_Msk    (7UL << RCC_CCIPR2_ETH1SEL_Pos)

/* HSI base frequency (64 MHz default after reset) */
#define HSI_FREQ 64000000UL

#ifdef WHAL_CFG_CLOCK_API_MAPPING_STM32N6_PLL
#define whal_Stm32n6RccPll_Init   whal_Clock_Init
#define whal_Stm32n6RccPll_Deinit whal_Clock_Deinit
#define whal_Stm32n6Rcc_Enable    whal_Clock_Enable
#define whal_Stm32n6Rcc_Disable   whal_Clock_Disable
#endif /* WHAL_CFG_CLOCK_API_MAPPING_STM32N6_PLL */

#ifdef WHAL_CFG_CLOCK_API_MAPPING_STM32N6_HSI
#define whal_Stm32n6RccHsi_Init   whal_Clock_Init
#define whal_Stm32n6RccHsi_Deinit whal_Clock_Deinit
#define whal_Stm32n6Rcc_Enable    whal_Clock_Enable
#define whal_Stm32n6Rcc_Disable   whal_Clock_Disable
#endif /* WHAL_CFG_CLOCK_API_MAPPING_STM32N6_HSI */

whal_Error whal_Stm32n6RccPll_Init(whal_Clock *clkDev)
{
    whal_Stm32n6Rcc_Cfg *cfg;
    whal_Stm32n6Rcc_Pll1Cfg *pllCfg;
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32n6Rcc_Cfg *)clkDev->cfg;
    pllCfg = (whal_Stm32n6Rcc_Pll1Cfg *)cfg->sysClkCfg;
    base = clkDev->regmap.base;

    /* Enable HSE if selected as PLL1 source */
    if (pllCfg->clkSrc == WHAL_STM32N6_RCC_PLL1SRC_HSE) {
        whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSEON_Msk,
                        RCC_CR_HSEON_Msk);
        while (!(whal_Reg_Read(base, RCC_SR_REG) & RCC_SR_HSERDY_Msk))
            ;
    }

    /* Ensure HSI is on (fallback during PLL config) */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSION_Msk, RCC_CR_HSION_Msk);
    while (!(whal_Reg_Read(base, RCC_SR_REG) & RCC_SR_HSIRDY_Msk))
        ;

    /* Ensure PLL1 is off before configuring */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);
    while (whal_Reg_Read(base, RCC_SR_REG) & RCC_SR_PLL1RDY_Msk)
        ;

    /* Configure PLL1CFGR1: source, DIVM, DIVN, disable bypass */
    whal_Reg_Write(base, RCC_PLL1CFGR1_REG,
                   whal_SetBits(RCC_PLL1CFGR1_PLL1SEL_Msk,
                                RCC_PLL1CFGR1_PLL1SEL_Pos,
                                pllCfg->clkSrc) |
                   whal_SetBits(RCC_PLL1CFGR1_PLL1DIVM_Msk,
                                RCC_PLL1CFGR1_PLL1DIVM_Pos,
                                pllCfg->m) |
                   whal_SetBits(RCC_PLL1CFGR1_PLL1DIVN_Msk,
                                RCC_PLL1CFGR1_PLL1DIVN_Pos,
                                pllCfg->n));

    /* Configure PLL1CFGR3: post dividers and enable them */
    whal_Reg_Update(base, RCC_PLL1CFGR3_REG,
                    RCC_PLL1CFGR3_PLL1PDIVEN_Msk |
                    RCC_PLL1CFGR3_PLL1PDIV1_Msk |
                    RCC_PLL1CFGR3_PLL1PDIV2_Msk,
                    RCC_PLL1CFGR3_PLL1PDIVEN_Msk |
                    whal_SetBits(RCC_PLL1CFGR3_PLL1PDIV1_Msk,
                                 RCC_PLL1CFGR3_PLL1PDIV1_Pos,
                                 pllCfg->pdiv1) |
                    whal_SetBits(RCC_PLL1CFGR3_PLL1PDIV2_Msk,
                                 RCC_PLL1CFGR3_PLL1PDIV2_Pos,
                                 pllCfg->pdiv2));

    /* Enable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, RCC_CR_PLL1ON_Msk);

    /* Wait for PLL1 lock */
    while (!(whal_Reg_Read(base, RCC_SR_REG) & RCC_SR_PLL1RDY_Msk))
        ;

    /* Switch CPU clock to ic1_ck (PLL1 output) */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_CPUSW_Msk,
                    whal_SetBits(RCC_CFGR1_CPUSW_Msk,
                                 RCC_CFGR1_CPUSW_Pos,
                                 cfg->cpuClkSrc));

    /* Wait for CPU clock switch to complete */
    while ((whal_Reg_Read(base, RCC_CFGR1_REG) & RCC_CFGR1_CPUSWS_Msk) !=
           whal_SetBits(RCC_CFGR1_CPUSWS_Msk, RCC_CFGR1_CPUSWS_Pos,
                        cfg->cpuClkSrc))
        ;

    /* Switch system bus clock */
    whal_Reg_Update(base, RCC_CFGR1_REG, RCC_CFGR1_SYSSW_Msk,
                    whal_SetBits(RCC_CFGR1_SYSSW_Msk,
                                 RCC_CFGR1_SYSSW_Pos,
                                 cfg->sysClkSrc));

    /* Wait for system clock switch to complete */
    while ((whal_Reg_Read(base, RCC_CFGR1_REG) & RCC_CFGR1_SYSSWS_Msk) !=
           whal_SetBits(RCC_CFGR1_SYSSWS_Msk, RCC_CFGR1_SYSSWS_Pos,
                        cfg->sysClkSrc))
        ;

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6RccPll_Deinit(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Switch CPU and system bus back to HSI */
    whal_Reg_Update(base, RCC_CFGR1_REG,
                    RCC_CFGR1_CPUSW_Msk | RCC_CFGR1_SYSSW_Msk,
                    whal_SetBits(RCC_CFGR1_CPUSW_Msk, RCC_CFGR1_CPUSW_Pos,
                                 WHAL_STM32N6_RCC_CPUCLK_SRC_HSI) |
                    whal_SetBits(RCC_CFGR1_SYSSW_Msk, RCC_CFGR1_SYSSW_Pos,
                                 WHAL_STM32N6_RCC_SYSCLK_SRC_HSI));

    /* Wait for CPU switch */
    while ((whal_Reg_Read(base, RCC_CFGR1_REG) & RCC_CFGR1_CPUSWS_Msk) !=
           whal_SetBits(RCC_CFGR1_CPUSWS_Msk, RCC_CFGR1_CPUSWS_Pos,
                        WHAL_STM32N6_RCC_CPUCLK_SRC_HSI))
        ;

    /* Wait for system switch */
    while ((whal_Reg_Read(base, RCC_CFGR1_REG) & RCC_CFGR1_SYSSWS_Msk) !=
           whal_SetBits(RCC_CFGR1_SYSSWS_Msk, RCC_CFGR1_SYSSWS_Pos,
                        WHAL_STM32N6_RCC_SYSCLK_SRC_HSI))
        ;

    /* Disable PLL1 */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_PLL1ON_Msk, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6RccHsi_Init(whal_Clock *clkDev)
{
    size_t base;

    if (!clkDev || !clkDev->cfg)
        return WHAL_EINVAL;

    base = clkDev->regmap.base;

    /* Enable HSI */
    whal_Reg_Update(base, RCC_CR_REG, RCC_CR_HSION_Msk, RCC_CR_HSION_Msk);

    /* Wait for HSI ready */
    while (!(whal_Reg_Read(base, RCC_SR_REG) & RCC_SR_HSIRDY_Msk))
        ;

    /* Select HSI as CPU and system clock */
    whal_Reg_Update(base, RCC_CFGR1_REG,
                    RCC_CFGR1_CPUSW_Msk | RCC_CFGR1_SYSSW_Msk,
                    whal_SetBits(RCC_CFGR1_CPUSW_Msk, RCC_CFGR1_CPUSW_Pos,
                                 WHAL_STM32N6_RCC_CPUCLK_SRC_HSI) |
                    whal_SetBits(RCC_CFGR1_SYSSW_Msk, RCC_CFGR1_SYSSW_Pos,
                                 WHAL_STM32N6_RCC_SYSCLK_SRC_HSI));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6RccHsi_Deinit(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;

    /* HSI is the default clock, nothing to do */
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6Rcc_Enable(whal_Clock *clkDev, const void *clk)
{
    const whal_Stm32n6Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (const whal_Stm32n6Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6Rcc_Disable(whal_Clock *clkDev, const void *clk)
{
    const whal_Stm32n6Rcc_Clk *stClk;

    if (!clkDev || !clk)
        return WHAL_EINVAL;

    stClk = (const whal_Stm32n6Rcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, stClk->enablePos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6Rcc_Ext_SetEth1If(whal_Clock *clkDev,
                                         whal_Stm32n6Rcc_Eth1If mode)
{
    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, RCC_CCIPR2_REG,
                    RCC_CCIPR2_ETH1SEL_Msk,
                    whal_SetBits(RCC_CCIPR2_ETH1SEL_Msk,
                                 RCC_CCIPR2_ETH1SEL_Pos, mode));

    return WHAL_SUCCESS;
}

#if !defined(WHAL_CFG_CLOCK_API_MAPPING_STM32N6_PLL) && \
    !defined(WHAL_CFG_CLOCK_API_MAPPING_STM32N6_HSI)
const whal_ClockDriver whal_Stm32n6RccPll_Driver = {
    .Init = whal_Stm32n6RccPll_Init,
    .Deinit = whal_Stm32n6RccPll_Deinit,
    .Enable = whal_Stm32n6Rcc_Enable,
    .Disable = whal_Stm32n6Rcc_Disable,
};

const whal_ClockDriver whal_Stm32n6RccHsi_Driver = {
    .Init = whal_Stm32n6RccHsi_Init,
    .Deinit = whal_Stm32n6RccHsi_Deinit,
    .Enable = whal_Stm32n6Rcc_Enable,
    .Disable = whal_Stm32n6Rcc_Disable,
};
#endif
