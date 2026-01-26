#include <wolfHAL/error.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/st_rcc.h>
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

static whal_Error whal_StRcc_InitPllClk(whal_Clock *clkDev,
                                        whal_StRcc_Cfg *cfg,
                                        whal_StRcc_PllClkCfg *pllCfg)
{
    whal_Error err;
    const whal_Regmap *reg = &clkDev->regmap;
    size_t pllCfgMask = 
        ST_RCC_PLLCFGR_PLLSRC_MASK |
        ST_RCC_PLLCFGR_PLLM_MASK |
        ST_RCC_PLLCFGR_PLLN_MASK |
        ST_RCC_PLLCFGR_PLLP_MASK |
        ST_RCC_PLLCFGR_PLLQ_MASK |
        ST_RCC_PLLCFGR_PLLREN_MASK |
        ST_RCC_PLLCFGR_PLLR_MASK;

    if (cfg->sysClkSrc != WHAL_ST_RCC_SYSCLK_SRC_PLL) {
        return WHAL_EINVAL;
    }

    err = whal_Reg_Update(reg, ST_RCC_CFGR_REG, ST_RCC_CFGR_SW,
                          whal_SetBits(ST_RCC_CFGR_SW, cfg->sysClkSrc));
    if (err) {
        return err;
    }

    err = whal_Reg_Update(reg, ST_RCC_PLLCFGR_REG, pllCfgMask,
                          whal_SetBits(ST_RCC_PLLCFGR_PLLSRC_MASK, pllCfg->clkSrc) |
                          whal_SetBits(ST_RCC_PLLCFGR_PLLM_MASK, pllCfg->m) |
                          whal_SetBits(ST_RCC_PLLCFGR_PLLN_MASK, pllCfg->n) |
                          whal_SetBits(ST_RCC_PLLCFGR_PLLP_MASK, pllCfg->p) |
                          whal_SetBits(ST_RCC_PLLCFGR_PLLQ_MASK, pllCfg->q) |
                          whal_SetBits(ST_RCC_PLLCFGR_PLLREN_MASK, 1) |
                          whal_SetBits(ST_RCC_PLLCFGR_PLLR_MASK, pllCfg->r));
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StRcc_Init(whal_Clock *clkDev)
{
    whal_Error err;
    whal_StRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StRcc_Cfg *)clkDev->cfg;

    switch (cfg->sysClkSrc) {
    case WHAL_ST_RCC_SYSCLK_SRC_PLL: {
        err = whal_StRcc_InitPllClk(clkDev, cfg, &cfg->sysClkCfg.pll);

    } break;
    default:
        err = WHAL_EINVAL;
    }

    return err;
}

whal_Error whal_StRcc_Deinit(whal_Clock *clkDev)
{
    return WHAL_SUCCESS;
}

whal_Error whal_StRcc_Enable(whal_Clock *clkDev)
{
    whal_Error err;
    whal_StRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StRcc_Cfg *)clkDev->cfg;

    if (cfg->sysClkSrc == WHAL_ST_RCC_SYSCLK_SRC_PLL) {
        err = whal_Reg_Update(&clkDev->regmap, ST_RCC_CR_REG, ST_RCC_CR_PLLON_MASK,
                              whal_SetBits(ST_RCC_CR_PLLON_MASK, 1));
        if (err) {
            return err;
        }
    }

    for (int i = 0; i < cfg->periphClkEnCount; ++i) {
        switch (cfg->periphClkEn[i]) {
        case WHAL_ST_RCC_PERIPH_GPIOA:
            err = whal_Reg_Update(&clkDev->regmap, ST_RCC_AHB2ENR_REG, ST_RCC_AHB2ENR_GPIOAEN,
                                  whal_SetBits(ST_RCC_AHB2ENR_GPIOAEN, 1));
            if (err) {
                return err;
            }

            break;

        case WHAL_ST_RCC_PERIPH_GPIOB:
            err = whal_Reg_Update(&clkDev->regmap, ST_RCC_AHB2ENR_REG, ST_RCC_AHB2ENR_GPIOBEN,
                                  whal_SetBits(ST_RCC_AHB2ENR_GPIOBEN, 1));
            if (err) {
                return err;
            }

            break;

        case WHAL_ST_RCC_PERIPH_LPUART1:
            err = whal_Reg_Update(&clkDev->regmap, ST_RCC_APB1ENR2_REG, ST_RCC_APB1ENR2_LPUART1EN,
                                  whal_SetBits(ST_RCC_APB1ENR2_LPUART1EN, 1));
            if (err) {
                return err;
            }

            break;

        case WHAL_ST_RCC_PERIPH_FLASH:
            err = whal_Reg_Update(&clkDev->regmap, ST_RCC_AHB3ENR_REG, ST_RCC_AHB3ENR_FLASHEN,
                                  whal_SetBits(ST_RCC_AHB3ENR_FLASHEN, 1));
            if (err) {
                return err;
            }

            break;

        default:
            return WHAL_EINVAL;
        }
    }


    return WHAL_SUCCESS;
}

whal_Error whal_StRcc_Disable(whal_Clock *clkDev)
{
    whal_Error err;
    whal_StRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StRcc_Cfg *)clkDev->cfg;

    if (cfg->sysClkSrc == WHAL_ST_RCC_SYSCLK_SRC_PLL) {

        err = whal_Reg_Update(&clkDev->regmap, ST_RCC_CFGR_REG, ST_RCC_CFGR_SW,
                              whal_SetBits(ST_RCC_CFGR_SW, WHAL_ST_RCC_SYSCLK_SRC_MSI));
        if (err) {
            return err;
        }

        err = whal_Reg_Update(&clkDev->regmap, ST_RCC_CR_REG, ST_RCC_CR_MSIRANGE,
                              whal_SetBits(ST_RCC_CR_MSIRANGE, WHAL_ST_RCC_MSIRANGE_4MHz));
        if (err) {
            return err;
        }

        err = whal_Reg_Update(&clkDev->regmap, ST_RCC_CR_REG,
                              ST_RCC_CR_PLLON_MASK,
                              whal_SetBits(ST_RCC_CR_PLLON_MASK, 0));
        if (err) {
            return err;
        }
    }

    for (int i = 0; i < cfg->periphClkEnCount; ++i) {
        switch (cfg->periphClkEn[i]) {
        case WHAL_ST_RCC_PERIPH_GPIOA:
            err = whal_Reg_Update(&clkDev->regmap, ST_RCC_AHB2ENR_REG, ST_RCC_AHB2ENR_GPIOAEN,
                                  whal_SetBits(ST_RCC_AHB2ENR_GPIOAEN, 0));
            if (err) {
                return err;
            }

            break;

        case WHAL_ST_RCC_PERIPH_GPIOB:
            err = whal_Reg_Update(&clkDev->regmap, ST_RCC_AHB2ENR_REG, ST_RCC_AHB2ENR_GPIOBEN,
                                  whal_SetBits(ST_RCC_AHB2ENR_GPIOBEN, 0));
            if (err) {
                return err;
            }

            break;

        case WHAL_ST_RCC_PERIPH_LPUART1:
            err = whal_Reg_Update(&clkDev->regmap, ST_RCC_APB1ENR2_REG, ST_RCC_APB1ENR2_LPUART1EN,
                                  whal_SetBits(ST_RCC_APB1ENR2_LPUART1EN, 0));
            if (err) {
                return err;
            }

            break;

        case WHAL_ST_RCC_PERIPH_FLASH:
            err = whal_Reg_Update(&clkDev->regmap, ST_RCC_AHB3ENR_REG, ST_RCC_AHB3ENR_FLASHEN,
                                  whal_SetBits(ST_RCC_AHB3ENR_FLASHEN, 0));
            if (err) {
                return err;
            }

            break;

        default:
            return WHAL_EINVAL;
        }
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StRcc_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    
    whal_StRcc_Cfg *cfg;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StRcc_Cfg *)clkDev->cfg;
    if (cfg->sysClkSrc == WHAL_ST_RCC_SYSCLK_SRC_PLL) {
        whal_StRcc_PllClkCfg *pllClkCfg = &cfg->sysClkCfg.pll;
        /* Sys freq = ((srcFreq / pllm) * plln) / pllr */ 
        size_t srcFreq;
        size_t pllm = pllClkCfg->m + 1;
        size_t plln = pllClkCfg->n;
        size_t pllr = pllClkCfg->r + 1;

        if (pllClkCfg->clkSrc == WHAL_ST_RCC_PLLCLK_SRC_MSI) {
            srcFreq = 4000000;
        }
        else {
            return WHAL_EINVAL;
        }

        *rateOut = ((srcFreq / pllm) * plln) / pllr;
    }
    return WHAL_SUCCESS;
}

whal_Error whal_StRcc_Cmd(whal_Clock *clkDev, size_t cmd, void *args)
{
    return WHAL_SUCCESS;
}

const whal_ClockDriver whal_StRcc_Driver = {
    .Init = whal_StRcc_Init,
    .Deinit = whal_StRcc_Deinit,
    .Enable = whal_StRcc_Enable,
    .Disable = whal_StRcc_Disable,
    .GetRate = whal_StRcc_GetRate,
    .Cmd = whal_StRcc_Cmd,
};
