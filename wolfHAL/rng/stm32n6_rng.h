#ifndef WHAL_STM32N6_RNG_H
#define WHAL_STM32N6_RNG_H

/**
 * @file stm32n6_rng.h
 * @brief STM32N6 RNG driver (alias for STM32WBA RNG).
 *
 * The STM32N6 RNG peripheral is register-compatible with the STM32WBA RNG
 * (CR/SR/DR/NSCR/HTCR at identical offsets). This header re-exports under
 * STM32N6-specific names.
 */

#include <wolfHAL/rng/stm32wba_rng.h>

typedef whal_Stm32wba_Rng_Cfg whal_Stm32n6_Rng_Cfg;

#ifndef WHAL_CFG_STM32N6_RNG_DIRECT_API_MAPPING
#define whal_Stm32n6_Rng_Driver   whal_Stm32wba_Rng_Driver
#define whal_Stm32n6_Rng_Init     whal_Stm32wba_Rng_Init
#define whal_Stm32n6_Rng_Deinit   whal_Stm32wba_Rng_Deinit
#define whal_Stm32n6_Rng_Generate whal_Stm32wba_Rng_Generate
#endif /* !WHAL_CFG_STM32N6_RNG_DIRECT_API_MAPPING */

#endif /* WHAL_STM32N6_RNG_H */
