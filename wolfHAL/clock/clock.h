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
    /* Read back the effective clock rate. */
    whal_Error (*GetRate)(whal_Clock *clkDev, size_t *rate);
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
 * @retval WHAL_SUCCESS Driver-specific init routine ran successfully.
 * @retval WHAL_EINVAL  Null pointer or driver rejected the configuration.
 */
#ifdef WHAL_CFG_NO_CALLBACKS
#define whal_Clock_Init(clkDev) ((clkDev)->driver->Init((clkDev)))
#define whal_Clock_Deinit(clkDev) ((clkDev)->driver->Deinit((clkDev)))
#define whal_Clock_Enable(clkDev, clk) ((clkDev)->driver->Enable((clkDev), (clk)))
#define whal_Clock_Disable(clkDev, clk) ((clkDev)->driver->Disable((clkDev), (clk)))
#define whal_Clock_GetRate(clkDev, rate) ((clkDev)->driver->GetRate((clkDev), (rate)))
#else
/*
 * @brief Initializes a clock device and its backing driver.
 *
 * @param clkDev Pointer to the clock instance to bring up.
 *
 * @retval WHAL_SUCCESS Driver-specific init routine ran successfully.
 * @retval WHAL_EINVAL  Null pointer or driver rejected the configuration.
 */
whal_Error whal_Clock_Init(whal_Clock *clkDev);
/*
 * @brief Releases resources previously allocated for a clock device.
 *
 * @param clkDev Pointer to the clock instance to tear down.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit routine ran successfully.
 * @retval WHAL_EINVAL  Null pointer or driver refused to deinitialize.
 */
whal_Error whal_Clock_Deinit(whal_Clock *clkDev);
/*
 * @brief Enables the hardware clock so that downstream consumers can use it.
 *
 * @param clkDev Pointer to the clock instance to enable.
 *
 * @retval WHAL_SUCCESS Clock was enabled (or already running).
 * @retval WHAL_EINVAL  Null pointer or driver-specific enable failed.
 */
whal_Error whal_Clock_Enable(whal_Clock *clkDev, const void *clk);
/*
 * @brief Disables the hardware clock to save power or enforce resets.
 *
 * @param clkDev Pointer to the clock instance to disable.
 *
 * @retval WHAL_SUCCESS Clock was disabled (or already stopped).
 * @retval WHAL_EINVAL  Null pointer or driver-specific disable failed.
 */
whal_Error whal_Clock_Disable(whal_Clock *clkDev, const void *clk);
/*
 * @brief Reports the current output rate for a clock device.
 *
 * @param clkDev  Pointer to the clock instance being queried.
 * @param rate    Storage for the computed frequency in Hz.
 *
 * @retval WHAL_SUCCESS Result stored in @p rate.
 * @retval WHAL_EINVAL  Null pointer or driver could not provide a rate.
 */
whal_Error whal_Clock_GetRate(whal_Clock *clkDev, size_t *rate);
#endif

#endif /* WHAL_CLOCK_H */
