#ifndef WHAL_ST_FLASH_H
#define WHAL_ST_FLASH_H

#include <wolfHAL/flash/flash.h>

/*
 * @file st_flash.h
 * @brief STM32-specific flash driver configuration and commands.
 */

/*
 * @brief STM32 flash configuration placeholder. Extend as options are needed.
 */
typedef struct whal_StFlash_Cfg {
    size_t startAddr;
    size_t size;
} whal_StFlash_Cfg;

/*
 * @brief Latency wait-state settings for STM32 flash.
 */
enum whal_StFlash_Latency {
    WHAL_ST_FLASH_LATENCY_0,
    WHAL_ST_FLASH_LATENCY_1,
    WHAL_ST_FLASH_LATENCY_2,
    WHAL_ST_FLASH_LATENCY_3,
};

/*
 * @brief Command arguments for changing flash latency.
 */
typedef struct whal_StFlash_SetLatencyArgs {
    enum whal_StFlash_Latency latency;
} whal_StFlash_SetLatencyArgs;

/*
 * @brief STM32 flash driver command selectors.
 */
enum whal_StFlash_Cmd {
    WHAL_ST_FLASH_CMD_SET_LATENCY,
};

/*
 * @brief Driver instance for STM32 flash.
 */
extern whal_FlashDriver whal_StFlash_Driver;

/*
 * @brief Initialize the STM32 flash interface.
 *
 * @param flashDev Flash device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StFlash_Init(whal_Flash *flashDev);
/*
 * @brief Deinitialize the STM32 flash interface.
 *
 * @param flashDev Flash device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StFlash_Deinit(whal_Flash *flashDev);
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
whal_Error whal_StFlash_Lock(whal_Flash *flashDev, size_t addr, size_t len);
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
whal_Error whal_StFlash_Unlock(whal_Flash *flashDev, size_t addr, size_t len);
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
whal_Error whal_StFlash_Read(whal_Flash *flashDev, size_t addr, uint8_t *data,
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
whal_Error whal_StFlash_Write(whal_Flash *flashDev, size_t addr, uint8_t *data,
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
whal_Error whal_StFlash_Erase(whal_Flash *flashDev, size_t addr, size_t dataSz);
/*
 * @brief Dispatch a driver-specific flash command.
 *
 * @param flashDev Flash device instance.
 * @param cmd      Driver-defined command selector.
 * @param args     Optional command arguments.
 *
 * @retval WHAL_SUCCESS Command handled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StFlash_Cmd(whal_Flash *flashDev, size_t cmd, void *args);

#endif /* WHAL_ST_FLASH_H */
