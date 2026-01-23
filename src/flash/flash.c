#include <stdint.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Flash_Init(whal_Flash *flashDev)
{
    if (!flashDev || !flashDev->driver || !flashDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return flashDev->driver->Init(flashDev);
}

inline whal_Error whal_Flash_Deinit(whal_Flash *flashDev)
{
    if (!flashDev || !flashDev->driver || !flashDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return flashDev->driver->Deinit(flashDev);
}

inline whal_Error whal_Flash_Write(whal_Flash *flashDev, size_t addr, uint8_t *data,
                                  size_t dataSz)
{
    if (!flashDev || !flashDev->driver || !flashDev->driver->Write || !data) {
        return WHAL_EINVAL;
    }

    return flashDev->driver->Write(flashDev, addr, data, dataSz);
}

inline whal_Error whal_Flash_Erase(whal_Flash *flashDev, size_t addr,
                                  size_t dataSz)
{
    if (!flashDev || !flashDev->driver || !flashDev->driver->Erase) {
        return WHAL_EINVAL;
    }

    return flashDev->driver->Erase(flashDev, addr, dataSz);
}

inline whal_Error whal_Flash_Cmd(whal_Flash *flashDev, size_t cmd, void *args)
{
    if (!flashDev || !flashDev->driver || !flashDev->driver->Cmd) {
        return WHAL_EINVAL;
    }

    return flashDev->driver->Cmd(flashDev, cmd, args);
}
