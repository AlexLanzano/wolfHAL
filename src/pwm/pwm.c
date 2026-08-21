/* pwm.c
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

#include <wolfHAL/pwm/pwm.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Pwm_Init(whal_Pwm *dev)
{
    if (!dev || !dev->driver || !dev->driver->Init) {
        return WHAL_EINVAL;
    }

    return dev->driver->Init(dev);
}

inline whal_Error whal_Pwm_Deinit(whal_Pwm *dev)
{
    if (!dev || !dev->driver || !dev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return dev->driver->Deinit(dev);
}

inline whal_Error whal_Pwm_Start(whal_Pwm *dev, uint8_t channel,
                                 const whal_Pwm_ChannelCfg *cfg)
{
    if (!dev || !dev->driver || !dev->driver->Start || !cfg ||
        cfg->pulseCycles > cfg->periodCycles) {
        return WHAL_EINVAL;
    }

    return dev->driver->Start(dev, channel, cfg);
}

inline whal_Error whal_Pwm_Stop(whal_Pwm *dev, uint8_t channel)
{
    if (!dev || !dev->driver || !dev->driver->Stop) {
        return WHAL_EINVAL;
    }

    return dev->driver->Stop(dev, channel);
}
