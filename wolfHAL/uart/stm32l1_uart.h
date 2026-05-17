/* stm32l1_uart.h
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

#ifndef WHAL_STM32L1_UART_H
#define WHAL_STM32L1_UART_H

/*
 * @file stm32l1_uart.h
 * @brief STM32L1 UART driver (alias for STM32F4 UART).
 *
 * The STM32L1 USART uses the same USARTv1 register layout as the STM32F4
 * (SR/DR at 0x00/0x04, BRR at 0x08, CR1 at 0x0C).
 */

#include <wolfHAL/uart/stm32f4_uart.h>

typedef whal_Stm32f4_Uart_Cfg whal_Stm32l1_Uart_Cfg;

#define WHAL_STM32L1_UART_BRR WHAL_STM32F4_UART_BRR

#ifndef WHAL_CFG_STM32L1_UART_DIRECT_API_MAPPING
#define whal_Stm32l1_Uart_Driver    whal_Stm32f4_Uart_Driver
#define whal_Stm32l1_Uart_Init      whal_Stm32f4_Uart_Init
#define whal_Stm32l1_Uart_Deinit    whal_Stm32f4_Uart_Deinit
#define whal_Stm32l1_Uart_Send      whal_Stm32f4_Uart_Send
#define whal_Stm32l1_Uart_Recv      whal_Stm32f4_Uart_Recv
#define whal_Stm32l1_Uart_SendAsync whal_Stm32f4_Uart_SendAsync
#define whal_Stm32l1_Uart_RecvAsync whal_Stm32f4_Uart_RecvAsync
#endif /* !WHAL_CFG_STM32L1_UART_DIRECT_API_MAPPING */

#endif /* WHAL_STM32L1_UART_H */
