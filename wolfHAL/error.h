#ifndef WHAL_ERROR_H
#define WHAL_ERROR_H


/*
 * @file error.h
 * @brief Shared error codes for wolfHAL APIs.
 */

/* Signed 16-bit status code type used by wolfHAL. */
typedef int whal_Error;

enum {
    /* Operation completed successfully. */
    WHAL_SUCCESS = 0,
    /* Invalid argument (null pointer, bad configuration). */
    WHAL_EINVAL = -4000,
    /* Resource not ready or busy. */
    WHAL_ENOTREADY = -4001,
    /* Hardware device error. */
    WHAL_EHARDWARE = -4002,
    /* Operation timed out. */
    WHAL_ETIMEOUT = -4003,
    /* Operation or argument not supported by the selected driver/hardware
     * (use for requests that are valid in general but this implementation
     * cannot fulfill — e.g., hardware lacks the feature or the specific
     * parameter combination isn't supported). For universally invalid
     * arguments (null pointer, out-of-range enum) return WHAL_EINVAL. */
    WHAL_ENOTSUP = -4004,
};

#endif /* WHAL_ERROR_H */
