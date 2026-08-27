/* board.c
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

/* Example board configuration for the LPC55S6x EVK dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/nxp/lpc55s6x.h>
#include "peripheral.h"

/* SysTick timing (must precede g_whalTimeout below) */
volatile uint32_t g_tick = 0;
volatile uint8_t g_waiting = 0;
volatile uint8_t g_tickOverflow = 0;

uint32_t Board_GetTick(void)
{
    return g_tick;
}

whal_Timeout g_whalTimeout = {
    .timeoutTicks = 1000,
    .GetTick = Board_GetTick,
};

/* Separate instance for the SD card driver's multi-second poll loops (ACMD41
 * power-up, write/erase programming); kept distinct from g_whalTimeout because
 * each whal_Sdhc_Request restarts the transport's own timeout. */
whal_Timeout g_whalTimeoutSdio = {
    .timeoutTicks = 2000,
    .GetTick = Board_GetTick,
};

void SysTick_Handler(void)
{
    uint32_t tickBefore = g_tick++;
    if (g_waiting) {
        if (tickBefore > g_tick)
            g_tickOverflow = 1;
    }
}

void Board_WaitMs(size_t ms)
{
    uint32_t startCount = g_tick;
    g_waiting = 1;
    while (1) {
        uint32_t currentCount = g_tick;
        if (g_tickOverflow) {
            if ((UINT32_MAX - startCount) + currentCount > ms) {
                break;
            }
        } else if (currentCount - startCount > ms) {
            break;
        }
    }

    g_waiting = 0;
    g_tickOverflow = 0;
}

static const whal_Lpc55s6x_Syscon_PeriphClk g_periphClks[] = {
    { WHAL_LPC55S6X_IOCON_CLK },
    { WHAL_LPC55S6X_GPIO0_CLK },
    { WHAL_LPC55S6X_GPIO1_CLK },
    { WHAL_LPC55S6X_FC0_CLK },
    { WHAL_LPC55S6X_HSSPI_CLK },
    { WHAL_LPC55S6X_CTIMER2_CLK },
    { WHAL_LPC55S6X_SDHC_CLK },
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Clock tree bring-up. TODO: fill in real PLL/main-clock parameters. */
    err = whal_Lpc55s6x_Syscon_EnablePll();
    if (err)
        return err;

    err = whal_Lpc55s6x_Syscon_SetMainClock();
    if (err)
        return err;

    err = whal_Lpc55s6x_Anactrl_EnableFroHf();
    if (err)
        return err;

    for (size_t i = 0; i < PERIPH_CLK_COUNT; ++i) {
        err = whal_Lpc55s6x_Syscon_EnablePeriphClk(&g_periphClks[i]);
        if (err)
            return err;
    }

    err = whal_Lpc55s6x_Syscon_SetFlexcommClk(
            0, WHAL_LPC55S6X_SYSCON_FCCLKSEL_FROHF, 0);
    if (err)
        return err;

    err = whal_Lpc55s6x_Syscon_SetHsSpiClk(WHAL_LPC55S6X_SYSCON_FCCLKSEL_FROHF);
    if (err)
        return err;

    err = whal_Lpc55s6x_Syscon_SetCtimerClk(
            2, WHAL_LPC55S6X_SYSCON_CTIMERCLKSEL_FRO96M);
    if (err)
        return err;

    /* Divide FRO96M by 2: the SDIO function clock (cclk_in) is rated <= 50 MHz. */
    err = whal_Lpc55s6x_Syscon_SetSdioClk(
            WHAL_LPC55S6X_SYSCON_SDIOCLKSEL_FRO96M, 1);
    if (err)
        return err;

    err = whal_Gpio_Init(BOARD_GPIO_DEV);
    if (err)
        return err;

    err = whal_Uart_Init(BOARD_UART_DEV);
    if (err)
        return err;

    err = whal_Spi_Init(BOARD_SPI_DEV);
    if (err)
        return err;

    err = whal_Pwm_Init(BOARD_PWM_DEV);
    if (err)
        return err;

    err = whal_Sdhc_Init(BOARD_SDHC_DEV);
    if (err)
        return err;

    err = whal_Timer_Init(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_Timer_Start(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = Peripheral_Init();
    if (err)
        return err;

    return WHAL_SUCCESS;
}

whal_Error Board_Deinit(void)
{
    whal_Error err;

    err = Peripheral_Deinit();
    if (err)
        return err;

    err = whal_Timer_Stop(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_Timer_Deinit(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_Sdhc_Deinit(BOARD_SDHC_DEV);
    if (err)
        return err;

    err = whal_Pwm_Deinit(BOARD_PWM_DEV);
    if (err)
        return err;

    err = whal_Spi_Deinit(BOARD_SPI_DEV);
    if (err)
        return err;

    err = whal_Uart_Deinit(BOARD_UART_DEV);
    if (err)
        return err;

    err = whal_Gpio_Deinit(BOARD_GPIO_DEV);
    if (err)
        return err;

    for (size_t i = 0; i < PERIPH_CLK_COUNT; ++i) {
        err = whal_Lpc55s6x_Syscon_DisablePeriphClk(&g_periphClks[i]);
        if (err)
            return err;
    }

    return WHAL_SUCCESS;
}
