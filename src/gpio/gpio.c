/* gpio.c
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

#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Gpio_Init(whal_Gpio *gpioDev)
{
    if (!gpioDev)
        return WHAL_EINVAL;
    if (!gpioDev->driver || !gpioDev->driver->Init)
        return WHAL_ENOTSUP;

    return gpioDev->driver->Init(gpioDev);
}

inline whal_Error whal_Gpio_Deinit(whal_Gpio *gpioDev)
{
    if (!gpioDev)
        return WHAL_EINVAL;
    if (!gpioDev->driver || !gpioDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return gpioDev->driver->Deinit(gpioDev);
}

inline whal_Error whal_Gpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value)
{
    if (!gpioDev || !value)
        return WHAL_EINVAL;
    if (!gpioDev->driver || !gpioDev->driver->Get)
        return WHAL_ENOTSUP;

    return gpioDev->driver->Get(gpioDev, pin, value);
}

inline whal_Error whal_Gpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value)
{
    if (!gpioDev)
        return WHAL_EINVAL;
    if (!gpioDev->driver || !gpioDev->driver->Set)
        return WHAL_ENOTSUP;

    return gpioDev->driver->Set(gpioDev, pin, value);
}
