#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/error.h>

whal_Error whal_Gpio_Init(whal_Gpio *gpioDev)
{
    if (!gpioDev)
        return WHAL_EINVAL;
    if (!gpioDev->driver || !gpioDev->driver->Init)
        return WHAL_ENOTSUP;

    return gpioDev->driver->Init(gpioDev);
}

whal_Error whal_Gpio_Deinit(whal_Gpio *gpioDev)
{
    if (!gpioDev)
        return WHAL_EINVAL;
    if (!gpioDev->driver || !gpioDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return gpioDev->driver->Deinit(gpioDev);
}

whal_Error whal_Gpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value)
{
    if (!gpioDev || !value)
        return WHAL_EINVAL;
    if (!gpioDev->driver || !gpioDev->driver->Get)
        return WHAL_ENOTSUP;

    return gpioDev->driver->Get(gpioDev, pin, value);
}

whal_Error whal_Gpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value)
{
    if (!gpioDev)
        return WHAL_EINVAL;
    if (!gpioDev->driver || !gpioDev->driver->Set)
        return WHAL_ENOTSUP;

    return gpioDev->driver->Set(gpioDev, pin, value);
}

