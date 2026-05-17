/* error.h
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

#ifndef WHAL_ERROR_H
#define WHAL_ERROR_H


/*
 * @file error.h
 * @brief Shared error codes for wolfHAL APIs.
 */

/* Status code type used by wolfHAL.  Zero is success, non-zero is error. */
typedef int whal_Error;

enum {
    /* Operation completed successfully. */
    WHAL_SUCCESS = 0,
    /* Invalid argument (null pointer, bad configuration). */
    WHAL_EINVAL = 1,
    /* Resource not ready or busy. */
    WHAL_ENOTREADY = 2,
    /* Hardware device error. */
    WHAL_EHARDWARE = 3,
    /* Operation timed out. */
    WHAL_ETIMEOUT = 4,
    /* Operation or argument not supported by the selected driver/hardware
     * (use for requests that are valid in general but this implementation
     * cannot fulfill — e.g., hardware lacks the feature or the specific
     * parameter combination isn't supported). For universally invalid
     * arguments (null pointer, out-of-range enum) return WHAL_EINVAL. */
    WHAL_ENOTSUP = 5,
};

#endif /* WHAL_ERROR_H */
