#ifndef WHAL_STM32H563XX_H
#define WHAL_STM32H563XX_H

#include <wolfHAL/platform/arm/cortex_m4.h>

#include <wolfHAL/clock/stm32h5_rcc.h>
#include <wolfHAL/gpio/stm32h5_gpio.h>
#include <wolfHAL/uart/stm32h5_uart.h>

/*
 * @file stm32h563xx.h
 * @brief Convenience initializers for STM32H563xx device instances.
 *
 * Base addresses from RM0481 Table 2 (memory map).
 * RCC: 0x44020C00
 * GPIO: 0x42020000 (port A), 0x400 spacing per port
 * USART1: 0x40013800
 * USART2: 0x40004400
 * USART3: 0x40004800
 * SysTick: ARM core peripheral (0xE000E010)
 */

/* --- Device macros --- */

#define WHAL_STM32H563_RCC_PLL_DEVICE   \
    .regmap = {                         \
        .base = 0x44020C00,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h5RccPll_Driver

#define WHAL_STM32H563_RCC_HSI_DEVICE   \
    .regmap = {                         \
        .base = 0x44020C00,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h5RccHsi_Driver

#define WHAL_STM32H563_GPIO_DEVICE      \
    .regmap = {                         \
        .base = 0x42020000,             \
        .size = 0x2400,                 \
    },                                  \
    .driver = &whal_Stm32h5Gpio_Driver

#define WHAL_STM32H563_USART1_DEVICE    \
    .regmap = {                         \
        .base = 0x40013800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h5Uart_Driver

#define WHAL_STM32H563_USART2_DEVICE    \
    .regmap = {                         \
        .base = 0x40004400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h5Uart_Driver

#define WHAL_STM32H563_USART3_DEVICE    \
    .regmap = {                         \
        .base = 0x40004800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32h5Uart_Driver

/* --- Clock gate macros --- */
/* RCC_AHB2ENR (offset 0x08C) */

#define WHAL_STM32H563_GPIOA_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 0),       \
    .enablePos = 0

#define WHAL_STM32H563_GPIOB_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#define WHAL_STM32H563_GPIOC_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 2),       \
    .enablePos = 2

#define WHAL_STM32H563_GPIOD_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 3),       \
    .enablePos = 3

#define WHAL_STM32H563_GPIOE_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 4),       \
    .enablePos = 4

#define WHAL_STM32H563_GPIOF_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 5),       \
    .enablePos = 5

#define WHAL_STM32H563_GPIOG_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 6),       \
    .enablePos = 6

#define WHAL_STM32H563_GPIOH_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 7),       \
    .enablePos = 7

#define WHAL_STM32H563_GPIOI_CLOCK  \
    .regOffset = 0x08C,             \
    .enableMask = (1UL << 8),       \
    .enablePos = 8

/* RCC_AHB1ENR (offset 0x088) */

#define WHAL_STM32H563_FLASH_CLOCK  \
    .regOffset = 0x088,             \
    .enableMask = (1UL << 8),       \
    .enablePos = 8

/* RCC_APB1LENR (offset 0x09C) */

#define WHAL_STM32H563_USART2_CLOCK \
    .regOffset = 0x09C,             \
    .enableMask = (1UL << 17),      \
    .enablePos = 17

#define WHAL_STM32H563_USART3_CLOCK \
    .regOffset = 0x09C,             \
    .enableMask = (1UL << 18),      \
    .enablePos = 18

/* RCC_APB2ENR (offset 0x0A4) */

#define WHAL_STM32H563_USART1_CLOCK \
    .regOffset = 0x0A4,             \
    .enableMask = (1UL << 14),      \
    .enablePos = 14

#endif /* WHAL_STM32H563XX_H */
