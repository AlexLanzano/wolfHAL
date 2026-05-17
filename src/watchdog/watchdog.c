/* watchdog.c
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

#include <wolfHAL/watchdog/watchdog.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Watchdog_Init(whal_Watchdog *wdgDev)
{
    if (!wdgDev || !wdgDev->driver || !wdgDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return wdgDev->driver->Init(wdgDev);
}

inline whal_Error whal_Watchdog_Deinit(whal_Watchdog *wdgDev)
{
    if (!wdgDev || !wdgDev->driver || !wdgDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return wdgDev->driver->Deinit(wdgDev);
}

inline whal_Error whal_Watchdog_Refresh(whal_Watchdog *wdgDev)
{
    if (!wdgDev || !wdgDev->driver || !wdgDev->driver->Refresh) {
        return WHAL_EINVAL;
    }

    return wdgDev->driver->Refresh(wdgDev);
}
