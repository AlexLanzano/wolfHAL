#include <wolfHAL/supply/supply.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Supply_Init(whal_Supply *supplyDev)
{
    if (!supplyDev || !supplyDev->driver || !supplyDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return supplyDev->driver->Init(supplyDev);
}

inline whal_Error whal_Supply_Deinit(whal_Supply *supplyDev)
{
    if (!supplyDev || !supplyDev->driver || !supplyDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return supplyDev->driver->Deinit(supplyDev);
}

inline whal_Error whal_Supply_Enable(whal_Supply *supplyDev, void *supply)
{
    if (!supplyDev || !supplyDev->driver || !supplyDev->driver->Enable) {
        return WHAL_EINVAL;
    }

    return supplyDev->driver->Enable(supplyDev, supply);
}

inline whal_Error whal_Supply_Disable(whal_Supply *supplyDev, void *supply)
{
    if (!supplyDev || !supplyDev->driver || !supplyDev->driver->Disable) {
        return WHAL_EINVAL;
    }

    return supplyDev->driver->Disable(supplyDev, supply);
}
