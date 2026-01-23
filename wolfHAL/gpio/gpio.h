#ifndef WHAL_GPIO_H
#define WHAL_GPIO_H

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <stddef.h>

/*
 * @file gpio.h
 * @brief Generic GPIO abstraction and driver interface.
 */

typedef struct whal_Gpio whal_Gpio;

/*
 * @brief Driver vtable for GPIO devices.
 */
typedef struct {
    /* Initialize GPIO hardware and configured pins. */
    whal_Error (*Init)(whal_Gpio *gpioDev); 
    /* Deinitialize GPIO hardware. */
    whal_Error (*Deinit)(whal_Gpio *gpioDev); 
    /* Read a pin value. */
    whal_Error (*Get)(whal_Gpio *gpioDev, size_t pin, size_t *value); 
    /* Write a pin value. */
    whal_Error (*Set)(whal_Gpio *gpioDev, size_t pin, size_t value); 
    /* Issue driver-specific commands. */
    whal_Error (*Cmd)(whal_Gpio *gpioDev, size_t cmd, void *args);
} whal_GpioDriver;

/*
 * @brief GPIO device instance containing driver, configuration, and pin table.
 */
struct whal_Gpio {
    const whal_Regmap regmap;
    const whal_GpioDriver *driver;
    void *pinCfg;
    size_t pinCount;
};

/*
 * @brief Initialize a GPIO device and its pins.
 *
 * @param gpioDev GPIO instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or missing callbacks.
 */
whal_Error whal_Gpio_Init(whal_Gpio *gpioDev);
/*
 * @brief Deinitialize a GPIO device.
 *
 * @param gpioDev GPIO instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer or missing callbacks.
 */
whal_Error whal_Gpio_Deinit(whal_Gpio *gpioDev);
/*
 * @brief Read the state of a pin.
 *
 * @param gpioDev GPIO instance containing the pin.
 * @param pin     Pin index into the configured pin table.
 * @param value   Storage for the sampled pin value.
 *
 * @retval WHAL_SUCCESS Pin value stored in @p value.
 * @retval WHAL_EINVAL  Null pointer, missing callbacks, or bad pin.
 */
whal_Error whal_Gpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value);
/*
 * @brief Set the state of a pin.
 *
 * @param gpioDev GPIO instance containing the pin.
 * @param pin     Pin index into the configured pin table.
 * @param value   Output value to drive (typically 0 or 1).
 *
 * @retval WHAL_SUCCESS Pin updated.
 * @retval WHAL_EINVAL  Null pointer, missing callbacks, or bad pin.
 */
whal_Error whal_Gpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value);
/*
 * @brief Send a driver-specific command to the GPIO device.
 *
 * @param gpioDev GPIO instance to command.
 * @param cmd     Driver-defined command selector.
 * @param args    Optional command argument payload.
 *
 * @retval WHAL_SUCCESS Command accepted and executed.
 * @retval WHAL_EINVAL  Null pointer or missing callbacks.
 */
whal_Error whal_Gpio_Cmd(whal_Gpio *gpioDev, size_t cmd, void *args);

#endif /* WHAL_GPIO_H */
