#include <stdint.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/flash/stm32wb_flash.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>
#include <wolfHAL/bitops.h>

#define ST_FLASH_ACR_REG 0x00
#define ST_FLASH_ACR_LATENCY WHAL_MASK_RANGE(2, 0)

#define ST_FLASH_KEYR_REG 0x08
#define ST_FLASH_KEYR_KEY WHAL_MASK_RANGE(31, 0)

#define ST_FLASH_SR_REG 0x10
#define ST_FLASH_SR_EOP WHAL_MASK(0)
#define ST_FLASH_SR_OP_ERR WHAL_MASK(1)
#define ST_FLASH_SR_PROG_ERR WHAL_MASK(3)
#define ST_FLASH_SR_WRP_ERR WHAL_MASK(4)
#define ST_FLASH_SR_PGA_ERR WHAL_MASK(5)
#define ST_FLASH_SR_SIZ_ERR WHAL_MASK(6)
#define ST_FLASH_SR_PGS_ERR WHAL_MASK(7)
#define ST_FLASH_SR_MISS_ERR WHAL_MASK(8)
#define ST_FLASH_SR_FAST_ERR WHAL_MASK(9)
#define ST_FLASH_SR_RD_ERR WHAL_MASK(14)
#define ST_FLASH_SR_OPTV_ERR WHAL_MASK(15)
#define ST_FLASH_SR_BSY WHAL_MASK(16)
#define ST_FLASH_SR_CFGBSY WHAL_MASK(18)
#define ST_FLASH_SR_PESD WHAL_MASK(19)

#define ST_FLASH_SR_ALL_ERR (ST_FLASH_SR_OP_ERR | ST_FLASH_SR_PROG_ERR | ST_FLASH_SR_WRP_ERR | \
                             ST_FLASH_SR_PGA_ERR | ST_FLASH_SR_SIZ_ERR | ST_FLASH_SR_PGS_ERR | \
                             ST_FLASH_SR_MISS_ERR | ST_FLASH_SR_FAST_ERR | ST_FLASH_SR_RD_ERR | \
                             ST_FLASH_SR_OPTV_ERR)

#define ST_FLASH_CR_REG 0x14
#define ST_FLASH_CR_PG WHAL_MASK(0)
#define ST_FLASH_CR_PER WHAL_MASK(1)
#define ST_FLASH_CR_PNB WHAL_MASK_RANGE(10, 3)
#define ST_FLASH_CR_STRT WHAL_MASK(16)
#define ST_FLASH_CR_LOCK WHAL_MASK(31)

whal_Error whal_Stm32wbFlash_Init(whal_Flash *flashDev)
{
    whal_Error err;
    whal_Stm32wbFlash_Cfg *cfg = flashDev->cfg;

    err = whal_Clock_Enable(cfg->clkCtrl, cfg->clk); 
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbFlash_Deinit(whal_Flash *flashDev)
{
    whal_Error err;
    whal_Stm32wbFlash_Cfg *cfg = flashDev->cfg;

    err = whal_Clock_Disable(cfg->clkCtrl, cfg->clk); 
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbFlash_Lock(whal_Flash *flashDev, size_t addr, size_t len)
{
    (void)addr;
    (void)len;

    const whal_Regmap *regmap = &flashDev->regmap;
    whal_Reg_Update(regmap->base, ST_FLASH_CR_REG, ST_FLASH_CR_LOCK, 
                    whal_SetBits(ST_FLASH_CR_LOCK, 1));
    
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbFlash_Unlock(whal_Flash *flashDev, size_t addr, size_t len)
{
    (void)addr;
    (void)len;

    const whal_Regmap *regmap = &flashDev->regmap;

    whal_Reg_Update(regmap->base, ST_FLASH_KEYR_REG, ST_FLASH_KEYR_KEY, 0x45670123);
    whal_Reg_Update(regmap->base, ST_FLASH_KEYR_REG, ST_FLASH_KEYR_KEY, 0xCDEF89AB);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbFlash_Read(whal_Flash *flashDev, size_t addr, uint8_t *data,
                             size_t dataSz)
{
    uint8_t *flashAddr = (uint8_t *)addr;
    for (size_t i = 0; i < dataSz; ++i) {
        data[i] = flashAddr[i];
    }
    return WHAL_SUCCESS;
}

static whal_Error whal_Stm32wbFlash_WriteOrErase(whal_Flash *flashDev, size_t addr, const uint8_t *data,
                                            size_t dataSz, uint8_t write)
{
    whal_Stm32wbFlash_Cfg *cfg = flashDev->cfg;
    const whal_Regmap *regmap = &flashDev->regmap;
    size_t bsy;
    size_t pesd;
    size_t cfgbsy = 0;

    /* Write address is not 8B aligned */
    if (addr & 0xf || addr < cfg->startAddr || addr + dataSz > cfg->startAddr + cfg->size) {
        return WHAL_EINVAL;
    }

    whal_Reg_Get(regmap->base, ST_FLASH_SR_REG, ST_FLASH_SR_BSY, &bsy);
    whal_Reg_Get(regmap->base, ST_FLASH_SR_REG, ST_FLASH_SR_PESD, &pesd);

    if (bsy || pesd) {
        return WHAL_ENOTREADY;
    }

    /* Clear all errors */
    whal_Reg_Update(regmap->base, ST_FLASH_SR_REG, ST_FLASH_SR_ALL_ERR, 0xffffffff);

    if (write) {
        /* Enable flash programming */
        whal_Reg_Update(regmap->base, ST_FLASH_CR_REG, ST_FLASH_CR_PG, 1);

        for (size_t i = 0; i < dataSz; i += 8) {
            uint32_t *flashAddr = (uint32_t *)(addr + i);
            uint32_t *dataAddr = (uint32_t *)(data + i);
            flashAddr[0] = dataAddr[0];
            flashAddr[1] = dataAddr[1];

            do {
                whal_Reg_Get(regmap->base, ST_FLASH_SR_REG, ST_FLASH_SR_CFGBSY, &cfgbsy);
            } while (cfgbsy);
        }
    }
    else {
        size_t startPage, endPage;
        startPage = (addr - cfg->startAddr) >> 12; 
        endPage = ((addr - cfg->startAddr) + dataSz) >> 12;
        
        whal_Reg_Update(regmap->base, ST_FLASH_CR_REG, ST_FLASH_CR_PER,
                        whal_SetBits(ST_FLASH_CR_PER, 1));

        for (size_t page = startPage; page <= endPage; ++page) {
            whal_Reg_Update(regmap->base, ST_FLASH_CR_REG, ST_FLASH_CR_PNB,
                            whal_SetBits(ST_FLASH_CR_PNB, page));

            whal_Reg_Update(regmap->base, ST_FLASH_CR_REG, ST_FLASH_CR_STRT, 
                            whal_SetBits(ST_FLASH_CR_STRT, 1));

            do {
                whal_Reg_Get(regmap->base, ST_FLASH_SR_REG, ST_FLASH_SR_CFGBSY, &cfgbsy);
            } while (cfgbsy);
        }

        whal_Reg_Update(regmap->base, ST_FLASH_CR_REG, ST_FLASH_CR_PER,
                        whal_SetBits(ST_FLASH_CR_PER, 0));
    }

    /* Disable flash programming */
    whal_Reg_Update(regmap->base, ST_FLASH_CR_REG, ST_FLASH_CR_PG, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbFlash_Write(whal_Flash *flashDev, size_t addr, const uint8_t *data,
                                size_t dataSz)
{
    return whal_Stm32wbFlash_WriteOrErase(flashDev, addr, data, dataSz, 1);
}

whal_Error whal_Stm32wbFlash_Erase(whal_Flash *flashDev, size_t addr,
                                size_t dataSz)
{
    return whal_Stm32wbFlash_WriteOrErase(flashDev, addr, NULL, dataSz, 0);
}

whal_Error whal_Stm32wbFlash_Ext_SetLatency(whal_Flash *flashDev, enum whal_Stm32wbFlash_Latency latency)
{
    if (!flashDev) {
        return WHAL_EINVAL;
    }

    const whal_Regmap *reg = &flashDev->regmap;
    whal_Reg_Update(reg->base, ST_FLASH_ACR_REG, ST_FLASH_ACR_LATENCY, latency);
    return WHAL_SUCCESS;
}

whal_FlashDriver whal_Stm32wbFlash_Driver = {
    .Init = whal_Stm32wbFlash_Init,
    .Deinit = whal_Stm32wbFlash_Deinit,
    .Lock = whal_Stm32wbFlash_Lock,
    .Unlock = whal_Stm32wbFlash_Unlock,
    .Read = whal_Stm32wbFlash_Read,
    .Write = whal_Stm32wbFlash_Write,
    .Erase = whal_Stm32wbFlash_Erase,
};
