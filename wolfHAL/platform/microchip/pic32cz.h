#ifndef WHAL_PIC32CZ_H
#define WHAL_PIC32CZ_H

#include <wolfHAL/clock/pic32cz_clock.h>
#include <wolfHAL/supply/pic32cz_supc.h>
#include <wolfHAL/gpio/pic32cz_gpio.h>
#include <wolfHAL/platform/arm/cortex_m7.h>

#define WHAL_PIC32CZ_SUPPLY_DEVICE      \
    .regmap = {                         \
        .base = 0x44020000,             \
        .size = 0x2000,                 \
    },                                  \
    .driver = &whal_Pic32czSupc_Driver

#define WHAL_PIC32CZ_CLOCK_PLL_DEVICE   \
    .regmap = {                         \
        .base = 0x44040000,             \
        .size = 0x14000,                \
    },                                  \
    .driver = &whal_Pic32czClockPll_Driver

#define WHAL_PIC32CZ_GPIO_DEVICE        \
    .regmap = {                         \
        .base = 0x44840000,             \
        .size = 0x2000,                 \
    },                                  \
    .driver = &whal_Pic32czGpio_Driver

#define WHAL_PIC32CZ_SUPPLY_PLL     \
    .enableMask = (1 << 18)

#endif /* WHAL_PIC32CZ_H */
