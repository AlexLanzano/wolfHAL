#ifndef WHAL_STM32WB55XX_H
#define WHAL_STM32WB55XX_H

#include <wolfHAL/platform/arm/cortex_m4.h>

#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/gpio/stm32wb_gpio.h>
#include <wolfHAL/uart/stm32wb_uart.h>
#include <wolfHAL/flash/stm32wb_flash.h>

/*
 * @file stm32wb55xx.h
 * @brief Convenience initializers for STM32WB55xx device instances.
 */

#define WHAL_STM32WB55_RCC_REGMAP       \
    .base = 0x58000000,                 \
    .size = 0x400

#define WHAL_STM32WB55_GPIO_REGMAP      \
    .base = 0x48000000,                 \
    .size = 0x400
#define WHAL_STM32WB55_GPIO_DRIVER &whal_Stm32wb_Gpio_Driver

#define WHAL_STM32WB55_UART1_REGMAP     \
    .base = 0x40013800,                 \
    .size = 0x400
#define WHAL_STM32WB55_UART1_DRIVER &whal_Stm32wb_Uart_Driver

#define WHAL_STM32WB55_FLASH_REGMAP     \
    .base = 0x58004000,                 \
    .size = 0x400
#define WHAL_STM32WB55_FLASH_DRIVER &whal_Stm32wb_Flash_Driver

#define WHAL_STM32WB55_GPIOA_GATE       \
    .regOffset = 0x4C,                  \
    .enableMask = (1UL << 0),           \
    .enablePos = 0

#define WHAL_STM32WB55_GPIOB_GATE       \
    .regOffset = 0x4C,                  \
    .enableMask = (1UL << 1),           \
    .enablePos = 1

#define WHAL_STM32WB55_UART1_GATE       \
    .regOffset = 0x60,                  \
    .enableMask = (1UL << 14),          \
    .enablePos = 14

#endif /* WHAL_STM32WB55XX_H */
