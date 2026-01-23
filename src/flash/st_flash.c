#include <stdint.h>
#include <wolfHAL/flash/st_flash.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>
#include <wolfHAL/bitops.h>

#define ST_FLASH_ACR_REG 0x00
#define ST_FLASH_ACR_LATENCY WHAL_MASK_RANGE(2, 0)

whal_Error whal_StFlash_Init(whal_Flash *flashDev)
{
    (void)flashDev;
    return WHAL_SUCCESS;
}

whal_Error whal_StFlash_Deinit(whal_Flash *flashDev)
{
    (void)flashDev;
    return WHAL_SUCCESS;
}

whal_Error whal_StFlash_Write(whal_Flash *flashDev, size_t addr, uint8_t *data,
                                size_t dataSz)
{
    (void)flashDev;
    (void)addr;
    (void)data;
    (void)dataSz;
    return WHAL_SUCCESS;
}

whal_Error whal_StFlash_Erase(whal_Flash *flashDev, size_t addr,
                                size_t dataSz)
{
    (void)flashDev;
    (void)addr;
    (void)dataSz;
    return WHAL_SUCCESS;
}

static whal_Error whal_StFlash_Cmd_SetLatency(whal_Flash *flashDev, whal_StFlash_SetLatencyArgs *args)
{
    if (!args) {
        return WHAL_EINVAL;
    }

    const whal_Regmap *reg = &flashDev->regmap;
    return whal_Reg_Update(reg, ST_FLASH_ACR_REG, ST_FLASH_ACR_LATENCY, args->latency);
}

whal_Error whal_StFlash_Cmd(whal_Flash *flashDev, size_t cmd, void *args)
{
    switch (cmd) {
    case WHAL_ST_FLASH_CMD_SET_LATENCY:
        return whal_StFlash_Cmd_SetLatency(flashDev, args);

    default:
        return WHAL_EINVAL;
    }
    
    return WHAL_SUCCESS;
}

whal_FlashDriver whal_StFlash_Driver = {
    .Init = whal_StFlash_Init,
    .Deinit = whal_StFlash_Deinit,
    .Write = whal_StFlash_Write,
    .Erase = whal_StFlash_Erase,
    .Cmd = whal_StFlash_Cmd,
};
