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

typedef whal_Stm32wb_Aes_Cfg whal_Stm32wba_Aes_Cfg;

#ifndef WHAL_CFG_STM32WBA_AES_DIRECT_API_MAPPING
#define whal_Stm32wba_Aes_Driver  whal_Stm32wb_Aes_Driver
#define whal_Stm32wba_Aes_Init    whal_Stm32wb_Aes_Init
#define whal_Stm32wba_Aes_Deinit  whal_Stm32wb_Aes_Deinit
#define whal_Stm32wba_Aes_StartOp whal_Stm32wb_Aes_StartOp
#define whal_Stm32wba_Aes_Process whal_Stm32wb_Aes_Process
#define whal_Stm32wba_Aes_EndOp   whal_Stm32wb_Aes_EndOp
#endif /* !WHAL_CFG_STM32WBA_AES_DIRECT_API_MAPPING */

#endif /* WHAL_STM32WBA_AES_H */
