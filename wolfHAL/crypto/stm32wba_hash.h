#ifndef WHAL_STM32WBA_HASH_H
#define WHAL_STM32WBA_HASH_H

#include <stdint.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32wba_hash.h
 * @brief STM32WBA HASH hardware accelerator driver.
 *
 * The STM32WBA HASH peripheral supports SHA-1, SHA-224, SHA-256 and HMAC
 * variants. This driver exposes those algorithms through the generic
 * whal_Crypto interface using the StartOp/Process/EndOp pattern.
 */

/*
 * @brief HASH device configuration.
 */
typedef struct {
    whal_Timeout *timeout;
} whal_Stm32wba_Hash_Cfg;

#ifndef WHAL_CFG_STM32WBA_HASH_DIRECT_API_MAPPING
/*
 * @brief Driver instance for STM32WBA HASH peripheral.
 */
extern const whal_CryptoDriver whal_Stm32wba_Hash_Driver;

/*
 * @brief Initialize the STM32WBA HASH peripheral.
 *
 * @param cryptoDev Crypto device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wba_Hash_Init(whal_Crypto *cryptoDev);

/*
 * @brief Deinitialize the STM32WBA HASH peripheral.
 *
 * @param cryptoDev Crypto device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wba_Hash_Deinit(whal_Crypto *cryptoDev);

/*
 * @brief Start a hash/HMAC operation.
 *
 * @param cryptoDev Crypto device instance.
 * @param opId      Algorithm identifier (WHAL_CRYPTO_SHA* / WHAL_CRYPTO_HMAC_SHA*).
 * @param opArgs    Pointer to whal_Crypto_HashArgs or whal_Crypto_HmacArgs.
 *
 * @retval WHAL_SUCCESS   Operation started.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during setup.
 */
whal_Error whal_Stm32wba_Hash_StartOp(whal_Crypto *cryptoDev, size_t opId,
                                     void *opArgs);

/*
 * @brief Feed data into an active hash/HMAC operation.
 *
 * @param cryptoDev Crypto device instance.
 * @param opId      Algorithm identifier.
 * @param opArgs    Pointer to whal_Crypto_HashArgs or whal_Crypto_HmacArgs.
 *
 * @retval WHAL_SUCCESS   Data processed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during processing.
 */
whal_Error whal_Stm32wba_Hash_Process(whal_Crypto *cryptoDev, size_t opId,
                                     void *opArgs);

/*
 * @brief End a hash/HMAC operation and read the digest.
 *
 * @param cryptoDev Crypto device instance.
 * @param opId      Algorithm identifier.
 * @param opArgs    Pointer to whal_Crypto_HashArgs or whal_Crypto_HmacArgs.
 *
 * @retval WHAL_SUCCESS   Operation finalized.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during finalization.
 */
whal_Error whal_Stm32wba_Hash_EndOp(whal_Crypto *cryptoDev, size_t opId,
                                   void *opArgs);
#endif /* !WHAL_CFG_STM32WBA_HASH_DIRECT_API_MAPPING */

#endif /* WHAL_STM32WBA_HASH_H */
