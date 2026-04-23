#ifndef WHAL_STM32L1_WWDG_H
#define WHAL_STM32L1_WWDG_H

/*
 * @file stm32l1_wwdg.h
 * @brief STM32L1 WWDG driver (alias for STM32F0 WWDG).
 *
 * The STM32L1 WWDG peripheral uses the same register layout as the STM32F0
 * (2-bit WDGTB prescaler at CFR bits 8:7).
 */

#include <wolfHAL/watchdog/stm32f0_wwdg.h>

typedef whal_Stm32f0Wwdg_Cfg whal_Stm32l1Wwdg_Cfg;

#ifndef WHAL_CFG_WATCHDOG_API_MAPPING_STM32L1_WWDG
#define whal_Stm32l1Wwdg_Driver  whal_Stm32f0Wwdg_Driver
#define whal_Stm32l1Wwdg_Init    whal_Stm32f0Wwdg_Init
#define whal_Stm32l1Wwdg_Deinit  whal_Stm32f0Wwdg_Deinit
#define whal_Stm32l1Wwdg_Refresh whal_Stm32f0Wwdg_Refresh
#endif /* !WHAL_CFG_WATCHDOG_API_MAPPING_STM32L1_WWDG */

#endif /* WHAL_STM32L1_WWDG_H */
