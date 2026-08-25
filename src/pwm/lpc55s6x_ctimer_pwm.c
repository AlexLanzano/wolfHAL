/* lpc55s6x_ctimer_pwm.c
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include <stdint.h>
#ifdef WHAL_CFG_LPC55S6X_CTIMER_PWM_SINGLE_INSTANCE
#include "board.h"  /* provides whal_Lpc55s6x_Ctimer_Pwm_Dev device instance (possibly via platform alias macro) */
#endif
#include <wolfHAL/pwm/lpc55s6x_ctimer_pwm.h>
#include <wolfHAL/pwm/pwm.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>

/* CTIMER registers (offsets within the CTIMER instance base). */
#define CTIMER_TCR_REG      0x04
#define CTIMER_TCR_CEN_Msk  (1UL << 0)
#define CTIMER_TCR_CRST_Msk (1UL << 1)

#define CTIMER_PR_REG 0x0C

#define CTIMER_MCR_REG      0x14
#define CTIMER_MCR_MR3R_Msk (1UL << 10)

#define CTIMER_MR0_REG 0x18
#define CTIMER_MR3_REG 0x24

#define CTIMER_PWMC_REG       0x74
#define CTIMER_PWMC_PWMEN_Msk (WHAL_BITMASK(3) << 0)

/* Three PWM outputs (MAT0..2 via MR0..2); MR3 sets the shared period. */
#define CTIMER_PWM_NUM_CHANNELS 3

#ifdef WHAL_CFG_LPC55S6X_CTIMER_PWM_DIRECT_API_MAPPING
#define whal_Lpc55s6x_Ctimer_Pwm_Init   whal_Pwm_Init
#define whal_Lpc55s6x_Ctimer_Pwm_Deinit whal_Pwm_Deinit
#define whal_Lpc55s6x_Ctimer_Pwm_Start  whal_Pwm_Start
#define whal_Lpc55s6x_Ctimer_Pwm_Stop   whal_Pwm_Stop
#endif /* WHAL_CFG_LPC55S6X_CTIMER_PWM_DIRECT_API_MAPPING */

#ifdef WHAL_CFG_LPC55S6X_CTIMER_PWM_SINGLE_INSTANCE
const whal_Pwm whal_Lpc55s6x_Ctimer_Pwm_Dev = WHAL_CFG_LPC55S6X_CTIMER_PWM_DEV;
#endif

whal_Error whal_Lpc55s6x_Ctimer_Pwm_Init(whal_Pwm *dev)
{
#ifdef WHAL_CFG_LPC55S6X_CTIMER_PWM_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Ctimer_Pwm_Dev.base;
    whal_Lpc55s6x_Ctimer_Pwm_Cfg *devCfg =
        (whal_Lpc55s6x_Ctimer_Pwm_Cfg *)whal_Lpc55s6x_Ctimer_Pwm_Dev.cfg;
    (void)dev;
#else
    size_t base;
    whal_Lpc55s6x_Ctimer_Pwm_Cfg *devCfg;

    if (!dev || !dev->cfg)
        return WHAL_EINVAL;

    base = dev->base;
    devCfg = (whal_Lpc55s6x_Ctimer_Pwm_Cfg *)dev->cfg;
#endif

    whal_Reg_Write(base, CTIMER_TCR_REG, CTIMER_TCR_CRST_Msk);
    whal_Reg_Write(base, CTIMER_PR_REG, devCfg->prescaler);
    whal_Reg_Write(base, CTIMER_MCR_REG, CTIMER_MCR_MR3R_Msk);
    whal_Reg_Write(base, CTIMER_TCR_REG, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Ctimer_Pwm_Deinit(whal_Pwm *dev)
{
#ifdef WHAL_CFG_LPC55S6X_CTIMER_PWM_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Ctimer_Pwm_Dev.base;
    (void)dev;
#else
    size_t base;

    if (!dev)
        return WHAL_EINVAL;

    base = dev->base;
#endif

    whal_Reg_Write(base, CTIMER_TCR_REG, 0);
    whal_Reg_Write(base, CTIMER_PWMC_REG, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Ctimer_Pwm_Start(whal_Pwm *dev, uint8_t channel,
                                          const whal_Pwm_ChannelCfg *cfg)
{
    uint32_t mr3, mrx, pwmc;
    size_t mrReg;
#ifdef WHAL_CFG_LPC55S6X_CTIMER_PWM_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Ctimer_Pwm_Dev.base;
    (void)dev;

    if (!cfg)
        return WHAL_EINVAL;
#else
    size_t base;

    if (!dev || !cfg)
        return WHAL_EINVAL;

    base = dev->base;
#endif

    if (cfg->periodCycles == 0 || cfg->pulseCycles > cfg->periodCycles)
        return WHAL_EINVAL;

    if (channel >= CTIMER_PWM_NUM_CHANNELS ||
        cfg->polarity != WHAL_PWM_POLARITY_NORMAL ||
        cfg->pulseCount != WHAL_PWM_PULSE_COUNT_CONTINUOUS)
        return WHAL_ENOTSUP;

#ifdef WHAL_CFG_64BIT_TICK
    if (cfg->periodCycles > 0xFFFFFFFFULL)
        return WHAL_ENOTSUP;
#endif

    /* Output is LOW until MRx, then HIGH to the period end, so the asserted
     * width is (period - MRx). */
    mr3 = (uint32_t)(cfg->periodCycles - 1);
    mrx = (uint32_t)(cfg->periodCycles - cfg->pulseCycles);
    mrReg = CTIMER_MR0_REG + (size_t)channel * 4;

    pwmc = (uint32_t)whal_Reg_Read(base, CTIMER_PWMC_REG);

    if ((pwmc & CTIMER_PWMC_PWMEN_Msk & ~(1UL << channel)) != 0) {
        /* Another channel already owns the shared period; it must match. */
        if ((uint32_t)whal_Reg_Read(base, CTIMER_MR3_REG) != mr3)
            return WHAL_ENOTSUP;

        whal_Reg_Write(base, mrReg, mrx);
        whal_Reg_Update(base, CTIMER_PWMC_REG, 1UL << channel, 1UL << channel);
    } else {
        whal_Reg_Write(base, CTIMER_TCR_REG, CTIMER_TCR_CRST_Msk);
        whal_Reg_Write(base, CTIMER_MR3_REG, mr3);
        whal_Reg_Write(base, mrReg, mrx);
        whal_Reg_Update(base, CTIMER_PWMC_REG, 1UL << channel, 1UL << channel);
        whal_Reg_Write(base, CTIMER_TCR_REG, CTIMER_TCR_CEN_Msk);
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Ctimer_Pwm_Stop(whal_Pwm *dev, uint8_t channel)
{
#ifdef WHAL_CFG_LPC55S6X_CTIMER_PWM_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Ctimer_Pwm_Dev.base;
    (void)dev;
#else
    size_t base;

    if (!dev)
        return WHAL_EINVAL;

    base = dev->base;
#endif

    if (channel >= CTIMER_PWM_NUM_CHANNELS)
        return WHAL_ENOTSUP;

    whal_Reg_Update(base, CTIMER_PWMC_REG, 1UL << channel, 0);

    if ((whal_Reg_Read(base, CTIMER_PWMC_REG) & CTIMER_PWMC_PWMEN_Msk) == 0)
        whal_Reg_Write(base, CTIMER_TCR_REG, 0);

    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_LPC55S6X_CTIMER_PWM_DIRECT_API_MAPPING
const whal_PwmDriver whal_Lpc55s6x_Ctimer_Pwm_Driver = {
    .Init = whal_Lpc55s6x_Ctimer_Pwm_Init,
    .Deinit = whal_Lpc55s6x_Ctimer_Pwm_Deinit,
    .Start = whal_Lpc55s6x_Ctimer_Pwm_Start,
    .Stop = whal_Lpc55s6x_Ctimer_Pwm_Stop,
};
#endif /* !WHAL_CFG_LPC55S6X_CTIMER_PWM_DIRECT_API_MAPPING */
