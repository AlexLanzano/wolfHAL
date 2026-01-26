#ifndef WHAL_UART_H
#define WHAL_UART_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/error.h>

/*
 * @file uart.h
 * @brief Generic UART abstraction and driver interface.
 */

typedef struct whal_Uart whal_Uart;

/*
 * @brief Driver vtable for UART devices.
 */
typedef struct {
    /* Initialize the UART hardware. */
    whal_Error (*Init)(whal_Uart *uartDev);
    /* Deinitialize the UART hardware. */
    whal_Error (*Deinit)(whal_Uart *uartDev);
    /* Transmit a buffer. */
    whal_Error (*Send)(whal_Uart *uartDev, const uint8_t *data, size_t dataSz);
    /* Receive into a buffer. */
    whal_Error (*Recv)(whal_Uart *uartDev, uint8_t *data, size_t dataSz);
    /* Issue driver-specific commands. */
    whal_Error (*Cmd)(whal_Uart *uartDev, size_t cmd, void *args);
} whal_UartDriver;

/*
 * @brief UART device instance tying a register map and driver.
 */
struct whal_Uart {
    const whal_Regmap regmap;
    const whal_UartDriver *driver;
    void *cfg;
};

/*
 * @brief Initializes a UART device and its driver.
 *
 * @param uartDev Pointer to the UART instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or driver rejected configuration.
 */
whal_Error whal_Uart_Init(whal_Uart *uartDev);

/*
 * @brief Deinitializes a UART device and releases resources.
 *
 * @param uartDev Pointer to the UART instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer or driver refused to deinit.
 */
whal_Error whal_Uart_Deinit(whal_Uart *uartDev);

/*
 * @brief Sends a buffer over the UART.
 *
 * @param uartDev Pointer to the UART instance.
 * @param data    Buffer to transmit.
 * @param dataSz  Number of bytes to send.
 *
 * @retval WHAL_SUCCESS Buffer was queued or transmitted.
 * @retval WHAL_EINVAL  Null pointer or driver failed to send.
 */
whal_Error whal_Uart_Send(whal_Uart *uartDev, const uint8_t *data, size_t dataSz);

/*
 * @brief Receives data from the UART into a buffer.
 *
 * @param uartDev Pointer to the UART instance.
 * @param data    Destination buffer.
 * @param dataSz  Maximum number of bytes to read.
 *
 * @retval WHAL_SUCCESS Buffer was filled or receive started.
 * @retval WHAL_EINVAL  Null pointer or driver failed to receive.
 */
whal_Error whal_Uart_Recv(whal_Uart *uartDev, uint8_t *data, size_t dataSz);

/*
 * @brief Issues a driver-specific command to a UART device.
 *
 * @param uartDev Pointer to the UART instance.
 * @param cmd     Numeric command selector defined by the driver.
 * @param args    Optional command arguments, interpreted per cmd.
 *
 * @retval WHAL_SUCCESS Command accepted and executed.
 * @retval WHAL_EINVAL  Null pointer, unknown command, or bad args.
 */
whal_Error whal_Uart_Cmd(whal_Uart *uartDev, size_t cmd, void *args);

#endif /* WHAL_UART_H */
