#include <wolfHAL/watchdog/stm32wb_wwdg.h>
#include <wolfHAL/watchdog/watchdog.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define CR_REG 0x00

#define CR_T_Pos 0
#define CR_T_Msk (WHAL_BITMASK(7) << CR_T_Pos)

#define CR_WDGA_Pos 7
#define CR_WDGA_Msk (1UL << CR_WDGA_Pos)


#define CFR_REG 0x04

#define CFR_W_Pos 0
#define CFR_W_Msk (WHAL_BITMASK(7) << CFR_W_Pos)

#define CFR_EWI_Pos 9
#define CFR_EWI_Msk (1UL << CFR_EWI_Pos)

#define CFR_WDGTB_Pos 11
#define CFR_WDGTB_Msk (WHAL_BITMASK(3) << CFR_WDGTB_Pos)

whal_Error whal_Stm32wbWwdg_Init(whal_Watchdog *wdgDev)
{
    size_t base;
    whal_Stm32wbWwdg_Cfg *cfg;

    if (!wdgDev || !wdgDev->cfg)
        return WHAL_EINVAL;

    base = wdgDev->regmap.base;
    cfg = wdgDev->cfg;

    if (cfg->prescaler > 7 || cfg->window > 0x7F || cfg->counter > 0x7F)
        return WHAL_EINVAL;

    /* Configure window and prescaler */
    whal_Reg_Update(base, CFR_REG, CFR_W_Msk | CFR_WDGTB_Msk,
                    whal_SetBits(CFR_W_Msk, CFR_W_Pos, cfg->window) |
                    whal_SetBits(CFR_WDGTB_Msk, CFR_WDGTB_Pos, cfg->prescaler));

    /* Set counter and enable WWDG */
    whal_Reg_Update(base, CR_REG, CR_T_Msk | CR_WDGA_Msk,
                    whal_SetBits(CR_T_Msk, CR_T_Pos, cfg->counter) |
                    whal_SetBits(CR_WDGA_Msk, CR_WDGA_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbWwdg_Deinit(whal_Watchdog *wdgDev)
{
    (void)wdgDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbWwdg_Refresh(whal_Watchdog *wdgDev)
{
    size_t base;
    whal_Stm32wbWwdg_Cfg *cfg;

    if (!wdgDev || !wdgDev->cfg)
        return WHAL_EINVAL;

    base = wdgDev->regmap.base;
    cfg = wdgDev->cfg;

    whal_Reg_Update(base, CR_REG, CR_T_Msk,
                    whal_SetBits(CR_T_Msk, CR_T_Pos, cfg->counter));

    return WHAL_SUCCESS;
}

const whal_WatchdogDriver whal_Stm32wbWwdg_Driver = {
    .Init = whal_Stm32wbWwdg_Init,
    .Deinit = whal_Stm32wbWwdg_Deinit,
    .Refresh = whal_Stm32wbWwdg_Refresh,
};
