/* stm32c0_spi.h
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

#ifndef WHAL_STM32C0_SPI_H
#define WHAL_STM32C0_SPI_H

/*
 * @file stm32c0_spi.h
 * @brief STM32C0 SPI driver (alias for STM32WB SPI).
 *
 * The STM32C0 SPI peripheral is register-compatible with the STM32WB SPI.
 * This header re-exports the STM32WB SPI driver types and symbols under
 * STM32C0-specific names. The underlying implementation is shared.
 */

#include <wolfHAL/spi/stm32wb_spi.h>

typedef whal_Stm32wb_Spi_Cfg whal_Stm32c0_Spi_Cfg;

#ifndef WHAL_CFG_STM32C0_SPI_DIRECT_API_MAPPING
#define whal_Stm32c0_Spi_Driver   whal_Stm32wb_Spi_Driver
#define whal_Stm32c0_Spi_Init     whal_Stm32wb_Spi_Init
#define whal_Stm32c0_Spi_Deinit   whal_Stm32wb_Spi_Deinit
#define whal_Stm32c0_Spi_StartCom whal_Stm32wb_Spi_StartCom
#define whal_Stm32c0_Spi_EndCom   whal_Stm32wb_Spi_EndCom
#define whal_Stm32c0_Spi_SendRecv whal_Stm32wb_Spi_SendRecv
#endif /* !WHAL_CFG_STM32C0_SPI_DIRECT_API_MAPPING */

#endif /* WHAL_STM32C0_SPI_H */
