#ifndef WHAL_STM32N6_HASH_H
#define WHAL_STM32N6_HASH_H

/**
 * @file stm32n6_hash.h
 * @brief STM32N6 HASH driver (alias for STM32WBA HASH).
 *
 * The STM32N6 HASH peripheral is register-compatible with the STM32WBA HASH
 * (CR/DIN/STR/HRAx/IMR/SR/CSRx at identical offsets). This header re-exports
 * under STM32N6-specific names.
 */

#include <wolfHAL/crypto/stm32wba_hash.h>

typedef whal_Stm32wba_Hash_Cfg whal_Stm32n6_Hash_Cfg;

#ifndef WHAL_CFG_STM32N6_HASH_DIRECT_API_MAPPING
#define whal_Stm32n6_Hash_Driver  whal_Stm32wba_Hash_Driver
#define whal_Stm32n6_Hash_Init    whal_Stm32wba_Hash_Init
#define whal_Stm32n6_Hash_Deinit  whal_Stm32wba_Hash_Deinit
#define whal_Stm32n6_Hash_StartOp whal_Stm32wba_Hash_StartOp
#define whal_Stm32n6_Hash_Process whal_Stm32wba_Hash_Process
#define whal_Stm32n6_Hash_EndOp   whal_Stm32wba_Hash_EndOp
#endif /* !WHAL_CFG_STM32N6_HASH_DIRECT_API_MAPPING */

#endif /* WHAL_STM32N6_HASH_H */
