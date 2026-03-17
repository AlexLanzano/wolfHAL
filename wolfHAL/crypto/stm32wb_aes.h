#ifndef WHAL_STM32WB_AES_H
#define WHAL_STM32WB_AES_H

#include <stdint.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32wb_aes.h
 * @brief STM32WB AES hardware accelerator driver.
 *
 * The STM32WB AES1 peripheral supports 128/256-bit keys in ECB, CBC,
 * CTR, GCM, GMAC, and CCM modes. This driver exposes those modes through the
 * generic whal_Crypto Op interface.
 */

/*
 * @brief AES device configuration.
 */
typedef struct {
    whal_Clock *clkCtrl;
    const void *clk;
    whal_Timeout *timeout;
} whal_Stm32wbAes_Cfg;

/*
 * @brief Driver instance for STM32WB AES peripheral.
 */
extern const whal_CryptoDriver whal_Stm32wbAes_Driver;

/*
 * @brief Initialize the STM32WB AES peripheral.
 *
 * @param cryptoDev Crypto device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbAes_Init(whal_Crypto *cryptoDev);

/*
 * @brief Deinitialize the STM32WB AES peripheral.
 *
 * @param cryptoDev Crypto device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbAes_Deinit(whal_Crypto *cryptoDev);


/*
 * @brief Perform AES-ECB encrypt or decrypt.
 *
 * Compatible with whal_Crypto_OpFunc. Cast opArgs to whal_Crypto_AesEcbArgs.
 *
 * @param cryptoDev Crypto device instance.
 * @param opArgs    Pointer to whal_Crypto_AesEcbArgs.
 *
 * @retval WHAL_SUCCESS   Operation completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during operation.
 */
whal_Error whal_Stm32wbAes_AesEcb(whal_Crypto *cryptoDev, void *opArgs);

/*
 * @brief Perform AES-CBC encrypt or decrypt.
 *
 * Compatible with whal_Crypto_OpFunc. Cast opArgs to whal_Crypto_AesCbcArgs.
 *
 * @param cryptoDev Crypto device instance.
 * @param opArgs    Pointer to whal_Crypto_AesCbcArgs.
 *
 * @retval WHAL_SUCCESS   Operation completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during operation.
 */
whal_Error whal_Stm32wbAes_AesCbc(whal_Crypto *cryptoDev, void *opArgs);

/*
 * @brief Perform AES-CTR encrypt or decrypt.
 *
 * Compatible with whal_Crypto_OpFunc. Cast opArgs to whal_Crypto_AesCtrArgs.
 *
 * @param cryptoDev Crypto device instance.
 * @param opArgs    Pointer to whal_Crypto_AesCtrArgs.
 *
 * @retval WHAL_SUCCESS   Operation completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during operation.
 */
whal_Error whal_Stm32wbAes_AesCtr(whal_Crypto *cryptoDev, void *opArgs);

/*
 * @brief Perform AES-GCM encrypt or decrypt.
 *
 * Compatible with whal_Crypto_OpFunc. Cast opArgs to whal_Crypto_AesGcmArgs.
 *
 * @param cryptoDev Crypto device instance.
 * @param opArgs    Pointer to whal_Crypto_AesGcmArgs.
 *
 * @retval WHAL_SUCCESS   Operation completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during operation.
 */
whal_Error whal_Stm32wbAes_AesGcm(whal_Crypto *cryptoDev, void *opArgs);

/*
 * @brief Perform AES-GMAC authentication (no payload).
 *
 * Compatible with whal_Crypto_OpFunc. Cast opArgs to whal_Crypto_AesGmacArgs.
 *
 * @param cryptoDev Crypto device instance.
 * @param opArgs    Pointer to whal_Crypto_AesGmacArgs.
 *
 * @retval WHAL_SUCCESS   Operation completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during operation.
 */
whal_Error whal_Stm32wbAes_AesGmac(whal_Crypto *cryptoDev, void *opArgs);

/*
 * @brief Perform AES-CCM encrypt or decrypt.
 *
 * Compatible with whal_Crypto_OpFunc. Cast opArgs to whal_Crypto_AesCcmArgs.
 *
 * @param cryptoDev Crypto device instance.
 * @param opArgs    Pointer to whal_Crypto_AesCcmArgs.
 *
 * @retval WHAL_SUCCESS   Operation completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Hardware error during operation.
 */
whal_Error whal_Stm32wbAes_AesCcm(whal_Crypto *cryptoDev, void *opArgs);

#endif /* WHAL_STM32WB_AES_H */
