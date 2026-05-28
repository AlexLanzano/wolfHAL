/* stm32u5_gpdma.h
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

#ifndef WHAL_STM32U5_GPDMA_H
#define WHAL_STM32U5_GPDMA_H

/**
 * @file stm32u5_gpdma.h
 * @brief STM32U5 GPDMA driver (alias for STM32WBA GPDMA).
 *
 * The STM32U5 GPDMA controller is register-compatible with the STM32WBA
 * GPDMA. This header re-exports the STM32WBA GPDMA driver types and symbols
 * under STM32U5-specific names.
 */

#include <wolfHAL/dma/stm32wba_gpdma.h>

typedef whal_Stm32wba_Gpdma_Dir   whal_Stm32u5_Gpdma_Dir;
typedef whal_Stm32wba_Gpdma_Width whal_Stm32u5_Gpdma_Width;
typedef whal_Stm32wba_Gpdma_Inc   whal_Stm32u5_Gpdma_Inc;
typedef whal_Stm32wba_Gpdma_ChCfg whal_Stm32u5_Gpdma_ChCfg;
typedef whal_Stm32wba_Gpdma_Cfg   whal_Stm32u5_Gpdma_Cfg;
typedef whal_Stm32wba_Gpdma_Callback whal_Stm32u5_Gpdma_Callback;

#define whal_Stm32u5_Gpdma_Dev whal_Stm32wba_Gpdma_Dev

#ifndef WHAL_CFG_STM32U5_GPDMA_DIRECT_API_MAPPING
#define whal_Stm32u5_Gpdma_Driver     whal_Stm32wba_Gpdma_Driver
#define whal_Stm32u5_Gpdma_IRQHandler whal_Stm32wba_Gpdma_IRQHandler
#endif /* !WHAL_CFG_STM32U5_GPDMA_DIRECT_API_MAPPING */

/**
 * @brief Transfer direction (re-exported from STM32WBA).
 */
#define WHAL_STM32U5_GPDMA_DIR_PERIPH_TO_MEM WHAL_STM32WBA_GPDMA_DIR_PERIPH_TO_MEM
#define WHAL_STM32U5_GPDMA_DIR_MEM_TO_PERIPH WHAL_STM32WBA_GPDMA_DIR_MEM_TO_PERIPH
#define WHAL_STM32U5_GPDMA_DIR_MEM_TO_MEM    WHAL_STM32WBA_GPDMA_DIR_MEM_TO_MEM

/**
 * @brief Data width (re-exported from STM32WBA).
 */
#define WHAL_STM32U5_GPDMA_WIDTH_8BIT  WHAL_STM32WBA_GPDMA_WIDTH_8BIT
#define WHAL_STM32U5_GPDMA_WIDTH_16BIT WHAL_STM32WBA_GPDMA_WIDTH_16BIT
#define WHAL_STM32U5_GPDMA_WIDTH_32BIT WHAL_STM32WBA_GPDMA_WIDTH_32BIT

/**
 * @brief Address increment (re-exported from STM32WBA).
 */
#define WHAL_STM32U5_GPDMA_INC_DISABLE WHAL_STM32WBA_GPDMA_INC_DISABLE
#define WHAL_STM32U5_GPDMA_INC_ENABLE  WHAL_STM32WBA_GPDMA_INC_ENABLE

#endif /* WHAL_STM32U5_GPDMA_H */
