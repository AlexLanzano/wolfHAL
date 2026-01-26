#include <stdint.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Uart_Init(whal_Uart *uartDev)
{
    if (!uartDev || !uartDev->driver || !uartDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return uartDev->driver->Init(uartDev);
}

inline whal_Error whal_Uart_Deinit(whal_Uart *uartDev)
{
    if (!uartDev || !uartDev->driver || !uartDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return uartDev->driver->Deinit(uartDev);
}

inline whal_Error whal_Uart_Send(whal_Uart *uartDev, const uint8_t *data, size_t dataSz)
{
    if (!uartDev || !uartDev->driver || !uartDev->driver->Send || !data) {
        return WHAL_EINVAL;
    }

    return uartDev->driver->Send(uartDev, data, dataSz);
}

inline whal_Error whal_Uart_Recv(whal_Uart *uartDev, uint8_t *data, size_t dataSz)
{
    if (!uartDev || !uartDev->driver || !uartDev->driver->Recv || !data) {
        return WHAL_EINVAL;
    }

    return uartDev->driver->Recv(uartDev, data, dataSz);
}

inline whal_Error whal_Uart_Cmd(whal_Uart *uartDev, size_t cmd, void *args)
{
    if (!uartDev || !uartDev->driver || !uartDev->driver->Cmd) {
        return WHAL_EINVAL;
    }

    return uartDev->driver->Cmd(uartDev, cmd, args);
}
