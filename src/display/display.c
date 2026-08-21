/* display.c
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

#include <wolfHAL/display/display.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Display_Init(whal_Display *dev)
{
    if (!dev || !dev->driver || !dev->driver->Init) {
        return WHAL_EINVAL;
    }

    return dev->driver->Init(dev);
}

inline whal_Error whal_Display_Deinit(whal_Display *dev)
{
    if (!dev || !dev->driver || !dev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return dev->driver->Deinit(dev);
}

inline whal_Error whal_Display_Update(whal_Display *dev, uint16_t x, uint16_t y,
                                      uint16_t w, uint16_t h,
                                      const void *data, size_t dataSz)
{
    if (!dev || !dev->driver || !dev->driver->Update ||
        !data || dataSz == 0 || w == 0 || h == 0) {
        return WHAL_EINVAL;
    }

    return dev->driver->Update(dev, x, y, w, h, data, dataSz);
}
