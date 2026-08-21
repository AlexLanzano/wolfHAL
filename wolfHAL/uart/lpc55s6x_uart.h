/* lpc55s6x_uart.h
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

#ifndef WHAL_LPC55S6X_UART_H
#define WHAL_LPC55S6X_UART_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/timeout.h>
#include <wolfHAL/bitops.h>

/*
 * @file lpc55s6x_uart.h
 * @brief LPC55S6x FLEXCOMM USART driver configuration.
 */

/*
 * USART CFG register (offset 0x000) bit fields.
 */
#define WHAL_LPC55S6X_USART_CFG_ENABLE_Pos    0
#define WHAL_LPC55S6X_USART_CFG_ENABLE_Msk    (1UL << WHAL_LPC55S6X_USART_CFG_ENABLE_Pos)
#define WHAL_LPC55S6X_USART_CFG_DATALEN_Pos   2
#define WHAL_LPC55S6X_USART_CFG_DATALEN_Msk   (WHAL_BITMASK(2) << WHAL_LPC55S6X_USART_CFG_DATALEN_Pos)
#define WHAL_LPC55S6X_USART_CFG_PARITYSEL_Pos 4
#define WHAL_LPC55S6X_USART_CFG_PARITYSEL_Msk (WHAL_BITMASK(2) << WHAL_LPC55S6X_USART_CFG_PARITYSEL_Pos)
#define WHAL_LPC55S6X_USART_CFG_STOPLEN_Pos   6
#define WHAL_LPC55S6X_USART_CFG_STOPLEN_Msk   (1UL << WHAL_LPC55S6X_USART_CFG_STOPLEN_Pos)

/* CFG DATALEN field values. */
#define WHAL_LPC55S6X_USART_DATALEN_7BIT 0x0
#define WHAL_LPC55S6X_USART_DATALEN_8BIT 0x1
#define WHAL_LPC55S6X_USART_DATALEN_9BIT 0x2

/* CFG PARITYSEL field values. */
#define WHAL_LPC55S6X_USART_PARITY_NONE 0x0
#define WHAL_LPC55S6X_USART_PARITY_EVEN 0x2
#define WHAL_LPC55S6X_USART_PARITY_ODD  0x3

/* CFG STOPLEN field values. */
#define WHAL_LPC55S6X_USART_STOP_1 0x0
#define WHAL_LPC55S6X_USART_STOP_2 0x1

/*
 * USART CTL register (offset 0x004) bit fields.
 */
#define WHAL_LPC55S6X_USART_CTL_TXBRKEN_Pos   1
#define WHAL_LPC55S6X_USART_CTL_TXBRKEN_Msk   (1UL << WHAL_LPC55S6X_USART_CTL_TXBRKEN_Pos)
#define WHAL_LPC55S6X_USART_CTL_ADDRDET_Pos   2
#define WHAL_LPC55S6X_USART_CTL_ADDRDET_Msk   (1UL << WHAL_LPC55S6X_USART_CTL_ADDRDET_Pos)
#define WHAL_LPC55S6X_USART_CTL_TXDIS_Pos     6
#define WHAL_LPC55S6X_USART_CTL_TXDIS_Msk     (1UL << WHAL_LPC55S6X_USART_CTL_TXDIS_Pos)
#define WHAL_LPC55S6X_USART_CTL_CC_Pos        8
#define WHAL_LPC55S6X_USART_CTL_CC_Msk        (1UL << WHAL_LPC55S6X_USART_CTL_CC_Pos)
#define WHAL_LPC55S6X_USART_CTL_CLRCCRX_Pos   9
#define WHAL_LPC55S6X_USART_CTL_CLRCCRX_Msk   (1UL << WHAL_LPC55S6X_USART_CTL_CLRCCRX_Pos)
#define WHAL_LPC55S6X_USART_CTL_AUTOBAUD_Pos  16
#define WHAL_LPC55S6X_USART_CTL_AUTOBAUD_Msk  (1UL << WHAL_LPC55S6X_USART_CTL_AUTOBAUD_Pos)

/*
 * @brief Build a USART CFG register word.
 *
 * @param enable    USART enable: 0 disabled, 1 enabled.
 * @param datalen   Data length (WHAL_LPC55S6X_USART_DATALEN_*).
 * @param paritysel Parity (WHAL_LPC55S6X_USART_PARITY_*).
 * @param stoplen   Stop bits (WHAL_LPC55S6X_USART_STOP_*).
 */
#define WHAL_LPC55S6X_USART_CFG(enable, datalen, paritysel, stoplen) ( \
    whal_SetBits(WHAL_LPC55S6X_USART_CFG_ENABLE_Msk,    WHAL_LPC55S6X_USART_CFG_ENABLE_Pos,    (enable))    | \
    whal_SetBits(WHAL_LPC55S6X_USART_CFG_DATALEN_Msk,   WHAL_LPC55S6X_USART_CFG_DATALEN_Pos,   (datalen))   | \
    whal_SetBits(WHAL_LPC55S6X_USART_CFG_PARITYSEL_Msk, WHAL_LPC55S6X_USART_CFG_PARITYSEL_Pos, (paritysel)) | \
    whal_SetBits(WHAL_LPC55S6X_USART_CFG_STOPLEN_Msk,   WHAL_LPC55S6X_USART_CFG_STOPLEN_Pos,   (stoplen)))

/*
 * @brief Build a USART CTL register word.
 *
 * @param txbrken  Continuous break enable.
 * @param addrdet  Address detect mode.
 * @param txdis    Transmit disable.
 * @param cc       Continuous clock (synchronous mode).
 * @param clrccrx  Clear continuous clock on received character.
 * @param autobaud Autobaud enable.
 */
#define WHAL_LPC55S6X_USART_CTL(txbrken, addrdet, txdis, cc, clrccrx, autobaud) ( \
    whal_SetBits(WHAL_LPC55S6X_USART_CTL_TXBRKEN_Msk,  WHAL_LPC55S6X_USART_CTL_TXBRKEN_Pos,  (txbrken))  | \
    whal_SetBits(WHAL_LPC55S6X_USART_CTL_ADDRDET_Msk,  WHAL_LPC55S6X_USART_CTL_ADDRDET_Pos,  (addrdet))  | \
    whal_SetBits(WHAL_LPC55S6X_USART_CTL_TXDIS_Msk,    WHAL_LPC55S6X_USART_CTL_TXDIS_Pos,    (txdis))    | \
    whal_SetBits(WHAL_LPC55S6X_USART_CTL_CC_Msk,       WHAL_LPC55S6X_USART_CTL_CC_Pos,       (cc))       | \
    whal_SetBits(WHAL_LPC55S6X_USART_CTL_CLRCCRX_Msk,  WHAL_LPC55S6X_USART_CTL_CLRCCRX_Pos,  (clrccrx))  | \
    whal_SetBits(WHAL_LPC55S6X_USART_CTL_AUTOBAUD_Msk, WHAL_LPC55S6X_USART_CTL_AUTOBAUD_Pos, (autobaud)))

/*
 * @brief LPC55S6x USART configuration parameters.
 */
typedef struct whal_Lpc55s6x_Uart_Cfg {
    uint32_t fclkHz;      /* FLEXCOMM function clock (FCLK) frequency, for BRG */
    uint32_t baud;        /* Target baud rate */
    uint32_t cfgReg;      /* Packed CFG register word (WHAL_LPC55S6X_USART_CFG) */
    uint32_t ctlReg;      /* Packed CTL register word (WHAL_LPC55S6X_USART_CTL) */
    whal_Timeout *timeout;
} whal_Lpc55s6x_Uart_Cfg;

/*
 * @brief Single-instance device struct. Defined in the driver TU
 * from the WHAL_CFG_LPC55S6X_UART_DEV initializer in board.h.
 */
#ifdef WHAL_CFG_LPC55S6X_UART_SINGLE_INSTANCE
extern const whal_Uart whal_Lpc55s6x_Uart_Dev;
#endif

#ifndef WHAL_CFG_LPC55S6X_UART_DIRECT_API_MAPPING
/*
 * @brief Driver instance for LPC55S6x UART.
 */
extern const whal_UartDriver whal_Lpc55s6x_Uart_Driver;

/*
 * @brief Initialize the LPC55S6x UART peripheral.
 *
 * @param uartDev UART device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Uart_Init(whal_Uart *uartDev);

/*
 * @brief Deinitialize the LPC55S6x UART peripheral.
 *
 * @param uartDev UART device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Uart_Deinit(whal_Uart *uartDev);

/*
 * @brief Transmit a buffer over UART (blocking, polled).
 *
 * @param uartDev UART device instance.
 * @param data    Buffer to transmit.
 * @param dataSz  Number of bytes to transmit.
 *
 * @retval WHAL_SUCCESS All bytes transmitted.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Uart_Send(whal_Uart *uartDev, const void *data, size_t dataSz);

/*
 * @brief Receive a buffer over UART (blocking, polled).
 *
 * @param uartDev UART device instance.
 * @param data    Receive buffer.
 * @param dataSz  Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS All bytes received.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Uart_Recv(whal_Uart *uartDev, void *data, size_t dataSz);
#endif /* !WHAL_CFG_LPC55S6X_UART_DIRECT_API_MAPPING */

#endif /* WHAL_LPC55S6X_UART_H */
