#ifndef WHAL_TIMER_H
#define WHAL_TIMER_H

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <stddef.h>

/*
 * @file timer.h
 * @brief Generic timer abstraction for periodic events or counters.
 */

typedef struct whal_Timer whal_Timer;

/*
 * @brief Driver vtable for timer devices.
 */
typedef struct {
    /* Initialize the timer hardware. */
    whal_Error (*Init)(whal_Timer *timerDev);
    /* Deinitialize the timer hardware. */
    whal_Error (*Deinit)(whal_Timer *timerDev);
    /* Start the timer running. */
    whal_Error (*Start)(whal_Timer *timerDev);
    /* Stop the timer. */
    whal_Error (*Stop)(whal_Timer *timerDev);
    /* Reset the timer counter and configuration as needed. */
    whal_Error (*Reset)(whal_Timer *timerDev);
} whal_TimerDriver;

/*
 * @brief Timer device instance pairing configuration with a driver.
 */
struct whal_Timer{
    const whal_Regmap regmap;
    const whal_TimerDriver *driver;
    void *cfg;
};

/*
 * @brief Initialize a timer device.
 *
 * @param timerDev Timer instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
#ifdef WHAL_CFG_DIRECT_CALLBACKS
#define whal_Timer_Init(timerDev) ((timerDev)->driver->Init((timerDev)))
#define whal_Timer_Deinit(timerDev) ((timerDev)->driver->Deinit((timerDev)))
#define whal_Timer_Start(timerDev) ((timerDev)->driver->Start((timerDev)))
#define whal_Timer_Stop(timerDev) ((timerDev)->driver->Stop((timerDev)))
#define whal_Timer_Reset(timerDev) ((timerDev)->driver->Reset((timerDev)))
#else
/*
 * @brief Initialize a timer device.
 *
 * @param timerDev Timer instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Timer_Init(whal_Timer *timerDev);
/*
 * @brief Deinitialize a timer device.
 *
 * @param timerDev Timer instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Timer_Deinit(whal_Timer *timerDev);
/*
 * @brief Start a timer.
 *
 * @param timerDev Timer instance to start.
 *
 * @retval WHAL_SUCCESS Timer started.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Timer_Start(whal_Timer *timerDev);
/*
 * @brief Stop a timer.
 *
 * @param timerDev Timer instance to stop.
 *
 * @retval WHAL_SUCCESS Timer stopped.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Timer_Stop(whal_Timer *timerDev);
/*
 * @brief Reset a timer.
 *
 * @param timerDev Timer instance to reset.
 *
 * @retval WHAL_SUCCESS Timer reset.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Timer_Reset(whal_Timer *timerDev);
#endif

#endif /* WHAL_TIMER_H */
