#include <stdint.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/clock/pic32cz_clock.h>
#include <wolfHAL/error.h>

/*
 * PIC32CZ Clock System Register Map.
 * OSCCTRL @ 0x00000, GCLK @ 0x10000, MCLK @ 0x12000.
 */

#define OSCCTRL 0x00000

#define OSCCTRL_PLLxCTRL_REG(pllInst)        (0x40 + ((pllInst) * 20))
#define OSCCTRL_PLLxCTRL_ENABLE_Pos          1
#define OSCCTRL_PLLxCTRL_ENABLE_Msk          (1UL << OSCCTRL_PLLxCTRL_ENABLE_Pos)
#define OSCCTRL_PLLxCTRL_REFSEL_Pos          8
#define OSCCTRL_PLLxCTRL_REFSEL_Msk          (WHAL_BITMASK(3) << OSCCTRL_PLLxCTRL_REFSEL_Pos)
#define OSCCTRL_PLLxCTRL_BWSEL_Pos           11
#define OSCCTRL_PLLxCTRL_BWSEL_Msk           (WHAL_BITMASK(3) << OSCCTRL_PLLxCTRL_BWSEL_Pos)

#define OSCCTRL_PLLxFBDIV_REG(pllInst)       (0x44 + ((pllInst) * 20))
#define OSCCTRL_PLLxFBDIV_Pos                0
#define OSCCTRL_PLLxFBDIV_Msk                (WHAL_BITMASK(10) << OSCCTRL_PLLxFBDIV_Pos)

#define OSCCTRL_PLLxREFDIV_REG(pllInst)      (0x48 + ((pllInst) * 20))
#define OSCCTRL_PLLxREFDIV_Pos               0
#define OSCCTRL_PLLxREFDIV_Msk               (WHAL_BITMASK(6) << OSCCTRL_PLLxREFDIV_Pos)

#define OSCCTRL_PLLxPOSTDIVA_REG(pllInst)    (0x4C + ((pllInst) * 20))

#define OSCCTRL_STATUS_REG                   (OSCCTRL + 0x10)
#define OSCCTRL_STATUS_PLLxLOCK_Pos(pllInst) (24 + (pllInst))
#define OSCCTRL_STATUS_PLLxLOCK_Msk(pllInst) (1UL << OSCCTRL_STATUS_PLLxLOCK_Pos(pllInst))

#define GCLK 0x10000

#define GCLK_GENCTRLx_REG(gclkInst) ((GCLK + 0x20 + ((gclkInst) * 0x4)))
#define GCLK_GENCTRLx_SRC_Pos       0
#define GCLK_GENCTRLx_SRC_Msk       (WHAL_BITMASK(5) << GCLK_GENCTRLx_SRC_Pos)
#define GCLK_GENCTRLx_GENEN_Pos     8
#define GCLK_GENCTRLx_GENEN_Msk     (1UL << GCLK_GENCTRLx_GENEN_Pos)
#define GCLK_GENCTRLx_DIV_Pos       16
#define GCLK_GENCTRLx_DIV_Msk       (WHAL_BITMASK(16) << GCLK_GENCTRLx_DIV_Pos)

#define GCLK_PCHCTRLx_REG(periphChannel) (GCLK + 0x80 + ((periphChannel) * 0x4))
#define GCLK_PCHCTRLx_GEN_Pos            0
#define GCLK_PCHCTRLx_GEN_Msk            (WHAL_BITMASK(4) << GCLK_PCHCTRLx_GEN_Pos)
#define GCLK_PCHCTRLx_CHEN_Pos           6
#define GCLK_PCHCTRLx_CHEN_Msk           (1UL << GCLK_PCHCTRLx_CHEN_Pos)

#define GCLK_SYNCBUSY_REG                    (GCLK + 0x04)
#define GCLK_SYNCBUSY_GENCTRLx_Pos(gclkInst) (2 + (gclkInst))
#define GCLK_SYNCBUSY_GENCTRLx_Msk(gclkInst) (1UL << GCLK_SYNCBUSY_GENCTRLx_Pos(gclkInst))

#define MCLK 0x12000
#define MCLK_INTFLAG_REG       (MCLK + 0x08)
#define MCLK_INTFLAG_CKRDY_Pos 0
#define MCLK_INTFLAG_CKRDY_Msk (1UL << MCLK_INTFLAG_CKRDY_Pos)
#define MCLK_DIV1_REG          (MCLK + 0x10)
#define MCLK_DIV1_Pos          0
#define MCLK_DIV1_Msk          (WHAL_BITMASK(8) << MCLK_DIV1_Pos)
#define MCLK_CLKxMSK_REG(enableInst) (MCLK + 0x3C + ((enableInst) * 0x4))

whal_Error whal_Pic32cz_Clock_EnablePll(whal_Clock *clkDev,
                                       const whal_Pic32cz_Clock_PllCfg *cfg)
{
    size_t status;
    uint8_t i;
    size_t pllCtrlReg, pllFbdivReg, pllRefdivReg, pllPostdivaReg;

    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    pllCtrlReg     = OSCCTRL_PLLxCTRL_REG(cfg->pllInst);
    pllFbdivReg    = OSCCTRL_PLLxFBDIV_REG(cfg->pllInst);
    pllRefdivReg   = OSCCTRL_PLLxREFDIV_REG(cfg->pllInst);
    pllPostdivaReg = OSCCTRL_PLLxPOSTDIVA_REG(cfg->pllInst);

    whal_Reg_Update(clkDev->regmap.base, pllFbdivReg, OSCCTRL_PLLxFBDIV_Msk,
                    whal_SetBits(OSCCTRL_PLLxFBDIV_Msk, OSCCTRL_PLLxFBDIV_Pos,
                                 cfg->fbDiv));
    whal_Reg_Update(clkDev->regmap.base, pllRefdivReg, OSCCTRL_PLLxREFDIV_Msk,
                    whal_SetBits(OSCCTRL_PLLxREFDIV_Msk, OSCCTRL_PLLxREFDIV_Pos,
                                 cfg->refDiv));
    for (i = 0; i < cfg->outCfgCount; i++) {
        const whal_Pic32cz_Clock_PllOutCfg *out = &cfg->outCfg[i];
        whal_Reg_Update(clkDev->regmap.base, pllPostdivaReg,
                        out->outEnMask | out->postDivMask,
                        whal_SetBits(out->postDivMask, out->postDivPos, out->postDiv) |
                        whal_SetBits(out->outEnMask, out->outEnPos, 1));
    }

    whal_Reg_Update(clkDev->regmap.base, pllCtrlReg,
                    OSCCTRL_PLLxCTRL_ENABLE_Msk |
                    OSCCTRL_PLLxCTRL_REFSEL_Msk |
                    OSCCTRL_PLLxCTRL_BWSEL_Msk,
                    OSCCTRL_PLLxCTRL_ENABLE_Msk |
                    whal_SetBits(OSCCTRL_PLLxCTRL_REFSEL_Msk, OSCCTRL_PLLxCTRL_REFSEL_Pos, cfg->refSel) |
                    whal_SetBits(OSCCTRL_PLLxCTRL_BWSEL_Msk, OSCCTRL_PLLxCTRL_BWSEL_Pos, cfg->bwSel));

    do {
        whal_Reg_Get(clkDev->regmap.base, OSCCTRL_STATUS_REG,
                     OSCCTRL_STATUS_PLLxLOCK_Msk(cfg->pllInst),
                     OSCCTRL_STATUS_PLLxLOCK_Pos(cfg->pllInst), &status);
    } while (!status);
    return WHAL_SUCCESS;
}

whal_Error whal_Pic32cz_Clock_EnableGclkGen(whal_Clock *clkDev,
                                           const whal_Pic32cz_Clock_GenCfg *cfg)
{
    size_t status;

    if (!clkDev || !cfg)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, GCLK_GENCTRLx_REG(cfg->gen),
                    GCLK_GENCTRLx_SRC_Msk | GCLK_GENCTRLx_GENEN_Msk |
                    GCLK_GENCTRLx_DIV_Msk,
                    whal_SetBits(GCLK_GENCTRLx_SRC_Msk, GCLK_GENCTRLx_SRC_Pos, cfg->genSrc) |
                    GCLK_GENCTRLx_GENEN_Msk |
                    whal_SetBits(GCLK_GENCTRLx_DIV_Msk, GCLK_GENCTRLx_DIV_Pos, cfg->genDiv));
    do {
        whal_Reg_Get(clkDev->regmap.base, GCLK_SYNCBUSY_REG,
                     GCLK_SYNCBUSY_GENCTRLx_Msk(cfg->gen),
                     GCLK_SYNCBUSY_GENCTRLx_Pos(cfg->gen), &status);
    } while (status);
    return WHAL_SUCCESS;
}

whal_Error whal_Pic32cz_Clock_SetMclkDiv(whal_Clock *clkDev, uint8_t div)
{
    size_t status;

    if (!clkDev)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, MCLK_DIV1_REG, MCLK_DIV1_Msk,
                    whal_SetBits(MCLK_DIV1_Msk, MCLK_DIV1_Pos, div));
    do {
        whal_Reg_Get(clkDev->regmap.base, MCLK_INTFLAG_REG,
                     MCLK_INTFLAG_CKRDY_Msk, MCLK_INTFLAG_CKRDY_Pos, &status);
    } while (!status);
    return WHAL_SUCCESS;
}

whal_Error whal_Pic32cz_Clock_EnablePeriphClk(whal_Clock *clkDev,
                                             const whal_Pic32cz_Clock_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, GCLK_PCHCTRLx_REG(clk->gclkPeriphChannel),
                    GCLK_PCHCTRLx_GEN_Msk | GCLK_PCHCTRLx_CHEN_Msk,
                    whal_SetBits(GCLK_PCHCTRLx_GEN_Msk, GCLK_PCHCTRLx_GEN_Pos, clk->gclkPeriphSrc) |
                    GCLK_PCHCTRLx_CHEN_Msk);
    whal_Reg_Update(clkDev->regmap.base, MCLK_CLKxMSK_REG(clk->mclkEnableInst),
                    clk->mclkEnableMask,
                    whal_SetBits(clk->mclkEnableMask, clk->mclkEnablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Pic32cz_Clock_DisablePeriphClk(whal_Clock *clkDev,
                                              const whal_Pic32cz_Clock_PeriphClk *clk)
{
    if (!clkDev || !clk)
        return WHAL_EINVAL;

    whal_Reg_Update(clkDev->regmap.base, MCLK_CLKxMSK_REG(clk->mclkEnableInst),
                    clk->mclkEnableMask,
                    whal_SetBits(clk->mclkEnableMask, clk->mclkEnablePos, 0));
    whal_Reg_Update(clkDev->regmap.base, GCLK_PCHCTRLx_REG(clk->gclkPeriphChannel),
                    GCLK_PCHCTRLx_CHEN_Msk, 0);
    return WHAL_SUCCESS;
}
