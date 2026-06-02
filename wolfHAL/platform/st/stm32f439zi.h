/* stm32f439zi.h
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

#ifndef WHAL_STM32F439ZI_H
#define WHAL_STM32F439ZI_H

#include <wolfHAL/platform/arm/cortex_m4.h>

#include <wolfHAL/clock/stm32f4_rcc.h>
#include <wolfHAL/gpio/stm32f4_gpio.h>
#include <wolfHAL/uart/stm32f4_uart.h>
#include <wolfHAL/spi/stm32f4_spi.h>
#include <wolfHAL/flash/stm32f4_flash.h>
#include <wolfHAL/rng/stm32f4_rng.h>
#include <wolfHAL/watchdog/stm32f4_iwdg.h>
#include <wolfHAL/watchdog/stm32f4_wwdg.h>
#include <wolfHAL/i2c/stm32f4_i2c.h>
#include <wolfHAL/dma/stm32f4_dma.h>
#include <wolfHAL/crypto/stm32f4_hash.h>
#include <wolfHAL/eth/stm32f4_eth.h>
/* The following are scaffolded for future drivers — uncomment as each is
 * implemented:
 *   #include <wolfHAL/crypto/stm32f4_aes.h>
 */

/**
 * @file stm32f439zi.h
 * @brief Device base addresses and clock-gate descriptors for STM32F439ZI.
 *
 * Address sources: RM0090 Table 1 (STM32F4xx register boundary addresses),
 * RCC chapter 6 (STM32F42xxx and STM32F43xxx).
 *
 * STM32F439ZI: Cortex-M4F @ 168 MHz, 2 MB flash (dual bank), 256 KB SRAM
 * (192 KB SRAM1/2/3 + 64 KB CCM), 144-pin LQFP.
 */

/* --- AHB1 peripheral bases (0x4002 0000 - 0x4002 FFFF) --- */

#define WHAL_STM32F439_GPIO_BASE  0x40020000  /* GPIOA; 0x400 per port */
#define WHAL_STM32F439_RCC_BASE   0x40023800
#define WHAL_STM32F439_FLASH_BASE 0x40023C00
#define WHAL_STM32F439_DMA1_BASE  0x40026000
#define WHAL_STM32F439_DMA2_BASE  0x40026400
#define WHAL_STM32F439_ETH_BASE   0x40028000

/* --- AHB2 peripheral bases (0x5006 0000 - 0x5006 0BFF) --- */

#define WHAL_STM32F439_CRYP_BASE  0x50060000
#define WHAL_STM32F439_HASH_BASE  0x50060400
#define WHAL_STM32F439_RNG_BASE   0x50060800

/* --- APB1 peripheral bases (0x4000 0000 - 0x4000 7FFF) --- */

#define WHAL_STM32F439_WWDG_BASE   0x40002C00
#define WHAL_STM32F439_IWDG_BASE   0x40003000
#define WHAL_STM32F439_SPI2_BASE   0x40003800
#define WHAL_STM32F439_SPI3_BASE   0x40003C00
#define WHAL_STM32F439_USART2_BASE 0x40004400
#define WHAL_STM32F439_USART3_BASE 0x40004800
#define WHAL_STM32F439_UART4_BASE  0x40004C00
#define WHAL_STM32F439_UART5_BASE  0x40005000
#define WHAL_STM32F439_I2C1_BASE   0x40005400
#define WHAL_STM32F439_I2C2_BASE   0x40005800
#define WHAL_STM32F439_I2C3_BASE   0x40005C00
#define WHAL_STM32F439_PWR_BASE    0x40007000
#define WHAL_STM32F439_UART7_BASE  0x40007800
#define WHAL_STM32F439_UART8_BASE  0x40007C00

/* --- APB2 peripheral bases (0x4001 0000 - 0x4001 6FFF) --- */

#define WHAL_STM32F439_USART1_BASE 0x40011000
#define WHAL_STM32F439_USART6_BASE 0x40011400
#define WHAL_STM32F439_SPI1_BASE   0x40013000
#define WHAL_STM32F439_SPI4_BASE   0x40013400
#define WHAL_STM32F439_SYSCFG_BASE 0x40013800
#define WHAL_STM32F439_SPI5_BASE   0x40015000
#define WHAL_STM32F439_SPI6_BASE   0x40015400

/* --- Vtable-pointer driver macros --- */
/* Flash can coexist with SPI-NOR via vtable dispatch. Crypto peripheral
 * vtables are referenced by per-algo init in board.h. */

#define WHAL_STM32F439_FLASH_DRIVER &whal_Stm32f4_Flash_Driver
#define WHAL_STM32F439_HASH_DRIVER  &whal_Stm32f4_Hash_CryptoDriver
/* Activate when AES driver is implemented:
 *   #define WHAL_STM32F439_AES_DRIVER   &whal_Stm32f4_Aes_CryptoDriver
 */

/* --- AHB1ENR clock gates (RCC offset 0x30) --- */

#define WHAL_STM32F439_GPIOA_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  0), .enablePos =  0
#define WHAL_STM32F439_GPIOB_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  1), .enablePos =  1
#define WHAL_STM32F439_GPIOC_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  2), .enablePos =  2
#define WHAL_STM32F439_GPIOD_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  3), .enablePos =  3
#define WHAL_STM32F439_GPIOE_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  4), .enablePos =  4
#define WHAL_STM32F439_GPIOF_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  5), .enablePos =  5
#define WHAL_STM32F439_GPIOG_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  6), .enablePos =  6
#define WHAL_STM32F439_GPIOH_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  7), .enablePos =  7
#define WHAL_STM32F439_GPIOI_CLOCK    .regOffset = 0x030, .enableMask = (1UL <<  8), .enablePos =  8
#define WHAL_STM32F439_CRC_CLOCK      .regOffset = 0x030, .enableMask = (1UL << 12), .enablePos = 12
#define WHAL_STM32F439_BKPSRAM_CLOCK  .regOffset = 0x030, .enableMask = (1UL << 18), .enablePos = 18
#define WHAL_STM32F439_DMA1_CLOCK     .regOffset = 0x030, .enableMask = (1UL << 21), .enablePos = 21
#define WHAL_STM32F439_DMA2_CLOCK     .regOffset = 0x030, .enableMask = (1UL << 22), .enablePos = 22
#define WHAL_STM32F439_ETHMAC_CLOCK   .regOffset = 0x030, .enableMask = (1UL << 25), .enablePos = 25
#define WHAL_STM32F439_ETHMACTX_CLOCK .regOffset = 0x030, .enableMask = (1UL << 26), .enablePos = 26
#define WHAL_STM32F439_ETHMACRX_CLOCK .regOffset = 0x030, .enableMask = (1UL << 27), .enablePos = 27
#define WHAL_STM32F439_ETHMACPTP_CLOCK .regOffset = 0x030, .enableMask = (1UL << 28), .enablePos = 28

/* --- AHB2ENR clock gates (RCC offset 0x34) --- */

#define WHAL_STM32F439_DCMI_CLOCK     .regOffset = 0x034, .enableMask = (1UL << 0), .enablePos = 0
#define WHAL_STM32F439_CRYP_CLOCK     .regOffset = 0x034, .enableMask = (1UL << 4), .enablePos = 4
#define WHAL_STM32F439_HASH_CLOCK     .regOffset = 0x034, .enableMask = (1UL << 5), .enablePos = 5
#define WHAL_STM32F439_RNG_CLOCK      .regOffset = 0x034, .enableMask = (1UL << 6), .enablePos = 6

/* --- APB1ENR clock gates (RCC offset 0x40) --- */

#define WHAL_STM32F439_TIM2_CLOCK    .regOffset = 0x040, .enableMask = (1UL <<  0), .enablePos =  0
#define WHAL_STM32F439_TIM3_CLOCK    .regOffset = 0x040, .enableMask = (1UL <<  1), .enablePos =  1
#define WHAL_STM32F439_TIM4_CLOCK    .regOffset = 0x040, .enableMask = (1UL <<  2), .enablePos =  2
#define WHAL_STM32F439_TIM5_CLOCK    .regOffset = 0x040, .enableMask = (1UL <<  3), .enablePos =  3
#define WHAL_STM32F439_TIM6_CLOCK    .regOffset = 0x040, .enableMask = (1UL <<  4), .enablePos =  4
#define WHAL_STM32F439_TIM7_CLOCK    .regOffset = 0x040, .enableMask = (1UL <<  5), .enablePos =  5
#define WHAL_STM32F439_WWDG_CLOCK    .regOffset = 0x040, .enableMask = (1UL << 11), .enablePos = 11
#define WHAL_STM32F439_SPI2_CLOCK    .regOffset = 0x040, .enableMask = (1UL << 14), .enablePos = 14
#define WHAL_STM32F439_SPI3_CLOCK    .regOffset = 0x040, .enableMask = (1UL << 15), .enablePos = 15
#define WHAL_STM32F439_USART2_CLOCK  .regOffset = 0x040, .enableMask = (1UL << 17), .enablePos = 17
#define WHAL_STM32F439_USART3_CLOCK  .regOffset = 0x040, .enableMask = (1UL << 18), .enablePos = 18
#define WHAL_STM32F439_UART4_CLOCK   .regOffset = 0x040, .enableMask = (1UL << 19), .enablePos = 19
#define WHAL_STM32F439_UART5_CLOCK   .regOffset = 0x040, .enableMask = (1UL << 20), .enablePos = 20
#define WHAL_STM32F439_I2C1_CLOCK    .regOffset = 0x040, .enableMask = (1UL << 21), .enablePos = 21
#define WHAL_STM32F439_I2C2_CLOCK    .regOffset = 0x040, .enableMask = (1UL << 22), .enablePos = 22
#define WHAL_STM32F439_I2C3_CLOCK    .regOffset = 0x040, .enableMask = (1UL << 23), .enablePos = 23
#define WHAL_STM32F439_PWR_CLOCK     .regOffset = 0x040, .enableMask = (1UL << 28), .enablePos = 28
#define WHAL_STM32F439_DAC_CLOCK     .regOffset = 0x040, .enableMask = (1UL << 29), .enablePos = 29
#define WHAL_STM32F439_UART7_CLOCK   .regOffset = 0x040, .enableMask = (1UL << 30), .enablePos = 30
#define WHAL_STM32F439_UART8_CLOCK   .regOffset = 0x040, .enableMask = (1UL << 31), .enablePos = 31

/* --- APB2ENR clock gates (RCC offset 0x44) --- */

#define WHAL_STM32F439_TIM1_CLOCK    .regOffset = 0x044, .enableMask = (1UL <<  0), .enablePos =  0
#define WHAL_STM32F439_TIM8_CLOCK    .regOffset = 0x044, .enableMask = (1UL <<  1), .enablePos =  1
#define WHAL_STM32F439_USART1_CLOCK  .regOffset = 0x044, .enableMask = (1UL <<  4), .enablePos =  4
#define WHAL_STM32F439_USART6_CLOCK  .regOffset = 0x044, .enableMask = (1UL <<  5), .enablePos =  5
#define WHAL_STM32F439_SPI1_CLOCK    .regOffset = 0x044, .enableMask = (1UL << 12), .enablePos = 12
#define WHAL_STM32F439_SPI4_CLOCK    .regOffset = 0x044, .enableMask = (1UL << 13), .enablePos = 13
#define WHAL_STM32F439_SYSCFG_CLOCK  .regOffset = 0x044, .enableMask = (1UL << 14), .enablePos = 14
#define WHAL_STM32F439_SPI5_CLOCK    .regOffset = 0x044, .enableMask = (1UL << 20), .enablePos = 20
#define WHAL_STM32F439_SPI6_CLOCK    .regOffset = 0x044, .enableMask = (1UL << 21), .enablePos = 21

#endif /* WHAL_STM32F439ZI_H */
