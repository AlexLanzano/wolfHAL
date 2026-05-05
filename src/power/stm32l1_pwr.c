#include <wolfHAL/power/stm32l1_pwr.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define PWR_CR_REG       0x00
#define PWR_CR_VOS_Pos   11
#define PWR_CR_VOS_Msk   (WHAL_BITMASK(2) << PWR_CR_VOS_Pos)
#define PWR_CSR_REG      0x04
#define PWR_CSR_VOSF_Pos 4
#define PWR_CSR_VOSF_Msk (1UL << PWR_CSR_VOSF_Pos)

whal_Error whal_Stm32l1_Pwr_SetVosRange(whal_Power *powerDev,
                                       whal_Stm32l1_Pwr_VosRange range,
                                       whal_Timeout *timeout)
{
    size_t base;
    whal_Error err;

    if (!powerDev)
        return WHAL_EINVAL;

    base = powerDev->base;

    err = whal_Reg_ReadPoll(base, PWR_CSR_REG, PWR_CSR_VOSF_Msk, 0, timeout);
    if (err)
        return err;

    whal_Reg_Update(base, PWR_CR_REG, PWR_CR_VOS_Msk,
                    whal_SetBits(PWR_CR_VOS_Msk, PWR_CR_VOS_Pos, range));

    return whal_Reg_ReadPoll(base, PWR_CSR_REG, PWR_CSR_VOSF_Msk, 0, timeout);
}
