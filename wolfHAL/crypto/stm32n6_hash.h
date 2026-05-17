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

typedef whal_Stm32wba_HmacSha1_State   whal_Stm32n6_HmacSha1_State;
typedef whal_Stm32wba_HmacSha224_State  whal_Stm32n6_HmacSha224_State;
typedef whal_Stm32wba_HmacSha256_State  whal_Stm32n6_HmacSha256_State;

#define whal_Stm32n6_Hash_Dev       whal_Stm32wba_Hash_Dev
#define whal_Stm32n6_Sha1_Dev       whal_Stm32wba_Sha1_Dev
#define whal_Stm32n6_Sha224_Dev     whal_Stm32wba_Sha224_Dev
#define whal_Stm32n6_Sha256_Dev     whal_Stm32wba_Sha256_Dev
#define whal_Stm32n6_HmacSha1_Dev   whal_Stm32wba_HmacSha1_Dev
#define whal_Stm32n6_HmacSha224_Dev whal_Stm32wba_HmacSha224_Dev
#define whal_Stm32n6_HmacSha256_Dev whal_Stm32wba_HmacSha256_Dev

#define whal_Stm32n6_Hash_CryptoDriver    whal_Stm32wba_Hash_CryptoDriver

#define whal_Stm32n6_Hash_Sha1Driver      whal_Stm32wba_Hash_Sha1Driver
#define whal_Stm32n6_Hash_Sha224Driver    whal_Stm32wba_Hash_Sha224Driver
#define whal_Stm32n6_Hash_Sha256Driver    whal_Stm32wba_Hash_Sha256Driver
#define whal_Stm32n6_Hash_HmacSha1Driver  whal_Stm32wba_Hash_HmacSha1Driver
#define whal_Stm32n6_Hash_HmacSha224Driver whal_Stm32wba_Hash_HmacSha224Driver
#define whal_Stm32n6_Hash_HmacSha256Driver whal_Stm32wba_Hash_HmacSha256Driver

#endif /* WHAL_STM32N6_HASH_H */
