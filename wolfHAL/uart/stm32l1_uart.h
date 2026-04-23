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

typedef whal_Stm32f4Uart_Cfg whal_Stm32l1Uart_Cfg;

#define WHAL_STM32L1_UART_BRR WHAL_STM32F4_UART_BRR

#ifndef WHAL_CFG_UART_API_MAPPING_STM32L1
#define whal_Stm32l1Uart_Driver    whal_Stm32f4Uart_Driver
#define whal_Stm32l1Uart_Init      whal_Stm32f4Uart_Init
#define whal_Stm32l1Uart_Deinit    whal_Stm32f4Uart_Deinit
#define whal_Stm32l1Uart_Send      whal_Stm32f4Uart_Send
#define whal_Stm32l1Uart_Recv      whal_Stm32f4Uart_Recv
#define whal_Stm32l1Uart_SendAsync whal_Stm32f4Uart_SendAsync
#define whal_Stm32l1Uart_RecvAsync whal_Stm32f4Uart_RecvAsync
#endif /* !WHAL_CFG_UART_API_MAPPING_STM32L1 */

#endif /* WHAL_STM32L1_UART_H */
