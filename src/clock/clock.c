#include <wolfHAL/clock/clock.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Clock_Init(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;
    if (!clkDev->driver || !clkDev->driver->Init)
        return WHAL_ENOTIMPL;

    return clkDev->driver->Init(clkDev);
}

inline whal_Error whal_Clock_Deinit(whal_Clock *clkDev)
{
    if (!clkDev)
        return WHAL_EINVAL;
    if (!clkDev->driver || !clkDev->driver->Deinit)
        return WHAL_ENOTIMPL;

    return clkDev->driver->Deinit(clkDev);
}

inline whal_Error whal_Clock_Enable(whal_Clock *clkDev, const void *clk)
{
    if (!clkDev)
        return WHAL_EINVAL;
    if (!clkDev->driver || !clkDev->driver->Enable)
        return WHAL_ENOTIMPL;

    return clkDev->driver->Enable(clkDev, clk);
}

inline whal_Error whal_Clock_Disable(whal_Clock *clkDev, const void *clk)
{
    if (!clkDev)
        return WHAL_EINVAL;
    if (!clkDev->driver || !clkDev->driver->Disable)
        return WHAL_ENOTIMPL;

    return clkDev->driver->Disable(clkDev, clk);
}

