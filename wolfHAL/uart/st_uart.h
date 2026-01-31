#ifndef WHAL_ST_UART_H
#define WHAL_ST_UART_H

#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/uart/uart.h>
#include <stddef.h>

/*
 * @file st_uart.h
 * @brief STM32 UART driver configuration.
 */

/*
 * @brief STM32 UART configuration parameters.
 */
typedef struct whal_StUart_Cfg {
    whal_Clock *clkCtrl;
    void *clk;
    uint8_t lpuart;
    uint32_t baud;
} whal_StUart_Cfg;

/*
 * @brief Driver instance for STM32 UART peripheral.
 */
extern whal_UartDriver whal_StUart_Driver;

/*
 * @brief Initialize the STM32 UART peripheral.
 *
 * @param uartDev UART device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StUart_Init(whal_Uart *uartDev);
/*
 * @brief Deinitialize the STM32 UART peripheral.
 *
 * @param uartDev UART device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StUart_Deinit(whal_Uart *uartDev);
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
whal_Error whal_StUart_Send(whal_Uart *uartDev, const uint8_t *data, size_t dataSz);
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
whal_Error whal_StUart_Recv(whal_Uart *uartDev, uint8_t *data, size_t dataSz);

#endif /* WHAL_ST_UART_H */
