#ifndef WHAL_STM32H7_UART_H
#define WHAL_STM32H7_UART_H

/*
 * @file stm32h7_uart.h
 * @brief STM32H7 UART driver (alias for STM32WB UART).
 *
 * The STM32H7 USART peripheral is register-compatible with the STM32WB USART.
 * This header re-exports the STM32WB UART driver types and symbols under
 * STM32H7-specific names. The underlying implementation is shared.
 */

#include <wolfHAL/uart/stm32wb_uart.h>

typedef whal_Stm32wbUart_Cfg whal_Stm32h7Uart_Cfg;

/*
 * @brief Compute UART BRR register value.
 *
 * @param clk  Peripheral clock frequency in Hz.
 * @param baud Desired baud rate.
 */
#define WHAL_STM32H7_UART_BRR(clk, baud)   WHAL_STM32WB_UART_BRR(clk, baud)

/*
 * @brief Compute LPUART BRR register value.
 *
 * @param clk  Peripheral clock frequency in Hz.
 * @param baud Desired baud rate.
 */
#define WHAL_STM32H7_LPUART_BRR(clk, baud) WHAL_STM32WB_LPUART_BRR(clk, baud)

#define whal_Stm32h7Uart_Driver whal_Stm32wbUart_Driver
#define whal_Stm32h7Uart_Init   whal_Stm32wbUart_Init
#define whal_Stm32h7Uart_Deinit whal_Stm32wbUart_Deinit
#define whal_Stm32h7Uart_Send   whal_Stm32wbUart_Send
#define whal_Stm32h7Uart_Recv   whal_Stm32wbUart_Recv

#endif /* WHAL_STM32H7_UART_H */
