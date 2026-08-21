/* board.h
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

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/nxp/lpc55s6x.h>

extern whal_Timeout g_whalTimeout;
extern volatile uint32_t g_tick;

enum {
    LED_PIN,
    UART_TX_PIN,
    UART_RX_PIN,
    PIN_COUNT,
};

#define BOARD_LED_PIN 0

/* BOARD_*_DEV: how this board reaches each peripheral. WHAL_INTERNAL_DEV for
 * single-instance drivers (driver ignores the pointer). */
#define BOARD_GPIO_DEV WHAL_INTERNAL_DEV
#define BOARD_UART_DEV WHAL_INTERNAL_DEV

/* GPIO dev initializer — single-instance device defined in lpc55s6x_gpio.c. */
#define WHAL_CFG_LPC55S6X_GPIO_DEV { \
    .base = WHAL_LPC55S6X_GPIO_BASE, \
    .cfg  = (void *)&(const whal_Lpc55s6x_Gpio_Cfg){ \
        .pinCfgCount = PIN_COUNT, \
        .pinCfg = (whal_Lpc55s6x_Gpio_PinCfg[PIN_COUNT]){ \
            [LED_PIN] = { \
                .port = 1, \
                .pin  = 9, \
                .dir  = WHAL_LPC55S6X_GPIO_DIR_OUTPUT, \
            }, \
            [UART_TX_PIN] = { \
                .port = 0, \
                .pin  = 30, \
            }, \
            [UART_RX_PIN] = { \
                .port = 0, \
                .pin  = 29, \
            }, \
        }, \
    }, \
}

/* UART dev initializer — single-instance device defined in lpc55s6x_uart.c. */
#define WHAL_CFG_LPC55S6X_UART_DEV { \
    .base = WHAL_LPC55S6X_UART_BASE, \
    /* .driver: direct API mapping */ \
    .cfg  = (void *)&(const whal_Lpc55s6x_Uart_Cfg){ \
        .baud    = 115200, \
        .timeout = &g_whalTimeout, \
    }, \
}

/* SysTick dev initializer — single-instance device defined in systick.c. */
#define WHAL_CFG_SYSTICK_DEV { \
    .base = WHAL_CORTEX_M33_SYSTICK_BASE, \
    /* .driver: direct API mapping */ \
    .cfg  = (void *)&(const whal_SysTick_Cfg){ \
        .cyclesPerTick = 12000000 / 1000, \
        .clkSrc  = WHAL_SYSTICK_CLKSRC_SYSCLK, \
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED, \
    }, \
}

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
