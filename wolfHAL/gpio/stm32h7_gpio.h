#ifndef WHAL_STM32H7_GPIO_H
#define WHAL_STM32H7_GPIO_H

/*
 * @file stm32h7_gpio.h
 * @brief STM32H7 GPIO driver (alias for STM32WB GPIO).
 *
 * The STM32H7 GPIO peripheral is register-compatible with the STM32WB GPIO.
 * This header re-exports the STM32WB GPIO driver types and symbols under
 * STM32H7-specific names. The underlying implementation is shared.
 */

#include <wolfHAL/gpio/stm32wb_gpio.h>

typedef whal_Stm32wbGpio_Cfg    whal_Stm32h7Gpio_Cfg;
typedef whal_Stm32wbGpio_PinCfg whal_Stm32h7Gpio_PinCfg;

#define whal_Stm32h7Gpio_Driver whal_Stm32wbGpio_Driver
#define whal_Stm32h7Gpio_Init   whal_Stm32wbGpio_Init
#define whal_Stm32h7Gpio_Deinit whal_Stm32wbGpio_Deinit
#define whal_Stm32h7Gpio_Get    whal_Stm32wbGpio_Get
#define whal_Stm32h7Gpio_Set    whal_Stm32wbGpio_Set

/*
 * @brief GPIO mode selection (re-exported from STM32WB).
 */
#define WHAL_STM32H7_GPIO_MODE_IN    WHAL_STM32WB_GPIO_MODE_IN
#define WHAL_STM32H7_GPIO_MODE_OUT   WHAL_STM32WB_GPIO_MODE_OUT
#define WHAL_STM32H7_GPIO_MODE_ALTFN WHAL_STM32WB_GPIO_MODE_ALTFN
#define WHAL_STM32H7_GPIO_MODE_ANALOG WHAL_STM32WB_GPIO_MODE_ANALOG

/*
 * @brief GPIO output type (re-exported from STM32WB).
 */
#define WHAL_STM32H7_GPIO_OUTTYPE_PUSHPULL  WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL
#define WHAL_STM32H7_GPIO_OUTTYPE_OPENDRAIN WHAL_STM32WB_GPIO_OUTTYPE_OPENDRAIN

/*
 * @brief GPIO speed selection (re-exported from STM32WB).
 */
#define WHAL_STM32H7_GPIO_SPEED_LOW    WHAL_STM32WB_GPIO_SPEED_LOW
#define WHAL_STM32H7_GPIO_SPEED_MEDIUM WHAL_STM32WB_GPIO_SPEED_MEDIUM
#define WHAL_STM32H7_GPIO_SPEED_FAST   WHAL_STM32WB_GPIO_SPEED_FAST
#define WHAL_STM32H7_GPIO_SPEED_HIGH   WHAL_STM32WB_GPIO_SPEED_HIGH

/*
 * @brief GPIO pull-up/pull-down selection (re-exported from STM32WB).
 */
#define WHAL_STM32H7_GPIO_PULL_NONE WHAL_STM32WB_GPIO_PULL_NONE
#define WHAL_STM32H7_GPIO_PULL_UP   WHAL_STM32WB_GPIO_PULL_UP
#define WHAL_STM32H7_GPIO_PULL_DOWN WHAL_STM32WB_GPIO_PULL_DOWN

/*
 * @brief GPIO port selection (re-exported from STM32WB).
 */
#define WHAL_STM32H7_GPIO_PORT_A WHAL_STM32WB_GPIO_PORT_A
#define WHAL_STM32H7_GPIO_PORT_B WHAL_STM32WB_GPIO_PORT_B
#define WHAL_STM32H7_GPIO_PORT_C WHAL_STM32WB_GPIO_PORT_C
#define WHAL_STM32H7_GPIO_PORT_D WHAL_STM32WB_GPIO_PORT_D
#define WHAL_STM32H7_GPIO_PORT_E WHAL_STM32WB_GPIO_PORT_E
#define WHAL_STM32H7_GPIO_PORT_F WHAL_STM32WB_GPIO_PORT_F
#define WHAL_STM32H7_GPIO_PORT_G WHAL_STM32WB_GPIO_PORT_G
#define WHAL_STM32H7_GPIO_PORT_H WHAL_STM32WB_GPIO_PORT_H

#endif /* WHAL_STM32H7_GPIO_H */
