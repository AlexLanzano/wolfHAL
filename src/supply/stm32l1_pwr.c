#include <wolfHAL/supply/stm32l1_pwr.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define PWR_CR_REG       0x00
#define PWR_CR_VOS_Pos   11
#define PWR_CR_VOS_Msk   (WHAL_BITMASK(2) << PWR_CR_VOS_Pos)
#define PWR_CSR_REG      0x04
#define PWR_CSR_VOSF_Pos 4
#define PWR_CSR_VOSF_Msk (1UL << PWR_CSR_VOSF_Pos)

#ifdef WHAL_CFG_SUPPLY_API_MAPPING_STM32L1
#define whal_Stm32l1Pwr_Init    whal_Supply_Init
#define whal_Stm32l1Pwr_Deinit  whal_Supply_Deinit
#define whal_Stm32l1Pwr_Enable  whal_Supply_Enable
#define whal_Stm32l1Pwr_Disable whal_Supply_Disable
#endif /* WHAL_CFG_SUPPLY_API_MAPPING_STM32L1 */

whal_Error whal_Stm32l1Pwr_Init(whal_Supply *supplyDev)
{
    whal_Stm32l1Pwr_Cfg *cfg;
    size_t base;
    whal_Error err;

    if (!supplyDev || !supplyDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32l1Pwr_Cfg *)supplyDev->cfg;
    base = supplyDev->regmap.base;

    err = whal_Reg_ReadPoll(base, PWR_CSR_REG, PWR_CSR_VOSF_Msk, 0,
                            cfg->timeout);
    if (err)
        return err;

    whal_Reg_Update(base, PWR_CR_REG, PWR_CR_VOS_Msk,
                    whal_SetBits(PWR_CR_VOS_Msk, PWR_CR_VOS_Pos,
                                 cfg->vosRange));

    return whal_Reg_ReadPoll(base, PWR_CSR_REG, PWR_CSR_VOSF_Msk, 0,
                             cfg->timeout);
}

whal_Error whal_Stm32l1Pwr_Deinit(whal_Supply *supplyDev)
{
    (void)supplyDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1Pwr_Enable(whal_Supply *supplyDev, void *supply)
{
    (void)supplyDev;
    (void)supply;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32l1Pwr_Disable(whal_Supply *supplyDev, void *supply)
{
    (void)supplyDev;
    (void)supply;
    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_SUPPLY_API_MAPPING_STM32L1
const whal_SupplyDriver whal_Stm32l1Pwr_Driver = {
    .Init = whal_Stm32l1Pwr_Init,
    .Deinit = whal_Stm32l1Pwr_Deinit,
    .Enable = whal_Stm32l1Pwr_Enable,
    .Disable = whal_Stm32l1Pwr_Disable,
};
#endif /* !WHAL_CFG_SUPPLY_API_MAPPING_STM32L1 */
