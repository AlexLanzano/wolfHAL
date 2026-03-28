#ifndef WHAL_STM32H5_FLASH_H
#define WHAL_STM32H5_FLASH_H

#include <wolfHAL/flash/flash.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32h5_flash.h
 * @brief STM32H5 flash driver configuration.
 *
 * The STM32H5 embedded flash provides:
 * - Up to 2 MB organized in dual banks of 8 KB sectors
 * - 128-bit (flash-word) programming with write buffer
 * - Sector erase and bank/mass erase operations
 * - Non-secure and secure register variants (this driver uses non-secure)
 */

/*
 * @brief Flash device configuration.
 */
typedef struct whal_Stm32h5Flash_Cfg {
    size_t startAddr;     /* Flash base address (typically 0x08000000) */
    size_t size;          /* Flash size in bytes */
    whal_Timeout *timeout;
} whal_Stm32h5Flash_Cfg;

/*
 * @brief Driver instance for STM32H5 flash.
 */
extern const whal_FlashDriver whal_Stm32h5Flash_Driver;

/*
 * @brief Initialize the STM32H5 flash interface.
 *
 * @param flashDev Flash device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Flash_Init(whal_Flash *flashDev);

/*
 * @brief Deinitialize the STM32H5 flash interface.
 *
 * @param flashDev Flash device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Flash_Deinit(whal_Flash *flashDev);

/*
 * @brief Lock the flash control register.
 *
 * @param flashDev Flash device instance.
 * @param addr     Unused.
 * @param len      Unused.
 *
 * @retval WHAL_SUCCESS Lock applied.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Flash_Lock(whal_Flash *flashDev, size_t addr,
                                   size_t len);

/*
 * @brief Unlock the flash control register.
 *
 * @param flashDev Flash device instance.
 * @param addr     Unused.
 * @param len      Unused.
 *
 * @retval WHAL_SUCCESS Unlock applied.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Flash_Unlock(whal_Flash *flashDev, size_t addr,
                                     size_t len);

/*
 * @brief Read data from flash into a buffer.
 *
 * @param flashDev Flash device instance.
 * @param addr     Flash address to read.
 * @param data     Destination buffer.
 * @param dataSz   Number of bytes to read.
 *
 * @retval WHAL_SUCCESS Read completed.
 */
whal_Error whal_Stm32h5Flash_Read(whal_Flash *flashDev, size_t addr,
                                   uint8_t *data, size_t dataSz);

/*
 * @brief Write data to flash.
 *
 * Data is programmed in 128-bit (16 byte) flash-word chunks.
 * Address and size must be 16-byte aligned.
 *
 * @param flashDev Flash device instance.
 * @param addr     Flash address to program (16-byte aligned).
 * @param data     Buffer to program.
 * @param dataSz   Number of bytes to program (multiple of 16).
 *
 * @retval WHAL_SUCCESS   Program completed.
 * @retval WHAL_EINVAL    Invalid arguments or alignment.
 * @retval WHAL_EHARDWARE Flash error during programming.
 */
whal_Error whal_Stm32h5Flash_Write(whal_Flash *flashDev, size_t addr,
                                    const uint8_t *data, size_t dataSz);

/*
 * @brief Erase flash sectors covering the given range.
 *
 * Erases 8 KB sectors. Address must fall within configured flash bounds.
 *
 * @param flashDev Flash device instance.
 * @param addr     Flash address to start erasing.
 * @param dataSz   Number of bytes to erase.
 *
 * @retval WHAL_SUCCESS   Erase completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_EHARDWARE Flash error during erase.
 */
whal_Error whal_Stm32h5Flash_Erase(whal_Flash *flashDev, size_t addr,
                                    size_t dataSz);

#endif /* WHAL_STM32H5_FLASH_H */
