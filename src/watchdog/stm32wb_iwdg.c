#include <wolfHAL/watchdog/stm32wb_iwdg.h>
#include <wolfHAL/watchdog/watchdog.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>

/*
 * STM32WB IWDG Register Definitions
 *
 * The IWDG is a free-running downcounter clocked from the LSI (~32 kHz).
 * Once started via the key register, it cannot be stopped. If the counter
 * reaches zero without being refreshed, a system reset is generated.
 */

/* Key Register — write-only, controls access and refresh */
#define IWDG_KR_REG     0x00
#define IWDG_KEY_START  0xCCCC  /* Start the watchdog */
#define IWDG_KEY_RELOAD 0xAAAA  /* Reload the counter */
#define IWDG_KEY_ACCESS 0x5555  /* Enable write access to PR, RLR, WINR */

/* Prescaler Register */
#define IWDG_PR_REG     0x04
#define IWDG_PR_Pos     0
#define IWDG_PR_Msk     (0x7UL << IWDG_PR_Pos)

/* Reload Register */
#define IWDG_RLR_REG    0x08
#define IWDG_RLR_Pos    0
#define IWDG_RLR_Msk    (0xFFFUL << IWDG_RLR_Pos)

/* Status Register */
#define IWDG_SR_REG     0x0C
#define IWDG_SR_PVU_Pos 0       /* Prescaler value update */
#define IWDG_SR_PVU_Msk (1UL << IWDG_SR_PVU_Pos)
#define IWDG_SR_RVU_Pos 1       /* Reload value update */
#define IWDG_SR_RVU_Msk (1UL << IWDG_SR_RVU_Pos)

#ifdef WHAL_CFG_WATCHDOG_API_MAPPING_STM32WB_IWDG
#define whal_Stm32wbIwdg_Init    whal_Watchdog_Init
#define whal_Stm32wbIwdg_Deinit  whal_Watchdog_Deinit
#define whal_Stm32wbIwdg_Refresh whal_Watchdog_Refresh
#endif /* WHAL_CFG_WATCHDOG_API_MAPPING_STM32WB_IWDG */

whal_Error whal_Stm32wbIwdg_Init(whal_Watchdog *wdgDev)
{
    const whal_Regmap *reg;
    whal_Stm32wbIwdg_Cfg *cfg;
    whal_Error err;

    if (!wdgDev || !wdgDev->cfg) {
        return WHAL_EINVAL;
    }

    reg = &wdgDev->regmap;
    cfg = wdgDev->cfg;

    if (cfg->prescaler > 6 || cfg->reload > 0xFFF) {
        return WHAL_EINVAL;
    }

    /* Start the IWDG */
    whal_Reg_Write(reg->base, IWDG_KR_REG, IWDG_KEY_START);

    /* Enable register access */
    whal_Reg_Write(reg->base, IWDG_KR_REG, IWDG_KEY_ACCESS);

    /* Set prescaler */
    whal_Reg_Write(reg->base, IWDG_PR_REG, cfg->prescaler);

    /* Set reload value */
    whal_Reg_Write(reg->base, IWDG_RLR_REG, cfg->reload);

    /* Wait for registers to update */
    err = whal_Reg_ReadPoll(reg->base, IWDG_SR_REG,
                            IWDG_SR_PVU_Msk | IWDG_SR_RVU_Msk, 0,
                            cfg->timeout);
    if (err)
        return err;

    /* Refresh counter with new reload value */
    whal_Reg_Write(reg->base, IWDG_KR_REG, IWDG_KEY_RELOAD);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbIwdg_Deinit(whal_Watchdog *wdgDev)
{
    if (!wdgDev) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbIwdg_Refresh(whal_Watchdog *wdgDev)
{
    if (!wdgDev) {
        return WHAL_EINVAL;
    }

    whal_Reg_Write(wdgDev->regmap.base, IWDG_KR_REG, IWDG_KEY_RELOAD);

    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_WATCHDOG_API_MAPPING_STM32WB_IWDG
const whal_WatchdogDriver whal_Stm32wbIwdg_Driver = {
    .Init = whal_Stm32wbIwdg_Init,
    .Deinit = whal_Stm32wbIwdg_Deinit,
    .Refresh = whal_Stm32wbIwdg_Refresh,
};
#endif /* !WHAL_CFG_WATCHDOG_API_MAPPING_STM32WB_IWDG */
