/* stm32wb_lptim_pwm.c
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
#include <wolfHAL/pwm/stm32wb_lptim_pwm.h>
#include <wolfHAL/pwm/pwm.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>

/*
 * STM32WB LPTIM Register Definitions (RM0434 Rev 14, Section 27.7)
 */
#define LPTIM_ISR_REG   0x00
#define LPTIM_ISR_CMPOK_Pos  3                                  /* Compare register update OK */
#define LPTIM_ISR_CMPOK_Msk  (1UL << LPTIM_ISR_CMPOK_Pos)
#define LPTIM_ISR_ARROK_Pos  4                                  /* Autoreload register update OK */
#define LPTIM_ISR_ARROK_Msk  (1UL << LPTIM_ISR_ARROK_Pos)

#define LPTIM_ICR_REG   0x04
#define LPTIM_ICR_CMPOKCF_Msk  (1UL << 3)                       /* Clear CMPOK */
#define LPTIM_ICR_ARROKCF_Msk  (1UL << 4)                       /* Clear ARROK */

#define LPTIM_CFGR_REG  0x0C
#define LPTIM_CFGR_CKSEL_Pos   0                                /* Clock source (0 = internal) */
#define LPTIM_CFGR_CKSEL_Msk   (1UL << LPTIM_CFGR_CKSEL_Pos)
#define LPTIM_CFGR_PRESC_Pos   9                                /* Clock prescaler */
#define LPTIM_CFGR_PRESC_Msk   (WHAL_BITMASK(3) << LPTIM_CFGR_PRESC_Pos)
#define LPTIM_CFGR_WAVE_Msk    (1UL << 20)                      /* Waveform shape (0 = PWM) */
#define LPTIM_CFGR_WAVPOL_Msk  (1UL << 21)                      /* Waveform polarity */

#define LPTIM_CR_REG    0x10
#define LPTIM_CR_ENABLE_Msk    (1UL << 0)                       /* LPTIM enable */
#define LPTIM_CR_CNTSTRT_Msk   (1UL << 2)                       /* Continuous-mode start */

#define LPTIM_CMP_REG   0x14                                    /* Compare register (16-bit) */
#define LPTIM_ARR_REG   0x18                                    /* Autoreload register (16-bit) */

/* The autoreload and compare registers are 16-bit, so the period
 * (ARR + 1) can be at most 65536 ticks. */
#define LPTIM_PERIOD_MAX  0x10000UL

whal_Error whal_Stm32wb_Lptim_Pwm_Init(whal_Pwm *dev)
{
    size_t base;
    whal_Stm32wb_Lptim_Pwm_Cfg *cfg;
    uint32_t cfgr;

    if (!dev || !dev->cfg) {
        return WHAL_EINVAL;
    }

    base = dev->base;
    cfg = (whal_Stm32wb_Lptim_Pwm_Cfg *)dev->cfg;

    /* LPTIM_CFGR is writable only while the timer is disabled. Apply the
     * instance-static settings here: clock source (CKSEL), prescaler, and
     * PWM waveform (WAVE = 0). The per-call polarity and the ARR/CMP waveform
     * are programmed by Start. The board owns the LPTIM kernel clock. */
    whal_Reg_Write(base, LPTIM_CR_REG, 0);
    cfgr = whal_SetBits(LPTIM_CFGR_PRESC_Msk, LPTIM_CFGR_PRESC_Pos,
                        cfg->prescaler) |
           whal_SetBits(LPTIM_CFGR_CKSEL_Msk, LPTIM_CFGR_CKSEL_Pos,
                        cfg->clkSel);
    whal_Reg_Write(base, LPTIM_CFGR_REG, cfgr);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Lptim_Pwm_Deinit(whal_Pwm *dev)
{
    if (!dev) {
        return WHAL_EINVAL;
    }

    /* Disable the timer and clear its configuration. The board owns the
     * LPTIM kernel clock, so clock gating is left to it. */
    whal_Reg_Write(dev->base, LPTIM_CR_REG, 0);
    whal_Reg_Write(dev->base, LPTIM_CFGR_REG, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Lptim_Pwm_Start(whal_Pwm *dev, uint8_t channel,
                                        const whal_Pwm_ChannelCfg *channelCfg)
{
    size_t base;
    whal_Stm32wb_Lptim_Pwm_Cfg *cfg;
    uint32_t arr, cmp;
    whal_Error err;

    if (!dev || !dev->cfg || !channelCfg) {
        return WHAL_EINVAL;
    }
    if (channel != WHAL_STM32WB_LPTIM_PWM_CHANNEL) {
        return WHAL_ENOTSUP;
    }

    base = dev->base;
    cfg = (whal_Stm32wb_Lptim_Pwm_Cfg *)dev->cfg;

    /* The LPTIM has no pulse counter, so only continuous output is
     * supported; reject any finite pulse count. The 16-bit ARR also bounds
     * the period. */
    if (channelCfg->periodCycles == 0 ||
        channelCfg->periodCycles > LPTIM_PERIOD_MAX ||
        channelCfg->pulseCount != WHAL_PWM_PULSE_COUNT_CONTINUOUS) {
        return WHAL_ENOTSUP;
    }

    /* Counter runs 0..ARR, so a period of N ticks needs ARR = N - 1. The
     * LPTIM output is high while CNT > CMP and is cleared on the ARR match,
     * so the high time is (ARR - CMP) ticks; invert the pulse width into a
     * compare value, clamping the full-on case (where pulseCycles equals
     * periodCycles would underflow) to CMP = 0. */
    arr = channelCfg->periodCycles - 1;
    if (channelCfg->pulseCycles >= channelCfg->periodCycles) {
        cmp = 0;
    } else {
        cmp = arr - channelCfg->pulseCycles;
    }

    /* Disable the timer so CFGR/ARR/CMP are writable, then set this call's
     * output polarity. The clock source, prescaler, and waveform mode were
     * set by Init; WAVPOL is updated in place to preserve them. */
    whal_Reg_Write(base, LPTIM_CR_REG, 0);
    whal_Reg_Update(base, LPTIM_CFGR_REG, LPTIM_CFGR_WAVPOL_Msk,
                    channelCfg->polarity != WHAL_PWM_POLARITY_NORMAL ?
                        LPTIM_CFGR_WAVPOL_Msk : 0);

    /* ARR and CMP are only writable once the timer is enabled, and each
     * write crosses into the LPTIM clock domain and must be acknowledged by
     * ARROK/CMPOK before the next access. Clear any stale flags first. */
    whal_Reg_Update(base, LPTIM_CR_REG, LPTIM_CR_ENABLE_Msk,
                    LPTIM_CR_ENABLE_Msk);
    whal_Reg_Write(base, LPTIM_ICR_REG,
                   LPTIM_ICR_ARROKCF_Msk | LPTIM_ICR_CMPOKCF_Msk);

    whal_Reg_Write(base, LPTIM_ARR_REG, arr);
    err = whal_Reg_ReadPoll(base, LPTIM_ISR_REG, LPTIM_ISR_ARROK_Msk,
                            LPTIM_ISR_ARROK_Msk, cfg->timeout);
    if (err != WHAL_SUCCESS) {
        return err;
    }

    whal_Reg_Write(base, LPTIM_CMP_REG, cmp);
    err = whal_Reg_ReadPoll(base, LPTIM_ISR_REG, LPTIM_ISR_CMPOK_Msk,
                            LPTIM_ISR_CMPOK_Msk, cfg->timeout);
    if (err != WHAL_SUCCESS) {
        return err;
    }

    /* Waveform programmed; begin continuous counting. */
    whal_Reg_Update(base, LPTIM_CR_REG, LPTIM_CR_CNTSTRT_Msk,
                    LPTIM_CR_CNTSTRT_Msk);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Lptim_Pwm_Stop(whal_Pwm *dev, uint8_t channel)
{
    if (!dev) {
        return WHAL_EINVAL;
    }
    if (channel != WHAL_STM32WB_LPTIM_PWM_CHANNEL) {
        return WHAL_ENOTSUP;
    }

    /* Clearing ENABLE stops the counter and parks the output; Start
     * reprograms the waveform and resumes. */
    whal_Reg_Update(dev->base, LPTIM_CR_REG, LPTIM_CR_ENABLE_Msk, 0);

    return WHAL_SUCCESS;
}

const whal_PwmDriver whal_Stm32wb_Lptim_Pwm_Driver = {
    .Init   = whal_Stm32wb_Lptim_Pwm_Init,
    .Deinit = whal_Stm32wb_Lptim_Pwm_Deinit,
    .Start  = whal_Stm32wb_Lptim_Pwm_Start,
    .Stop   = whal_Stm32wb_Lptim_Pwm_Stop,
};
