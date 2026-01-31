#ifndef WHAL_STGPIO_H
#define WHAL_STGPIO_H

#include <stdint.h>
#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/st_rcc.h>
#include <wolfHAL/regmap.h>

/*
 * @file st_gpio.h
 * @brief STM32 GPIO driver configuration types.
 */

/*
 * @brief STM32 GPIO port identifiers.
 */
typedef enum {
    WHAL_STGPIO_PORT_A,
    WHAL_STGPIO_PORT_B,
    WHAL_STGPIO_PORT_C,
    WHAL_STGPIO_PORT_D,
    WHAL_STGPIO_PORT_E,
    WHAL_STGPIO_PORT_F,
    WHAL_STGPIO_PORT_G,
    WHAL_STGPIO_PORT_H,
} whal_StGpio_Port;

/*
 * @brief GPIO direction and function modes.
 */
typedef enum {
    WHAL_STGPIO_MODE_IN,
    WHAL_STGPIO_MODE_OUT,
    WHAL_STGPIO_MODE_ALTFN,
    WHAL_STGPIO_MODE_ANALOG,
} whal_StGpio_Mode;

/*
 * @brief Output driver type.
 */
typedef enum {
    WHAL_STGPIO_OUTTYPE_PUSHPULL,
    WHAL_STGPIO_OUTTYPE_OPENDRAIN,
} whal_StGpio_OutType;

/*
 * @brief Output speed settings.
 */
typedef enum {
    WHAL_STGPIO_SPEED_LOW,
    WHAL_STGPIO_SPEED_MEDIUM,
    WHAL_STGPIO_SPEED_FAST,
    WHAL_STGPIO_SPEED_HIGH,
} whal_StGpio_Speed;

/*
 * @brief Pull resistor configuration.
 */
typedef enum {
    WHAL_STGPIO_PULL_NONE,
    WHAL_STGPIO_PULL_UP,
    WHAL_STGPIO_PULL_DOWN,
} whal_StGpio_Pull;

/*
 * @brief Per-pin STM32 GPIO configuration.
 */
typedef struct {
    whal_StGpio_Port port;
    uint8_t pin;
    whal_StGpio_Mode mode;
    whal_StGpio_OutType outType;
    whal_StGpio_Speed speed;
    whal_StGpio_Pull pull;
    uint8_t altFn;
} whal_StGpio_PinCfg;

typedef struct {
    whal_Clock *clkCtrl;
    void *clk;

    whal_StGpio_PinCfg *pinCfg;
    size_t pinCount;
} whal_StGpio_Cfg;

/*
 * @brief Driver instance for STM32 GPIO.
 */
extern const whal_GpioDriver whal_StGpio_Driver;

/*
 * @brief Initialize the STM32 GPIO peripheral and configured pins.
 *
 * @param gpioDev GPIO device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StGpio_Init(whal_Gpio *gpioDev);
/*
 * @brief Deinitialize the STM32 GPIO peripheral.
 *
 * @param gpioDev GPIO device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StGpio_Deinit(whal_Gpio *gpioDev);
/*
 * @brief Read a GPIO pin value.
 *
 * @param gpioDev GPIO device instance.
 * @param pin     Pin index in the configured pin table.
 * @param value   Output for the sampled pin value.
 *
 * @retval WHAL_SUCCESS Pin value read.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StGpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value);
/*
 * @brief Set a GPIO pin value.
 *
 * @param gpioDev GPIO device instance.
 * @param pin     Pin index in the configured pin table.
 * @param value   Value to drive.
 *
 * @retval WHAL_SUCCESS Pin updated.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_StGpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value);

#endif /* WHAL_STGPIO_H */
