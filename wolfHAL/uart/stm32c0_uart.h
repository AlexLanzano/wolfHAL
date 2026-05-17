/* stm32c0_uart.h
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

#ifndef WHAL_STM32C0_UART_H
#define WHAL_STM32C0_UART_H

/*
 * @file stm32c0_uart.h
 * @brief STM32C0 UART driver (alias for STM32WB UART).
 *
 * The STM32C0 USART peripheral is register-compatible with the STM32WB USART.
 * This header re-exports the STM32WB UART driver types and symbols under
 * STM32C0-specific names. The underlying implementation is shared.
 */

#include <wolfHAL/uart/stm32wb_uart.h>

typedef whal_Stm32wb_Uart_Cfg whal_Stm32c0_Uart_Cfg;

#define whal_Stm32c0_Uart_Dev whal_Stm32wb_Uart_Dev

/*
 * @brief Compute UART BRR register value.
 *
 * @param clk  Peripheral clock frequency in Hz.
 * @param baud Desired baud rate.
 */
#define WHAL_STM32C0_UART_BRR(clk, baud)   WHAL_STM32WB_UART_BRR(clk, baud)

/*
 * @brief Compute LPUART BRR register value.
 *
 * @param clk  Peripheral clock frequency in Hz.
 * @param baud Desired baud rate.
 */
#define WHAL_STM32C0_LPUART_BRR(clk, baud) WHAL_STM32WB_LPUART_BRR(clk, baud)

#ifndef WHAL_CFG_STM32C0_UART_DIRECT_API_MAPPING
#define whal_Stm32c0_Uart_Driver whal_Stm32wb_Uart_Driver
#define whal_Stm32c0_Uart_Init   whal_Stm32wb_Uart_Init
#define whal_Stm32c0_Uart_Deinit whal_Stm32wb_Uart_Deinit
#define whal_Stm32c0_Uart_Send   whal_Stm32wb_Uart_Send
#define whal_Stm32c0_Uart_Recv   whal_Stm32wb_Uart_Recv
#endif /* !WHAL_CFG_STM32C0_UART_DIRECT_API_MAPPING */

#endif /* WHAL_STM32C0_UART_H */
