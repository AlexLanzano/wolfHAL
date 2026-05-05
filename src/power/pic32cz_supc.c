#include <wolfHAL/bitops.h>
#include <wolfHAL/power/pic32cz_supc.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>

#define VREGCTRL_REG          0x1C
#define VREGCTRL_AVREGEN_Pos  16
#define VREGCTRL_AVREGEN_Msk  (WHAL_BITMASK(3) << VREGCTRL_AVREGEN_Pos)

whal_Error whal_Pic32cz_Supc_EnableSupply(whal_Power *powerDev,
                                         const whal_Pic32cz_Supc_Supply *supply)
{
    if (!powerDev || !supply)
        return WHAL_EINVAL;

    whal_Reg_Update(powerDev->base, VREGCTRL_REG, supply->enableMask,
                    whal_SetBits(supply->enableMask, supply->enablePos, 1));
    return WHAL_SUCCESS;
}

whal_Error whal_Pic32cz_Supc_DisableSupply(whal_Power *powerDev,
                                          const whal_Pic32cz_Supc_Supply *supply)
{
    if (!powerDev || !supply)
        return WHAL_EINVAL;

    whal_Reg_Update(powerDev->base, VREGCTRL_REG, supply->enableMask,
                    whal_SetBits(supply->enableMask, supply->enablePos, 0));
    return WHAL_SUCCESS;
}
