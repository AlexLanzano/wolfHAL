/* lpc55s6x_syscon.h
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

#ifndef WHAL_LPC55S6X_SYSCON_H
#define WHAL_LPC55S6X_SYSCON_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/*
 * @file lpc55s6x_syscon.h
 * @brief LPC55S6x SYSCON board-level clock driver.
 *
 * Boards drive the clock tree imperatively from Board_Init by calling the
 * helpers below in order. SYSCON is a board-level driver: there is no
 * whal_Clock device struct, no generic API, no vtable, and no direct-API
 * mapping. The peripheral lives at a fixed address (WHAL_LPC55S6X_SYSCON_BASE)
 * and every helper is a `static inline` taking no device pointer.
 */

#define WHAL_LPC55S6X_SYSCON_BASE 0x40000000

/* AHBCLKCTRLn peripheral clock gate registers (one enable bit per peripheral). */
#define WHAL_LPC55S6X_SYSCON_AHBCLKCTRL0 0x200
#define WHAL_LPC55S6X_SYSCON_AHBCLKCTRL1 0x204
#define WHAL_LPC55S6X_SYSCON_AHBCLKCTRL2 0x208

/*
 * TODO: add the remaining register offsets/masks from the TRM (MAINCLKSELA/B
 * main clock mux, PLL0/PLL1 control/status, flash access-cycle config).
 */

/*
 * @brief Peripheral clock gate descriptor (AHBCLKCTRLn register + enable bit).
 */
typedef struct whal_Lpc55s6x_Syscon_PeriphClk {
    size_t regOffset;   /* AHBCLKCTRLn offset from SYSCON base */
    size_t enableMask;  /* peripheral clock-enable bit mask */
} whal_Lpc55s6x_Syscon_PeriphClk;

/*
 * @brief Enable and lock a PLL.
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Syscon_EnablePll(void)
{
    /* TODO: implement — configure and enable the PLL via WHAL_LPC55S6X_SYSCON_BASE
     * registers, then poll the lock bit. Clock bring-up runs before the system
     * timer, so a bare do/while on the lock bit is acceptable here. */
    return WHAL_SUCCESS;
}

/*
 * @brief Select the main clock source and CPU divider.
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Syscon_SetMainClock(void)
{
    /* TODO: implement — program MAINCLKSELA/B and the AHB clock divider. */
    return WHAL_SUCCESS;
}

/*
 * @brief Enable a peripheral clock gate.
 *
 * @param clk Peripheral clock descriptor (AHBCLKCTRLn register + enable bit).
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Syscon_EnablePeriphClk(
    const whal_Lpc55s6x_Syscon_PeriphClk *clk)
{
    whal_Reg_Update(WHAL_LPC55S6X_SYSCON_BASE, clk->regOffset,
                    clk->enableMask, clk->enableMask);
    return WHAL_SUCCESS;
}

/*
 * @brief Disable a peripheral clock gate.
 *
 * @param clk Peripheral clock descriptor (AHBCLKCTRLn register + enable bit).
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Syscon_DisablePeriphClk(
    const whal_Lpc55s6x_Syscon_PeriphClk *clk)
{
    whal_Reg_Update(WHAL_LPC55S6X_SYSCON_BASE, clk->regOffset,
                    clk->enableMask, 0);
    return WHAL_SUCCESS;
}

#endif /* WHAL_LPC55S6X_SYSCON_H */
