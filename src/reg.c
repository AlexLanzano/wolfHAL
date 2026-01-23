#include <wolfHAL/regmap.h>
#include <wolfHAL/error.h>
#include <wolfHAL/bitops.h>
#include <stddef.h>

whal_Error whal_Reg_Update(const whal_Regmap *regmap, size_t offset, size_t mask, size_t value)
{
    volatile size_t *reg;
    if (!regmap || offset > regmap->size) {
        return WHAL_EINVAL;
    }

    reg = (size_t *)(regmap->base + offset);
    *reg = (*reg & ~mask) | (value & mask);

    return WHAL_SUCCESS;
}

whal_Error whal_Reg_Get(const whal_Regmap *regmap, size_t offset, size_t mask, size_t *value)
{
    size_t val;
    if (!regmap || offset > regmap->size || !value) {
        return WHAL_EINVAL;
    }

    val = *(volatile size_t *)(regmap->base + offset);
    *value = whal_GetBits(mask, val);

    return WHAL_SUCCESS;
}
