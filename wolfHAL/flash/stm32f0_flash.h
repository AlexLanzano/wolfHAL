#ifndef WHAL_STM32F0_FLASH_H
#define WHAL_STM32F0_FLASH_H

#include <wolfHAL/flash/flash.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32f0_flash.h
 * @brief STM32F0 flash driver configuration.
 *
 * The STM32F0 embedded flash provides:
 * - Up to 256 KB organized in 2 KB pages
 * - Half-word (16-bit) programming
 * - Page erase via FLASH_AR address register
 * - Configurable wait states based on CPU frequency
 */

typedef struct whal_Stm32f0Flash_Cfg {
    size_t startAddr;
    size_t size;
    whal_Timeout *timeout;
} whal_Stm32f0Flash_Cfg;

typedef enum whal_Stm32f0Flash_Latency {
    WHAL_STM32F0_FLASH_LATENCY_0,
    WHAL_STM32F0_FLASH_LATENCY_1,
} whal_Stm32f0Flash_Latency;

#ifndef WHAL_CFG_FLASH_API_MAPPING_STM32F0
extern const whal_FlashDriver whal_Stm32f0Flash_Driver;

whal_Error whal_Stm32f0Flash_Init(whal_Flash *flashDev);
whal_Error whal_Stm32f0Flash_Deinit(whal_Flash *flashDev);
whal_Error whal_Stm32f0Flash_Lock(whal_Flash *flashDev, size_t addr, size_t len);
whal_Error whal_Stm32f0Flash_Unlock(whal_Flash *flashDev, size_t addr, size_t len);
whal_Error whal_Stm32f0Flash_Read(whal_Flash *flashDev, size_t addr, void *data,
                                   size_t dataSz);
whal_Error whal_Stm32f0Flash_Write(whal_Flash *flashDev, size_t addr,
                                    const void *data, size_t dataSz);
whal_Error whal_Stm32f0Flash_Erase(whal_Flash *flashDev, size_t addr,
                                    size_t dataSz);
#endif /* !WHAL_CFG_FLASH_API_MAPPING_STM32F0 */

whal_Error whal_Stm32f0Flash_Ext_SetLatency(whal_Flash *flashDev,
                                              enum whal_Stm32f0Flash_Latency latency);

#endif /* WHAL_STM32F0_FLASH_H */
