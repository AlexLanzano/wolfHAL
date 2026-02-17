#ifndef WHAL_STM32WB55XX_H
#define WHAL_STM32WB55XX_H

#include <wolfHAL/platform/arm/cortex_m4.h>

#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/gpio/stm32wb_gpio.h>
#include <wolfHAL/uart/stm32wb_uart.h>
#include <wolfHAL/spi/stm32wb_spi.h>
#include <wolfHAL/flash/stm32wb_flash.h>

/*
 * @file stm32wb55xx.h
 * @brief Convenience initializers for STM32WB55xx device instances.
 */

#define WHAL_STM32WB55_LPUART1_DEVICE   \
    .regmap = {                         \
        .base = 0x40008000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbLpuart_Driver

#define WHAL_STM32WB55_SPI1_DEVICE      \
    .regmap = {                         \
        .base = 0x40013000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbSpi_Driver

#define WHAL_STM32WB55_UART1_DEVICE     \
    .regmap = {                         \
        .base = 0x40013800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbUart_Driver

#define WHAL_STM32WB55_GPIO_DEVICE      \
    .regmap = {                         \
        .base = 0x48000000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbGpio_Driver

#define WHAL_STM32WB55_RCC_PLL_DEVICE   \
    .regmap = {                         \
        .base = 0x58000000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbRccPll_Driver

#define WHAL_STM32WB55_RCC_MSI_DEVICE   \
    .regmap = {                         \
        .base = 0x58000000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbRccPll_Driver

#define WHAL_STM32WB55_FLASH_DEVICE     \
    .regmap = {                         \
        .base = 0x58004000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbFlash_Driver


#define WHAL_STM32WB55_PLL_CLOCK    \
    .regOffset = 0x00,              \
    .enableMask = (1 << 24)

#define WHAL_STM32WB55_GPIOA_CLOCK  \
    .regOffset = 0x4C,              \
    .enableMask = (1 << 0)

#define WHAL_STM32WB55_GPIOB_CLOCK  \
    .regOffset = 0x4C,              \
    .enableMask = (1 << 1)

#define WHAL_STM32WB55_FLASH_CLOCK  \
    .regOffset = 0x50,              \
    .enableMask = (1 << 25)

#define WHAL_STM32WB55_LPUART1_CLOCK    \
    .regOffset = 0x5C,                  \
    .enableMask = (1 << 0)

#define WHAL_STM32WB55_UART1_CLOCK  \
    .regOffset = 0x60,              \
    .enableMask = (1 << 14)

#define WHAL_STM32WB55_SPI1_CLOCK   \
    .regOffset = 0x60,              \
    .enableMask = (1 << 12)

#endif /* WHAL_STM32WB55XX_H */
