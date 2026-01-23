#ifndef WHAL_STM32WB55XX_H
#define WHAL_STM32WB55XX_H

#include <wolfHAL/platform/arm/cortex_m4.h>

#include <wolfHAL/clock/st_rcc.h>
#include <wolfHAL/gpio/st_gpio.h>
#include <wolfHAL/uart/st_uart.h>
#include <wolfHAL/flash/st_flash.h>

/*
 * @file stm32wb55xx.h
 * @brief Convenience initializers for STM32WB55xx device instances.
 */

#define WHAL_STM32WB55_LPUART1_DEVICE   \
    .regmap = {                         \
        .base = 0x40008000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_StUart_Driver

#define WHAL_STM32WB55_SPI1_DEVICE      \
    .regmap = {                         \
        .base = 0x40013000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_StSpi_Driver

#define WHAL_STM32WB55_GPIO_DEVICE      \
    .regmap = {                         \
        .base = 0x48000000,             \
        .size = 0x2000,                 \
    },                                  \
    .driver = &whal_StGpio_Driver

#define WHAL_STM32WB55_RCC_DEVICE       \
    .regmap = {                         \
        .base = 0x58000000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_StRcc_Driver


#define WHAL_STM32WB55_FLASH_DEVICE     \
    .regmap = {                         \
        .base = 0x58004000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_StFlash_Driver


#endif /* WHAL_STM32WB55XX_H */
