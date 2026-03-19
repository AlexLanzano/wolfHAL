#include <stdint.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/clock/pic32cz_clock.h>
#include <wolfHAL/error.h>

/*
 * PIC32CZ Clock System Register Map
 *
 * The clock controller base address contains three subsystems at fixed offsets:
 * - OSCCTRL at 0x00000: Oscillator and PLL control
 * - GCLK at 0x10000: Generic clock controller
 * - MCLK at 0x12000: Main clock controller
 */

/* OSCCTRL - Oscillator Controller (base offset 0x00000) */
#define OSCCTRL 0x00000

/* PLL Control Register - enables PLL and selects reference/bandwidth */
#define OSCCTRL_PLLxCTRL_REG(pllInst) (0x40 + (pllInst * 20))
#define OSCCTRL_PLLxCTRL_ENABLE_Pos 1                                                  /* PLL enable */
#define OSCCTRL_PLLxCTRL_ENABLE_Msk (1UL << OSCCTRL_PLLxCTRL_ENABLE_Pos)

#define OSCCTRL_PLLxCTRL_REFSEL_Pos 8                                                  /* Reference select */
#define OSCCTRL_PLLxCTRL_REFSEL_Msk (WHAL_BITMASK(3) << OSCCTRL_PLLxCTRL_REFSEL_Pos)

#define OSCCTRL_PLLxCTRL_BWSEL_Pos 11                                                  /* Bandwidth select */
#define OSCCTRL_PLLxCTRL_BWSEL_Msk (WHAL_BITMASK(3) << OSCCTRL_PLLxCTRL_BWSEL_Pos)

/* PLL Feedback Divider Register - sets VCO multiplication factor */
#define OSCCTRL_PLLxFBDIV_REG(pllInst) (0x44 + (pllInst * 20))
#define OSCCTRL_PLLxFBDIV_Pos 0                                                /* Feedback divider (16-1023) */
#define OSCCTRL_PLLxFBDIV_Msk (WHAL_BITMASK(10) << OSCCTRL_PLLxFBDIV_Pos)

/* PLL Reference Divider Register - divides input reference clock */
#define OSCCTRL_PLLxREFDIV_REG(pllInst) (0x48 + (pllInst * 20))
#define OSCCTRL_PLLxREFDIV_Pos 0                                               /* Reference divider (1-63) */
#define OSCCTRL_PLLxREFDIV_Msk (WHAL_BITMASK(6) << OSCCTRL_PLLxREFDIV_Pos)

/* PLL Post-Divider A Register - divides VCO for each output */
#define OSCCTRL_PLLxPOSTDIVA_REG(pllInst) (0x4C + (pllInst * 20))
#define OSCCTRL_PLLxPOSTDIVA_POSTDIV0_Pos 0                                                       /* Output 0 divider */
#define OSCCTRL_PLLxPOSTDIVA_POSTDIV0_Msk (WHAL_BITMASK(6) << OSCCTRL_PLLxPOSTDIVA_POSTDIV0_Pos)

#define OSCCTRL_PLLxPOSTDIVA_OUTEN0_Pos 7                                                         /* Output 0 enable */
#define OSCCTRL_PLLxPOSTDIVA_OUTEN0_Msk (1UL << OSCCTRL_PLLxPOSTDIVA_OUTEN0_Pos)

/* OSCCTRL Status Register - PLL lock and oscillator ready flags */
#define OSCCTRL_STATUS_REG (OSCCTRL + 0x10)
#define OSCCTRL_STATUS_PLLxLOCK_Pos(pllInst) (24 + (pllInst))
#define OSCCTRL_STATUS_PLLxLOCK_Msk(pllInst) (1UL << (OSCCTRL_STATUS_PLLxLOCK_Pos(pllInst)))

/* GCLK - Generic Clock Controller (base offset 0x10000) */
#define GCLK 0x10000

/* Generator Control Register - configures clock source and divider per generator */
#define GCLK_GENCTRLx_REG(gclkInst) ((GCLK + 0x20 + (gclkInst * 0x4)))
#define GCLK_GENCTRLx_SRC_Pos 0                                                   /* Source selection */
#define GCLK_GENCTRLx_SRC_Msk (WHAL_BITMASK(5) << GCLK_GENCTRLx_SRC_Pos)

#define GCLK_GENCTRLx_GENEN_Pos 8                                                 /* Generator enable */
#define GCLK_GENCTRLx_GENEN_Msk (1UL << GCLK_GENCTRLx_GENEN_Pos)

#define GCLK_GENCTRLx_DIV_Pos 16                                                  /* Division factor */
#define GCLK_GENCTRLx_DIV_Msk (WHAL_BITMASK(16) << GCLK_GENCTRLx_DIV_Pos)

/* Peripheral Channel Control Register - connects generator to peripheral */
#define GCLK_PCHCTRLx_REG(periphChannel) (GCLK + 0x80 + (periphChannel * 0x4))
#define GCLK_PCHCTRLx_GEN_Pos 0                                                 /* Generator selection */
#define GCLK_PCHCTRLx_GEN_Msk (WHAL_BITMASK(4) << GCLK_PCHCTRLx_GEN_Pos)

#define GCLK_PCHCTRLx_CHEN_Pos 6                                                /* Channel enable */
#define GCLK_PCHCTRLx_CHEN_Msk (1UL << GCLK_PCHCTRLx_CHEN_Pos)

/* GCLK Synchronization Busy Register - poll after writing GENCTRLx */
#define GCLK_SYNCBUSY_REG (GCLK + 0x04)
#define GCLK_SYNCBUSY_GENCTRLx_Pos(gclkInst) (2 + (gclkInst))
#define GCLK_SYNCBUSY_GENCTRLx_Msk(gclkInst) (1UL << (GCLK_SYNCBUSY_GENCTRLx_Pos(gclkInst)))

/* MCLK - Main Clock Controller (base offset 0x12000) */
#define MCLK 0x12000

/* MCLK Interrupt Flag Register - clock ready status */
#define MCLK_INTFLAG_REG (MCLK + 0x08)
#define MCLK_INTFLAG_CKRDY_Pos 0                                                  /* Clock ready */
#define MCLK_INTFLAG_CKRDY_Msk (1UL << MCLK_INTFLAG_CKRDY_Pos)

/* CPU Clock Divider Register */
#define MCLK_DIV1_REG (MCLK + 0x10)
#define MCLK_DIV1_Pos 0                                              /* CPU clock divider */
#define MCLK_DIV1_Msk (WHAL_BITMASK(8) << MCLK_DIV1_Pos)

/* Peripheral Clock Mask Registers - enable/disable bus clocks to peripherals */
#define MCLK_CLKxMSK_REG(enableInst) (MCLK + 0x3C + (enableInst * 0x4))

whal_Error whal_Pic32czClockPll_Init(whal_Clock *clkDev)
{
    whal_Pic32czClock_Cfg *cfg;
    whal_Pic32czClockPll_OscCtrlCfg *oscCtrlCfg;
    whal_Pic32czClock_MclkCfg *mclkCfg;

    size_t PLLxCTRL_REG;
    size_t PLLxFBDIV_REG;
    size_t PLLxREFDIV_REG;
    size_t PLLxPOSTDIVA_REG;
    size_t status;

    if (!clkDev || !clkDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = clkDev->cfg;
    oscCtrlCfg = cfg->oscCtrlCfg;
    mclkCfg = cfg->mclkCfg;

    /* Calculate register addresses for the selected PLL instance */
    PLLxCTRL_REG = OSCCTRL_PLLxCTRL_REG(oscCtrlCfg->pllInst);
    PLLxFBDIV_REG = OSCCTRL_PLLxFBDIV_REG(oscCtrlCfg->pllInst);
    PLLxREFDIV_REG = OSCCTRL_PLLxREFDIV_REG(oscCtrlCfg->pllInst);
    PLLxPOSTDIVA_REG = OSCCTRL_PLLxPOSTDIVA_REG(oscCtrlCfg->pllInst);

    /* Configure PLL feedback divider (sets VCO multiplication factor) */
    whal_Reg_Update(clkDev->regmap.base, PLLxFBDIV_REG, OSCCTRL_PLLxFBDIV_Msk,
                    whal_SetBits(OSCCTRL_PLLxFBDIV_Msk, OSCCTRL_PLLxFBDIV_Pos, oscCtrlCfg->fbDiv));

    /* Configure PLL reference divider (divides input clock before PLL) */
    whal_Reg_Update(clkDev->regmap.base, PLLxREFDIV_REG, OSCCTRL_PLLxREFDIV_Msk,
                    whal_SetBits(OSCCTRL_PLLxREFDIV_Msk, OSCCTRL_PLLxREFDIV_Pos, oscCtrlCfg->refDiv));

    /* Configure each PLL output with its post-divider and enable it */
    for (uint8_t i = 0; i < oscCtrlCfg->outCfgCount; ++i) {
        whal_Pic32czClockPll_OutCfg *outCfg = &oscCtrlCfg->outCfg[i];
        whal_Reg_Update(clkDev->regmap.base, PLLxPOSTDIVA_REG,
                        outCfg->outEnMask | outCfg->postDivMask,
                        whal_SetBits(outCfg->postDivMask, outCfg->postDivPos, outCfg->postDiv) |
                        whal_SetBits(outCfg->outEnMask, outCfg->outEnPos, 1));
    }

    /* Enable PLL with selected reference source and loop filter bandwidth */
    whal_Reg_Update(clkDev->regmap.base, PLLxCTRL_REG,
                    OSCCTRL_PLLxCTRL_ENABLE_Msk | OSCCTRL_PLLxCTRL_REFSEL_Msk | OSCCTRL_PLLxCTRL_BWSEL_Msk,
                    whal_SetBits(OSCCTRL_PLLxCTRL_ENABLE_Msk, OSCCTRL_PLLxCTRL_ENABLE_Pos, 1) |
                    whal_SetBits(OSCCTRL_PLLxCTRL_REFSEL_Msk, OSCCTRL_PLLxCTRL_REFSEL_Pos, oscCtrlCfg->refSel) |
                    whal_SetBits(OSCCTRL_PLLxCTRL_BWSEL_Msk, OSCCTRL_PLLxCTRL_BWSEL_Pos, oscCtrlCfg->bwSel));

    /* Wait for PLL to lock */
    do {
        whal_Reg_Get(clkDev->regmap.base, OSCCTRL_STATUS_REG,
                     OSCCTRL_STATUS_PLLxLOCK_Msk(oscCtrlCfg->pllInst),
                     OSCCTRL_STATUS_PLLxLOCK_Pos(oscCtrlCfg->pllInst), &status);
    } while (!status);

    /* Configure CPU clock divider in MCLK */
    whal_Reg_Update(clkDev->regmap.base, MCLK_DIV1_REG, MCLK_DIV1_Msk,
                    whal_SetBits(MCLK_DIV1_Msk, MCLK_DIV1_Pos, mclkCfg->div));

    /* Wait for clock divider change to take effect */
    do {
        whal_Reg_Get(clkDev->regmap.base, MCLK_INTFLAG_REG,
                     MCLK_INTFLAG_CKRDY_Msk, MCLK_INTFLAG_CKRDY_Pos, &status);
    } while (!status);

    /* Configure each GCLK generator with its source and divider */
    for (uint8_t i = 0; i < cfg->gclkCfgCount; ++i) {
        whal_Pic32czClock_GclkCfg *gclkCfg = &cfg->gclkCfg[i];
        whal_Reg_Update(clkDev->regmap.base, GCLK_GENCTRLx_REG(gclkCfg->gen),
                        GCLK_GENCTRLx_SRC_Msk | GCLK_GENCTRLx_GENEN_Msk | GCLK_GENCTRLx_DIV_Msk,
                        whal_SetBits(GCLK_GENCTRLx_SRC_Msk, GCLK_GENCTRLx_SRC_Pos, gclkCfg->genSrc) |
                        whal_SetBits(GCLK_GENCTRLx_GENEN_Msk, GCLK_GENCTRLx_GENEN_Pos, 1) |
                        whal_SetBits(GCLK_GENCTRLx_DIV_Msk, GCLK_GENCTRLx_DIV_Pos, gclkCfg->genDiv));

        /* Wait for generator synchronization */
        do {
            whal_Reg_Get(clkDev->regmap.base, GCLK_SYNCBUSY_REG,
                         GCLK_SYNCBUSY_GENCTRLx_Msk(gclkCfg->gen),
                         GCLK_SYNCBUSY_GENCTRLx_Pos(gclkCfg->gen), &status);
        } while (status);
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czClockPll_Deinit(whal_Clock *clkDev)
{
    if (!clkDev) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czClock_Enable(whal_Clock *clkDev, const void *clk)
{
    const whal_Pic32czClock_Clk *pic32Clk = clk;

    if (!clkDev) {
        return WHAL_EINVAL;
    }

    /*
     * Enable peripheral clock in two steps:
     * 1. Connect the GCLK peripheral channel to a generator and enable it
     * 2. Enable the peripheral's bus clock in MCLK
     */

    /* Enable GCLK peripheral channel and connect to specified generator */
    whal_Reg_Update(clkDev->regmap.base, GCLK_PCHCTRLx_REG(pic32Clk->gclkPeriphChannel),
                    GCLK_PCHCTRLx_GEN_Msk | GCLK_PCHCTRLx_CHEN_Msk,
                    whal_SetBits(GCLK_PCHCTRLx_GEN_Msk, GCLK_PCHCTRLx_GEN_Pos, pic32Clk->gclkPeriphSrc) |
                    whal_SetBits(GCLK_PCHCTRLx_CHEN_Msk, GCLK_PCHCTRLx_CHEN_Pos, 1));

    /* Enable bus clock for peripheral in MCLK mask register */
    whal_Reg_Update(clkDev->regmap.base, MCLK_CLKxMSK_REG(pic32Clk->mclkEnableInst),
                    pic32Clk->mclkEnableMask,
                    whal_SetBits(pic32Clk->mclkEnableMask, pic32Clk->mclkEnablePos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czClock_Disable(whal_Clock *clkDev, const void *clk)
{
    const whal_Pic32czClock_Clk *pic32Clk = clk;

    if (!clkDev) {
        return WHAL_EINVAL;
    }

    /* Disable bus clock for peripheral in MCLK mask register */
    whal_Reg_Update(clkDev->regmap.base, MCLK_CLKxMSK_REG(pic32Clk->mclkEnableInst),
                    pic32Clk->mclkEnableMask,
                    whal_SetBits(pic32Clk->mclkEnableMask, pic32Clk->mclkEnablePos, 0));

    /* Disable GCLK peripheral channel */
    whal_Reg_Update(clkDev->regmap.base, GCLK_PCHCTRLx_REG(pic32Clk->gclkPeriphChannel),
                    GCLK_PCHCTRLx_GEN_Msk | GCLK_PCHCTRLx_CHEN_Msk,
                    whal_SetBits(GCLK_PCHCTRLx_GEN_Msk, GCLK_PCHCTRLx_GEN_Pos, pic32Clk->gclkPeriphSrc) |
                    whal_SetBits(GCLK_PCHCTRLx_CHEN_Msk, GCLK_PCHCTRLx_CHEN_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czClock_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    if (!clkDev || !rateOut) {
        return WHAL_EINVAL;
    }

    /* TODO: Calculate actual clock rate from PLL and divider settings */
    *rateOut = 0;
    return WHAL_SUCCESS;
}

const whal_ClockDriver whal_Pic32czClockPll_Driver = {
    .Init = whal_Pic32czClockPll_Init,
    .Deinit = whal_Pic32czClockPll_Deinit,
    .Enable = whal_Pic32czClock_Enable,
    .Disable = whal_Pic32czClock_Disable,
    .GetRate = whal_Pic32czClock_GetRate,
};
