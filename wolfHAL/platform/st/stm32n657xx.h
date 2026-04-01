#ifndef WHAL_STM32N657XX_H
#define WHAL_STM32N657XX_H

#include <wolfHAL/platform/arm/cortex_m55.h>

#include <wolfHAL/clock/stm32n6_rcc.h>
#include <wolfHAL/gpio/stm32n6_gpio.h>
#include <wolfHAL/uart/stm32n6_uart.h>
#include <wolfHAL/spi/stm32n6_spi.h>
#include <wolfHAL/rng/stm32n6_rng.h>

/*
 * @file stm32n657xx.h
 * @brief Convenience initializers for STM32N657xx device instances.
 *
 * Base addresses from RM0486 Section 2.3 (memory map), nonsecure aliases.
 * RCC: 0x46028000
 * GPIO: 0x46020000 (port A), 0x400 spacing per port
 * USART1: 0x42001000
 * USART2: 0x40004400
 * USART3: 0x40004800
 * SPI1: 0x42003000
 * SPI2: 0x40003800
 * SPI3: 0x40003C00
 * RNG: 0x44020000
 * SysTick: ARM core peripheral (0xE000E010)
 */

/* --- RCC device macros --- */

#define WHAL_STM32N657_RCC_HSI_DEVICE   \
    .regmap = {                         \
        .base = 0x46028000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6RccHsi_Driver

#define WHAL_STM32N657_RCC_PLL_DEVICE   \
    .regmap = {                         \
        .base = 0x46028000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6RccPll_Driver

/* --- GPIO device macros --- */

#define WHAL_STM32N657_GPIO_DEVICE      \
    .regmap = {                         \
        .base = 0x46020000,             \
        .size = 0x2C00,                 \
    },                                  \
    .driver = &whal_Stm32n6Gpio_Driver

/* --- UART device macros --- */

#define WHAL_STM32N657_USART1_DEVICE    \
    .regmap = {                         \
        .base = 0x42001000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6Uart_Driver

#define WHAL_STM32N657_USART2_DEVICE    \
    .regmap = {                         \
        .base = 0x40004400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6Uart_Driver

#define WHAL_STM32N657_USART3_DEVICE    \
    .regmap = {                         \
        .base = 0x40004800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6Uart_Driver

/* --- SPI device macros --- */

#define WHAL_STM32N657_SPI1_DEVICE      \
    .regmap = {                         \
        .base = 0x42003000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6Spi_Driver

#define WHAL_STM32N657_SPI2_DEVICE      \
    .regmap = {                         \
        .base = 0x40003800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6Spi_Driver

#define WHAL_STM32N657_SPI3_DEVICE      \
    .regmap = {                         \
        .base = 0x40003C00,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6Spi_Driver

/* --- RNG device macros --- */

#define WHAL_STM32N657_RNG_DEVICE       \
    .regmap = {                         \
        .base = 0x44020000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32n6Rng_Driver

/* --- Clock gate macros --- */

/* RCC_AHB4ENR (offset 0x25C) - GPIO clocks */

#define WHAL_STM32N657_GPIOA_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 0),       \
    .enablePos = 0

#define WHAL_STM32N657_GPIOB_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#define WHAL_STM32N657_GPIOC_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 2),       \
    .enablePos = 2

#define WHAL_STM32N657_GPIOD_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 3),       \
    .enablePos = 3

#define WHAL_STM32N657_GPIOE_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 4),       \
    .enablePos = 4

#define WHAL_STM32N657_GPIOF_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 5),       \
    .enablePos = 5

#define WHAL_STM32N657_GPIOG_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 6),       \
    .enablePos = 6

#define WHAL_STM32N657_GPIOH_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 7),       \
    .enablePos = 7

#define WHAL_STM32N657_GPION_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 13),      \
    .enablePos = 13

#define WHAL_STM32N657_GPIOO_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 14),      \
    .enablePos = 14

#define WHAL_STM32N657_GPIOP_CLOCK  \
    .regOffset = 0x25C,             \
    .enableMask = (1UL << 15),      \
    .enablePos = 15

/* RCC_AHB3ENR (offset 0x258) - Security peripherals */

#define WHAL_STM32N657_RNG_CLOCK    \
    .regOffset = 0x258,             \
    .enableMask = (1UL << 0),       \
    .enablePos = 0

#define WHAL_STM32N657_HASH_CLOCK   \
    .regOffset = 0x258,             \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#define WHAL_STM32N657_CRYP_CLOCK   \
    .regOffset = 0x258,             \
    .enableMask = (1UL << 2),       \
    .enablePos = 2

/* RCC_APB1LENR (offset 0x264) - APB1 Low peripherals */

#define WHAL_STM32N657_USART2_CLOCK \
    .regOffset = 0x264,             \
    .enableMask = (1UL << 17),      \
    .enablePos = 17

#define WHAL_STM32N657_USART3_CLOCK \
    .regOffset = 0x264,             \
    .enableMask = (1UL << 18),      \
    .enablePos = 18

#define WHAL_STM32N657_SPI2_CLOCK   \
    .regOffset = 0x264,             \
    .enableMask = (1UL << 14),      \
    .enablePos = 14

#define WHAL_STM32N657_SPI3_CLOCK   \
    .regOffset = 0x264,             \
    .enableMask = (1UL << 15),      \
    .enablePos = 15

/* RCC_APB2ENR (offset 0x26C) - APB2 peripherals */

#define WHAL_STM32N657_USART1_CLOCK \
    .regOffset = 0x26C,             \
    .enableMask = (1UL << 4),       \
    .enablePos = 4

#define WHAL_STM32N657_SPI1_CLOCK   \
    .regOffset = 0x26C,             \
    .enableMask = (1UL << 12),      \
    .enablePos = 12

/* RCC_BUSENR (offset 0x244) - Bus enables */

#define WHAL_STM32N657_AHB3_BUS_CLOCK   \
    .regOffset = 0x244,                 \
    .enableMask = (1UL << 5),           \
    .enablePos = 5

#define WHAL_STM32N657_AHB4_BUS_CLOCK   \
    .regOffset = 0x244,                 \
    .enableMask = (1UL << 6),           \
    .enablePos = 6

#define WHAL_STM32N657_APB1_BUS_CLOCK   \
    .regOffset = 0x244,                 \
    .enableMask = (1UL << 8),           \
    .enablePos = 8

#define WHAL_STM32N657_APB2_BUS_CLOCK   \
    .regOffset = 0x244,                 \
    .enableMask = (1UL << 9),           \
    .enablePos = 9

#endif /* WHAL_STM32N657XX_H */
