#ifndef WHAL_STM32L1_IWDG_H
#define WHAL_STM32L1_IWDG_H

/*
 * @file stm32l1_iwdg.h
 * @brief STM32L1 IWDG driver (alias for STM32WB IWDG).
 *
 * The STM32L1 IWDG peripheral is register-compatible with the STM32WB IWDG.
 */

#include <wolfHAL/watchdog/stm32wb_iwdg.h>

typedef whal_Stm32wbIwdg_Cfg whal_Stm32l1Iwdg_Cfg;

#ifndef WHAL_CFG_WATCHDOG_API_MAPPING_STM32L1_IWDG
#define whal_Stm32l1Iwdg_Driver  whal_Stm32wbIwdg_Driver
#define whal_Stm32l1Iwdg_Init    whal_Stm32wbIwdg_Init
#define whal_Stm32l1Iwdg_Deinit  whal_Stm32wbIwdg_Deinit
#define whal_Stm32l1Iwdg_Refresh whal_Stm32wbIwdg_Refresh
#endif /* !WHAL_CFG_WATCHDOG_API_MAPPING_STM32L1_IWDG */

#define WHAL_STM32L1_IWDG_PR_4   WHAL_STM32WB_IWDG_PR_4
#define WHAL_STM32L1_IWDG_PR_8   WHAL_STM32WB_IWDG_PR_8
#define WHAL_STM32L1_IWDG_PR_16  WHAL_STM32WB_IWDG_PR_16
#define WHAL_STM32L1_IWDG_PR_32  WHAL_STM32WB_IWDG_PR_32
#define WHAL_STM32L1_IWDG_PR_64  WHAL_STM32WB_IWDG_PR_64
#define WHAL_STM32L1_IWDG_PR_128 WHAL_STM32WB_IWDG_PR_128
#define WHAL_STM32L1_IWDG_PR_256 WHAL_STM32WB_IWDG_PR_256

#endif /* WHAL_STM32L1_IWDG_H */
