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

/*
 * @file lpc55s6x_uart.h
 * @brief LPC55S6x FLEXCOMM USART driver configuration.
 */

/*
 * @brief LPC55S6x USART configuration parameters.
 *
 * TODO: add the fields the driver needs to program the baud rate generator
 * (e.g. the source clock frequency and BRG/OSR divider values from the TRM).
 */
typedef struct whal_Lpc55s6x_Uart_Cfg {
    uint32_t baud;
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
