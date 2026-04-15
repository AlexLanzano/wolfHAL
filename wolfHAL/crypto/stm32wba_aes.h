#ifndef WHAL_STM32WBA_AES_H
#define WHAL_STM32WBA_AES_H

/*
 * @file stm32wba_aes.h
 * @brief STM32WBA AES driver (alias for STM32WB AES).
 *
 * The STM32WBA AES peripheral is register-compatible with the STM32WB AES.
 * This header re-exports the STM32WB AES driver types and symbols under
 * STM32WBA-specific names.
 */

#include <wolfHAL/crypto/stm32wb_aes.h>

typedef whal_Stm32wbAes_Cfg whal_Stm32wbaAes_Cfg;

#ifndef WHAL_CFG_CRYPTO_API_MAPPING_STM32WBA_AES
#define whal_Stm32wbaAes_Driver  whal_Stm32wbAes_Driver
#define whal_Stm32wbaAes_Init    whal_Stm32wbAes_Init
#define whal_Stm32wbaAes_Deinit  whal_Stm32wbAes_Deinit
#define whal_Stm32wbaAes_StartOp whal_Stm32wbAes_StartOp
#define whal_Stm32wbaAes_Process whal_Stm32wbAes_Process
#define whal_Stm32wbaAes_EndOp   whal_Stm32wbAes_EndOp
#endif /* !WHAL_CFG_CRYPTO_API_MAPPING_STM32WBA_AES */

#endif /* WHAL_STM32WBA_AES_H */
