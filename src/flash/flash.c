/* flash.c
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
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Flash_Init(whal_Flash *flashDev)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Init)
        return WHAL_ENOTSUP;

    return flashDev->driver->Init(flashDev);
}

inline whal_Error whal_Flash_Deinit(whal_Flash *flashDev)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return flashDev->driver->Deinit(flashDev);
}

inline whal_Error whal_Flash_Lock(whal_Flash *flashDev, size_t addr, size_t len)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Lock)
        return WHAL_ENOTSUP;

    return flashDev->driver->Lock(flashDev, addr, len);
}

inline whal_Error whal_Flash_Unlock(whal_Flash *flashDev, size_t addr, size_t len)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Unlock)
        return WHAL_ENOTSUP;

    return flashDev->driver->Unlock(flashDev, addr, len);
}

inline whal_Error whal_Flash_Read(whal_Flash *flashDev, size_t addr, void *data,
                                  size_t dataSz)
{
    if (!flashDev || !data)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Read)
        return WHAL_ENOTSUP;

    return flashDev->driver->Read(flashDev, addr, data, dataSz);
}

inline whal_Error whal_Flash_Write(whal_Flash *flashDev, size_t addr, const void *data,
                                  size_t dataSz)
{
    if (!flashDev || !data)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Write)
        return WHAL_ENOTSUP;

    return flashDev->driver->Write(flashDev, addr, data, dataSz);
}

inline whal_Error whal_Flash_Erase(whal_Flash *flashDev, size_t addr,
                                  size_t dataSz)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Erase)
        return WHAL_ENOTSUP;

    return flashDev->driver->Erase(flashDev, addr, dataSz);
}
