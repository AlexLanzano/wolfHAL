/* stm32wb55xx.h
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

#ifndef WHAL_STM32WB55XX_H
#define WHAL_STM32WB55XX_H

#include <wolfHAL/platform/arm/cortex_m4.h>

#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/gpio/stm32wb_gpio.h>
#include <wolfHAL/uart/stm32wb_uart.h>
#include <wolfHAL/uart/stm32wb_uart_dma.h>
#include <wolfHAL/spi/stm32wb_spi.h>
#include <wolfHAL/flash/stm32wb_flash.h>
#include <wolfHAL/rng/stm32wb_rng.h>
#include <wolfHAL/crypto/stm32wb_aes.h>
#include <wolfHAL/crypto/stm32wb_pka.h>
#include <wolfHAL/dma/stm32wb_dma.h>
#include <wolfHAL/i2c/stm32wb_i2c.h>
#include <wolfHAL/watchdog/stm32wb_iwdg.h>
#include <wolfHAL/watchdog/stm32wb_wwdg.h>
#include <wolfHAL/pwm/stm32wb_lptim_pwm.h>

/*
 * @file stm32wb55xx.h
 * @brief Convenience initializers for STM32WB55xx device instances.
 */

#define WHAL_STM32WB55_LPUART1_BASE 0x40008000
#define WHAL_STM32WB55_LPUART1_DRIVER &whal_Stm32wb_Uart_Driver

#define WHAL_STM32WB55_SPI1_BASE 0x40013000
#define WHAL_STM32WB55_SPI1_DRIVER &whal_Stm32wb_Spi_Driver

#define WHAL_STM32WB55_UART1_BASE 0x40013800
#define WHAL_STM32WB55_UART1_DRIVER &whal_Stm32wb_Uart_Driver

#define WHAL_STM32WB55_GPIO_BASE 0x48000000
#define WHAL_STM32WB55_GPIO_DRIVER &whal_Stm32wb_Gpio_Driver

#define WHAL_STM32WB55_RNG_BASE 0x58001000
#define WHAL_STM32WB55_RNG_DRIVER &whal_Stm32wb_Rng_Driver

#define WHAL_STM32WB55_AES1_BASE 0x50060000
#define WHAL_STM32WB55_AES1_DRIVER &whal_Stm32wb_Aes_CryptoDriver

#define WHAL_STM32WB55_PKA_BASE 0x58002000
#define WHAL_STM32WB55_PKA_DRIVER &whal_Stm32wb_Pka_CryptoDriver

#define WHAL_STM32WB55_I2C1_BASE 0x40005400
#define WHAL_STM32WB55_I2C1_DRIVER &whal_Stm32wb_I2c_Driver

#define WHAL_STM32WB55_I2C3_BASE 0x40005C00
#define WHAL_STM32WB55_I2C3_DRIVER &whal_Stm32wb_I2c_Driver

#define WHAL_STM32WB55_LPTIM1_BASE 0x40007C00
#define WHAL_STM32WB55_LPTIM1_PWM_DRIVER &whal_Stm32wb_Lptim_Pwm_Driver

#define WHAL_STM32WB55_IWDG_BASE 0x40003000
#define WHAL_STM32WB55_IWDG_DRIVER &whal_Stm32wb_Iwdg_Driver

#define WHAL_STM32WB55_WWDG_BASE 0x40002C00
#define WHAL_STM32WB55_WWDG_DRIVER &whal_Stm32wb_Wwdg_Driver

#define WHAL_STM32WB55_FLASH_BASE 0x58004000
#define WHAL_STM32WB55_FLASH_DRIVER &whal_Stm32wb_Flash_Driver

#define WHAL_STM32WB55_DMA1_BASE 0x40020000
#define WHAL_STM32WB55_DMA1_DRIVER &whal_Stm32wb_Dma_Driver

#define WHAL_STM32WB55_DMA2_BASE 0x40020400
#define WHAL_STM32WB55_DMA2_DRIVER &whal_Stm32wb_Dma_Driver

#define WHAL_STM32WB55_DMA1_CFG         \
    .dmamuxBase = 0x40020800,           \
    .dmamuxChOffset = 0,                \
    .numChannels = 7

#define WHAL_STM32WB55_DMA2_CFG         \
    .dmamuxBase = 0x40020800,           \
    .dmamuxChOffset = 7,                \
    .numChannels = 5

#define WHAL_STM32WB55_GPIOA_GATE  \
    .regOffset = 0x4C,              \
    .enableMask = (1UL << 0),       \
    .enablePos = 0

#define WHAL_STM32WB55_GPIOB_GATE  \
    .regOffset = 0x4C,              \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#define WHAL_STM32WB55_AES1_GATE    \
    .regOffset = 0x4C,               \
    .enableMask = (1UL << 16),       \
    .enablePos = 16

#define WHAL_STM32WB55_PKA_GATE     \
    .regOffset = 0x50,               \
    .enableMask = (1UL << 16),       \
    .enablePos = 16

#define WHAL_STM32WB55_WWDG_GATE   \
    .regOffset = 0x58,              \
    .enableMask = (1UL << 11),      \
    .enablePos = 11

#define WHAL_STM32WB55_RNG_GATE    \
    .regOffset = 0x50,              \
    .enableMask = (1UL << 18),      \
    .enablePos = 18

#define WHAL_STM32WB55_FLASH_GATE  \
    .regOffset = 0x50,              \
    .enableMask = (1UL << 25),      \
    .enablePos = 25

#define WHAL_STM32WB55_I2C1_GATE   \
    .regOffset = 0x58,              \
    .enableMask = (1UL << 21),      \
    .enablePos = 21

#define WHAL_STM32WB55_I2C3_GATE   \
    .regOffset = 0x58,              \
    .enableMask = (1UL << 23),      \
    .enablePos = 23

#define WHAL_STM32WB55_LPTIM1_GATE \
    .regOffset = 0x58,             \
    .enableMask = (1UL << 31),     \
    .enablePos = 31

#define WHAL_STM32WB55_LPUART1_GATE    \
    .regOffset = 0x5C,                  \
    .enableMask = (1UL << 0),           \
    .enablePos = 0

#define WHAL_STM32WB55_UART1_GATE  \
    .regOffset = 0x60,              \
    .enableMask = (1UL << 14),      \
    .enablePos = 14

#define WHAL_STM32WB55_SPI1_GATE   \
    .regOffset = 0x60,              \
    .enableMask = (1UL << 12),      \
    .enablePos = 12

#define WHAL_STM32WB55_DMA1_GATE   \
    .regOffset = 0x48,              \
    .enableMask = (1UL << 0),       \
    .enablePos = 0

#define WHAL_STM32WB55_DMA2_GATE   \
    .regOffset = 0x48,              \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#define WHAL_STM32WB55_UART1_TX_DMA_CFG     \
    .dir = WHAL_STM32WB_DMA_DIR_MEM_TO_PERIPH, \
    .width = WHAL_STM32WB_DMA_WIDTH_8BIT,      \
    .srcInc = WHAL_STM32WB_DMA_INC_ENABLE,     \
    .dstInc = WHAL_STM32WB_DMA_INC_DISABLE,    \
    .dmamuxReqId = 15

#define WHAL_STM32WB55_UART1_RX_DMA_CFG     \
    .dir = WHAL_STM32WB_DMA_DIR_PERIPH_TO_MEM, \
    .width = WHAL_STM32WB_DMA_WIDTH_8BIT,      \
    .srcInc = WHAL_STM32WB_DMA_INC_DISABLE,    \
    .dstInc = WHAL_STM32WB_DMA_INC_ENABLE,     \
    .dmamuxReqId = 14

#define WHAL_STM32WB55_DMAMUX1_GATE \
    .regOffset = 0x48,               \
    .enableMask = (1UL << 2),        \
    .enablePos = 2

#endif /* WHAL_STM32WB55XX_H */
