#ifndef WHAL_STM32H5_RNG_H
#define WHAL_STM32H5_RNG_H

#include <stdint.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32h5_rng.h
 * @brief STM32H5 RNG driver configuration.
 *
 * The STM32H5 true random number generator provides 32-bit random values
 * from an analog noise source with NIST SP800-90B conditioning. It features
 * a 4-word output FIFO and requires a CONDRST sequence to apply configuration
 * changes. This driver uses the NIST-certified configuration from AN4230.
 */

/*
 * @brief RNG device configuration.
 */
typedef struct whal_Stm32h5Rng_Cfg {
    whal_Timeout *timeout;
} whal_Stm32h5Rng_Cfg;

/*
 * @brief Driver instance for STM32H5 RNG peripheral.
 */
extern const whal_RngDriver whal_Stm32h5Rng_Driver;

/*
 * @brief Initialize the STM32H5 RNG peripheral.
 *
 * @param rngDev RNG device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Rng_Init(whal_Rng *rngDev);

/*
 * @brief Deinitialize the STM32H5 RNG peripheral.
 *
 * @param rngDev RNG device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Rng_Deinit(whal_Rng *rngDev);

/*
 * @brief Generate random data.
 *
 * Polls for DRDY and fills the output buffer. The RNG must be
 * initialized via Init() before calling this function.
 *
 * @param rngDev    RNG device instance.
 * @param rngData   Destination buffer.
 * @param rngDataSz Number of random bytes to generate.
 *
 * @retval WHAL_SUCCESS   Buffer filled with random data.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Seed or clock error detected.
 * @retval WHAL_ETIMEOUT  Timed out waiting for random data.
 */
whal_Error whal_Stm32h5Rng_Generate(whal_Rng *rngDev, void *rngData,
                                     size_t rngDataSz);

#endif /* WHAL_STM32H5_RNG_H */
