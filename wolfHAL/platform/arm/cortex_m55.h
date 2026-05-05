#ifndef WHAL_CORTEX_M55_H
#define WHAL_CORTEX_M55_H

#include <wolfHAL/timer/systick.h>
#include <wolfHAL/irq/cortex_m4_nvic.h>

#define WHAL_CORTEX_M55_SYSTICK_BASE 0xE000E010
#define WHAL_CORTEX_M55_SYSTICK_DRIVER &whal_SysTick_Driver

#define WHAL_CORTEX_M55_NVIC_BASE 0xE000E100
#define WHAL_CORTEX_M55_NVIC_DRIVER &whal_Nvic_Driver

#endif /* WHAL_CORTEX_M55_H */
