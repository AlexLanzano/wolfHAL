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

/* Flexcomm function-clock source select (FCCLKSELn, +4 per instance). */
#define WHAL_LPC55S6X_SYSCON_FCCLKSEL0        0x2B0
#define WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Pos 0
#define WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk (WHAL_BITMASK(3) << WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Pos)

/* FCCLKSEL SEL values (shared by HSLSPICLKSEL). */
#define WHAL_LPC55S6X_SYSCON_FCCLKSEL_MAINCLK 0
#define WHAL_LPC55S6X_SYSCON_FCCLKSEL_FRO12M  2
#define WHAL_LPC55S6X_SYSCON_FCCLKSEL_FROHF   3

/* High-speed SPI (Flexcomm 8) function-clock source select (no FRG). */
#define WHAL_LPC55S6X_SYSCON_HSLSPICLKSEL     0x2D0

/* CTimer function-clock source select (CTIMERCLKSELn, +4 per instance). */
#define WHAL_LPC55S6X_SYSCON_CTIMERCLKSEL0       0x26C
#define WHAL_LPC55S6X_SYSCON_CTIMERCLKSEL_FRO96M 3

/* SDIO function-clock source select and divider. */
#define WHAL_LPC55S6X_SYSCON_SDIOCLKSEL             0x2F8
#define WHAL_LPC55S6X_SYSCON_SDIOCLKSEL_FRO96M      3
#define WHAL_LPC55S6X_SYSCON_SDIOCLKDIV             0x3BC
#define WHAL_LPC55S6X_SYSCON_SDIOCLKDIV_DIV_Msk     (WHAL_BITMASK(8) << 0)
#define WHAL_LPC55S6X_SYSCON_SDIOCLKDIV_RESET_Msk   (1UL << 29)
#define WHAL_LPC55S6X_SYSCON_SDIOCLKDIV_REQFLAG_Msk (1UL << 31)

/* Flexcomm fractional rate generator (FLEXFRGnCTRL, +4 per instance). */
#define WHAL_LPC55S6X_SYSCON_FLEXFRGCTRL0     0x320
#define WHAL_LPC55S6X_SYSCON_FLEXFRG_DIV_Pos  0
#define WHAL_LPC55S6X_SYSCON_FLEXFRG_DIV_Msk  (WHAL_BITMASK(8) << WHAL_LPC55S6X_SYSCON_FLEXFRG_DIV_Pos)
#define WHAL_LPC55S6X_SYSCON_FLEXFRG_MULT_Pos 8
#define WHAL_LPC55S6X_SYSCON_FLEXFRG_MULT_Msk (WHAL_BITMASK(8) << WHAL_LPC55S6X_SYSCON_FLEXFRG_MULT_Pos)

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
 * @brief Select a Flexcomm instance's function clock (FCCLKSEL + FRG).
 *
 * @param instance Flexcomm interface index.
 * @param sel      Clock source (WHAL_LPC55S6X_SYSCON_FCCLKSEL_*).
 * @param mult     FRG numerator (0 = passthrough); denominator fixed at 256.
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Syscon_SetFlexcommClk(
    size_t instance, size_t sel, size_t mult)
{
    whal_Reg_Update(WHAL_LPC55S6X_SYSCON_BASE,
                    WHAL_LPC55S6X_SYSCON_FCCLKSEL0 + instance * 4,
                    WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk,
                                 WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Pos, sel));
    whal_Reg_Update(WHAL_LPC55S6X_SYSCON_BASE,
                    WHAL_LPC55S6X_SYSCON_FLEXFRGCTRL0 + instance * 4,
                    WHAL_LPC55S6X_SYSCON_FLEXFRG_DIV_Msk |
                    WHAL_LPC55S6X_SYSCON_FLEXFRG_MULT_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SYSCON_FLEXFRG_DIV_Msk,
                                 WHAL_LPC55S6X_SYSCON_FLEXFRG_DIV_Pos, 0xFF) |
                    whal_SetBits(WHAL_LPC55S6X_SYSCON_FLEXFRG_MULT_Msk,
                                 WHAL_LPC55S6X_SYSCON_FLEXFRG_MULT_Pos, mult));
    return WHAL_SUCCESS;
}

/*
 * @brief Select a CTimer instance's function clock source.
 *
 * @param instance CTimer index (0-4).
 * @param sel      Clock source (WHAL_LPC55S6X_SYSCON_CTIMERCLKSEL_*).
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Syscon_SetCtimerClk(size_t instance,
                                                           size_t sel)
{
    whal_Reg_Update(WHAL_LPC55S6X_SYSCON_BASE,
                    WHAL_LPC55S6X_SYSCON_CTIMERCLKSEL0 + instance * 4,
                    WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk,
                                 WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Pos, sel));
    return WHAL_SUCCESS;
}

/*
 * @brief Select the high-speed SPI (Flexcomm 8) function clock source.
 *
 * HS_SPI has no fractional rate generator; the source drives it directly.
 *
 * @param sel Clock source (WHAL_LPC55S6X_SYSCON_FCCLKSEL_*).
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Syscon_SetHsSpiClk(size_t sel)
{
    whal_Reg_Update(WHAL_LPC55S6X_SYSCON_BASE,
                    WHAL_LPC55S6X_SYSCON_HSLSPICLKSEL,
                    WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk,
                                 WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Pos, sel));
    return WHAL_SUCCESS;
}

/*
 * @brief Select the SDIO function-clock source and divider.
 *
 * The SDIO clock divider comes up halted, so this clears HALT while writing the
 * divider value, pulses the counter reset, and waits for the output to stabilize
 * (REQFLAG). Clock bring-up runs before the system timer, so the bare wait loop
 * is acceptable here.
 *
 * @param sel Clock source (WHAL_LPC55S6X_SYSCON_SDIOCLKSEL_*).
 * @param div Divider value (0 = divide by 1).
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Syscon_SetSdioClk(size_t sel, size_t div)
{
    whal_Reg_Update(WHAL_LPC55S6X_SYSCON_BASE,
                    WHAL_LPC55S6X_SYSCON_SDIOCLKSEL,
                    WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Msk,
                                 WHAL_LPC55S6X_SYSCON_FCCLKSEL_SEL_Pos, sel));
    whal_Reg_Write(WHAL_LPC55S6X_SYSCON_BASE,
                   WHAL_LPC55S6X_SYSCON_SDIOCLKDIV,
                   WHAL_LPC55S6X_SYSCON_SDIOCLKDIV_RESET_Msk |
                   (div & WHAL_LPC55S6X_SYSCON_SDIOCLKDIV_DIV_Msk));
    whal_Reg_Write(WHAL_LPC55S6X_SYSCON_BASE,
                   WHAL_LPC55S6X_SYSCON_SDIOCLKDIV,
                   div & WHAL_LPC55S6X_SYSCON_SDIOCLKDIV_DIV_Msk);
    while (whal_Reg_Read(WHAL_LPC55S6X_SYSCON_BASE,
                         WHAL_LPC55S6X_SYSCON_SDIOCLKDIV)
           & WHAL_LPC55S6X_SYSCON_SDIOCLKDIV_REQFLAG_Msk)
        ;
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
