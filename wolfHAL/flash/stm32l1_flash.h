#ifndef WHAL_STM32L1_FLASH_H
#define WHAL_STM32L1_FLASH_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32l1_flash.h
 * @brief STM32L1 flash driver configuration.
 *
 * The STM32L1 flash uses a PECR-based unlock model:
 *   1. Unlock FLASH_PECR via FLASH_PEKEYR (PEKEY1=0x89ABCDEF, PEKEY2=0x02030405)
 *   2. Unlock program memory via FLASH_PRGKEYR (PRGKEY1=0x8C9DAEBF, PRGKEY2=0x13141516)
 *
 * Register layout:
 *   FLASH_ACR     = 0x00  (LATENCY, ACC64, PRFTEN, RUN_PD, SLEEP_PD)
 *   FLASH_PECR    = 0x04  (PELOCK, PRGLOCK, OPTLOCK, PROG, ERASE, FPRG, DATA, FTDW)
 *   FLASH_PDKEYR  = 0x08
 *   FLASH_PEKEYR  = 0x0C
 *   FLASH_PRGKEYR = 0x10
 *   FLASH_OPTKEYR = 0x14
 *   FLASH_SR      = 0x18  (BSY, EOP, ENDHV, READY, WRPERR, PGAERR, SIZERR, OPTVERR)
 *   FLASH_OBR     = 0x1C
 *   FLASH_WRPR1   = 0x20
 *
 * Programming:
 *   - Write alignment: 4 bytes (word)
 *   - Erase granularity: 256 bytes (page)
 *   - Fast word write: unlock PECR + PRGKEYR, write 32-bit word to flash address
 *   - Page erase: set ERASE+PROG in PECR, write 0x00000000 to first word of page
 */

typedef struct whal_Stm32l1Flash_Cfg {
    size_t startAddr;
    size_t size;
    whal_Timeout *timeout;
} whal_Stm32l1Flash_Cfg;

/*
 * @brief Flash access latency values for STM32L1.
 *
 * 0 WS: HCLK <= 16 MHz (voltage range 1 / 2), HCLK <= 8 MHz (range 3)
 * 1 WS: 16 < HCLK <= 32 MHz (range 1), 8 < HCLK <= 16 MHz (range 2/3)
 */
typedef enum {
    WHAL_STM32L1_FLASH_LATENCY_0 = 0,
    WHAL_STM32L1_FLASH_LATENCY_1 = 1,
} whal_Stm32l1Flash_Latency;

#ifndef WHAL_CFG_FLASH_API_MAPPING_STM32L1
extern const whal_FlashDriver whal_Stm32l1Flash_Driver;

whal_Error whal_Stm32l1Flash_Init(whal_Flash *flashDev);
whal_Error whal_Stm32l1Flash_Deinit(whal_Flash *flashDev);
whal_Error whal_Stm32l1Flash_Lock(whal_Flash *flashDev, size_t addr, size_t len);
whal_Error whal_Stm32l1Flash_Unlock(whal_Flash *flashDev, size_t addr, size_t len);
whal_Error whal_Stm32l1Flash_Read(whal_Flash *flashDev, size_t addr, void *data,
                                   size_t dataSz);
whal_Error whal_Stm32l1Flash_Write(whal_Flash *flashDev, size_t addr,
                                    const void *data, size_t dataSz);
whal_Error whal_Stm32l1Flash_Erase(whal_Flash *flashDev, size_t addr,
                                    size_t dataSz);
#endif /* !WHAL_CFG_FLASH_API_MAPPING_STM32L1 */

/*
 * @brief Set flash access latency (FLASH_ACR).
 *
 * Enables 64-bit access (ACC64) and prefetch (PRFTEN) as required by RM0038
 * §3.9.1 before programming the LATENCY bit, and polls until LATENCY matches
 * the requested value. Must be called by the board before increasing SYSCLK
 * beyond 16 MHz.
 */
whal_Error whal_Stm32l1Flash_Ext_SetLatency(whal_Stm32l1Flash_Latency latency);

#endif /* WHAL_STM32L1_FLASH_H */
