#ifndef WHAL_CORTEX_M33_H
#define WHAL_CORTEX_M33_H

#include <wolfHAL/timer/systick.h>

#define WHAL_CORTEX_M33_SYSTICK_REGMAP  \
    .base = 0xE000E010,                 \
    .size = 0x400
#define WHAL_CORTEX_M33_SYSTICK_DRIVER &whal_SysTick_Driver

#endif /* WHAL_CORTEX_M33_H */
