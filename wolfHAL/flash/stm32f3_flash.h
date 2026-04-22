#ifndef WHAL_STM32F3_FLASH_H
#define WHAL_STM32F3_FLASH_H

/*
 * @file stm32f3_flash.h
 * @brief STM32F3 flash driver (alias for STM32F0 flash).
 *
 * The STM32F3 embedded flash uses the same register layout as the STM32F0:
 * ACR/KEYR/SR/CR/AR with identical bit positions, 2 KB page erase, and
 * half-word (16-bit) programming.
 */

#include <wolfHAL/flash/stm32f0_flash.h>

typedef whal_Stm32f0Flash_Cfg whal_Stm32f3Flash_Cfg;

#define WHAL_STM32F3_FLASH_LATENCY_0 WHAL_STM32F0_FLASH_LATENCY_0
#define WHAL_STM32F3_FLASH_LATENCY_1 WHAL_STM32F0_FLASH_LATENCY_1
#define WHAL_STM32F3_FLASH_LATENCY_2 2

#ifndef WHAL_CFG_FLASH_API_MAPPING_STM32F3
#define whal_Stm32f3Flash_Driver whal_Stm32f0Flash_Driver
#define whal_Stm32f3Flash_Init   whal_Stm32f0Flash_Init
#define whal_Stm32f3Flash_Deinit whal_Stm32f0Flash_Deinit
#define whal_Stm32f3Flash_Lock   whal_Stm32f0Flash_Lock
#define whal_Stm32f3Flash_Unlock whal_Stm32f0Flash_Unlock
#define whal_Stm32f3Flash_Read   whal_Stm32f0Flash_Read
#define whal_Stm32f3Flash_Write  whal_Stm32f0Flash_Write
#define whal_Stm32f3Flash_Erase  whal_Stm32f0Flash_Erase
#endif /* !WHAL_CFG_FLASH_API_MAPPING_STM32F3 */

#define whal_Stm32f3Flash_Ext_SetLatency whal_Stm32f0Flash_Ext_SetLatency

#endif /* WHAL_STM32F3_FLASH_H */
