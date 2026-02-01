#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/flash/stm32wb_flash.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define ST_RCC_CR_REG 0x000
#define ST_RCC_CR_MSIRANGE WHAL_MASK_RANGE(7, 4)
#define ST_RCC_CR_PLLON_MASK WHAL_MASK(24)

#define ST_RCC_CFGR_REG 0x008
#define ST_RCC_CFGR_SW WHAL_MASK_RANGE(1, 0)
#define ST_RCC_CFGR_SWS WHAL_MASK_RANGE(3, 2)
#define ST_RCC_CFGR_HPRE WHAL_MASK_RANGE(7, 4)
#define ST_RCC_CFGR_PPRE1 WHAL_MASK_RANGE(10, 8)
#define ST_RCC_CFGR_PPRE2 WHAL_MASK_RANGE(13, 11)
#define ST_RCC_CFGR_STOPWUCK WHAL_MASK(15)
#define ST_RCC_CFGR_HPREF WHAL_MASK(16)
#define ST_RCC_CFGR_PPRE1F WHAL_MASK(17)
#define ST_RCC_CFGR_PPRE2F WHAL_MASK(18)
#define ST_RCC_CFGR_MCOSEL WHAL_MASK_RANGE(27, 24)
#define ST_RCC_CFGR_MCOPRE WHAL_MASK_RANGE(30, 28)

#define ST_RCC_PLLCFGR_REG 0x00C
#define ST_RCC_PLLCFGR_PLLSRC_MASK WHAL_MASK_RANGE(1, 0)
#define ST_RCC_PLLCFGR_PLLM_MASK   WHAL_MASK_RANGE(6, 4)
#define ST_RCC_PLLCFGR_PLLN_MASK   WHAL_MASK_RANGE(14, 8)
#define ST_RCC_PLLCFGR_PLLP_MASK   WHAL_MASK_RANGE(21, 17)
#define ST_RCC_PLLCFGR_PLLQ_MASK   WHAL_MASK_RANGE(27, 25)
#define ST_RCC_PLLCFGR_PLLREN_MASK WHAL_MASK(28)
#define ST_RCC_PLLCFGR_PLLR_MASK   WHAL_MASK_RANGE(31, 29)

#define ST_RCC_PLLCFGR_MASK \
        ST_RCC_PLLCFGR_PLLSRC_MASK | \
        ST_RCC_PLLCFGR_PLLM_MASK | \
        ST_RCC_PLLCFGR_PLLN_MASK | \
        ST_RCC_PLLCFGR_PLLP_MASK | \
        ST_RCC_PLLCFGR_PLLQ_MASK | \
        ST_RCC_PLLCFGR_PLLREN_MASK | \
        ST_RCC_PLLCFGR_PLLR_MASK

#define ST_RCC_AHB2ENR_REG 0x04C
#define ST_RCC_AHB2ENR_GPIOAEN WHAL_MASK(0)
#define ST_RCC_AHB2ENR_GPIOBEN WHAL_MASK(1)
#define ST_RCC_AHB2ENR_GPIOCEN WHAL_MASK(2)
#define ST_RCC_AHB2ENR_GPIODEN WHAL_MASK(3)
#define ST_RCC_AHB2ENR_GPIOEEN WHAL_MASK(4)
#define ST_RCC_AHB2ENR_GPIOHEN WHAL_MASK(7)
#define ST_RCC_AHB2ENR_ADCEN   WHAL_MASK(13)
#define ST_RCC_AHB2ENR_AES1EN  WHAL_MASK(16)

#define ST_RCC_AHB3ENR_REG 0x50
#define ST_RCC_AHB3ENR_FLASHEN WHAL_MASK(25)

#define ST_RCC_APB1ENR2_REG 0x05C
#define ST_RCC_APB1ENR2_LPUART1EN WHAL_MASK(0)
#define ST_RCC_APB1ENR2_LPTIM2EN  WHAL_MASK(5)

whal_Error whal_Stm32wbRccPll_Init(whal_Clock *clkDev)
{
    whal_Error err;
    whal_Stm32wbRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32wbRcc_Cfg *)clkDev->cfg;
    whal_Stm32wbRcc_PllClkCfg *pllCfg = cfg->sysClkCfg;

    err = whal_Stm32wbFlash_Ext_SetLatency(cfg->flash, cfg->flashLatency);
    if (err) {
        return err;
    }

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CFGR_REG, ST_RCC_CFGR_SW,
                    whal_SetBits(ST_RCC_CFGR_SW, cfg->sysClkSrc));

    whal_Reg_Update(clkDev->regmap.base,
                    ST_RCC_PLLCFGR_REG, ST_RCC_PLLCFGR_MASK,
                    whal_SetBits(ST_RCC_PLLCFGR_PLLSRC_MASK, pllCfg->clkSrc) |
                    whal_SetBits(ST_RCC_PLLCFGR_PLLM_MASK, pllCfg->m) |
                    whal_SetBits(ST_RCC_PLLCFGR_PLLN_MASK, pllCfg->n) |
                    whal_SetBits(ST_RCC_PLLCFGR_PLLP_MASK, pllCfg->p) |
                    whal_SetBits(ST_RCC_PLLCFGR_PLLQ_MASK, pllCfg->q) |
                    whal_SetBits(ST_RCC_PLLCFGR_PLLREN_MASK, 1) |
                    whal_SetBits(ST_RCC_PLLCFGR_PLLR_MASK, pllCfg->r));

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CR_REG,
                    ST_RCC_CR_PLLON_MASK,
                    whal_SetBits(ST_RCC_CR_PLLON_MASK, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccMsi_Init(whal_Clock *clkDev)
{
    whal_Error err;
    whal_Stm32wbRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32wbRcc_Cfg *)clkDev->cfg;
    whal_Stm32wbRcc_MsiClkCfg *msiCfg = cfg->sysClkCfg;

    err = whal_Stm32wbFlash_Ext_SetLatency(cfg->flash, cfg->flashLatency);
    if (err) {
        return err;
    }

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CFGR_REG, ST_RCC_CFGR_SW,
                    whal_SetBits(ST_RCC_CFGR_SW, WHAL_STM32WB_RCC_SYSCLK_SRC_MSI));

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CR_REG, ST_RCC_CR_MSIRANGE,
                    whal_SetBits(ST_RCC_CR_MSIRANGE, msiCfg->freq));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccPll_Deinit(whal_Clock *clkDev)
{
    whal_Stm32wbRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32wbRcc_Cfg *)clkDev->cfg;

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CFGR_REG, ST_RCC_CFGR_SW,
                    whal_SetBits(ST_RCC_CFGR_SW, WHAL_STM32WB_RCC_SYSCLK_SRC_MSI));

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CR_REG, ST_RCC_CR_MSIRANGE,
                    whal_SetBits(ST_RCC_CR_MSIRANGE, WHAL_STM32WB_RCC_MSIRANGE_4MHz));

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CR_REG,
                    ST_RCC_CR_PLLON_MASK,
                    whal_SetBits(ST_RCC_CR_PLLON_MASK, 0));

    whal_Stm32wbFlash_Ext_SetLatency(cfg->flash, WHAL_STM32WB_FLASH_LATENCY_0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccMsi_Deinit(whal_Clock *clkDev)
{
    whal_Stm32wbRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32wbRcc_Cfg *)clkDev->cfg;

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CFGR_REG, ST_RCC_CFGR_SW,
                    whal_SetBits(ST_RCC_CFGR_SW, WHAL_STM32WB_RCC_SYSCLK_SRC_MSI));

    whal_Reg_Update(clkDev->regmap.base, ST_RCC_CR_REG, ST_RCC_CR_MSIRANGE,
                    whal_SetBits(ST_RCC_CR_MSIRANGE, WHAL_STM32WB_RCC_MSIRANGE_4MHz));

    whal_Stm32wbFlash_Ext_SetLatency(cfg->flash, WHAL_STM32WB_FLASH_LATENCY_0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRcc_Enable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32wbRcc_Clk *stClk = (whal_Stm32wbRcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRcc_Disable(whal_Clock *clkDev, const void *clk)
{
    whal_Stm32wbRcc_Clk *stClk = (whal_Stm32wbRcc_Clk *)clk;

    whal_Reg_Update(clkDev->regmap.base, stClk->regOffset, stClk->enableMask,
                    whal_SetBits(stClk->enableMask, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccPll_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    
    whal_Stm32wbRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_Stm32wbRcc_Cfg *)clkDev->cfg;
    whal_Stm32wbRcc_PllClkCfg *pllClkCfg = cfg->sysClkCfg;
    /* Sys freq = ((srcFreq / pllm) * plln) / pllr */ 
    size_t srcFreq;
    size_t pllm = pllClkCfg->m + 1;
    size_t plln = pllClkCfg->n;
    size_t pllr = pllClkCfg->r + 1;

    if (pllClkCfg->clkSrc == WHAL_STM32WB_RCC_PLLCLK_SRC_MSI) {
        srcFreq = 4000000;
    }
    else {
        return WHAL_EINVAL;
    }

    *rateOut = ((srcFreq / pllm) * plln) / pllr;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRccMsi_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    size_t msiRange;

    if (!clkDev || !rateOut) {
        return WHAL_EINVAL;
    }

    whal_Reg_Get(clkDev->regmap.base, ST_RCC_CR_REG, ST_RCC_CR_MSIRANGE, &msiRange);
    
    switch (msiRange) {
    case WHAL_STM32WB_RCC_MSIRANGE_100kHz:
        *rateOut = 100000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_200kHz:
        *rateOut = 200000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_400kHz:
        *rateOut = 400000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_800kHz:
        *rateOut = 800000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_1MHz:
        *rateOut = 1000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_2MHz:
        *rateOut = 2000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_4MHz:
        *rateOut = 4000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_8MHz:
        *rateOut = 8000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_16MHz:
        *rateOut = 16000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_24MHz:
        *rateOut = 24000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_32MHz:
        *rateOut = 32000000;
        break;
    case WHAL_STM32WB_RCC_MSIRANGE_48MHz:
        *rateOut = 48000000;
        break;
    }

    return WHAL_SUCCESS;
}
const whal_ClockDriver whal_Stm32wbRccPll_Driver = {
    .Init = whal_Stm32wbRccPll_Init,
    .Deinit = whal_Stm32wbRccPll_Deinit,
    .Enable = whal_Stm32wbRcc_Enable,
    .Disable = whal_Stm32wbRcc_Disable,
    .GetRate = whal_Stm32wbRccPll_GetRate,
};

const whal_ClockDriver whal_Stm32wbRccMsi_Driver = {
    .Init = whal_Stm32wbRccMsi_Init,
    .Deinit = whal_Stm32wbRccMsi_Deinit,
    .Enable = whal_Stm32wbRcc_Enable,
    .Disable = whal_Stm32wbRcc_Disable,
    .GetRate = whal_Stm32wbRccMsi_GetRate,
};
