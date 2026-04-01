#ifndef WHAL_STM32H753XX_H
#define WHAL_STM32H753XX_H

#include <wolfHAL/platform/arm/cortex_m7.h>

#include <wolfHAL/clock/stm32h7_rcc.h>
#include <wolfHAL/gpio/stm32h7_gpio.h>
#include <wolfHAL/uart/stm32h7_uart.h>
#include <wolfHAL/spi/stm32h7_spi.h>
#include <wolfHAL/flash/stm32h7_flash.h>
#include <wolfHAL/rng/stm32h7_rng.h>
#include <wolfHAL/eth/stm32h7_eth.h>
#include <wolfHAL/crypto/stm32h7_aes.h>
#include <wolfHAL/eth_phy/eth_phy.h>

/*
 * @file stm32h753xx.h
 * @brief Convenience initializers for STM32H753xx device instances.
 *
 * Base addresses from RM0433 Table 8 (memory map).
 * RCC: 0x58024400
 * GPIO: 0x58020000 (port A), 0x400 spacing per port
 * USART1: 0x40011000
 * USART2: 0x40004400
 * USART3: 0x40004800
 * UART4:  0x40004C00
 * UART5:  0x40005000
 * USART6: 0x40011400
 * SPI1:   0x40013000
 * SPI2:   0x40003800
 * SPI3:   0x40003C00
 * SPI4:   0x40013400
 * SPI5:   0x40015000
 * SPI6:   0x58001400
 * Flash:  0x52002000
 * RNG:    0x48021800
 * ETH:    0x40028000
 * CRYP:   0x48021000
 * SysTick: ARM core peripheral (0xE000E010)
 */

/* --- Device macros --- */

#define WHAL_STM32H753_RCC_PLL_DEVICE   \
    .regmap = {                         \
        .base = 0x58024400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7RccPll_Driver

#define WHAL_STM32H753_RCC_HSI_DEVICE   \
    .regmap = {                         \
        .base = 0x58024400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7RccHsi_Driver

#define WHAL_STM32H753_GPIO_DEVICE      \
    .regmap = {                         \
        .base = 0x58020000,             \
        .size = 0x2C00,                 \
    },                                  \
    .driver = &whal_Stm32h7Gpio_Driver

#define WHAL_STM32H753_USART1_DEVICE    \
    .regmap = {                         \
        .base = 0x40011000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Uart_Driver

#define WHAL_STM32H753_USART2_DEVICE    \
    .regmap = {                         \
        .base = 0x40004400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Uart_Driver

#define WHAL_STM32H753_USART3_DEVICE    \
    .regmap = {                         \
        .base = 0x40004800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Uart_Driver

#define WHAL_STM32H753_UART4_DEVICE     \
    .regmap = {                         \
        .base = 0x40004C00,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Uart_Driver

#define WHAL_STM32H753_UART5_DEVICE     \
    .regmap = {                         \
        .base = 0x40005000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Uart_Driver

#define WHAL_STM32H753_USART6_DEVICE    \
    .regmap = {                         \
        .base = 0x40011400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Uart_Driver

/* SPI device macros */

#define WHAL_STM32H753_SPI1_DEVICE      \
    .regmap = {                         \
        .base = 0x40013000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Spi_Driver

#define WHAL_STM32H753_SPI2_DEVICE      \
    .regmap = {                         \
        .base = 0x40003800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Spi_Driver

#define WHAL_STM32H753_SPI3_DEVICE      \
    .regmap = {                         \
        .base = 0x40003C00,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Spi_Driver

#define WHAL_STM32H753_SPI4_DEVICE      \
    .regmap = {                         \
        .base = 0x40013400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Spi_Driver

/* RNG device macros */

#define WHAL_STM32H753_RNG_DEVICE       \
    .regmap = {                         \
        .base = 0x48021800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Rng_Driver

/* Flash device macros */

#define WHAL_STM32H753_FLASH_DEVICE     \
    .regmap = {                         \
        .base = 0x52002000,             \
        .size = 0x200,                  \
    },                                  \
    .driver = &whal_Stm32h7Flash_Driver

/* Ethernet device macros */

#define WHAL_STM32H753_ETH_DEVICE       \
    .regmap = {                         \
        .base = 0x40028000,             \
        .size = 0x1200,                 \
    },                                  \
    .driver = &whal_Stm32h7Eth_Driver

/* AES/Crypto device macros */

#define WHAL_STM32H753_AES_DEVICE       \
    .regmap = {                         \
        .base = 0x48021000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h7Aes_Driver

/* --- Clock gate macros --- */

/*
 * RCC_AHB4ENR (offset 0x0E0) - GPIO clocks on AHB4
 * STM32H7 GPIO ports A-K are all on AHB4.
 */

#define WHAL_STM32H753_GPIOA_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 0),       \
    .enablePos = 0

#define WHAL_STM32H753_GPIOB_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#define WHAL_STM32H753_GPIOC_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 2),       \
    .enablePos = 2

#define WHAL_STM32H753_GPIOD_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 3),       \
    .enablePos = 3

#define WHAL_STM32H753_GPIOE_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 4),       \
    .enablePos = 4

#define WHAL_STM32H753_GPIOF_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 5),       \
    .enablePos = 5

#define WHAL_STM32H753_GPIOG_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 6),       \
    .enablePos = 6

#define WHAL_STM32H753_GPIOH_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 7),       \
    .enablePos = 7

#define WHAL_STM32H753_GPIOI_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 8),       \
    .enablePos = 8

#define WHAL_STM32H753_GPIOJ_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 9),       \
    .enablePos = 9

#define WHAL_STM32H753_GPIOK_CLOCK  \
    .regOffset = 0x0E0,             \
    .enableMask = (1UL << 10),      \
    .enablePos = 10

/* RCC_AHB1ENR (offset 0x0D8) */

#define WHAL_STM32H753_ETH_CLOCK    \
    .regOffset = 0x0D8,             \
    .enableMask = (1UL << 15),      \
    .enablePos = 15

#define WHAL_STM32H753_ETHTX_CLOCK  \
    .regOffset = 0x0D8,             \
    .enableMask = (1UL << 16),      \
    .enablePos = 16

#define WHAL_STM32H753_ETHRX_CLOCK  \
    .regOffset = 0x0D8,             \
    .enableMask = (1UL << 17),      \
    .enablePos = 17

/* RCC_AHB2ENR (offset 0x0DC) */

#define WHAL_STM32H753_RNG_CLOCK    \
    .regOffset = 0x0DC,             \
    .enableMask = (1UL << 6),       \
    .enablePos = 6

#define WHAL_STM32H753_AES_CLOCK    \
    .regOffset = 0x0DC,             \
    .enableMask = (1UL << 4),       \
    .enablePos = 4

/* RCC_AHB3ENR (offset 0x0D4) */

#define WHAL_STM32H753_FLASH_CLOCK  \
    .regOffset = 0x0D4,             \
    .enableMask = (1UL << 8),       \
    .enablePos = 8

/* RCC_APB1LENR (offset 0x0E8) */

#define WHAL_STM32H753_USART2_CLOCK \
    .regOffset = 0x0E8,             \
    .enableMask = (1UL << 17),      \
    .enablePos = 17

#define WHAL_STM32H753_USART3_CLOCK \
    .regOffset = 0x0E8,             \
    .enableMask = (1UL << 18),      \
    .enablePos = 18

#define WHAL_STM32H753_UART4_CLOCK  \
    .regOffset = 0x0E8,             \
    .enableMask = (1UL << 19),      \
    .enablePos = 19

#define WHAL_STM32H753_UART5_CLOCK  \
    .regOffset = 0x0E8,             \
    .enableMask = (1UL << 20),      \
    .enablePos = 20

#define WHAL_STM32H753_SPI2_CLOCK   \
    .regOffset = 0x0E8,             \
    .enableMask = (1UL << 14),      \
    .enablePos = 14

#define WHAL_STM32H753_SPI3_CLOCK   \
    .regOffset = 0x0E8,             \
    .enableMask = (1UL << 15),      \
    .enablePos = 15

/* RCC_APB2ENR (offset 0x0F0) */

#define WHAL_STM32H753_USART1_CLOCK \
    .regOffset = 0x0F0,             \
    .enableMask = (1UL << 4),       \
    .enablePos = 4

#define WHAL_STM32H753_USART6_CLOCK \
    .regOffset = 0x0F0,             \
    .enableMask = (1UL << 5),       \
    .enablePos = 5

#define WHAL_STM32H753_SPI1_CLOCK   \
    .regOffset = 0x0F0,             \
    .enableMask = (1UL << 12),      \
    .enablePos = 12

#define WHAL_STM32H753_SPI4_CLOCK   \
    .regOffset = 0x0F0,             \
    .enableMask = (1UL << 13),      \
    .enablePos = 13

/* RCC_APB4ENR (offset 0x0F4) */

#define WHAL_STM32H753_SYSCFG_CLOCK \
    .regOffset = 0x0F4,             \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#endif /* WHAL_STM32H753XX_H */
