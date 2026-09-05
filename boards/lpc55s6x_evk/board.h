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
extern whal_Timeout g_whalTimeoutSdio;
extern volatile uint32_t g_tick;

enum {
    LED_PIN,
    UART_TX_PIN,
    UART_RX_PIN,
    SPI_SCK_PIN,
    SPI_MOSI_PIN,
    SPI_MISO_PIN,
    SPI_CS_PIN,
    PWM_PIN,
    SD_CLK_PIN,
    SD_CMD_PIN,
    SD_D0_PIN,
    PIN_COUNT,
};

#define BOARD_LED_PIN 0

/* BOARD_*_DEV: how this board reaches each peripheral. WHAL_INTERNAL_DEV for
 * single-instance drivers (driver ignores the pointer). */
#define BOARD_GPIO_DEV  WHAL_INTERNAL_DEV
#define BOARD_UART_DEV  WHAL_INTERNAL_DEV
#define BOARD_SPI_DEV   WHAL_INTERNAL_DEV
#define BOARD_PWM_DEV   WHAL_INTERNAL_DEV
#define BOARD_SDHC_DEV  WHAL_INTERNAL_DEV

/* GPIO dev initializer — single-instance device defined in lpc55s6x_gpio.c. */
#define WHAL_CFG_LPC55S6X_GPIO_DEV { \
    .base = WHAL_LPC55S6X_GPIO_BASE, \
    .cfg  = (void *)&(const whal_Lpc55s6x_Gpio_Cfg){ \
        .pinCfgCount = PIN_COUNT, \
        .pinCfg = (whal_Lpc55s6x_Gpio_PinCfg[PIN_COUNT]){ \
            [LED_PIN] = { \
                .port = 1, \
                .pin  = 4, \
                .dir  = WHAL_LPC55S6X_GPIO_DIR_OUTPUT, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(0, \
                    WHAL_LPC55S6X_IOCON_MODE_INACTIVE, 0, 0, 1, 0), \
            }, \
            [UART_TX_PIN] = { \
                .port = 0, \
                .pin  = 30, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(1, \
                    WHAL_LPC55S6X_IOCON_MODE_INACTIVE, 0, 0, 1, 0), \
            }, \
            [UART_RX_PIN] = { \
                .port = 0, \
                .pin  = 29, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(1, \
                    WHAL_LPC55S6X_IOCON_MODE_PULLUP, 0, 0, 1, 0), \
            }, \
            [SPI_SCK_PIN] = { \
                .port = 1, \
                .pin  = 2, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(6, \
                    WHAL_LPC55S6X_IOCON_MODE_INACTIVE, 0, 0, 1, 0), \
            }, \
            [SPI_MOSI_PIN] = { \
                .port = 0, \
                .pin  = 26, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(9, \
                    WHAL_LPC55S6X_IOCON_MODE_INACTIVE, 0, 0, 1, 0), \
            }, \
            [SPI_MISO_PIN] = { \
                .port = 1, \
                .pin  = 3, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(6, \
                    WHAL_LPC55S6X_IOCON_MODE_INACTIVE, 0, 0, 1, 0), \
            }, \
            [SPI_CS_PIN] = { \
                .port = 1, \
                .pin  = 1, \
                .dir  = WHAL_LPC55S6X_GPIO_DIR_OUTPUT, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(0, \
                    WHAL_LPC55S6X_IOCON_MODE_INACTIVE, 0, 0, 1, 0), \
            }, \
            [PWM_PIN] = { \
                .port = 1, \
                .pin  = 5, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(3, \
                    WHAL_LPC55S6X_IOCON_MODE_INACTIVE, 0, 0, 1, 0), \
            }, \
            [SD_CLK_PIN] = { \
                .port = 0, \
                .pin  = 7, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(2, \
                    WHAL_LPC55S6X_IOCON_MODE_INACTIVE, 1, 0, 1, 0), \
            }, \
            [SD_CMD_PIN] = { \
                .port = 0, \
                .pin  = 8, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(2, \
                    WHAL_LPC55S6X_IOCON_MODE_PULLUP, 1, 0, 1, 0), \
            }, \
            [SD_D0_PIN] = { \
                .port = 0, \
                .pin  = 24, \
                .ioconCfg = WHAL_LPC55S6X_IOCON_TYPE_D(2, \
                    WHAL_LPC55S6X_IOCON_MODE_PULLUP, 1, 0, 1, 0), \
            }, \
        }, \
    }, \
}

/* UART dev initializer — single-instance device defined in lpc55s6x_uart.c. */
#define WHAL_CFG_LPC55S6X_UART_DEV { \
    .base = WHAL_LPC55S6X_UART_BASE, \
    /* .driver: direct API mapping */ \
    .cfg  = (void *)&(const whal_Lpc55s6x_Uart_Cfg){ \
        .fclkHz  = 96000000, \
        .baud    = 115200, \
        .cfgReg  = WHAL_LPC55S6X_USART_CFG(1, WHAL_LPC55S6X_USART_DATALEN_8BIT, \
                       WHAL_LPC55S6X_USART_PARITY_NONE, WHAL_LPC55S6X_USART_STOP_1), \
        .ctlReg  = WHAL_LPC55S6X_USART_CTL(0, 0, 0, 0, 0, 0), \
        .timeout = &g_whalTimeout, \
    }, \
}

/* SPI dev initializer — single-instance device defined in lpc55s6x_spi.c.
 * cfg is non-const: StartCom writes _wordSz into it. */
#define WHAL_CFG_LPC55S6X_SPI_DEV { \
    .base = WHAL_LPC55S6X_SPI_BASE, \
    /* .driver: direct API mapping */ \
    .cfg  = (void *)&(whal_Lpc55s6x_Spi_Cfg){ \
        .fclkHz  = 96000000, \
        .cfgReg  = WHAL_LPC55S6X_SPI_CFG(1, 0, 0, 0, 0, 0, 0), \
        .timeout = &g_whalTimeout, \
    }, \
}

/* CTIMER2 PWM dev initializer — single-instance device defined in
 * lpc55s6x_ctimer_pwm.c. */
#define WHAL_CFG_LPC55S6X_CTIMER_PWM_DEV { \
    .base = WHAL_LPC55S6X_CTIMER2_BASE, \
    /* .driver: direct API mapping */ \
    .cfg  = (void *)&(const whal_Lpc55s6x_Ctimer_Pwm_Cfg){ \
        .prescaler = 0, \
        .clkSel    = WHAL_LPC55S6X_SYSCON_CTIMERCLKSEL_FRO96M, \
        .timeout   = &g_whalTimeout, \
    }, \
}

/* SDHC transport dev — single-instance device defined in lpc55s6x_sdhc.c. */
#define WHAL_CFG_LPC55S6X_SDHC_DEV { \
    .base = WHAL_LPC55S6X_SDHC_BASE, \
    /* .driver: direct API mapping */ \
    .cfg  = (void *)&(const whal_Lpc55s6x_Sdhc_Cfg){ \
        .fclkHz  = 48000000, \
        .cardNum = 0, \
        .timeout = &g_whalTimeout, \
    }, \
}

/* SysTick dev initializer — single-instance device defined in systick.c. */
#define WHAL_CFG_SYSTICK_DEV { \
    .base = WHAL_CORTEX_M33_SYSTICK_BASE, \
    /* .driver: direct API mapping */ \
    .cfg  = (void *)&(const whal_SysTick_Cfg){ \
        .cyclesPerTick = 48000000 / 1000, \
        .clkSrc  = WHAL_SYSTICK_CLKSRC_SYSCLK, \
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED, \
    }, \
}

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);
uint32_t Board_GetTick(void);

#endif /* BOARD_H */
