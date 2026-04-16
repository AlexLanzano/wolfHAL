#include <wolfHAL/timer/timer.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

inline whal_Error whal_Timer_Init(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Init)
        return WHAL_ENOTIMPL;

    return timerDev->driver->Init(timerDev);
}

inline whal_Error whal_Timer_Deinit(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Deinit)
        return WHAL_ENOTIMPL;

    return timerDev->driver->Deinit(timerDev);
}

inline whal_Error whal_Timer_Start(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Start)
        return WHAL_ENOTIMPL;

    return timerDev->driver->Start(timerDev);
}

inline whal_Error whal_Timer_Stop(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Stop)
        return WHAL_ENOTIMPL;

    return timerDev->driver->Stop(timerDev);
}

inline whal_Error whal_Timer_Reset(whal_Timer *timerDev)
{
    if (!timerDev)
        return WHAL_EINVAL;
    if (!timerDev->driver || !timerDev->driver->Reset)
        return WHAL_ENOTIMPL;

    return timerDev->driver->Reset(timerDev);
}
