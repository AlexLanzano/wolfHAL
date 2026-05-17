/* stm32wba_uart.h
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

#ifndef WHAL_STM32WBA_UART_H
#define WHAL_STM32WBA_UART_H

/*
 * @file stm32wba_uart.h
 * @brief STM32WBA UART driver (alias for STM32WB UART).
 *
 * The STM32WBA USART peripheral is register-compatible with the STM32WB USART.
 * This header re-exports the STM32WB UART driver types and symbols under
 * STM32WBA-specific names.
 */

#include <wolfHAL/uart/stm32wb_uart.h>

typedef whal_Stm32wb_Uart_Cfg whal_Stm32wba_Uart_Cfg;

#define whal_Stm32wba_Uart_Dev whal_Stm32wb_Uart_Dev

#ifndef WHAL_CFG_STM32WBA_UART_DIRECT_API_MAPPING
#define whal_Stm32wba_Uart_Driver whal_Stm32wb_Uart_Driver
#define whal_Stm32wba_Uart_Init   whal_Stm32wb_Uart_Init
#define whal_Stm32wba_Uart_Deinit whal_Stm32wb_Uart_Deinit
#define whal_Stm32wba_Uart_Send   whal_Stm32wb_Uart_Send
#define whal_Stm32wba_Uart_Recv   whal_Stm32wb_Uart_Recv
#endif /* !WHAL_CFG_STM32WBA_UART_DIRECT_API_MAPPING */

/*
 * @brief Baud rate register helpers (re-exported from STM32WB).
 */
#define WHAL_STM32WBA_UART_BRR   WHAL_STM32WB_UART_BRR
#define WHAL_STM32WBA_LPUART_BRR WHAL_STM32WB_LPUART_BRR

#endif /* WHAL_STM32WBA_UART_H */
