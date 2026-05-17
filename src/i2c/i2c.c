/* i2c.c
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

#include <wolfHAL/i2c/i2c.h>
#include <wolfHAL/error.h>

inline whal_Error whal_I2c_Init(whal_I2c *i2cDev)
{
    if (!i2cDev || !i2cDev->driver || !i2cDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return i2cDev->driver->Init(i2cDev);
}

inline whal_Error whal_I2c_Deinit(whal_I2c *i2cDev)
{
    if (!i2cDev || !i2cDev->driver || !i2cDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return i2cDev->driver->Deinit(i2cDev);
}

inline whal_Error whal_I2c_StartCom(whal_I2c *i2cDev, whal_I2c_ComCfg *comCfg)
{
    if (!i2cDev || !i2cDev->driver || !i2cDev->driver->StartCom || !comCfg) {
        return WHAL_EINVAL;
    }

    return i2cDev->driver->StartCom(i2cDev, comCfg);
}

inline whal_Error whal_I2c_EndCom(whal_I2c *i2cDev)
{
    if (!i2cDev || !i2cDev->driver || !i2cDev->driver->EndCom) {
        return WHAL_EINVAL;
    }

    return i2cDev->driver->EndCom(i2cDev);
}

inline whal_Error whal_I2c_Transfer(whal_I2c *i2cDev, whal_I2c_Msg *msgs,
                                    size_t numMsgs)
{
    if (!i2cDev || !i2cDev->driver || !i2cDev->driver->Transfer || !msgs) {
        return WHAL_EINVAL;
    }

    return i2cDev->driver->Transfer(i2cDev, msgs, numMsgs);
}
