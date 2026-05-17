/* timer.c
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

#include <wolfHAL/timer/timer.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>

inline whal_Error whal_Timer_Init(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Init)
        return WHAL_ENOTSUP;

    return timerDev->driver->Init(timerDev);
}

inline whal_Error whal_Timer_Deinit(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return timerDev->driver->Deinit(timerDev);
}

inline whal_Error whal_Timer_Start(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Start)
        return WHAL_ENOTSUP;

    return timerDev->driver->Start(timerDev);
}

inline whal_Error whal_Timer_Stop(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Stop)
        return WHAL_ENOTSUP;

    return timerDev->driver->Stop(timerDev);
}

inline whal_Error whal_Timer_Reset(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Reset)
        return WHAL_ENOTSUP;

    return timerDev->driver->Reset(timerDev);
}
