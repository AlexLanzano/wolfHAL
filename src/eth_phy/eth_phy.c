/* eth_phy.c
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

#include <wolfHAL/eth_phy/eth_phy.h>
#include <wolfHAL/error.h>

whal_Error whal_EthPhy_Init(whal_EthPhy *phyDev)
{
    if (!phyDev || !phyDev->driver || !phyDev->driver->Init)
        return WHAL_EINVAL;
    return phyDev->driver->Init(phyDev);
}

whal_Error whal_EthPhy_Deinit(whal_EthPhy *phyDev)
{
    if (!phyDev || !phyDev->driver || !phyDev->driver->Deinit)
        return WHAL_EINVAL;
    return phyDev->driver->Deinit(phyDev);
}

whal_Error whal_EthPhy_GetLinkState(whal_EthPhy *phyDev, uint8_t *up,
                                     uint8_t *speed, uint8_t *duplex)
{
    if (!phyDev || !phyDev->driver || !phyDev->driver->GetLinkState ||
        !up || !speed || !duplex)
        return WHAL_EINVAL;
    return phyDev->driver->GetLinkState(phyDev, up, speed, duplex);
}
