/* lpc55s6x_uart.c
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
#ifdef WHAL_CFG_LPC55S6X_UART_SINGLE_INSTANCE
#include "board.h"  /* provides whal_Lpc55s6x_Uart_Dev singleton */
#endif
#include <wolfHAL/uart/lpc55s6x_uart.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/*
 * LPC55S6x FLEXCOMM USART Register Definitions
 *
 * TODO: add register offsets, bit positions, and masks from the TRM.
 */

#ifdef WHAL_CFG_LPC55S6X_UART_DIRECT_API_MAPPING
#define whal_Lpc55s6x_Uart_Init      whal_Uart_Init
#define whal_Lpc55s6x_Uart_Deinit    whal_Uart_Deinit
#define whal_Lpc55s6x_Uart_Send      whal_Uart_Send
#define whal_Lpc55s6x_Uart_Recv      whal_Uart_Recv
#define whal_Lpc55s6x_Uart_SendAsync whal_Uart_SendAsync
#define whal_Lpc55s6x_Uart_RecvAsync whal_Uart_RecvAsync
#endif /* WHAL_CFG_LPC55S6X_UART_DIRECT_API_MAPPING */

#ifdef WHAL_CFG_LPC55S6X_UART_SINGLE_INSTANCE
const whal_Uart whal_Lpc55s6x_Uart_Dev = WHAL_CFG_LPC55S6X_UART_DEV;
#endif

whal_Error whal_Lpc55s6x_Uart_Init(whal_Uart *uartDev)
{
#ifdef WHAL_CFG_LPC55S6X_UART_SINGLE_INSTANCE
    const whal_Lpc55s6x_Uart_Cfg *cfg =
        (const whal_Lpc55s6x_Uart_Cfg *)whal_Lpc55s6x_Uart_Dev.cfg;
    size_t base = whal_Lpc55s6x_Uart_Dev.base;
    (void)uartDev;
#else
    whal_Lpc55s6x_Uart_Cfg *cfg;
    size_t base;

    if (!uartDev || !uartDev->cfg) {
        return WHAL_EINVAL;
    }

    base = uartDev->base;
    cfg = (whal_Lpc55s6x_Uart_Cfg *)uartDev->cfg;
#endif
    (void)base;
    (void)cfg;

    /* TODO: implement — program the baud rate generator and enable TX/RX. */
    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Uart_Deinit(whal_Uart *uartDev)
{
#ifdef WHAL_CFG_LPC55S6X_UART_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Uart_Dev.base;
    (void)uartDev;
#else
    size_t base;

    if (!uartDev) {
        return WHAL_EINVAL;
    }

    base = uartDev->base;
#endif
    (void)base;

    /* TODO: implement — disable the USART. */
    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Uart_Send(whal_Uart *uartDev, const void *data, size_t dataSz)
{
    const uint8_t *buf = data;
#ifdef WHAL_CFG_LPC55S6X_UART_SINGLE_INSTANCE
    const whal_Lpc55s6x_Uart_Cfg *cfg =
        (const whal_Lpc55s6x_Uart_Cfg *)whal_Lpc55s6x_Uart_Dev.cfg;
    size_t base = whal_Lpc55s6x_Uart_Dev.base;
    (void)uartDev;

    if (!data) {
        return WHAL_EINVAL;
    }
#else
    size_t base;
    whal_Lpc55s6x_Uart_Cfg *cfg;

    if (!uartDev || !uartDev->cfg || !data) {
        return WHAL_EINVAL;
    }

    base = uartDev->base;
    cfg = (whal_Lpc55s6x_Uart_Cfg *)uartDev->cfg;
#endif
    (void)base;
    (void)cfg;
    (void)buf;
    (void)dataSz;

    /* TODO: implement — poll the TX FIFO/ready flag (via whal_Reg_ReadPoll with
     * cfg->timeout) and write each byte. */
    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Uart_Recv(whal_Uart *uartDev, void *data, size_t dataSz)
{
    uint8_t *buf = data;
#ifdef WHAL_CFG_LPC55S6X_UART_SINGLE_INSTANCE
    const whal_Lpc55s6x_Uart_Cfg *cfg =
        (const whal_Lpc55s6x_Uart_Cfg *)whal_Lpc55s6x_Uart_Dev.cfg;
    size_t base = whal_Lpc55s6x_Uart_Dev.base;
    (void)uartDev;

    if (!data) {
        return WHAL_EINVAL;
    }
#else
    size_t base;
    whal_Lpc55s6x_Uart_Cfg *cfg;

    if (!uartDev || !uartDev->cfg || !data) {
        return WHAL_EINVAL;
    }

    base = uartDev->base;
    cfg = (whal_Lpc55s6x_Uart_Cfg *)uartDev->cfg;
#endif
    (void)base;
    (void)cfg;
    (void)buf;
    (void)dataSz;

    /* TODO: implement — poll the RX ready flag (via whal_Reg_ReadPoll with
     * cfg->timeout) and read each byte. */
    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Uart_SendAsync(whal_Uart *uartDev, const void *data, size_t dataSz)
{
    (void)dataSz;
    if (!uartDev || !data)
        return WHAL_EINVAL;
    return WHAL_ENOTSUP;
}

whal_Error whal_Lpc55s6x_Uart_RecvAsync(whal_Uart *uartDev, void *data, size_t dataSz)
{
    (void)dataSz;
    if (!uartDev || !data)
        return WHAL_EINVAL;
    return WHAL_ENOTSUP;
}

#ifndef WHAL_CFG_LPC55S6X_UART_DIRECT_API_MAPPING
const whal_UartDriver whal_Lpc55s6x_Uart_Driver = {
    .Init = whal_Lpc55s6x_Uart_Init,
    .Deinit = whal_Lpc55s6x_Uart_Deinit,
    .Send = whal_Lpc55s6x_Uart_Send,
    .Recv = whal_Lpc55s6x_Uart_Recv,
    .SendAsync = whal_Lpc55s6x_Uart_SendAsync,
    .RecvAsync = whal_Lpc55s6x_Uart_RecvAsync,
};
#endif /* !WHAL_CFG_LPC55S6X_UART_DIRECT_API_MAPPING */
