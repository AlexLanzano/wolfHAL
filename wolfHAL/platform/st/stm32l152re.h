#ifndef WHAL_STM32L152RE_H
#define WHAL_STM32L152RE_H

#include <wolfHAL/platform/arm/cortex_m3.h>

#include <wolfHAL/clock/stm32l1_rcc.h>
#include <wolfHAL/gpio/stm32l1_gpio.h>
#include <wolfHAL/uart/stm32l1_uart.h>
#include <wolfHAL/flash/stm32l1_flash.h>
#include <wolfHAL/power/stm32l1_pwr.h>

/*
 * @file stm32l152re.h
 * @brief Convenience initializers for STM32L152RE device instances.
 */

#define WHAL_STM32L152_RCC_REGMAP       \
    .base = 0x40023800,                 \
    .size = 0x400

#define WHAL_STM32L152_GPIO_REGMAP      \
    .base = 0x40020000,                 \
    .size = 0x2000
#define WHAL_STM32L152_GPIO_DRIVER &whal_Stm32l1_Gpio_Driver

#define WHAL_STM32L152_USART2_REGMAP    \
    .base = 0x40004400,                 \
    .size = 0x400
#define WHAL_STM32L152_USART2_DRIVER &whal_Stm32l1_Uart_Driver

#define WHAL_STM32L152_FLASH_REGMAP     \
    .base = 0x40023C00,                 \
    .size = 0x400
#define WHAL_STM32L152_FLASH_DRIVER &whal_Stm32l1_Flash_Driver

#define WHAL_STM32L152_PWR_REGMAP       \
    .base = 0x40007000,                 \
    .size = 0x400

/* RCC_AHBENR (offset 0x1C) */

#define WHAL_STM32L152_GPIOA_CLOCK      \
    .regOffset = 0x1C,                  \
    .enableMask = (1UL << 0),           \
    .enablePos = 0

/* RCC_APB1ENR (offset 0x24) */

#define WHAL_STM32L152_USART2_CLOCK     \
    .regOffset = 0x24,                  \
    .enableMask = (1UL << 17),          \
    .enablePos = 17

#define WHAL_STM32L152_PWR_CLOCK        \
    .regOffset = 0x24,                  \
    .enableMask = (1UL << 28),          \
    .enablePos = 28

#endif /* WHAL_STM32L152RE_H */
