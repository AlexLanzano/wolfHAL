#ifndef WHAL_STM32WB_UART_H
#define WHAL_STM32WB_UART_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/timeout.h>
/*
 * @file stm32wb_uart.h
 * @brief STM32WB UART driver — polled variant.
 */

/*
 * @brief Compute UART BRR register value.
 */
#define WHAL_STM32WB_UART_BRR(clk, baud)   ((clk) / (baud))

/*
 * @brief Compute LPUART BRR register value.
 */
#define WHAL_STM32WB_LPUART_BRR(clk, baud) ((uint32_t)(((uint64_t)(clk) * 256) / (baud)))

/* Polled UART */

/*
 * @brief Polled UART configuration.
 */
typedef struct whal_Stm32wbUart_Cfg {
    uint32_t brr;
    whal_Timeout *timeout;
} whal_Stm32wbUart_Cfg;

/*
 * @brief Polled UART driver. Implements Init, Deinit, Send, Recv.
 */
extern const whal_UartDriver whal_Stm32wbUart_Driver;

/*
 * @brief Initialize the STM32WB UART peripheral.
 *
 * @param uartDev UART device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbUart_Init(whal_Uart *uartDev);

/*
 * @brief Deinitialize the STM32WB UART peripheral.
 *
 * @param uartDev UART device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbUart_Deinit(whal_Uart *uartDev);

/*
 * @brief Transmit a buffer over UART (blocking, polled).
 *
 * @param uartDev UART device instance.
 * @param data    Buffer to transmit.
 * @param dataSz  Number of bytes to transmit.
 *
 * @retval WHAL_SUCCESS All bytes transmitted.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbUart_Send(whal_Uart *uartDev, const void *data, size_t dataSz);

/*
 * @brief Receive a buffer over UART (blocking, polled).
 *
 * @param uartDev UART device instance.
 * @param data    Receive buffer.
 * @param dataSz  Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS All bytes received.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbUart_Recv(whal_Uart *uartDev, void *data, size_t dataSz);

#endif /* WHAL_STM32WB_UART_H */
