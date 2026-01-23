#ifndef WHAL_CORTEX_M4_H
#define WHAL_CORTEX_M4_H

#include <wolfHAL/timer/systick.h>

#define WHAL_CORTEX_M4_SYSTICK_DEVICE   \
    .regmap = {                         \
        .base = 0xE000E010,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_SysTick_Driver

#endif /* WHAL_CORTEX_M4_H */
