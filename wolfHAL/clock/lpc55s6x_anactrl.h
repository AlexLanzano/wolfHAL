/* lpc55s6x_anactrl.h
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

#ifndef WHAL_LPC55S6X_ANACTRL_H
#define WHAL_LPC55S6X_ANACTRL_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>

/*
 * @file lpc55s6x_anactrl.h
 * @brief LPC55S6x ANACTRL board-level driver.
 *
 * Hosts the on-chip high-speed FRO (FRO_192M). Its 96 MHz output (fro_hf) is a
 * clock source that needs no PLL. Board-level driver: no device struct, no
 * vtable; the peripheral lives at WHAL_LPC55S6X_ANACTRL_BASE and every helper
 * is a static inline.
 */

#define WHAL_LPC55S6X_ANACTRL_BASE 0x40013000

#define WHAL_LPC55S6X_ANACTRL_FRO192M_CTRL               0x10
#define WHAL_LPC55S6X_ANACTRL_FRO192M_CTRL_ENA_96MHZ_Msk (1UL << 30)

/*
 * @brief Enable the FRO 96 MHz output (fro_hf).
 *
 * The FRO_192M oscillator is already running (its 12 MHz output is the reset
 * clock), so only the extra 96 MHz tap is enabled here. Read-modify-write keeps
 * the other FRO192M_CTRL bits — 12 MHz enable, factory trim, and reserved bit
 * 15 (clearing which stops the flash) — intact.
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Lpc55s6x_Anactrl_EnableFroHf(void)
{
    whal_Reg_Update(WHAL_LPC55S6X_ANACTRL_BASE,
                    WHAL_LPC55S6X_ANACTRL_FRO192M_CTRL,
                    WHAL_LPC55S6X_ANACTRL_FRO192M_CTRL_ENA_96MHZ_Msk,
                    WHAL_LPC55S6X_ANACTRL_FRO192M_CTRL_ENA_96MHZ_Msk);
    return WHAL_SUCCESS;
}

#endif /* WHAL_LPC55S6X_ANACTRL_H */
