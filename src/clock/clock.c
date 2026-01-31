#include <wolfHAL/clock/clock.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Clock_Init(whal_Clock *clkDev)
{
    if (!clkDev || !clkDev->driver || !clkDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return clkDev->driver->Init(clkDev);
}

inline whal_Error whal_Clock_Deinit(whal_Clock *clkDev)
{
    if (!clkDev || !clkDev->driver || !clkDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return clkDev->driver->Deinit(clkDev);
}

inline whal_Error whal_Clock_Enable(whal_Clock *clkDev, const void *clk)
{
    if (!clkDev || !clkDev->driver || !clkDev->driver->Enable) {
        return WHAL_EINVAL;
    }

    return clkDev->driver->Enable(clkDev, clk);
}

inline whal_Error whal_Clock_Disable(whal_Clock *clkDev, const void *clk)
{
    if (!clkDev || !clkDev->driver || !clkDev->driver->Disable) {
        return WHAL_EINVAL;
    }

    return clkDev->driver->Disable(clkDev, clk);
}

inline whal_Error whal_Clock_GetRate(whal_Clock *clkDev, size_t *rateOut)
{
    if (!clkDev || !clkDev->driver || !clkDev->driver->GetRate ||
        !rateOut) 
    {
        return WHAL_EINVAL;
    }

    return clkDev->driver->GetRate(clkDev, rateOut);
}

