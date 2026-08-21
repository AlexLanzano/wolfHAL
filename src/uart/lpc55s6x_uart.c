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

/* FLEXCOMM peripheral-select register (selects USART/SPI/I2C for this instance). */
#define FLEXCOMM_PSELID_REG          0xFF8
#define FLEXCOMM_PSELID_PERSEL_Pos   0
#define FLEXCOMM_PSELID_PERSEL_Msk   (WHAL_BITMASK(3) << FLEXCOMM_PSELID_PERSEL_Pos)
#define FLEXCOMM_PSELID_PERSEL_USART 0x1

/* USART registers (offsets within the FLEXCOMM address space). */
#define USART_CFG_REG 0x000
#define USART_CTL_REG 0x004

#define USART_BRG_REG        0x020
#define USART_BRG_BRGVAL_Pos 0
#define USART_BRG_BRGVAL_Msk (WHAL_BITMASK(16) << USART_BRG_BRGVAL_Pos)

#define USART_OSR_REG        0x028
#define USART_OSR_OSRVAL_Pos 0
#define USART_OSR_OSRVAL_Msk (WHAL_BITMASK(4) << USART_OSR_OSRVAL_Pos)

#define USART_FIFOCFG_REG          0xE00
#define USART_FIFOCFG_ENABLETX_Msk (1UL << 0)
#define USART_FIFOCFG_ENABLERX_Msk (1UL << 1)

#define USART_FIFOSTAT_REG            0xE04
#define USART_FIFOSTAT_TXNOTFULL_Msk  (1UL << 5)
#define USART_FIFOSTAT_RXNOTEMPTY_Msk (1UL << 6)

#define USART_FIFOWR_REG 0xE20
#define USART_FIFORD_REG 0xE30

/* Baud generator oversampling (OSRVAL = 15 -> 16x). */
#define USART_OVERSAMPLE 16

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
    uint32_t brg;
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

    /* Route this FLEXCOMM instance to its USART function. */
    whal_Reg_Update(base, FLEXCOMM_PSELID_REG, FLEXCOMM_PSELID_PERSEL_Msk,
                    whal_SetBits(FLEXCOMM_PSELID_PERSEL_Msk,
                                 FLEXCOMM_PSELID_PERSEL_Pos,
                                 FLEXCOMM_PSELID_PERSEL_USART));

    /* Only CFG is writable while disabled; enabling here (CFG carries ENABLE)
     * lets the remaining registers be configured. */
    whal_Reg_Write(base, USART_CFG_REG, cfg->cfgReg);
    whal_Reg_Write(base, USART_CTL_REG, cfg->ctlReg);

    /* Baud rate: BRGVAL = FCLK / (oversample * baud) - 1. */
    brg = (cfg->fclkHz / (USART_OVERSAMPLE * cfg->baud)) - 1;
    whal_Reg_Update(base, USART_OSR_REG, USART_OSR_OSRVAL_Msk,
                    whal_SetBits(USART_OSR_OSRVAL_Msk, USART_OSR_OSRVAL_Pos,
                                 USART_OVERSAMPLE - 1));
    whal_Reg_Update(base, USART_BRG_REG, USART_BRG_BRGVAL_Msk,
                    whal_SetBits(USART_BRG_BRGVAL_Msk, USART_BRG_BRGVAL_Pos, brg));

    /* Enable the transmit and receive FIFOs. */
    whal_Reg_Update(base, USART_FIFOCFG_REG,
                    USART_FIFOCFG_ENABLETX_Msk | USART_FIFOCFG_ENABLERX_Msk,
                    USART_FIFOCFG_ENABLETX_Msk | USART_FIFOCFG_ENABLERX_Msk);

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

    /* Writing 0 to CFG clears ENABLE and resets the USART state machine. */
    whal_Reg_Write(base, USART_CFG_REG, 0);

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

    for (size_t i = 0; i < dataSz; ++i) {
        whal_Error err = whal_Reg_ReadPoll(base, USART_FIFOSTAT_REG,
                                           USART_FIFOSTAT_TXNOTFULL_Msk,
                                           USART_FIFOSTAT_TXNOTFULL_Msk,
                                           cfg->timeout);
        if (err)
            return err;

        whal_Reg_Write(base, USART_FIFOWR_REG, buf[i]);
    }

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

    for (size_t i = 0; i < dataSz; ++i) {
        whal_Error err = whal_Reg_ReadPoll(base, USART_FIFOSTAT_REG,
                                           USART_FIFOSTAT_RXNOTEMPTY_Msk,
                                           USART_FIFOSTAT_RXNOTEMPTY_Msk,
                                           cfg->timeout);
        if (err)
            return err;

        buf[i] = (uint8_t)whal_Reg_Read(base, USART_FIFORD_REG);
    }

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
