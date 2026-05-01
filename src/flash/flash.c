#include <stdint.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>

whal_Error whal_Flash_Init(whal_Flash *flashDev)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Init)
        return WHAL_ENOTSUP;

    return flashDev->driver->Init(flashDev);
}

whal_Error whal_Flash_Deinit(whal_Flash *flashDev)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return flashDev->driver->Deinit(flashDev);
}

whal_Error whal_Flash_Lock(whal_Flash *flashDev, size_t addr, size_t len)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Lock)
        return WHAL_ENOTSUP;

    return flashDev->driver->Lock(flashDev, addr, len);
}

whal_Error whal_Flash_Unlock(whal_Flash *flashDev, size_t addr, size_t len)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Unlock)
        return WHAL_ENOTSUP;

    return flashDev->driver->Unlock(flashDev, addr, len);
}

whal_Error whal_Flash_Read(whal_Flash *flashDev, size_t addr, void *data,
                                  size_t dataSz)
{
    if (!flashDev || !data)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Read)
        return WHAL_ENOTSUP;

    return flashDev->driver->Read(flashDev, addr, data, dataSz);
}

whal_Error whal_Flash_Write(whal_Flash *flashDev, size_t addr, const void *data,
                                  size_t dataSz)
{
    if (!flashDev || !data)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Write)
        return WHAL_ENOTSUP;

    return flashDev->driver->Write(flashDev, addr, data, dataSz);
}

whal_Error whal_Flash_Erase(whal_Flash *flashDev, size_t addr,
                                  size_t dataSz)
{
    if (!flashDev)
        return WHAL_EINVAL;
    if (!flashDev->driver || !flashDev->driver->Erase)
        return WHAL_ENOTSUP;

    return flashDev->driver->Erase(flashDev, addr, dataSz);
}

