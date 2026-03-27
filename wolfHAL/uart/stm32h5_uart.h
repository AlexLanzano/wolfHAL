#ifndef WHAL_STM32H5_UART_H
#define WHAL_STM32H5_UART_H

/*
 * @file stm32h5_uart.h
 * @brief STM32H5 UART driver (alias for STM32WB UART).
 *
 * The STM32H5 USART peripheral is register-compatible with the STM32WB USART.
 * This header re-exports the STM32WB UART driver types and symbols under
 * STM32H5-specific names. The underlying implementation is shared.
 */

#include <wolfHAL/uart/stm32wb_uart.h>

typedef whal_Stm32wbUart_Cfg whal_Stm32h5Uart_Cfg;

/*
 * @brief Compute UART BRR register value.
 *
 * @param clk  Peripheral clock frequency in Hz.
 * @param baud Desired baud rate.
 */
#define WHAL_STM32H5_UART_BRR(clk, baud)   WHAL_STM32WB_UART_BRR(clk, baud)

/*
 * @brief Compute LPUART BRR register value.
 *
 * @param clk  Peripheral clock frequency in Hz.
 * @param baud Desired baud rate.
 */
#define WHAL_STM32H5_LPUART_BRR(clk, baud) WHAL_STM32WB_LPUART_BRR(clk, baud)

#define whal_Stm32h5Uart_Driver whal_Stm32wbUart_Driver
#define whal_Stm32h5Uart_Init   whal_Stm32wbUart_Init
#define whal_Stm32h5Uart_Deinit whal_Stm32wbUart_Deinit
#define whal_Stm32h5Uart_Send   whal_Stm32wbUart_Send
#define whal_Stm32h5Uart_Recv   whal_Stm32wbUart_Recv

#endif /* WHAL_STM32H5_UART_H */
