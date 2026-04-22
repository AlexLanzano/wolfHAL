#ifndef WHAL_STM32F3_I2C_H
#define WHAL_STM32F3_I2C_H

/*
 * @file stm32f3_i2c.h
 * @brief STM32F3 I2C driver (alias for STM32WB I2C).
 *
 * The STM32F3 I2C peripheral is register-compatible with the STM32WB I2C
 * (I2C v2 with CR1/CR2/TIMINGR).
 */

#include <wolfHAL/i2c/stm32wb_i2c.h>

typedef whal_Stm32wbI2c_Cfg whal_Stm32f3I2c_Cfg;

#ifndef WHAL_CFG_I2C_API_MAPPING_STM32F3
#define whal_Stm32f3I2c_Driver   whal_Stm32wbI2c_Driver
#define whal_Stm32f3I2c_Init     whal_Stm32wbI2c_Init
#define whal_Stm32f3I2c_Deinit   whal_Stm32wbI2c_Deinit
#define whal_Stm32f3I2c_StartCom whal_Stm32wbI2c_StartCom
#define whal_Stm32f3I2c_EndCom   whal_Stm32wbI2c_EndCom
#define whal_Stm32f3I2c_Transfer whal_Stm32wbI2c_Transfer
#endif /* !WHAL_CFG_I2C_API_MAPPING_STM32F3 */

#endif /* WHAL_STM32F3_I2C_H */
