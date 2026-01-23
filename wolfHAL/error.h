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
    /* Invalid argument or unsupported operation. */
    WHAL_EINVAL = -4000,
};

#endif /* WHAL_ERROR_H */
