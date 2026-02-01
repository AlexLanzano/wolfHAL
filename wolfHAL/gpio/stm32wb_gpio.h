#ifndef WHAL_STM32WB_GPIO_H
#define WHAL_STM32WB_GPIO_H

#include <stdint.h>
#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/regmap.h>

/*
 * @file stm32wb_gpio.h
 * @brief STM32 GPIO driver configuration types.
 */

/*
 * @brief STM32 GPIO port identifiers.
 */
typedef enum {
    WHAL_STM32WB_GPIO_PORT_A,
    WHAL_STM32WB_GPIO_PORT_B,
    WHAL_STM32WB_GPIO_PORT_C,
    WHAL_STM32WB_GPIO_PORT_D,
    WHAL_STM32WB_GPIO_PORT_E,
    WHAL_STM32WB_GPIO_PORT_F,
    WHAL_STM32WB_GPIO_PORT_G,
    WHAL_STM32WB_GPIO_PORT_H,
} whal_Stm32wbGpio_Port;

/*
 * @brief GPIO direction and function modes.
 */
typedef enum {
    WHAL_STM32WB_GPIO_MODE_IN,
    WHAL_STM32WB_GPIO_MODE_OUT,
    WHAL_STM32WB_GPIO_MODE_ALTFN,
    WHAL_STM32WB_GPIO_MODE_ANALOG,
} whal_Stm32wbGpio_Mode;

/*
 * @brief Output driver type.
 */
typedef enum {
    WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
    WHAL_STM32WB_GPIO_OUTTYPE_OPENDRAIN,
} whal_Stm32wbGpio_OutType;

/*
 * @brief Output speed settings.
 */
typedef enum {
    WHAL_STM32WB_GPIO_SPEED_LOW,
    WHAL_STM32WB_GPIO_SPEED_MEDIUM,
    WHAL_STM32WB_GPIO_SPEED_FAST,
    WHAL_STM32WB_GPIO_SPEED_HIGH,
} whal_Stm32wbGpio_Speed;

/*
 * @brief Pull resistor configuration.
 */
typedef enum {
    WHAL_STM32WB_GPIO_PULL_NONE,
    WHAL_STM32WB_GPIO_PULL_UP,
    WHAL_STM32WB_GPIO_PULL_DOWN,
} whal_Stm32wbGpio_Pull;

/*
 * @brief Per-pin STM32 GPIO configuration.
 */
typedef struct {
    whal_Stm32wbGpio_Port port;
    uint8_t pin;
    whal_Stm32wbGpio_Mode mode;
    whal_Stm32wbGpio_OutType outType;
    whal_Stm32wbGpio_Speed speed;
    whal_Stm32wbGpio_Pull pull;
    uint8_t altFn;
} whal_Stm32wbGpio_PinCfg;

typedef struct {
    whal_Clock *clkCtrl;
    void *clk;

    whal_Stm32wbGpio_PinCfg *pinCfg;
    size_t pinCount;
} whal_Stm32wbGpio_Cfg;

/*
 * @brief Driver instance for STM32 GPIO.
 */
extern const whal_GpioDriver whal_Stm32wbGpio_Driver;

/*
 * @brief Initialize the STM32 GPIO peripheral and configured pins.
 *
 * @param gpioDev GPIO device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbGpio_Init(whal_Gpio *gpioDev);
/*
 * @brief Deinitialize the STM32 GPIO peripheral.
 *
 * @param gpioDev GPIO device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbGpio_Deinit(whal_Gpio *gpioDev);
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
whal_Error whal_Stm32wbGpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value);
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
whal_Error whal_Stm32wbGpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value);

#endif /* WHAL_STM32WB_GPIO_H */
