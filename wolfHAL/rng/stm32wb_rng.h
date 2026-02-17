#ifndef WHAL_STM32WB_RNG_H
#define WHAL_STM32WB_RNG_H

#include <stdint.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/clock/clock.h>

/*
 * @file stm32wb_rng.h
 * @brief STM32WB RNG driver configuration.
 *
 * The STM32WB true random number generator provides 32-bit random values
 * from an analog noise source. The peripheral requires the RNG clock to
 * be enabled and produces one 32-bit word at a time via the DR register.
 */

/*
 * @brief RNG device configuration.
 */
typedef struct whal_Stm32wbRng_Cfg {
    whal_Clock *clkCtrl; /* Clock controller for RNG peripheral clock */
    const void *clk;     /* Clock descriptor */
} whal_Stm32wbRng_Cfg;

/*
 * @brief Driver instance for STM32WB RNG peripheral.
 */
extern const whal_RngDriver whal_Stm32wbRng_Driver;

/*
 * @brief Initialize the STM32WB RNG peripheral.
 *
 * @param rngDev RNG device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRng_Init(whal_Rng *rngDev);
/*
 * @brief Deinitialize the STM32WB RNG peripheral.
 *
 * @param rngDev RNG device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbRng_Deinit(whal_Rng *rngDev);
/*
 * @brief Generate random data.
 *
 * @param rngDev    RNG device instance.
 * @param rngData   Destination buffer.
 * @param rngDataSz Number of random bytes to generate.
 *
 * @retval WHAL_SUCCESS Buffer filled with random data.
 * @retval WHAL_EINVAL  Invalid arguments or seed/clock error detected.
 */
whal_Error whal_Stm32wbRng_Generate(whal_Rng *rngDev, uint8_t *rngData, size_t rngDataSz);

#endif /* WHAL_STM32WB_RNG_H */
