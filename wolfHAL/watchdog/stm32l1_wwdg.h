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

typedef whal_Stm32f0_Wwdg_Cfg whal_Stm32l1_Wwdg_Cfg;

#ifndef WHAL_CFG_STM32L1_WWDG_DIRECT_API_MAPPING
#define whal_Stm32l1_Wwdg_Driver  whal_Stm32f0_Wwdg_Driver
#define whal_Stm32l1_Wwdg_Init    whal_Stm32f0_Wwdg_Init
#define whal_Stm32l1_Wwdg_Deinit  whal_Stm32f0_Wwdg_Deinit
#define whal_Stm32l1_Wwdg_Refresh whal_Stm32f0_Wwdg_Refresh
#endif /* !WHAL_CFG_STM32L1_WWDG_DIRECT_API_MAPPING */

#endif /* WHAL_STM32L1_WWDG_H */
