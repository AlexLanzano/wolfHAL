/* block.c
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
#include <wolfHAL/block/block.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Block_Init(whal_Block *blockDev)
{
    if (!blockDev)
        return WHAL_EINVAL;
    if (!blockDev->driver || !blockDev->driver->Init)
        return WHAL_ENOTSUP;

    return blockDev->driver->Init(blockDev);
}

inline whal_Error whal_Block_Deinit(whal_Block *blockDev)
{
    if (!blockDev)
        return WHAL_EINVAL;
    if (!blockDev->driver || !blockDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return blockDev->driver->Deinit(blockDev);
}

inline whal_Error whal_Block_Read(whal_Block *blockDev, uint32_t block,
                                  void *data, uint32_t blockCount)
{
    if (!blockDev || !data)
        return WHAL_EINVAL;
    if (!blockDev->driver || !blockDev->driver->Read)
        return WHAL_ENOTSUP;

    return blockDev->driver->Read(blockDev, block, data, blockCount);
}

inline whal_Error whal_Block_Write(whal_Block *blockDev, uint32_t block,
                                   const void *data, uint32_t blockCount)
{
    if (!blockDev || !data)
        return WHAL_EINVAL;
    if (!blockDev->driver || !blockDev->driver->Write)
        return WHAL_ENOTSUP;

    return blockDev->driver->Write(blockDev, block, data, blockCount);
}

inline whal_Error whal_Block_Erase(whal_Block *blockDev, uint32_t block,
                                   uint32_t blockCount)
{
    if (!blockDev)
        return WHAL_EINVAL;
    if (!blockDev->driver || !blockDev->driver->Erase)
        return WHAL_ENOTSUP;

    return blockDev->driver->Erase(blockDev, block, blockCount);
}
