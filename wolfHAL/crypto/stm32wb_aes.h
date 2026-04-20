#ifndef WHAL_STM32WB_AES_H
#define WHAL_STM32WB_AES_H

#include <stdint.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32wb_aes.h
 * @brief STM32WB AES hardware accelerator driver.
 *
 * The STM32WB AES1 peripheral supports 128/256-bit keys in ECB, CBC,
 * CTR, GCM, GMAC, and CCM modes. This driver exposes those modes through the
 * generic whal_Crypto interface using the StartOp/Process/EndOp pattern.
 */

/*
 * @brief AES device configuration.
 */
typedef struct {
    whal_Timeout *timeout;
} whal_Stm32wbAes_Cfg;

#ifndef WHAL_CFG_CRYPTO_API_MAPPING_STM32WB_AES
/*
 * @brief Driver instance for STM32WB AES peripheral.
 */
extern const whal_CryptoDriver whal_Stm32wbAes_Driver;

/*
 * @brief Initialize the STM32WB AES peripheral.
 *
 * @param cryptoDev Cipher device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbAes_Init(whal_Crypto *cryptoDev);

/*
 * @brief Deinitialize the STM32WB AES peripheral.
 *
 * @param cryptoDev Cipher device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbAes_Deinit(whal_Crypto *cryptoDev);

/*
 * @brief Start an AES cipher operation.
 *
 * @param cryptoDev Cipher device instance.
 * @param opId  Cipher algorithm identifier.
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Operation started.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Unsupported opId or parameter (e.g. key size).
 * @retval WHAL_EHARDWARE Hardware error during setup.
 */
whal_Error whal_Stm32wbAes_StartOp(whal_Crypto *cryptoDev, size_t opId,
                                   void *opArgs);

/*
 * @brief Process data through an active AES operation.
 *
 * @param cryptoDev Cipher device instance.
 * @param opId  Cipher algorithm identifier.
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Data processed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Unsupported opId.
 * @retval WHAL_EHARDWARE Hardware error during processing.
 */
whal_Error whal_Stm32wbAes_Process(whal_Crypto *cryptoDev, size_t opId,
                                   void *opArgs);

/*
 * @brief End an AES cipher operation.
 *
 * @param cryptoDev Cipher device instance.
 * @param opId  Cipher algorithm identifier.
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Operation finalized.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Unsupported opId.
 * @retval WHAL_EHARDWARE Hardware error during finalization.
 */
whal_Error whal_Stm32wbAes_EndOp(whal_Crypto *cryptoDev, size_t opId,
                                 void *opArgs);
#endif /* !WHAL_CFG_CRYPTO_API_MAPPING_STM32WB_AES */

#endif /* WHAL_STM32WB_AES_H */
