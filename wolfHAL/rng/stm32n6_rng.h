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

typedef whal_Stm32wbaRng_Cfg whal_Stm32n6Rng_Cfg;

#ifndef WHAL_CFG_RNG_API_MAPPING_STM32N6
#define whal_Stm32n6Rng_Driver   whal_Stm32wbaRng_Driver
#define whal_Stm32n6Rng_Init     whal_Stm32wbaRng_Init
#define whal_Stm32n6Rng_Deinit   whal_Stm32wbaRng_Deinit
#define whal_Stm32n6Rng_Generate whal_Stm32wbaRng_Generate
#endif /* !WHAL_CFG_RNG_API_MAPPING_STM32N6 */

#endif /* WHAL_STM32N6_RNG_H */
