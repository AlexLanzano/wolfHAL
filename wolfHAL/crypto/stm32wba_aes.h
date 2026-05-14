#ifndef WHAL_STM32WBA_AES_H
#define WHAL_STM32WBA_AES_H

/**
 * @file stm32wba_aes.h
 * @brief STM32WBA AES driver (alias for STM32WB AES).
 *
 * The STM32WBA AES peripheral is register-compatible with the STM32WB AES.
 * This header re-exports the STM32WB AES driver types and symbols under
 * STM32WBA-specific names.
 */

#include <wolfHAL/crypto/stm32wb_aes.h>

typedef whal_Stm32wb_Aes_Cfg whal_Stm32wba_Aes_Cfg;

typedef whal_Stm32wb_AesGcm_State whal_Stm32wba_AesGcm_State;
typedef whal_Stm32wb_AesCcm_State whal_Stm32wba_AesCcm_State;

#define whal_Stm32wba_Aes_Dev     whal_Stm32wb_Aes_Dev
#define whal_Stm32wba_AesEcb_Dev  whal_Stm32wb_AesEcb_Dev
#define whal_Stm32wba_AesCbc_Dev  whal_Stm32wb_AesCbc_Dev
#define whal_Stm32wba_AesCtr_Dev  whal_Stm32wb_AesCtr_Dev
#define whal_Stm32wba_AesGcm_Dev  whal_Stm32wb_AesGcm_Dev
#define whal_Stm32wba_AesGmac_Dev whal_Stm32wb_AesGmac_Dev
#define whal_Stm32wba_AesCcm_Dev  whal_Stm32wb_AesCcm_Dev

#define whal_Stm32wba_Aes_CryptoDriver whal_Stm32wb_Aes_CryptoDriver

#define whal_Stm32wba_Aes_EcbDriver  whal_Stm32wb_Aes_EcbDriver
#define whal_Stm32wba_Aes_CbcDriver  whal_Stm32wb_Aes_CbcDriver
#define whal_Stm32wba_Aes_CtrDriver  whal_Stm32wb_Aes_CtrDriver
#define whal_Stm32wba_Aes_GcmDriver  whal_Stm32wb_Aes_GcmDriver
#define whal_Stm32wba_Aes_GmacDriver whal_Stm32wb_Aes_GmacDriver
#define whal_Stm32wba_Aes_CcmDriver  whal_Stm32wb_Aes_CcmDriver


#endif /* WHAL_STM32WBA_AES_H */
