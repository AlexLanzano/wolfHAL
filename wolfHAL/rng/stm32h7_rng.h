#ifndef WHAL_STM32H7_RNG_H
#define WHAL_STM32H7_RNG_H

/*
 * @file stm32h7_rng.h
 * @brief STM32H7 RNG driver (alias for STM32WB RNG).
 *
 * The STM32H7 RNG peripheral uses the same simple CR/SR/DR register layout
 * as the STM32WB RNG. No CONDRST sequence is required. This header
 * re-exports the STM32WB RNG driver types and symbols under STM32H7-specific
 * names.
 */

#include <wolfHAL/rng/stm32wb_rng.h>

typedef whal_Stm32wbRng_Cfg whal_Stm32h7Rng_Cfg;

#define whal_Stm32h7Rng_Driver   whal_Stm32wbRng_Driver
#define whal_Stm32h7Rng_Init     whal_Stm32wbRng_Init
#define whal_Stm32h7Rng_Deinit   whal_Stm32wbRng_Deinit
#define whal_Stm32h7Rng_Generate whal_Stm32wbRng_Generate

#endif /* WHAL_STM32H7_RNG_H */
