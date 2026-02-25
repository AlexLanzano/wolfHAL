#include <wolfHAL/regmap.h>
#include <wolfHAL/error.h>
#include <wolfHAL/bitops.h>
#include <stddef.h>

void whal_Reg_Update(const size_t base, const size_t offset, const size_t mask, const size_t value)
{
    volatile size_t *reg;

    reg = (size_t *)(base + offset);
    *reg = (*reg & ~mask) | (value & mask);
}

void whal_Reg_Get(const size_t base, const size_t offset, const size_t msk, const size_t pos, size_t *value)
{
    size_t val;

    val = *(volatile size_t *)(base + offset);
    *value = whal_GetBits(msk, pos, val);
}

