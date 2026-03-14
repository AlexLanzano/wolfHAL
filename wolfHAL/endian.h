#ifndef WHAL_ENDIAN_H
#define WHAL_ENDIAN_H

/*
 * @file endian.h
 * @brief Byte-order conversion helpers.
 */

#include <stdint.h>

/*
 * @brief Load a 32-bit value from a big-endian byte array.
 */
static inline uint32_t whal_LoadBe32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  | p[3];
}

/*
 * @brief Store a 32-bit value into a big-endian byte array.
 */
static inline void whal_StoreBe32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

#endif /* WHAL_ENDIAN_H */
