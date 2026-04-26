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

typedef whal_Stm32wbaHash_Cfg whal_Stm32n6Hash_Cfg;

#ifndef WHAL_CFG_CRYPTO_API_MAPPING_STM32N6_HASH
#define whal_Stm32n6Hash_Driver  whal_Stm32wbaHash_Driver
#define whal_Stm32n6Hash_Init    whal_Stm32wbaHash_Init
#define whal_Stm32n6Hash_Deinit  whal_Stm32wbaHash_Deinit
#define whal_Stm32n6Hash_StartOp whal_Stm32wbaHash_StartOp
#define whal_Stm32n6Hash_Process whal_Stm32wbaHash_Process
#define whal_Stm32n6Hash_EndOp   whal_Stm32wbaHash_EndOp
#endif /* !WHAL_CFG_CRYPTO_API_MAPPING_STM32N6_HASH */

#endif /* WHAL_STM32N6_HASH_H */
