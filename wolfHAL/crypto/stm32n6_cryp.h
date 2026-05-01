#ifndef WHAL_STM32N6_CRYP_H
#define WHAL_STM32N6_CRYP_H

#include <stdint.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/timeout.h>

/**
 * @file stm32n6_cryp.h
 * @brief STM32N6 CRYP (cryptographic processor) driver.
 *
 * The CRYP peripheral on the STM32N6 supports AES-128/192/256 in ECB, CBC,
 * CTR, GCM, GMAC, and CCM chaining modes. It uses banked key registers
 * (K0LR/K0RR..K3LR/K3RR) and a four-word FIFO interface (DINR/DOUTR) — a
 * different programming model from the SAES peripheral.
 */

/**
 * @brief CRYP device configuration.
 */
typedef struct {
    whal_Timeout *timeout;

    /* Driver-internal state, not initialized by user. */
    uint8_t       ccmCtr0[16];
} whal_Stm32n6_Cryp_Cfg;

#ifndef WHAL_CFG_STM32N6_CRYP_DIRECT_API_MAPPING
/**
 * @brief Driver instance for the STM32N6 CRYP peripheral.
 */
extern const whal_CryptoDriver whal_Stm32n6_Cryp_Driver;

/**
 * @brief Initialize the CRYP peripheral.
 *
 * @param cryptoDev Crypto device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6_Cryp_Init(whal_Crypto *cryptoDev);

/**
 * @brief Deinitialize the CRYP peripheral.
 *
 * @param cryptoDev Crypto device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6_Cryp_Deinit(whal_Crypto *cryptoDev);

/**
 * @brief Start an AES operation: configure CR, load key/IV, process AAD.
 *
 * @param cryptoDev Crypto device instance.
 * @param opId      Operation identifier (e.g. WHAL_CRYPTO_AES_GCM).
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Operation started.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Unsupported opId or parameter.
 * @retval WHAL_EHARDWARE Hardware error during setup.
 */
whal_Error whal_Stm32n6_Cryp_StartOp(whal_Crypto *cryptoDev, size_t opId,
                                    void *opArgs);

/**
 * @brief Process payload data through an active AES operation.
 *
 * @param cryptoDev Crypto device instance.
 * @param opId      Operation identifier.
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Data processed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Unsupported opId.
 * @retval WHAL_EHARDWARE Hardware error during processing.
 */
whal_Error whal_Stm32n6_Cryp_Process(whal_Crypto *cryptoDev, size_t opId,
                                    void *opArgs);

/**
 * @brief End an AES operation: finalize, read tag, release hardware.
 *
 * @param cryptoDev Crypto device instance.
 * @param opId      Operation identifier.
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Operation finalized.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Unsupported opId.
 * @retval WHAL_EHARDWARE Hardware error during finalization.
 */
whal_Error whal_Stm32n6_Cryp_EndOp(whal_Crypto *cryptoDev, size_t opId,
                                  void *opArgs);
#endif /* !WHAL_CFG_STM32N6_CRYP_DIRECT_API_MAPPING */

#endif /* WHAL_STM32N6_CRYP_H */
