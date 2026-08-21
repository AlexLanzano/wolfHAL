/* lpc55s6x_gpio.c
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

#include "board.h"  /* provides WHAL_CFG_LPC55S6X_GPIO_DEV initializer */
#include <wolfHAL/gpio/lpc55s6x_gpio.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/error.h>

/* IOCON pin config register: one 32-bit register per pin (PIO0_0 at offset 0). */
#define IOCON_PIN_REG(port, pin) (((port) * 32 + (pin)) * 4)

/* GPIO registers, grouped by function; per-port register at group + port*4. */
#define GPIO_DIR_REG(port)    (0x2000 + (port) * 4)
#define GPIO_PIN_REG(port)    (0x2100 + (port) * 4)
#define GPIO_SET_REG(port)    (0x2200 + (port) * 4)
#define GPIO_CLR_REG(port)    (0x2280 + (port) * 4)
#define GPIO_DIRSET_REG(port) (0x2380 + (port) * 4)
#define GPIO_DIRCLR_REG(port) (0x2400 + (port) * 4)

const whal_Gpio whal_Lpc55s6x_Gpio_Dev = WHAL_CFG_LPC55S6X_GPIO_DEV;

#ifdef WHAL_CFG_LPC55S6X_GPIO_DIRECT_API_MAPPING
#define whal_Lpc55s6x_Gpio_Init   whal_Gpio_Init
#define whal_Lpc55s6x_Gpio_Deinit whal_Gpio_Deinit
#define whal_Lpc55s6x_Gpio_Get    whal_Gpio_Get
#define whal_Lpc55s6x_Gpio_Set    whal_Gpio_Set
#endif /* WHAL_CFG_LPC55S6X_GPIO_DIRECT_API_MAPPING */

whal_Error whal_Lpc55s6x_Gpio_Init(whal_Gpio *gpioDev)
{
    const whal_Lpc55s6x_Gpio_Cfg *cfg =
        (const whal_Lpc55s6x_Gpio_Cfg *)whal_Lpc55s6x_Gpio_Dev.cfg;
    size_t base = whal_Lpc55s6x_Gpio_Dev.base;
    (void)gpioDev;

    for (size_t i = 0; i < cfg->pinCfgCount; ++i) {
        const whal_Lpc55s6x_Gpio_PinCfg *pinCfg = &cfg->pinCfg[i];
        size_t pinMask = (1UL << pinCfg->pin);

        /* Pin mux + electrical config (function, pull, digital mode). */
        whal_Reg_Write(WHAL_LPC55S6X_IOCON_BASE,
                       IOCON_PIN_REG(pinCfg->port, pinCfg->pin),
                       pinCfg->ioconCfg);

        if (pinCfg->dir == WHAL_LPC55S6X_GPIO_DIR_OUTPUT) {
            whal_Reg_Write(base, GPIO_DIRSET_REG(pinCfg->port), pinMask);
        }
        else {
            whal_Reg_Write(base, GPIO_DIRCLR_REG(pinCfg->port), pinMask);
        }
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Gpio_Deinit(whal_Gpio *gpioDev)
{
    (void)gpioDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Gpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value)
{
    const whal_Lpc55s6x_Gpio_Cfg *cfg =
        (const whal_Lpc55s6x_Gpio_Cfg *)whal_Lpc55s6x_Gpio_Dev.cfg;
    size_t base = whal_Lpc55s6x_Gpio_Dev.base;
    whal_Lpc55s6x_Gpio_PinCfg *pinCfg;
    (void)gpioDev;

    if (!value || pin >= cfg->pinCfgCount)
        return WHAL_EINVAL;

    pinCfg = &cfg->pinCfg[pin];

    whal_Reg_Get(base, GPIO_PIN_REG(pinCfg->port),
                 1UL << pinCfg->pin, pinCfg->pin, value);
    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Gpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value)
{
    const whal_Lpc55s6x_Gpio_Cfg *cfg =
        (const whal_Lpc55s6x_Gpio_Cfg *)whal_Lpc55s6x_Gpio_Dev.cfg;
    size_t base = whal_Lpc55s6x_Gpio_Dev.base;
    whal_Lpc55s6x_Gpio_PinCfg *pinCfg;
    (void)gpioDev;

    if (pin >= cfg->pinCfgCount)
        return WHAL_EINVAL;

    pinCfg = &cfg->pinCfg[pin];

    if (value) {
        whal_Reg_Write(base, GPIO_SET_REG(pinCfg->port), 1UL << pinCfg->pin);
    }
    else {
        whal_Reg_Write(base, GPIO_CLR_REG(pinCfg->port), 1UL << pinCfg->pin);
    }
    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_LPC55S6X_GPIO_DIRECT_API_MAPPING
const whal_GpioDriver whal_Lpc55s6x_Gpio_Driver = {
    .Init = whal_Lpc55s6x_Gpio_Init,
    .Deinit = whal_Lpc55s6x_Gpio_Deinit,
    .Get = whal_Lpc55s6x_Gpio_Get,
    .Set = whal_Lpc55s6x_Gpio_Set,
};
#endif /* !WHAL_CFG_LPC55S6X_GPIO_DIRECT_API_MAPPING */
