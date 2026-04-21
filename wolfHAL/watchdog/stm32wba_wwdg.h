#ifndef WHAL_STM32WBA_WWDG_H
#define WHAL_STM32WBA_WWDG_H

/*
 * @file stm32wba_wwdg.h
 * @brief STM32WBA WWDG driver (alias for STM32WB WWDG).
 *
 * The STM32WBA WWDG peripheral is register-compatible with the STM32WB WWDG.
 * This header re-exports the STM32WB WWDG driver types and symbols under
 * STM32WBA-specific names.
 */

#include <wolfHAL/watchdog/stm32wb_wwdg.h>

typedef whal_Stm32wbWwdg_Cfg whal_Stm32wbaWwdg_Cfg;

#ifndef WHAL_CFG_WATCHDOG_API_MAPPING_STM32WBA_WWDG
#define whal_Stm32wbaWwdg_Driver  whal_Stm32wbWwdg_Driver
#define whal_Stm32wbaWwdg_Init    whal_Stm32wbWwdg_Init
#define whal_Stm32wbaWwdg_Deinit  whal_Stm32wbWwdg_Deinit
#define whal_Stm32wbaWwdg_Refresh whal_Stm32wbWwdg_Refresh
#endif /* !WHAL_CFG_WATCHDOG_API_MAPPING_STM32WBA_WWDG */

/*
 * @brief Timebase prescaler values (re-exported from STM32WB).
 */
#define WHAL_STM32WBA_WWDG_TB_1   WHAL_STM32WB_WWDG_TB_1
#define WHAL_STM32WBA_WWDG_TB_2   WHAL_STM32WB_WWDG_TB_2
#define WHAL_STM32WBA_WWDG_TB_4   WHAL_STM32WB_WWDG_TB_4
#define WHAL_STM32WBA_WWDG_TB_8   WHAL_STM32WB_WWDG_TB_8
#define WHAL_STM32WBA_WWDG_TB_16  WHAL_STM32WB_WWDG_TB_16
#define WHAL_STM32WBA_WWDG_TB_32  WHAL_STM32WB_WWDG_TB_32
#define WHAL_STM32WBA_WWDG_TB_64  WHAL_STM32WB_WWDG_TB_64
#define WHAL_STM32WBA_WWDG_TB_128 WHAL_STM32WB_WWDG_TB_128

#endif /* WHAL_STM32WBA_WWDG_H */
