#ifndef WHAL_FLASH_H
#define WHAL_FLASH_H

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <stddef.h>
#include <stdint.h>

/*
 * @file flash.h
 * @brief Generic flash abstraction and driver interface.
 */

typedef struct whal_Flash whal_Flash;

/*
 * @brief Driver vtable for flash devices.
 */
typedef struct {
    /* Bring the flash peripheral into a usable state. */
    whal_Error (*Init)(whal_Flash *flashDev);
    /* Release any resources owned by the flash driver. */
    whal_Error (*Deinit)(whal_Flash *flashDev);
    /* Program a region of flash starting at @p addr. */
    whal_Error (*Write)(whal_Flash *flashDev, size_t addr, uint8_t *data, size_t dataSz);
    /* Erase a flash range starting at @p addr. */
    whal_Error (*Erase)(whal_Flash *flashDev, size_t addr, size_t dataSz);
    /* Issue a driver-specific command. */
    whal_Error (*Cmd)(whal_Flash *flashDev, size_t cmd, void *args);
} whal_FlashDriver;

/*
 * @brief Flash device instance tying configuration to a driver implementation.
 */
struct whal_Flash {
    const whal_Regmap regmap;
    const whal_FlashDriver *driver;
    void *cfg;
};

/*
 * @brief Initialize a flash device and its driver.
 *
 * @param flashDev Flash instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver callbacks.
 */
whal_Error whal_Flash_Init(whal_Flash *flashDev);
/*
 * @brief Deinitialize a flash device.
 *
 * @param flashDev Flash instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver callbacks.
 */
whal_Error whal_Flash_Deinit(whal_Flash *flashDev);
/*
 * @brief Write data into flash.
 *
 * @param flashDev Flash instance to program.
 * @param addr     Byte address in flash to start writing.
 * @param data     Buffer containing data to write.
 * @param dataSz   Number of bytes to write.
 *
 * @retval WHAL_SUCCESS Write accepted or completed.
 * @retval WHAL_EINVAL  Null pointer, missing callbacks, or bad arguments.
 */
whal_Error whal_Flash_Write(whal_Flash *flashDev, size_t addr, uint8_t *data, size_t dataSz);
/*
 * @brief Erase a region of flash.
 *
 * @param flashDev Flash instance to erase.
 * @param addr     Byte address in flash where erasure starts.
 * @param dataSz   Number of bytes (or sector-aligned size) to erase.
 *
 * @retval WHAL_SUCCESS Erase accepted or completed.
 * @retval WHAL_EINVAL  Null pointer or missing callbacks.
 */
whal_Error whal_Flash_Erase(whal_Flash *flashDev, size_t addr, size_t dataSz);
/*
 * @brief Send a driver-specific command to the flash device.
 *
 * @param flashDev Flash instance to command.
 * @param cmd      Driver-defined command selector.
 * @param args     Optional command argument payload.
 *
 * @retval WHAL_SUCCESS Command accepted and executed.
 * @retval WHAL_EINVAL  Null pointer or missing callbacks.
 */
whal_Error whal_Flash_Cmd(whal_Flash *flashDev, size_t cmd, void *args);

#endif /* WHAL_FLASH_H */
