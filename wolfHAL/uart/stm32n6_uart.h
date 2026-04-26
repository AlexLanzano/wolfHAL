#ifndef WHAL_STM32N6_UART_H
#define WHAL_STM32N6_UART_H

/**
 * @file stm32n6_uart.h
 * @brief STM32N6 UART driver (alias for STM32WB UART).
 *
 * The STM32N6 USART peripheral is register-compatible with the STM32WB USART
 * (CR1/CR2/CR3/BRR/GTPR/RTOR/RQR/ISR/ICR/RDR/TDR/PRESC at identical offsets).
 * This header re-exports under STM32N6-specific names.
 */

#include <wolfHAL/uart/stm32wb_uart.h>

typedef whal_Stm32wbUart_Cfg whal_Stm32n6Uart_Cfg;

#ifndef WHAL_CFG_UART_API_MAPPING_STM32N6
#define whal_Stm32n6Uart_Driver whal_Stm32wbUart_Driver
#define whal_Stm32n6Uart_Init   whal_Stm32wbUart_Init
#define whal_Stm32n6Uart_Deinit whal_Stm32wbUart_Deinit
#define whal_Stm32n6Uart_Send   whal_Stm32wbUart_Send
#define whal_Stm32n6Uart_Recv   whal_Stm32wbUart_Recv
#endif /* !WHAL_CFG_UART_API_MAPPING_STM32N6 */

#define WHAL_STM32N6_UART_BRR   WHAL_STM32WB_UART_BRR
#define WHAL_STM32N6_LPUART_BRR WHAL_STM32WB_LPUART_BRR

#endif /* WHAL_STM32N6_UART_H */
