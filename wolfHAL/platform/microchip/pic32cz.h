#ifndef WHAL_PIC32CZ_H
#define WHAL_PIC32CZ_H

#include <wolfHAL/clock/pic32cz_clock.h>
#include <wolfHAL/power/pic32cz_supc.h>
#include <wolfHAL/gpio/pic32cz_gpio.h>
#include <wolfHAL/uart/pic32cz_uart.h>
#include <wolfHAL/flash/pic32cz_flash.h>
#include <wolfHAL/platform/arm/cortex_m7.h>

#define WHAL_PIC32CZ_FLASH_REGMAP       \
    .base = 0x44002000,                 \
    .size = 0x4000
#define WHAL_PIC32CZ_FLASH_DRIVER &whal_Pic32cz_Flash_Driver

#define WHAL_PIC32CZ_SUPC_REGMAP        \
    .base = 0x44020000,                 \
    .size = 0x2000

#define WHAL_PIC32CZ_CLOCK_REGMAP       \
    .base = 0x44040000,                 \
    .size = 0x14000

#define WHAL_PIC32CZ_GPIO_REGMAP        \
    .base = 0x44840000,                 \
    .size = 0x2000
#define WHAL_PIC32CZ_GPIO_DRIVER &whal_Pic32cz_Gpio_Driver

#define WHAL_PIC32CZ_SERCOM4_UART_REGMAP    \
    .base = 0x46004000,                     \
    .size = 0x2000
#define WHAL_PIC32CZ_SERCOM4_UART_DRIVER &whal_Pic32cz_Uart_Driver

#define WHAL_PIC32CZ_SUPC_PLL       \
    .enableMask = (1UL << 18),      \
    .enablePos = 18

#endif /* WHAL_PIC32CZ_H */
