/* stm32f4_rng.h
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

#ifndef WHAL_STM32F4_RNG_H
#define WHAL_STM32F4_RNG_H

/**
 * @file stm32f4_rng.h
 * @brief STM32F4 RNG driver (alias for STM32WB RNG).
 *
 * The STM32F4 RNG is register-compatible with the STM32WB RNG
 * (CR/SR/DR at 0x00/0x04/0x08, same RNGEN/DRDY/CECS/SECS bit positions).
 * The RNG clock is sourced from PLL48CK (PLLQ output); the board must
 * configure PLLQ for 48 MHz before enabling the RNG clock gate.
 */

#include <wolfHAL/rng/stm32wb_rng.h>

typedef whal_Stm32wb_Rng_Cfg whal_Stm32f4_Rng_Cfg;

#define whal_Stm32f4_Rng_Dev whal_Stm32wb_Rng_Dev

#ifndef WHAL_CFG_STM32F4_RNG_DIRECT_API_MAPPING
#define whal_Stm32f4_Rng_Driver   whal_Stm32wb_Rng_Driver
#define whal_Stm32f4_Rng_Init     whal_Stm32wb_Rng_Init
#define whal_Stm32f4_Rng_Deinit   whal_Stm32wb_Rng_Deinit
#define whal_Stm32f4_Rng_Generate whal_Stm32wb_Rng_Generate
#endif /* !WHAL_CFG_STM32F4_RNG_DIRECT_API_MAPPING */

/* Config initializer macro alias. */
#define WHAL_CFG_STM32WB_RNG_DEV WHAL_CFG_STM32F4_RNG_DEV

#endif /* WHAL_STM32F4_RNG_H */
