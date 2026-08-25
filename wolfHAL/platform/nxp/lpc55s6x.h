/* lpc55s6x.h
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

#ifndef WHAL_LPC55S6X_H
#define WHAL_LPC55S6X_H

#include <wolfHAL/clock/lpc55s6x_syscon.h>
#include <wolfHAL/clock/lpc55s6x_anactrl.h>
#include <wolfHAL/gpio/lpc55s6x_gpio.h>
#include <wolfHAL/uart/lpc55s6x_uart.h>
#include <wolfHAL/spi/lpc55s6x_spi.h>
#include <wolfHAL/pwm/lpc55s6x_ctimer_pwm.h>
#include <wolfHAL/platform/arm/cortex_m33.h>

/*
 * @file lpc55s6x.h
 * @brief Convenience initializers for LPC55S6x device instances.
 *
 * TODO: fill in the real peripheral base addresses from the datasheet.
 */

#define WHAL_LPC55S6X_IOCON_BASE  0x40001000

#define WHAL_LPC55S6X_GPIO_BASE   0x4008C000
#define WHAL_LPC55S6X_GPIO_DRIVER &whal_Lpc55s6x_Gpio_Driver

#define WHAL_LPC55S6X_UART_BASE   0x40086000
#define WHAL_LPC55S6X_UART_DRIVER &whal_Lpc55s6x_Uart_Driver

#define WHAL_LPC55S6X_SPI_BASE    0x4009F000
#define WHAL_LPC55S6X_SPI_DRIVER  &whal_Lpc55s6x_Spi_Driver

#define WHAL_LPC55S6X_CTIMER2_BASE     0x40028000
#define WHAL_LPC55S6X_CTIMER_PWM_DRIVER &whal_Lpc55s6x_Ctimer_Pwm_Driver

/* Peripheral clock gate descriptors (AHBCLKCTRLn register + enable bit). */
#define WHAL_LPC55S6X_IOCON_CLK                     \
    .regOffset  = WHAL_LPC55S6X_SYSCON_AHBCLKCTRL0, \
    .enableMask = (1UL << 13)

#define WHAL_LPC55S6X_GPIO0_CLK                     \
    .regOffset  = WHAL_LPC55S6X_SYSCON_AHBCLKCTRL0, \
    .enableMask = (1UL << 14)

#define WHAL_LPC55S6X_GPIO1_CLK                     \
    .regOffset  = WHAL_LPC55S6X_SYSCON_AHBCLKCTRL0, \
    .enableMask = (1UL << 15)

#define WHAL_LPC55S6X_FC0_CLK                       \
    .regOffset  = WHAL_LPC55S6X_SYSCON_AHBCLKCTRL1, \
    .enableMask = (1UL << 11)

#define WHAL_LPC55S6X_HSSPI_CLK                     \
    .regOffset  = WHAL_LPC55S6X_SYSCON_AHBCLKCTRL2, \
    .enableMask = (1UL << 28)

#define WHAL_LPC55S6X_CTIMER2_CLK                   \
    .regOffset  = WHAL_LPC55S6X_SYSCON_AHBCLKCTRL1, \
    .enableMask = (1UL << 22)

#endif /* WHAL_LPC55S6X_H */
