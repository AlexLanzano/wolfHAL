#ifndef WHAL_STM32WBA_IWDG_H
#define WHAL_STM32WBA_IWDG_H

/*
 * @file stm32wba_iwdg.h
 * @brief STM32WBA IWDG driver (alias for STM32WB IWDG).
 *
 * The STM32WBA IWDG peripheral is register-compatible with the STM32WB IWDG.
 * This header re-exports the STM32WB IWDG driver types and symbols under
 * STM32WBA-specific names.
 */

#include <wolfHAL/watchdog/stm32wb_iwdg.h>

typedef whal_Stm32wbIwdg_Cfg whal_Stm32wbaIwdg_Cfg;

#ifndef WHAL_CFG_WATCHDOG_API_MAPPING_STM32WBA_IWDG
#define whal_Stm32wbaIwdg_Driver  whal_Stm32wbIwdg_Driver
#define whal_Stm32wbaIwdg_Init    whal_Stm32wbIwdg_Init
#define whal_Stm32wbaIwdg_Deinit  whal_Stm32wbIwdg_Deinit
#define whal_Stm32wbaIwdg_Refresh whal_Stm32wbIwdg_Refresh
#endif /* !WHAL_CFG_WATCHDOG_API_MAPPING_STM32WBA_IWDG */

/*
 * @brief Prescaler values (re-exported from STM32WB).
 */
#define WHAL_STM32WBA_IWDG_PR_4   WHAL_STM32WB_IWDG_PR_4
#define WHAL_STM32WBA_IWDG_PR_8   WHAL_STM32WB_IWDG_PR_8
#define WHAL_STM32WBA_IWDG_PR_16  WHAL_STM32WB_IWDG_PR_16
#define WHAL_STM32WBA_IWDG_PR_32  WHAL_STM32WB_IWDG_PR_32
#define WHAL_STM32WBA_IWDG_PR_64  WHAL_STM32WB_IWDG_PR_64
#define WHAL_STM32WBA_IWDG_PR_128 WHAL_STM32WB_IWDG_PR_128
#define WHAL_STM32WBA_IWDG_PR_256 WHAL_STM32WB_IWDG_PR_256

#endif /* WHAL_STM32WBA_IWDG_H */
