#ifndef WHAL_CORTEX_M3_H
#define WHAL_CORTEX_M3_H

#include <wolfHAL/timer/systick.h>
#include <wolfHAL/irq/cortex_m4_nvic.h>

#define WHAL_CORTEX_M3_SYSTICK_REGMAP   \
    .base = 0xE000E010,                 \
    .size = 0x400
#define WHAL_CORTEX_M3_SYSTICK_DRIVER &whal_SysTick_Driver

#define WHAL_CORTEX_M3_NVIC_REGMAP      \
    .base = 0xE000E100,                 \
    .size = 0x400
#define WHAL_CORTEX_M3_NVIC_DRIVER &whal_Nvic_Driver

#endif /* WHAL_CORTEX_M3_H */
