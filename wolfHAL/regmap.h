#ifndef WHAL_REGMAP_H
#define WHAL_REGMAP_H

#include <stddef.h>
#include <wolfHAL/error.h>

/*
 * @file regmap.h
 * @brief Helpers for accessing and manipulating memory-mapped registers.
 */

/*
 * @brief Description of a contiguous register map.
 */
typedef struct whal_Regmap {
    size_t base;
    size_t size;
} whal_Regmap;

/*
 * @brief Update a masked field within a memory-mapped register.
 *
 * The function applies a read-modify-write using @p mask to only touch the
 * desired bit field.
 *
 * @param base   Base address of the register block.
 * @param offset Byte offset from @p base to the register.
 * @param mask   Bit mask selecting the field to update.
 * @param value  Value to write, which will be masked and shifted.
 *
 * @note No return value. Callers are responsible for passing valid inputs.
 */
void whal_Reg_Update(size_t base, size_t offset, size_t mask, size_t value);
/*
 * @brief Read a masked field from a memory-mapped register.
 *
 * @param base   Base address of the register block.
 * @param offset Byte offset from @p base to the register.
 * @param mask   Bit mask selecting the field to extract.
 * @param value  Output storage for the decoded field.
 *
 * @note No return value. Callers are responsible for passing valid inputs.
 */
void whal_Reg_Get(size_t base, size_t offset, size_t mask, size_t *value);

#endif /* WHAL_REGMAP_H */
