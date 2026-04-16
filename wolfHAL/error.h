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
    /* Operation not implemented by the selected driver. */
    WHAL_ENOTIMPL = -4004,
};

#endif /* WHAL_ERROR_H */
