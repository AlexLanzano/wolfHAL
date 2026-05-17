/* bitops.h
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

#ifndef WHAL_BITOPS_H
#define WHAL_BITOPS_H

/*
 * @file bitops.h
 * @brief Bit manipulation helpers for register fields.
 */

#include <stdint.h>

/*
 * @brief Create a bitmask of @p width bits starting at bit 0.
 *
 * Example: WHAL_BITMASK(4) = 0xF
 */
#define WHAL_BITMASK(width) \
    ((1UL << (width)) - 1)

/*
 * @brief Encode a value into a bit field described by @p msk and @p pos.
 */
#define whal_SetBits(msk, pos, val) \
    (((val) << (pos)) & (msk))

/*
 * @brief Extract a bit field value from a register using @p msk and @p pos.
 */
#define whal_GetBits(msk, pos, reg) \
    (((reg) & (msk)) >> (pos))

#endif /* WHAL_BITOPS_H */
