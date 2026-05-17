/* timeout.h
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

#ifndef WHAL_TIMEOUT_H
#define WHAL_TIMEOUT_H

#include <wolfHAL/error.h>
#include <stdint.h>
#include <stddef.h>

/*
 * @file timeout.h
 * @brief Timeout abstraction for bounded polling and delays.
 *
 * The board sets timeoutTicks and a GetTick callback directly on a
 * whal_Timeout instance. Drivers use WHAL_TIMEOUT_START / WHAL_TIMEOUT_EXPIRED
 * macros for zero-overhead polling guards. When WHAL_CFG_NO_TIMEOUT is
 * defined, all timeout operations compile away completely.
 */

typedef struct {
    uint32_t timeoutTicks;
    uint32_t startTick;
    uint32_t (*GetTick)(void);
} whal_Timeout;

#ifdef WHAL_CFG_NO_TIMEOUT

#define WHAL_TIMEOUT_START(t)       ((void)(0))
#define WHAL_TIMEOUT_EXPIRED(t)     (0)

#else /* !WHAL_CFG_NO_TIMEOUT */

/*
 * @brief Snapshot the current tick.
 *
 * If @p t is NULL, this is a no-op so drivers can leave the timeout
 * pointer unset for unbounded polling.
 */
#define WHAL_TIMEOUT_START(t) do {                                          \
    if (t) {                                                                \
        (t)->startTick = (t)->GetTick();                                    \
    }                                                                       \
} while (0)

/*
 * @brief Evaluate to nonzero if the timeout has expired.
 *
 * Safe to call with a NULL pointer — returns 0 (not expired).
 */
#define WHAL_TIMEOUT_EXPIRED(t) \
    ((t) && ((uint32_t)((t)->GetTick() - (t)->startTick) >= (t)->timeoutTicks))

#endif /* WHAL_CFG_NO_TIMEOUT */

#endif /* WHAL_TIMEOUT_H */
