#ifndef WHAL_STM32WBA_I2C_H
#define WHAL_STM32WBA_I2C_H

/*
 * @file stm32wba_i2c.h
 * @brief STM32WBA I2C driver (alias for STM32WB I2C).
 *
 * The STM32WBA I2C peripheral is register-compatible with the STM32WB I2C.
 * This header re-exports the STM32WB I2C driver types and symbols under
 * STM32WBA-specific names.
 */

#include <wolfHAL/i2c/stm32wb_i2c.h>

typedef whal_Stm32wbI2c_Cfg whal_Stm32wbaI2c_Cfg;

#ifndef WHAL_CFG_I2C_API_MAPPING_STM32WBA
#define whal_Stm32wbaI2c_Driver   whal_Stm32wbI2c_Driver
#define whal_Stm32wbaI2c_Init     whal_Stm32wbI2c_Init
#define whal_Stm32wbaI2c_Deinit   whal_Stm32wbI2c_Deinit
#define whal_Stm32wbaI2c_StartCom whal_Stm32wbI2c_StartCom
#define whal_Stm32wbaI2c_EndCom   whal_Stm32wbI2c_EndCom
#define whal_Stm32wbaI2c_Transfer whal_Stm32wbI2c_Transfer
#endif /* !WHAL_CFG_I2C_API_MAPPING_STM32WBA */

#endif /* WHAL_STM32WBA_I2C_H */
