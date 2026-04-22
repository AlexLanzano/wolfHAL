#ifndef WHAL_STM32F0_UART_H
#define WHAL_STM32F0_UART_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32f0_uart.h
 * @brief STM32F0 UART driver — polled variant.
 *
 * The STM32F0 USART uses the same ISR/TDR/RDR register layout as the
 * STM32WB but does not have the FIFOEN bit in CR1 (no hardware FIFO).
 */

#define WHAL_STM32F0_UART_BRR(clk, baud) ((clk) / (baud))

typedef struct whal_Stm32f0Uart_Cfg {
    uint32_t brr;
    whal_Timeout *timeout;
} whal_Stm32f0Uart_Cfg;

#ifndef WHAL_CFG_UART_API_MAPPING_STM32F0
extern const whal_UartDriver whal_Stm32f0Uart_Driver;

whal_Error whal_Stm32f0Uart_Init(whal_Uart *uartDev);
whal_Error whal_Stm32f0Uart_Deinit(whal_Uart *uartDev);
whal_Error whal_Stm32f0Uart_Send(whal_Uart *uartDev, const void *data,
                                  size_t dataSz);
whal_Error whal_Stm32f0Uart_Recv(whal_Uart *uartDev, void *data,
                                  size_t dataSz);
#endif /* !WHAL_CFG_UART_API_MAPPING_STM32F0 */

#endif /* WHAL_STM32F0_UART_H */
