#ifndef WHAL_STM32N6_RNG_H
#define WHAL_STM32N6_RNG_H

#include <stdint.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32n6_rng.h
 * @brief STM32N6 RNG driver configuration.
 *
 * The STM32N6 true random number generator provides 32-bit random values
 * from an analog noise source with NIST SP800-90B conditioning. It features
 * a 4-word output FIFO and requires a CONDRST sequence to apply configuration
 * changes. This driver uses Configuration B from RM0486 Table 398.
 */

typedef struct whal_Stm32n6Rng_Cfg {
    whal_Timeout *timeout;
} whal_Stm32n6Rng_Cfg;

extern const whal_RngDriver whal_Stm32n6Rng_Driver;

whal_Error whal_Stm32n6Rng_Init(whal_Rng *rngDev);
whal_Error whal_Stm32n6Rng_Deinit(whal_Rng *rngDev);
whal_Error whal_Stm32n6Rng_Generate(whal_Rng *rngDev, void *rngData,
                                     size_t rngDataSz);

#endif /* WHAL_STM32N6_RNG_H */
