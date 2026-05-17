/* uart.c
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
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Uart_Init(whal_Uart *uartDev)
{
    if (!uartDev)
        return WHAL_EINVAL;
    if (!uartDev->driver || !uartDev->driver->Init)
        return WHAL_ENOTSUP;

    return uartDev->driver->Init(uartDev);
}

inline whal_Error whal_Uart_Deinit(whal_Uart *uartDev)
{
    if (!uartDev)
        return WHAL_EINVAL;
    if (!uartDev->driver || !uartDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return uartDev->driver->Deinit(uartDev);
}

inline whal_Error whal_Uart_Send(whal_Uart *uartDev, const void *data, size_t dataSz)
{
    if (!uartDev || !data)
        return WHAL_EINVAL;
    if (!uartDev->driver || !uartDev->driver->Send)
        return WHAL_ENOTSUP;

    return uartDev->driver->Send(uartDev, data, dataSz);
}

inline whal_Error whal_Uart_Recv(whal_Uart *uartDev, void *data, size_t dataSz)
{
    if (!uartDev || !data)
        return WHAL_EINVAL;
    if (!uartDev->driver || !uartDev->driver->Recv)
        return WHAL_ENOTSUP;

    return uartDev->driver->Recv(uartDev, data, dataSz);
}

inline whal_Error whal_Uart_SendAsync(whal_Uart *uartDev, const void *data, size_t dataSz)
{
    if (!uartDev || !data)
        return WHAL_EINVAL;
    if (!uartDev->driver || !uartDev->driver->SendAsync)
        return WHAL_ENOTSUP;

    return uartDev->driver->SendAsync(uartDev, data, dataSz);
}

inline whal_Error whal_Uart_RecvAsync(whal_Uart *uartDev, void *data, size_t dataSz)
{
    if (!uartDev || !data)
        return WHAL_EINVAL;
    if (!uartDev->driver || !uartDev->driver->RecvAsync)
        return WHAL_ENOTSUP;

    return uartDev->driver->RecvAsync(uartDev, data, dataSz);
}
