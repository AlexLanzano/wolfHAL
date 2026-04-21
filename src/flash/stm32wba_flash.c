#include <stdint.h>
#include <wolfHAL/flash/stm32wba_flash.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/*
 * STM32WBA Flash Register Definitions (RM0493)
 *
 * Flash interface base: 0x40022000
 */

/* Access Control Register */
#define FLASH_ACR_REG       0x000
#define FLASH_ACR_LATENCY_Msk 0x0FUL

/* Non-Secure Key Register - unlock sequence */
#define FLASH_NSKEYR_REG    0x008

/* Non-Secure Status Register (RM0493 section 7.9.6, page 217) */
#define FLASH_NSSR_REG       0x020
#define FLASH_NSSR_EOP_Pos   0
#define FLASH_NSSR_EOP_Msk   (1UL << FLASH_NSSR_EOP_Pos)
#define FLASH_NSSR_OPERR_Pos 1
#define FLASH_NSSR_OPERR_Msk (1UL << FLASH_NSSR_OPERR_Pos)
#define FLASH_NSSR_PROGERR_Pos 3
#define FLASH_NSSR_PROGERR_Msk (1UL << FLASH_NSSR_PROGERR_Pos)
#define FLASH_NSSR_WRPERR_Pos  4
#define FLASH_NSSR_WRPERR_Msk  (1UL << FLASH_NSSR_WRPERR_Pos)
#define FLASH_NSSR_PGAERR_Pos  5
#define FLASH_NSSR_PGAERR_Msk  (1UL << FLASH_NSSR_PGAERR_Pos)
#define FLASH_NSSR_SIZERR_Pos  6
#define FLASH_NSSR_SIZERR_Msk  (1UL << FLASH_NSSR_SIZERR_Pos)
#define FLASH_NSSR_PGSERR_Pos  7
#define FLASH_NSSR_PGSERR_Msk  (1UL << FLASH_NSSR_PGSERR_Pos)
#define FLASH_NSSR_OPTWERR_Pos 13
#define FLASH_NSSR_OPTWERR_Msk (1UL << FLASH_NSSR_OPTWERR_Pos)
#define FLASH_NSSR_BSY_Pos   16
#define FLASH_NSSR_BSY_Msk   (1UL << FLASH_NSSR_BSY_Pos)
#define FLASH_NSSR_WDW_Pos   17
#define FLASH_NSSR_WDW_Msk   (1UL << FLASH_NSSR_WDW_Pos)

#define FLASH_NSSR_ALL_ERR (FLASH_NSSR_OPERR_Msk | FLASH_NSSR_PROGERR_Msk | \
                            FLASH_NSSR_WRPERR_Msk | FLASH_NSSR_PGAERR_Msk | \
                            FLASH_NSSR_SIZERR_Msk | FLASH_NSSR_PGSERR_Msk | \
                            FLASH_NSSR_OPTWERR_Msk)

/* Non-Secure Control Register 1 (RM0493 section 7.9.8, page 220) */
#define FLASH_NSCR1_REG      0x028
#define FLASH_NSCR1_PG_Pos   0
#define FLASH_NSCR1_PG_Msk   (1UL << FLASH_NSCR1_PG_Pos)
#define FLASH_NSCR1_PER_Pos  1
#define FLASH_NSCR1_PER_Msk  (1UL << FLASH_NSCR1_PER_Pos)
#define FLASH_NSCR1_MER_Pos  2
#define FLASH_NSCR1_MER_Msk  (1UL << FLASH_NSCR1_MER_Pos)
#define FLASH_NSCR1_PNB_Pos  3
#define FLASH_NSCR1_PNB_Msk  (0x7FUL << FLASH_NSCR1_PNB_Pos)
#define FLASH_NSCR1_BWR_Pos  14
#define FLASH_NSCR1_BWR_Msk  (1UL << FLASH_NSCR1_BWR_Pos)
#define FLASH_NSCR1_STRT_Pos 16
#define FLASH_NSCR1_STRT_Msk (1UL << FLASH_NSCR1_STRT_Pos)
#define FLASH_NSCR1_LOCK_Pos 31
#define FLASH_NSCR1_LOCK_Msk (1UL << FLASH_NSCR1_LOCK_Pos)

/* Unlock keys */
#define FLASH_KEY1 0x45670123UL
#define FLASH_KEY2 0xCDEF89ABUL

/* Sector size: 8 KB */
#define FLASH_SECTOR_SIZE  0x2000
#define FLASH_SECTOR_SHIFT 13

whal_Error whal_Stm32wbaFlash_Init(whal_Flash *flashDev)
{
    (void)flashDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaFlash_Deinit(whal_Flash *flashDev)
{
    (void)flashDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaFlash_Lock(whal_Flash *flashDev, size_t addr, size_t len)
{
    (void)addr;
    (void)len;

    if (!flashDev)
        return WHAL_EINVAL;

    whal_Reg_Update(flashDev->regmap.base, FLASH_NSCR1_REG, FLASH_NSCR1_LOCK_Msk,
                    whal_SetBits(FLASH_NSCR1_LOCK_Msk, FLASH_NSCR1_LOCK_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaFlash_Unlock(whal_Flash *flashDev, size_t addr, size_t len)
{
    (void)addr;
    (void)len;

    if (!flashDev)
        return WHAL_EINVAL;

    /* Only write the unlock sequence if LOCK bit is set */
    if (whal_Reg_Read(flashDev->regmap.base, FLASH_NSCR1_REG) & FLASH_NSCR1_LOCK_Msk) {
        whal_Reg_Write(flashDev->regmap.base, FLASH_NSKEYR_REG, FLASH_KEY1);
        whal_Reg_Write(flashDev->regmap.base, FLASH_NSKEYR_REG, FLASH_KEY2);
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaFlash_Read(whal_Flash *flashDev, size_t addr, void *data, size_t dataSz)
{
    uint8_t *dataBuf = (uint8_t *)data;
    whal_Stm32wbaFlash_Cfg *cfg;

    if (!flashDev || !flashDev->cfg || !data)
        return WHAL_EINVAL;

    cfg = flashDev->cfg;

    if (addr < cfg->startAddr || addr + dataSz > cfg->startAddr + cfg->size)
        return WHAL_EINVAL;

    uint8_t *flashAddr = (uint8_t *)addr;
    for (size_t i = 0; i < dataSz; ++i)
        dataBuf[i] = flashAddr[i];

    return WHAL_SUCCESS;
}

static whal_Error WaitNotBusy(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, FLASH_NSSR_REG, FLASH_NSSR_BSY_Msk, 0, timeout);
}

static whal_Error CheckAndClearErrors(size_t base)
{
    size_t sr = whal_Reg_Read(base, FLASH_NSSR_REG);
    if (sr & FLASH_NSSR_ALL_ERR) {
        /* Clear error flags by writing 1 to them */
        whal_Reg_Write(base, FLASH_NSSR_REG, sr & FLASH_NSSR_ALL_ERR);
        return WHAL_EHARDWARE;
    }
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaFlash_Write(whal_Flash *flashDev, size_t addr,
                                     const void *data, size_t dataSz)
{
    const uint8_t *dataBuf = (const uint8_t *)data;
    whal_Stm32wbaFlash_Cfg *cfg;
    const whal_Regmap *regmap;
    whal_Error err;

    if (!flashDev || !flashDev->cfg || !data)
        return WHAL_EINVAL;

    cfg = flashDev->cfg;
    regmap = &flashDev->regmap;

    /* Address and size must be 16-byte aligned (128-bit flash-word) */
    if ((addr & 0xF) || (dataSz & 0xF))
        return WHAL_EINVAL;

    if (addr < cfg->startAddr || addr + dataSz > cfg->startAddr + cfg->size)
        return WHAL_EINVAL;

    err = WaitNotBusy(regmap->base, cfg->timeout);
    if (err)
        return err;

    /* Clear error flags */
    whal_Reg_Write(regmap->base, FLASH_NSSR_REG,
                   whal_Reg_Read(regmap->base, FLASH_NSSR_REG) & FLASH_NSSR_ALL_ERR);

    /* Enable programming */
    whal_Reg_Update(regmap->base, FLASH_NSCR1_REG, FLASH_NSCR1_PG_Msk,
                    whal_SetBits(FLASH_NSCR1_PG_Msk, FLASH_NSCR1_PG_Pos, 1));

    /* Program in 128-bit (16 byte) flash-word chunks */
    for (size_t i = 0; i < dataSz; i += 16) {
        uint32_t *flashAddr = (uint32_t *)(addr + i);
        const uint32_t *dataAddr = (const uint32_t *)(dataBuf + i);

        flashAddr[0] = dataAddr[0];
        flashAddr[1] = dataAddr[1];
        flashAddr[2] = dataAddr[2];
        flashAddr[3] = dataAddr[3];

        err = WaitNotBusy(regmap->base, cfg->timeout);
        if (err)
            goto cleanup;

        err = CheckAndClearErrors(regmap->base);
        if (err)
            goto cleanup;
    }

cleanup:
    whal_Reg_Update(regmap->base, FLASH_NSCR1_REG, FLASH_NSCR1_PG_Msk, 0);
    return err;
}

whal_Error whal_Stm32wbaFlash_Erase(whal_Flash *flashDev, size_t addr, size_t dataSz)
{
    whal_Stm32wbaFlash_Cfg *cfg;
    const whal_Regmap *regmap;
    whal_Error err;
    size_t offset, startPage, endPage;

    if (!flashDev || !flashDev->cfg)
        return WHAL_EINVAL;

    cfg = flashDev->cfg;
    regmap = &flashDev->regmap;

    if (dataSz == 0)
        return WHAL_SUCCESS;

    if (addr < cfg->startAddr || addr + dataSz > cfg->startAddr + cfg->size)
        return WHAL_EINVAL;

    err = WaitNotBusy(regmap->base, cfg->timeout);
    if (err)
        return err;

    /* Clear error flags */
    whal_Reg_Write(regmap->base, FLASH_NSSR_REG,
                   whal_Reg_Read(regmap->base, FLASH_NSSR_REG) & FLASH_NSSR_ALL_ERR);

    offset = addr - cfg->startAddr;
    startPage = offset >> FLASH_SECTOR_SHIFT;
    endPage = (offset + dataSz - 1) >> FLASH_SECTOR_SHIFT;

    for (size_t page = startPage; page <= endPage; page++) {
        /* Configure page erase: PER=1, PNB=page, STRT=1 */
        whal_Reg_Update(regmap->base, FLASH_NSCR1_REG,
                        FLASH_NSCR1_PER_Msk | FLASH_NSCR1_PNB_Msk | FLASH_NSCR1_STRT_Msk,
                        whal_SetBits(FLASH_NSCR1_PER_Msk, FLASH_NSCR1_PER_Pos, 1) |
                        whal_SetBits(FLASH_NSCR1_PNB_Msk, FLASH_NSCR1_PNB_Pos, page) |
                        whal_SetBits(FLASH_NSCR1_STRT_Msk, FLASH_NSCR1_STRT_Pos, 1));

        err = WaitNotBusy(regmap->base, cfg->timeout);
        if (err)
            goto cleanup;

        err = CheckAndClearErrors(regmap->base);
        if (err)
            goto cleanup;
    }

cleanup:
    whal_Reg_Update(regmap->base, FLASH_NSCR1_REG, FLASH_NSCR1_PER_Msk, 0);
    return err;
}

whal_Error whal_Stm32wbaFlash_Ext_SetLatency(whal_Flash *flashDev, uint8_t latency)
{
    if (!flashDev)
        return WHAL_EINVAL;

    whal_Reg_Update(flashDev->regmap.base, FLASH_ACR_REG, FLASH_ACR_LATENCY_Msk, latency);
    return WHAL_SUCCESS;
}

const whal_FlashDriver whal_Stm32wbaFlash_Driver = {
    .Init = whal_Stm32wbaFlash_Init,
    .Deinit = whal_Stm32wbaFlash_Deinit,
    .Lock = whal_Stm32wbaFlash_Lock,
    .Unlock = whal_Stm32wbaFlash_Unlock,
    .Read = whal_Stm32wbaFlash_Read,
    .Write = whal_Stm32wbaFlash_Write,
    .Erase = whal_Stm32wbaFlash_Erase,
};
