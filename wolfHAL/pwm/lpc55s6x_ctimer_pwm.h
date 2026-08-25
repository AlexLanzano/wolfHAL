/* lpc55s6x_ctimer_pwm.h
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

#ifndef WHAL_LPC55S6X_CTIMER_PWM_H
#define WHAL_LPC55S6X_CTIMER_PWM_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/pwm/pwm.h>
#include <wolfHAL/timeout.h>

/*
 * @file lpc55s6x_ctimer_pwm.h
 * @brief LPC55S6x CTIMER-based PWM driver configuration.
 */

/*
 * @brief LPC55S6x CTIMER PWM configuration parameters.
 */
typedef struct whal_Lpc55s6x_Ctimer_Pwm_Cfg {
    uint32_t prescaler;
    uint8_t clkSel;
    whal_Timeout *timeout;
} whal_Lpc55s6x_Ctimer_Pwm_Cfg;

/*
 * @brief Single-instance device struct. Defined in the driver TU
 * from the WHAL_CFG_LPC55S6X_CTIMER_PWM_DEV initializer in board.h.
 */
#if defined(WHAL_CFG_LPC55S6X_CTIMER_PWM_SINGLE_INSTANCE)
extern const whal_Pwm whal_Lpc55s6x_Ctimer_Pwm_Dev;
#endif

#ifndef WHAL_CFG_LPC55S6X_CTIMER_PWM_DIRECT_API_MAPPING
/*
 * @brief Driver instance for LPC55S6x CTIMER PWM peripheral.
 */
extern const whal_PwmDriver whal_Lpc55s6x_Ctimer_Pwm_Driver;

/*
 * @brief Initialize the LPC55S6x PWM (CTIMER) peripheral.
 *
 * @param dev PWM device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Ctimer_Pwm_Init(whal_Pwm *dev);

/*
 * @brief Deinitialize the LPC55S6x PWM (CTIMER) peripheral.
 *
 * @param dev PWM device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Ctimer_Pwm_Deinit(whal_Pwm *dev);

/*
 * @brief Apply a waveform to a channel and start PWM output.
 *
 * @param dev     PWM device instance.
 * @param channel Zero-based channel to start.
 * @param cfg     Waveform (period, pulse, polarity) to apply.
 *
 * @retval WHAL_SUCCESS Output started.
 * @retval WHAL_EINVAL  Invalid arguments.
 * @retval WHAL_ENOTSUP Channel or waveform unsupported.
 */
whal_Error whal_Lpc55s6x_Ctimer_Pwm_Start(whal_Pwm *dev, uint8_t channel,
                                          const whal_Pwm_ChannelCfg *cfg);

/*
 * @brief Stop PWM output on a channel.
 *
 * @param dev     PWM device instance.
 * @param channel Zero-based channel to stop.
 *
 * @retval WHAL_SUCCESS Output stopped.
 * @retval WHAL_EINVAL  Invalid arguments.
 * @retval WHAL_ENOTSUP Channel unsupported.
 */
whal_Error whal_Lpc55s6x_Ctimer_Pwm_Stop(whal_Pwm *dev, uint8_t channel);
#endif /* !WHAL_CFG_LPC55S6X_CTIMER_PWM_DIRECT_API_MAPPING */

#endif /* WHAL_LPC55S6X_CTIMER_PWM_H */
