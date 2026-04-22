#ifndef WHAL_STM32F0_WWDG_H
#define WHAL_STM32F0_WWDG_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/watchdog/watchdog.h>

/*
 * @file stm32f0_wwdg.h
 * @brief STM32F0 window watchdog (WWDG) driver.
 *
 * The F0 WWDG has a 2-bit WDGTB prescaler at CFR bits 8:7 (divides by
 * 1/2/4/8), unlike the WB which has a 3-bit WDGTB at bits 13:11.
 */

typedef struct {
    uint8_t prescaler;
    uint8_t window;
    uint8_t counter;
} whal_Stm32f0Wwdg_Cfg;

#ifndef WHAL_CFG_WATCHDOG_API_MAPPING_STM32F0_WWDG
extern const whal_WatchdogDriver whal_Stm32f0Wwdg_Driver;

whal_Error whal_Stm32f0Wwdg_Init(whal_Watchdog *wdgDev);
whal_Error whal_Stm32f0Wwdg_Deinit(whal_Watchdog *wdgDev);
whal_Error whal_Stm32f0Wwdg_Refresh(whal_Watchdog *wdgDev);
#endif /* !WHAL_CFG_WATCHDOG_API_MAPPING_STM32F0_WWDG */

#endif /* WHAL_STM32F0_WWDG_H */
