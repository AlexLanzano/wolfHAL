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

typedef whal_Stm32wbUart_Cfg whal_Stm32wbaUart_Cfg;

#ifndef WHAL_CFG_UART_API_MAPPING_STM32WBA
#define whal_Stm32wbaUart_Driver whal_Stm32wbUart_Driver
#define whal_Stm32wbaUart_Init   whal_Stm32wbUart_Init
#define whal_Stm32wbaUart_Deinit whal_Stm32wbUart_Deinit
#define whal_Stm32wbaUart_Send   whal_Stm32wbUart_Send
#define whal_Stm32wbaUart_Recv   whal_Stm32wbUart_Recv
#endif /* !WHAL_CFG_UART_API_MAPPING_STM32WBA */

/*
 * @brief Baud rate register helpers (re-exported from STM32WB).
 */
#define WHAL_STM32WBA_UART_BRR   WHAL_STM32WB_UART_BRR
#define WHAL_STM32WBA_LPUART_BRR WHAL_STM32WB_LPUART_BRR

#endif /* WHAL_STM32WBA_UART_H */
