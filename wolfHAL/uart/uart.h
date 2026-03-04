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
    whal_Error (*Send)(whal_Uart *uartDev, const void *data, size_t dataSz);
    /* Receive into a buffer. */
    whal_Error (*Recv)(whal_Uart *uartDev, void *data, size_t dataSz);
} whal_UartDriver;

/*
 * @brief UART device instance tying a register map and driver.
 */
struct whal_Uart {
    const whal_Regmap regmap;
    const whal_UartDriver *driver;
    void *cfg;
};

#ifdef WHAL_CFG_DIRECT_CALLBACKS
#define whal_Uart_Init(uartDev) ((uartDev)->driver->Init((uartDev)))
#define whal_Uart_Deinit(uartDev) ((uartDev)->driver->Deinit((uartDev)))
#define whal_Uart_Send(uartDev, data, dataSz) ((uartDev)->driver->Send((uartDev), (data), (dataSz)))
#define whal_Uart_Recv(uartDev, data, dataSz) ((uartDev)->driver->Recv((uartDev), (data), (dataSz)))
#else
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
whal_Error whal_Uart_Send(whal_Uart *uartDev, const void *data, size_t dataSz);

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
whal_Error whal_Uart_Recv(whal_Uart *uartDev, void *data, size_t dataSz);
#endif

#endif /* WHAL_UART_H */
