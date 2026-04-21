#ifndef WHAL_STM32WBA_FLASH_H
#define WHAL_STM32WBA_FLASH_H

#include <wolfHAL/flash/flash.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32wba_flash.h
 * @brief STM32WBA flash driver configuration.
 *
 * The STM32WBA embedded flash provides:
 * - Up to 1 MB organized in 8 KB pages (128 pages)
 * - 128-bit (flash-word) programming
 * - Non-secure register variants (NSCR1, NSSR, NSKEYR)
 * - TrustZone support (this driver uses non-secure registers)
 *
 * Key register differences from STM32H5:
 * - NSKEYR at offset 0x008 (not 0x004)
 * - No separate NSCCR register (errors cleared via NSSR)
 * - Single bank (no BKSEL bit)
 */

typedef struct whal_Stm32wbaFlash_Cfg {
    size_t startAddr;
    size_t size;
    whal_Timeout *timeout;
} whal_Stm32wbaFlash_Cfg;

extern const whal_FlashDriver whal_Stm32wbaFlash_Driver;

whal_Error whal_Stm32wbaFlash_Init(whal_Flash *flashDev);
whal_Error whal_Stm32wbaFlash_Deinit(whal_Flash *flashDev);
whal_Error whal_Stm32wbaFlash_Lock(whal_Flash *flashDev, size_t addr, size_t len);
whal_Error whal_Stm32wbaFlash_Unlock(whal_Flash *flashDev, size_t addr, size_t len);
whal_Error whal_Stm32wbaFlash_Read(whal_Flash *flashDev, size_t addr, void *data, size_t dataSz);
whal_Error whal_Stm32wbaFlash_Write(whal_Flash *flashDev, size_t addr, const void *data, size_t dataSz);
whal_Error whal_Stm32wbaFlash_Erase(whal_Flash *flashDev, size_t addr, size_t dataSz);

whal_Error whal_Stm32wbaFlash_Ext_SetLatency(whal_Flash *flashDev, uint8_t latency);

#endif /* WHAL_STM32WBA_FLASH_H */
