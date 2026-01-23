#ifndef WHAL_BITOPS_H
#define WHAL_BITOPS_H

/*
 * @file bitops.h
 * @brief Bit manipulation helpers for register fields.
 */

#if defined(__GNUC__)
/* Count trailing zeros in a mask. */
#define whal_CountTrailingZeros(mask) __builtin_ctz(mask)
#else
static size_t whal_CountTrailingZeros(mask)
{
    size_t cnt = 0;
    while ((mask & 1ul) == 0ul) {
        mask >>= 1;
        n++;
    }

    return n;
}
#endif

/*
 * @brief Encode a value into a bit field described by @p mask.
 */
#define whal_SetBits(mask, value) \
    (((value) << whal_CountTrailingZeros(mask)) & (mask))

/*
 * @brief Extract a bit field value from a register using @p mask.
 */
#define whal_GetBits(mask, reg) \
    (((reg) & (mask)) >> whal_CountTrailingZeros(mask))

/* Create a single-bit mask for bit @p bit. */
#define WHAL_MASK(bit) \
    (1ul << bit)

/* Create a mask covering bits @p high_bit down to @p low_bit (inclusive). */
#define WHAL_MASK_RANGE(high_bit, low_bit) \
    ((((1ul << (((high_bit) - (low_bit)) + 1)) - 1ul)) << (low_bit))

#endif /* WHAL_BITOPS_H */
