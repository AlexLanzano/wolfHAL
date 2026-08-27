/* sdhc.c
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
#include <wolfHAL/sdhc/sdhc.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Sdhc_Init(whal_Sdhc *dev)
{
    if (!dev)
        return WHAL_EINVAL;
    if (!dev->driver || !dev->driver->Init)
        return WHAL_ENOTSUP;

    return dev->driver->Init(dev);
}

inline whal_Error whal_Sdhc_Deinit(whal_Sdhc *dev)
{
    if (!dev)
        return WHAL_EINVAL;
    if (!dev->driver || !dev->driver->Deinit)
        return WHAL_ENOTSUP;

    return dev->driver->Deinit(dev);
}

inline whal_Error whal_Sdhc_ConfigureCom(whal_Sdhc *dev,
                                         const whal_Sdhc_ComCfg *comCfg)
{
    if (!dev || !comCfg)
        return WHAL_EINVAL;
    if (!dev->driver || !dev->driver->ConfigureCom)
        return WHAL_ENOTSUP;

    return dev->driver->ConfigureCom(dev, comCfg);
}

inline whal_Error whal_Sdhc_Request(whal_Sdhc *dev, whal_Sdhc_Command *cmd,
                                    whal_Sdhc_Data *data)
{
    if (!dev || !cmd)
        return WHAL_EINVAL;
    if (!dev->driver || !dev->driver->Request)
        return WHAL_ENOTSUP;

    return dev->driver->Request(dev, cmd, data);
}
