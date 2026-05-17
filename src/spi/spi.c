/* spi.c
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
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Spi_Init(whal_Spi *spiDev)
{
    if (!spiDev)
        return WHAL_EINVAL;
    if (!spiDev->driver || !spiDev->driver->Init)
        return WHAL_ENOTSUP;

    return spiDev->driver->Init(spiDev);
}

inline whal_Error whal_Spi_Deinit(whal_Spi *spiDev)
{
    if (!spiDev)
        return WHAL_EINVAL;
    if (!spiDev->driver || !spiDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return spiDev->driver->Deinit(spiDev);
}

inline whal_Error whal_Spi_StartCom(whal_Spi *spiDev, whal_Spi_ComCfg *comCfg)
{
    if (!spiDev || !comCfg)
        return WHAL_EINVAL;
    if (!spiDev->driver || !spiDev->driver->StartCom)
        return WHAL_ENOTSUP;

    return spiDev->driver->StartCom(spiDev, comCfg);
}

inline whal_Error whal_Spi_EndCom(whal_Spi *spiDev)
{
    if (!spiDev)
        return WHAL_EINVAL;
    if (!spiDev->driver || !spiDev->driver->EndCom)
        return WHAL_ENOTSUP;

    return spiDev->driver->EndCom(spiDev);
}

inline whal_Error whal_Spi_SendRecv(whal_Spi *spiDev, const void *tx, size_t txLen, void *rx, size_t rxLen)
{
    if (!spiDev)
        return WHAL_EINVAL;
    if (!spiDev->driver || !spiDev->driver->SendRecv)
        return WHAL_ENOTSUP;

    return spiDev->driver->SendRecv(spiDev, tx, txLen, rx, rxLen);
}
