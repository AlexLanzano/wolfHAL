/* stm32wba_i2c.h
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

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

typedef whal_Stm32wb_I2c_Cfg whal_Stm32wba_I2c_Cfg;

#ifndef WHAL_CFG_STM32WBA_I2C_DIRECT_API_MAPPING
#define whal_Stm32wba_I2c_Driver   whal_Stm32wb_I2c_Driver
#define whal_Stm32wba_I2c_Init     whal_Stm32wb_I2c_Init
#define whal_Stm32wba_I2c_Deinit   whal_Stm32wb_I2c_Deinit
#define whal_Stm32wba_I2c_StartCom whal_Stm32wb_I2c_StartCom
#define whal_Stm32wba_I2c_EndCom   whal_Stm32wb_I2c_EndCom
#define whal_Stm32wba_I2c_Transfer whal_Stm32wb_I2c_Transfer
#endif /* !WHAL_CFG_STM32WBA_I2C_DIRECT_API_MAPPING */

#endif /* WHAL_STM32WBA_I2C_H */
