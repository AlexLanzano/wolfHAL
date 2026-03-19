#ifndef WHAL_STM32WB_UART_H
#define WHAL_STM32WB_UART_H

#include <stdint.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/timeout.h>
#include <stddef.h>

/*
 * @file stm32wb_uart.h
 * @brief STM32 UART driver configuration.
 */

/*
 * @brief Compute UART BRR register value.
 */
#define WHAL_STM32WB_UART_BRR(clk, baud)   ((clk) / (baud))

/*
 * @brief Compute LPUART BRR register value.
 */
#define WHAL_STM32WB_LPUART_BRR(clk, baud) (((clk) / (baud)) * 256)

/*
 * @brief STM32 UART configuration parameters.
 */
typedef struct whal_Stm32wbUart_Cfg {
    uint32_t baud;
    whal_Timeout *timeout;
} whal_Stm32wbUart_Cfg;

/*
 * @brief Driver instance for STM32 UART peripheral.
 */
extern const whal_UartDriver whal_Stm32wbUart_Driver;

/*
 * @brief Initialize the STM32 UART peripheral.
 *
 * @param uartDev UART device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbUart_Init(whal_Uart *uartDev);
/*
 * @brief Deinitialize the STM32 UART peripheral.
 *
 * @param uartDev UART device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbUart_Deinit(whal_Uart *uartDev);
/*
 * @brief Transmit a buffer over UART.
 *
 * @param uartDev UART device instance.
 * @param data    Buffer to transmit.
 * @param dataSz  Number of bytes to transmit.
 *
 * @retval WHAL_SUCCESS Transfer completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbUart_Send(whal_Uart *uartDev, const void *data, size_t dataSz);
/*
 * @brief Receive a buffer over UART.
 *
 * @param uartDev UART device instance.
 * @param data    Receive buffer.
 * @param dataSz  Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS Transfer completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbUart_Recv(whal_Uart *uartDev, void *data, size_t dataSz);

#endif /* WHAL_STM32WB_UART_H */
