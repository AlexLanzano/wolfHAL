#include <stdint.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Rng_Init(whal_Rng *rngDev)
{
    if (!rngDev || !rngDev->driver || !rngDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return rngDev->driver->Init(rngDev);
}

inline whal_Error whal_Rng_Deinit(whal_Rng *rngDev)
{
    if (!rngDev || !rngDev->driver || !rngDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return rngDev->driver->Deinit(rngDev);
}

inline whal_Error whal_Rng_Generate(whal_Rng *rngDev, void *rngData, size_t rngDataSz)
{
    if (!rngDev || !rngDev->driver || !rngDev->driver->Generate || !rngData) {
        return WHAL_EINVAL;
    }

    return rngDev->driver->Generate(rngDev, rngData, rngDataSz);
}
