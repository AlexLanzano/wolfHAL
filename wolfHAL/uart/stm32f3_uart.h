/* stm32f3_uart.h
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

#ifndef WHAL_STM32F3_UART_H
#define WHAL_STM32F3_UART_H

/*
 * @file stm32f3_uart.h
 * @brief STM32F3 UART driver (alias for STM32F0 UART).
 *
 * The STM32F3 USART uses the same ISR/TDR/RDR register layout as the
 * STM32F0. This header re-exports under STM32F3-specific names.
 */

#include <wolfHAL/uart/stm32f0_uart.h>

typedef whal_Stm32f0_Uart_Cfg whal_Stm32f3_Uart_Cfg;

#define WHAL_STM32F3_UART_BRR(clk, baud) WHAL_STM32F0_UART_BRR(clk, baud)

#ifndef WHAL_CFG_STM32F3_UART_DIRECT_API_MAPPING
#define whal_Stm32f3_Uart_Driver whal_Stm32f0_Uart_Driver
#define whal_Stm32f3_Uart_Init   whal_Stm32f0_Uart_Init
#define whal_Stm32f3_Uart_Deinit whal_Stm32f0_Uart_Deinit
#define whal_Stm32f3_Uart_Send   whal_Stm32f0_Uart_Send
#define whal_Stm32f3_Uart_Recv   whal_Stm32f0_Uart_Recv
#endif /* !WHAL_CFG_STM32F3_UART_DIRECT_API_MAPPING */

#endif /* WHAL_STM32F3_UART_H */
