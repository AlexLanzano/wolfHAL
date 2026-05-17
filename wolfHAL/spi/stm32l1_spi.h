/* stm32l1_spi.h
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

#ifndef WHAL_STM32L1_SPI_H
#define WHAL_STM32L1_SPI_H

/*
 * @file stm32l1_spi.h
 * @brief STM32L1 SPI driver (alias for STM32F4 SPI).
 *
 * The STM32L1 SPI peripheral uses the same V1 SPI register layout as the
 * STM32F4 (CR1 with DFF bit at 0x00, CR2 at 0x04, SR at 0x08, DR at 0x0C).
 */

#include <wolfHAL/spi/stm32f4_spi.h>

typedef whal_Stm32f4_Spi_Cfg whal_Stm32l1_Spi_Cfg;

#ifndef WHAL_CFG_STM32L1_SPI_DIRECT_API_MAPPING
#define whal_Stm32l1_Spi_Driver   whal_Stm32f4_Spi_Driver
#define whal_Stm32l1_Spi_Init     whal_Stm32f4_Spi_Init
#define whal_Stm32l1_Spi_Deinit   whal_Stm32f4_Spi_Deinit
#define whal_Stm32l1_Spi_StartCom whal_Stm32f4_Spi_StartCom
#define whal_Stm32l1_Spi_EndCom   whal_Stm32f4_Spi_EndCom
#define whal_Stm32l1_Spi_SendRecv whal_Stm32f4_Spi_SendRecv
#endif /* !WHAL_CFG_STM32L1_SPI_DIRECT_API_MAPPING */

#endif /* WHAL_STM32L1_SPI_H */
