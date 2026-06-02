/* stm32f4_i2c.h
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

#ifndef WHAL_STM32F4_I2C_H
#define WHAL_STM32F4_I2C_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/i2c/i2c.h>
#include <wolfHAL/timeout.h>

/**
 * @file stm32f4_i2c.h
 * @brief STM32F4 I2C V1 driver configuration.
 *
 * The STM32F4 I2C peripheral uses the legacy I2Cv1 register set
 * (CR1/CR2/OAR1/OAR2/DR/SR1/SR2/CCR/TRISE/FLTR). The driver is
 * polling-based controller-only — START, byte read/write, and STOP
 * are driven from the foreground. 7-bit addressing only.
 *
 * Per-session frequency: the board provides f_PCLK1 in the cfg; the
 * driver computes CCR and TRISE from it at StartCom time.
 */

/**
 * @brief STM32F4 I2C configuration.
 */
typedef struct whal_Stm32f4_I2c_Cfg {
    uint32_t pclk1;       /**< APB1 clock feeding the peripheral, in Hz. */
    whal_Timeout *timeout;
} whal_Stm32f4_I2c_Cfg;

#if defined(WHAL_CFG_STM32F4_I2C_SINGLE_INSTANCE)
extern const whal_I2c whal_Stm32f4_I2c_Dev;
#endif

#ifndef WHAL_CFG_STM32F4_I2C_DIRECT_API_MAPPING
/**
 * @brief Driver vtable for STM32F4 I2C.
 */
extern const whal_I2cDriver whal_Stm32f4_I2c_Driver;

whal_Error whal_Stm32f4_I2c_Init(whal_I2c *i2cDev);
whal_Error whal_Stm32f4_I2c_Deinit(whal_I2c *i2cDev);
whal_Error whal_Stm32f4_I2c_StartCom(whal_I2c *i2cDev, whal_I2c_ComCfg *comCfg);
whal_Error whal_Stm32f4_I2c_EndCom(whal_I2c *i2cDev);
whal_Error whal_Stm32f4_I2c_Transfer(whal_I2c *i2cDev, whal_I2c_Msg *msgs,
                                     size_t numMsgs);
#endif /* !WHAL_CFG_STM32F4_I2C_DIRECT_API_MAPPING */

#endif /* WHAL_STM32F4_I2C_H */
