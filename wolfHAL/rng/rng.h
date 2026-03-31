#ifndef WHAL_RNG_H
#define WHAL_RNG_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/error.h>

/*
 * @file rng.h
 * @brief Generic RNG abstraction and driver interface.
 */

typedef struct whal_Rng whal_Rng;

/*
 * @brief Driver vtable for RNG devices.
 */
typedef struct {
    /* Initialize the RNG hardware. */
    whal_Error (*Init)(whal_Rng *rngDev);
    /* Deinitialize the RNG hardware. */
    whal_Error (*Deinit)(whal_Rng *rngDev);
    /* Generate random data into a buffer. */
    whal_Error (*Generate)(whal_Rng *rngDev, void *rngData, size_t rngDataSz);
} whal_RngDriver;

/*
 * @brief RNG device instance tying a register map and driver.
 */
struct whal_Rng {
    const whal_Regmap regmap;
    const whal_RngDriver *driver;
    void *cfg;
};

/*
 * @brief Initialize an RNG device and its driver.
 *
 * @param rngDev RNG instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
#ifdef WHAL_CFG_DIRECT_CALLBACKS
#define whal_Rng_Init(rngDev) ((rngDev)->driver->Init((rngDev)))
#define whal_Rng_Deinit(rngDev) ((rngDev)->driver->Deinit((rngDev)))
#define whal_Rng_Generate(rngDev, rngData, rngDataSz) \
    ((rngDev)->driver->Generate((rngDev), (rngData), (rngDataSz)))
#else
/*
 * @brief Initialize an RNG device and its driver.
 *
 * @param rngDev RNG instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Rng_Init(whal_Rng *rngDev);
/*
 * @brief Deinitialize an RNG device and release resources.
 *
 * @param rngDev RNG instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Rng_Deinit(whal_Rng *rngDev);
/*
 * @brief Generate random data into a buffer.
 *
 * @param rngDev    RNG instance to use.
 * @param rngData   Destination buffer.
 * @param rngDataSz Number of random bytes to generate.
 *
 * @retval WHAL_SUCCESS Buffer filled with random data.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Rng_Generate(whal_Rng *rngDev, void *rngData, size_t rngDataSz);
#endif

#endif /* WHAL_RNG_H */
