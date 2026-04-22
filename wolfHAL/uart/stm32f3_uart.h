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

typedef whal_Stm32f0Uart_Cfg whal_Stm32f3Uart_Cfg;

#define WHAL_STM32F3_UART_BRR(clk, baud) WHAL_STM32F0_UART_BRR(clk, baud)

#ifndef WHAL_CFG_UART_API_MAPPING_STM32F3
#define whal_Stm32f3Uart_Driver whal_Stm32f0Uart_Driver
#define whal_Stm32f3Uart_Init   whal_Stm32f0Uart_Init
#define whal_Stm32f3Uart_Deinit whal_Stm32f0Uart_Deinit
#define whal_Stm32f3Uart_Send   whal_Stm32f0Uart_Send
#define whal_Stm32f3Uart_Recv   whal_Stm32f0Uart_Recv
#endif /* !WHAL_CFG_UART_API_MAPPING_STM32F3 */

#endif /* WHAL_STM32F3_UART_H */
