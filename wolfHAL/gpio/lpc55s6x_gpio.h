/* lpc55s6x_gpio.h
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#ifndef WHAL_LPC55S6X_GPIO_H
#define WHAL_LPC55S6X_GPIO_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/bitops.h>

/*
 * @file lpc55s6x_gpio.h
 * @brief LPC55S6x GPIO driver configuration.
 */

/*
 * @brief Pin direction selection.
 */
enum {
    WHAL_LPC55S6X_GPIO_DIR_INPUT,  /* Configure pin as input */
    WHAL_LPC55S6X_GPIO_DIR_OUTPUT, /* Configure pin as output */
};

/*
 * IOCON pin register bit fields.
 *
 * FUNC..OD (bits 9:0) are common to all three pin types (D, A, I). ASW is
 * Type A only; SSEL..I2CFILTER are Type I only (pins PIO0_13/PIO0_14). One
 * 32-bit register per pin; see the WHAL_LPC55S6X_IOCON_TYPE_* helpers below.
 */
#define WHAL_LPC55S6X_IOCON_FUNC_Pos      0
#define WHAL_LPC55S6X_IOCON_FUNC_Msk      (WHAL_BITMASK(4) << WHAL_LPC55S6X_IOCON_FUNC_Pos)
#define WHAL_LPC55S6X_IOCON_MODE_Pos      4
#define WHAL_LPC55S6X_IOCON_MODE_Msk      (WHAL_BITMASK(2) << WHAL_LPC55S6X_IOCON_MODE_Pos)
#define WHAL_LPC55S6X_IOCON_SLEW_Pos      6
#define WHAL_LPC55S6X_IOCON_SLEW_Msk      (1UL << WHAL_LPC55S6X_IOCON_SLEW_Pos)
#define WHAL_LPC55S6X_IOCON_INVERT_Pos    7
#define WHAL_LPC55S6X_IOCON_INVERT_Msk    (1UL << WHAL_LPC55S6X_IOCON_INVERT_Pos)
#define WHAL_LPC55S6X_IOCON_DIGIMODE_Pos  8
#define WHAL_LPC55S6X_IOCON_DIGIMODE_Msk  (1UL << WHAL_LPC55S6X_IOCON_DIGIMODE_Pos)
#define WHAL_LPC55S6X_IOCON_OD_Pos        9
#define WHAL_LPC55S6X_IOCON_OD_Msk        (1UL << WHAL_LPC55S6X_IOCON_OD_Pos)

/* Type A only. */
#define WHAL_LPC55S6X_IOCON_ASW_Pos       10
#define WHAL_LPC55S6X_IOCON_ASW_Msk       (1UL << WHAL_LPC55S6X_IOCON_ASW_Pos)

/* Type I only (PIO0_13, PIO0_14). */
#define WHAL_LPC55S6X_IOCON_SSEL_Pos      10
#define WHAL_LPC55S6X_IOCON_SSEL_Msk      (1UL << WHAL_LPC55S6X_IOCON_SSEL_Pos)
#define WHAL_LPC55S6X_IOCON_EGP_Pos       11
#define WHAL_LPC55S6X_IOCON_EGP_Msk       (1UL << WHAL_LPC55S6X_IOCON_EGP_Pos)
#define WHAL_LPC55S6X_IOCON_FILTEROFF_Pos 12
#define WHAL_LPC55S6X_IOCON_FILTEROFF_Msk (1UL << WHAL_LPC55S6X_IOCON_FILTEROFF_Pos)
#define WHAL_LPC55S6X_IOCON_ECS_Pos       13
#define WHAL_LPC55S6X_IOCON_ECS_Msk       (1UL << WHAL_LPC55S6X_IOCON_ECS_Pos)
#define WHAL_LPC55S6X_IOCON_I2CDRIVE_Pos  14
#define WHAL_LPC55S6X_IOCON_I2CDRIVE_Msk  (1UL << WHAL_LPC55S6X_IOCON_I2CDRIVE_Pos)
#define WHAL_LPC55S6X_IOCON_I2CFILTER_Pos 15
#define WHAL_LPC55S6X_IOCON_I2CFILTER_Msk (1UL << WHAL_LPC55S6X_IOCON_I2CFILTER_Pos)

/*
 * @brief IOCON MODE field values (on-chip pull-up/pull-down control).
 */
#define WHAL_LPC55S6X_IOCON_MODE_INACTIVE 0x0
#define WHAL_LPC55S6X_IOCON_MODE_PULLDOWN 0x1
#define WHAL_LPC55S6X_IOCON_MODE_PULLUP   0x2
#define WHAL_LPC55S6X_IOCON_MODE_REPEATER 0x3

/*
 * @brief Build a Type D (standard digital pin) IOCON word.
 *
 * @param func     Pin function select (FUNC).
 * @param mode     Pull mode (WHAL_LPC55S6X_IOCON_MODE_*).
 * @param slew     Slew rate: 0 standard, 1 fast.
 * @param invert   Input polarity: 0 normal, 1 inverted.
 * @param digimode Digital mode: 0 analog, 1 digital.
 * @param od       Output drive: 0 push-pull, 1 open-drain.
 */
#define WHAL_LPC55S6X_IOCON_TYPE_D(func, mode, slew, invert, digimode, od) ( \
    whal_SetBits(WHAL_LPC55S6X_IOCON_FUNC_Msk,     WHAL_LPC55S6X_IOCON_FUNC_Pos,     (func))     | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_MODE_Msk,     WHAL_LPC55S6X_IOCON_MODE_Pos,     (mode))     | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_SLEW_Msk,     WHAL_LPC55S6X_IOCON_SLEW_Pos,     (slew))     | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_INVERT_Msk,   WHAL_LPC55S6X_IOCON_INVERT_Pos,   (invert))   | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_DIGIMODE_Msk, WHAL_LPC55S6X_IOCON_DIGIMODE_Pos, (digimode)) | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_OD_Msk,       WHAL_LPC55S6X_IOCON_OD_Pos,       (od)))

/*
 * @brief Build a Type A (analog-capable digital pin) IOCON word.
 *
 * @param func     Pin function select (FUNC).
 * @param mode     Pull mode (WHAL_LPC55S6X_IOCON_MODE_*).
 * @param slew     Slew rate: 0 standard, 1 fast.
 * @param invert   Input polarity: 0 normal, 1 inverted.
 * @param digimode Digital mode: 0 analog, 1 digital.
 * @param od       Output drive: 0 push-pull, 1 open-drain.
 * @param asw      Analog switch (usable only when digimode = 0).
 */
#define WHAL_LPC55S6X_IOCON_TYPE_A(func, mode, slew, invert, digimode, od, asw) ( \
    whal_SetBits(WHAL_LPC55S6X_IOCON_FUNC_Msk,     WHAL_LPC55S6X_IOCON_FUNC_Pos,     (func))     | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_MODE_Msk,     WHAL_LPC55S6X_IOCON_MODE_Pos,     (mode))     | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_SLEW_Msk,     WHAL_LPC55S6X_IOCON_SLEW_Pos,     (slew))     | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_INVERT_Msk,   WHAL_LPC55S6X_IOCON_INVERT_Pos,   (invert))   | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_DIGIMODE_Msk, WHAL_LPC55S6X_IOCON_DIGIMODE_Pos, (digimode)) | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_OD_Msk,       WHAL_LPC55S6X_IOCON_OD_Pos,       (od))       | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_ASW_Msk,      WHAL_LPC55S6X_IOCON_ASW_Pos,      (asw)))

/*
 * @brief Build a Type I (I2C/MFIO combo pin, PIO0_13/PIO0_14) IOCON word.
 *
 * @param func      Pin function select (FUNC).
 * @param mode      Pull mode (WHAL_LPC55S6X_IOCON_MODE_*).
 * @param slew      Slew rate: 0 standard, 1 fast.
 * @param invert    Input polarity: 0 normal, 1 inverted.
 * @param digimode  Digital mode: 0 analog, 1 digital.
 * @param od        Open-drain in GPIO mode (EGP = 1); no effect in I2C mode.
 * @param ssel      I2C supply selection: 0 = 3V3, 1 = 1V8.
 * @param egp       0 I2C mode, 1 GPIO mode.
 * @param filteroff Input glitch filter: 0 enabled, 1 disabled.
 * @param ecs       I2C pull-up current source enable.
 * @param i2cdrive  I2C drive strength.
 * @param i2cfilter I2C glitch filter: 0 = 50 ns, 1 = 10 ns.
 */
#define WHAL_LPC55S6X_IOCON_TYPE_I(func, mode, slew, invert, digimode, od, ssel, egp, \
                                   filteroff, ecs, i2cdrive, i2cfilter) ( \
    whal_SetBits(WHAL_LPC55S6X_IOCON_FUNC_Msk,      WHAL_LPC55S6X_IOCON_FUNC_Pos,      (func))      | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_MODE_Msk,      WHAL_LPC55S6X_IOCON_MODE_Pos,      (mode))      | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_SLEW_Msk,      WHAL_LPC55S6X_IOCON_SLEW_Pos,      (slew))      | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_INVERT_Msk,    WHAL_LPC55S6X_IOCON_INVERT_Pos,    (invert))    | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_DIGIMODE_Msk,  WHAL_LPC55S6X_IOCON_DIGIMODE_Pos,  (digimode))  | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_OD_Msk,        WHAL_LPC55S6X_IOCON_OD_Pos,        (od))        | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_SSEL_Msk,      WHAL_LPC55S6X_IOCON_SSEL_Pos,      (ssel))      | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_EGP_Msk,       WHAL_LPC55S6X_IOCON_EGP_Pos,       (egp))       | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_FILTEROFF_Msk, WHAL_LPC55S6X_IOCON_FILTEROFF_Pos, (filteroff)) | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_ECS_Msk,       WHAL_LPC55S6X_IOCON_ECS_Pos,       (ecs))       | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_I2CDRIVE_Msk,  WHAL_LPC55S6X_IOCON_I2CDRIVE_Pos,  (i2cdrive))  | \
    whal_SetBits(WHAL_LPC55S6X_IOCON_I2CFILTER_Msk, WHAL_LPC55S6X_IOCON_I2CFILTER_Pos, (i2cfilter)))

/*
 * @brief Single pin configuration.
 */
typedef struct whal_Lpc55s6x_Gpio_PinCfg {
    uint8_t  port;      /* Port index */
    uint8_t  pin;       /* Pin number within port */
    uint8_t  dir;       /* Direction: WHAL_LPC55S6X_GPIO_DIR_INPUT/OUTPUT */
    uint32_t ioconCfg;  /* Packed IOCON register word (WHAL_LPC55S6X_IOCON_TYPE_*) */
} whal_Lpc55s6x_Gpio_PinCfg;

/*
 * @brief GPIO device configuration.
 *
 * Holds the table of pins applied during Init.
 */
typedef struct whal_Lpc55s6x_Gpio_Cfg {
    size_t pinCfgCount;                 /* Number of pins to configure */
    whal_Lpc55s6x_Gpio_PinCfg *pinCfg;  /* Array of pin configurations */
} whal_Lpc55s6x_Gpio_Cfg;

/*
 * @brief Platform-owned GPIO device singleton. Defined in the driver TU
 * from the WHAL_CFG_LPC55S6X_GPIO_DEV initializer in board.h.
 */
extern const whal_Gpio whal_Lpc55s6x_Gpio_Dev;

#ifndef WHAL_CFG_LPC55S6X_GPIO_DIRECT_API_MAPPING
/*
 * @brief Driver instance for LPC55S6x GPIO.
 */
extern const whal_GpioDriver whal_Lpc55s6x_Gpio_Driver;

/*
 * @brief Initialize the LPC55S6x GPIO peripheral.
 *
 * @param gpioDev GPIO device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Gpio_Init(whal_Gpio *gpioDev);

/*
 * @brief Deinitialize the LPC55S6x GPIO peripheral.
 *
 * @param gpioDev GPIO device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Gpio_Deinit(whal_Gpio *gpioDev);

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
whal_Error whal_Lpc55s6x_Gpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value);

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
whal_Error whal_Lpc55s6x_Gpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value);
#endif /* !WHAL_CFG_LPC55S6X_GPIO_DIRECT_API_MAPPING */

#endif /* WHAL_LPC55S6X_GPIO_H */
