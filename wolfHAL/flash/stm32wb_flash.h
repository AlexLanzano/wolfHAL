#ifndef WHAL_STM32WB_FLASH_H
#define WHAL_STM32WB_FLASH_H

#include <wolfHAL/flash/flash.h>
#include <wolfHAL/clock/clock.h>

/*
 * @file stm32wb_flash.h
 * @brief STM32-specific flash driver configuration and helpers.
 */

/*
 * @brief STM32 flash configuration placeholder. Extend as options are needed.
 */
typedef struct whal_Stm32wbFlash_Cfg {
    whal_Clock *clkCtrl;
    const void *clk;
    size_t startAddr;
    size_t size;
} whal_Stm32wbFlash_Cfg;

/*
 * @brief Latency wait-state settings for STM32 flash.
 */
typedef enum whal_Stm32wbFlash_Latency {
    WHAL_STM32WB_FLASH_LATENCY_0,
    WHAL_STM32WB_FLASH_LATENCY_1,
    WHAL_STM32WB_FLASH_LATENCY_2,
    WHAL_STM32WB_FLASH_LATENCY_3,
} whal_Stm32wbFlash_Latency;

/*
 * @brief Driver instance for STM32 flash.
 */
extern whal_FlashDriver whal_Stm32wbFlash_Driver;

/*
 * @brief Initialize the STM32 flash interface.
 *
 * @param flashDev Flash device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbFlash_Init(whal_Flash *flashDev);
/*
 * @brief Deinitialize the STM32 flash interface.
 *
 * @param flashDev Flash device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbFlash_Deinit(whal_Flash *flashDev);
/*
 * @brief Lock a flash range.
 *
 * @param flashDev Flash device instance.
 * @param addr     Flash address to lock.
 * @param len      Number of bytes to lock.
 *
 * @retval WHAL_SUCCESS Lock applied.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbFlash_Lock(whal_Flash *flashDev, size_t addr, size_t len);
/*
 * @brief Unlock a flash range.
 *
 * @param flashDev Flash device instance.
 * @param addr     Flash address to unlock.
 * @param len      Number of bytes to unlock.
 *
 * @retval WHAL_SUCCESS Unlock applied.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbFlash_Unlock(whal_Flash *flashDev, size_t addr, size_t len);
/*
 * @brief Read data from flash into a buffer.
 *
 * @param flashDev Flash device instance.
 * @param addr     Flash address to read.
 * @param data     Destination buffer.
 * @param dataSz   Number of bytes to read.
 *
 * @retval WHAL_SUCCESS Read completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbFlash_Read(whal_Flash *flashDev, size_t addr, uint8_t *data,
                             size_t dataSz);
/*
 * @brief Write a block of data to flash.
 *
 * @param flashDev Flash device instance.
 * @param addr     Flash address to program.
 * @param data     Buffer to program.
 * @param dataSz   Number of bytes to program.
 *
 * @retval WHAL_SUCCESS Program completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbFlash_Write(whal_Flash *flashDev, size_t addr, const uint8_t *data,
                              size_t dataSz);
/*
 * @brief Erase a flash range.
 *
 * @param flashDev Flash device instance.
 * @param addr     Flash address to start erasing.
 * @param dataSz   Number of bytes to erase.
 *
 * @retval WHAL_SUCCESS Erase completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbFlash_Erase(whal_Flash *flashDev, size_t addr, size_t dataSz);
/*
 * @brief Update flash latency wait states.
 *
 * @param flashDev Flash device instance.
 * @param latency  Latency setting to apply.
 *
 * @retval WHAL_SUCCESS Latency updated.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbFlash_Ext_SetLatency(whal_Flash *flashDev, enum whal_Stm32wbFlash_Latency latency);

#endif /* WHAL_STM32WB_FLASH_H */
