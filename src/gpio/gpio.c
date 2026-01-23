#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Gpio_Init(whal_Gpio *gpioDev)
{
    if (!gpioDev || !gpioDev->driver || !gpioDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return gpioDev->driver->Init(gpioDev);
}

inline whal_Error whal_Gpio_Deinit(whal_Gpio *gpioDev)
{
    if (!gpioDev || !gpioDev->driver || !gpioDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return gpioDev->driver->Deinit(gpioDev);
    
}

inline whal_Error whal_Gpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value)
{
    if (!gpioDev || !gpioDev->driver || !gpioDev->driver->Get || !value) {
        return WHAL_EINVAL;
    }

    return gpioDev->driver->Get(gpioDev, pin, value);

}

inline whal_Error whal_Gpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value)
{
    if (!gpioDev || !gpioDev->driver || !gpioDev->driver->Set) {
        return WHAL_EINVAL;
    }

    return gpioDev->driver->Set(gpioDev, pin, value);
}

inline whal_Error whal_Gpio_Cmd(whal_Gpio *gpioDev, size_t cmd, void *args)
{
    if (!gpioDev || !gpioDev->driver || !gpioDev->driver->Cmd) {
        return WHAL_EINVAL;
    }

    return gpioDev->driver->Cmd(gpioDev, cmd, args);
}
