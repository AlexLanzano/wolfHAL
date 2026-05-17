/* pic32cz.h
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

#ifndef WHAL_PIC32CZ_H
#define WHAL_PIC32CZ_H

#include <wolfHAL/clock/pic32cz_clock.h>
#include <wolfHAL/gpio/pic32cz_gpio.h>
#include <wolfHAL/uart/pic32cz_uart.h>
#include <wolfHAL/flash/pic32cz_flash.h>
#include <wolfHAL/platform/arm/cortex_m7.h>

/*
 * @file pic32cz.h
 * @brief Convenience initializers for PIC32CZ device instances.
 */

#define WHAL_PIC32CZ_FLASH_BASE 0x44002000
#define WHAL_PIC32CZ_FLASH_DRIVER &whal_Pic32cz_Flash_Driver

#define WHAL_PIC32CZ_GPIO_BASE 0x44840000
#define WHAL_PIC32CZ_GPIO_DRIVER &whal_Pic32cz_Gpio_Driver

#define WHAL_PIC32CZ_SERCOM4_UART_BASE 0x46004000
#define WHAL_PIC32CZ_SERCOM4_UART_DRIVER &whal_Pic32cz_Uart_Driver

#define WHAL_PIC32CZ_SUPC_PLL       \
    .enableMask = (1UL << 18),      \
    .enablePos = 18

#endif /* WHAL_PIC32CZ_H */
