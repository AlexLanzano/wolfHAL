#ifndef WHAL_STM32WBA_RNG_H
#define WHAL_STM32WBA_RNG_H

#include <wolfHAL/rng/rng.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32wba_rng.h
 * @brief STM32WBA RNG driver configuration.
 *
 * The RNG kernel clock source must be selected in RCC_CCIPR2.RNGSEL
 * before using the RNG. Default after reset is LSE (often not enabled).
 */

typedef struct whal_Stm32wbaRng_Cfg {
    whal_Timeout *timeout;
} whal_Stm32wbaRng_Cfg;

extern const whal_RngDriver whal_Stm32wbaRng_Driver;

whal_Error whal_Stm32wbaRng_Init(whal_Rng *rngDev);
whal_Error whal_Stm32wbaRng_Deinit(whal_Rng *rngDev);
whal_Error whal_Stm32wbaRng_Generate(whal_Rng *rngDev, void *rngData, size_t rngDataSz);

#endif /* WHAL_STM32WBA_RNG_H */
