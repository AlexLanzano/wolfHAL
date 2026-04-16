#ifndef WHAL_CLOCK_H
#define WHAL_CLOCK_H

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <stddef.h>

/*
 * @file clock.h
 * @brief Generic clock abstraction for configuring and controlling clock sources.
 */

typedef struct whal_Clock whal_Clock;

/*
 * @brief Driver vtable for clock devices.
 */
typedef struct {
    /* Initialize the clock hardware. */
    whal_Error (*Init)(whal_Clock *clkDev);
    /* Deinitialize the clock hardware. */
    whal_Error (*Deinit)(whal_Clock *clkDev);
    /* Enable the clock output. */
    whal_Error (*Enable)(whal_Clock *clkDev, const void *clk);
    /* Disable the clock output. */
    whal_Error (*Disable)(whal_Clock *clkDev, const void *clk);
} whal_ClockDriver;

/*
 * @brief Clock device instance tying a register map, driver, and configuration.
 */
struct whal_Clock {
    const whal_Regmap regmap;
    const whal_ClockDriver *driver;
    void *cfg;
};

/*
 * @brief Initializes a clock device and its backing driver.
 *
 * @param clkDev Pointer to the clock instance to bring up.
 *
 * @retval WHAL_SUCCESS  Driver-specific init routine ran successfully.
 * @retval WHAL_EINVAL   Null pointer or driver rejected the configuration.
 * @retval WHAL_ENOTIMPL Operation not implemented by this driver.
 */
#ifdef WHAL_CFG_DIRECT_CALLBACKS
#define whal_Clock_Init(clkDev) ((clkDev)->driver->Init((clkDev)))
#define whal_Clock_Deinit(clkDev) ((clkDev)->driver->Deinit((clkDev)))
#define whal_Clock_Enable(clkDev, clk) ((clkDev)->driver->Enable((clkDev), (clk)))
#define whal_Clock_Disable(clkDev, clk) ((clkDev)->driver->Disable((clkDev), (clk)))
#else
/*
 * @brief Initializes a clock device and its backing driver.
 *
 * @param clkDev Pointer to the clock instance to bring up.
 *
 * @retval WHAL_SUCCESS  Driver-specific init routine ran successfully.
 * @retval WHAL_EINVAL   Null pointer or driver rejected the configuration.
 * @retval WHAL_ENOTIMPL Operation not implemented by this driver.
 */
whal_Error whal_Clock_Init(whal_Clock *clkDev);
/*
 * @brief Releases resources previously allocated for a clock device.
 *
 * @param clkDev Pointer to the clock instance to tear down.
 *
 * @retval WHAL_SUCCESS  Driver-specific deinit routine ran successfully.
 * @retval WHAL_EINVAL   Null pointer or driver refused to deinitialize.
 * @retval WHAL_ENOTIMPL Operation not implemented by this driver.
 */
whal_Error whal_Clock_Deinit(whal_Clock *clkDev);
/*
 * @brief Enables the hardware clock so that downstream consumers can use it.
 *
 * @param clkDev Pointer to the clock instance to enable.
 *
 * @retval WHAL_SUCCESS  Clock was enabled (or already running).
 * @retval WHAL_EINVAL   Null pointer or driver-specific enable failed.
 * @retval WHAL_ENOTIMPL Operation not implemented by this driver.
 */
whal_Error whal_Clock_Enable(whal_Clock *clkDev, const void *clk);
/*
 * @brief Disables the hardware clock to save power or enforce resets.
 *
 * @param clkDev Pointer to the clock instance to disable.
 *
 * @retval WHAL_SUCCESS  Clock was disabled (or already stopped).
 * @retval WHAL_EINVAL   Null pointer or driver-specific disable failed.
 * @retval WHAL_ENOTIMPL Operation not implemented by this driver.
 */
whal_Error whal_Clock_Disable(whal_Clock *clkDev, const void *clk);
#endif

#endif /* WHAL_CLOCK_H */
