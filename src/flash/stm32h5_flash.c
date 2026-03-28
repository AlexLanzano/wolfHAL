#include <stdint.h>
#include <wolfHAL/flash/stm32h5_flash.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/*
 * STM32H5 Flash Register Definitions
 *
 * The STM32H5 flash controller manages dual-bank embedded flash with 8 KB
 * sectors and 128-bit (flash-word) programming. Non-secure register variants
 * (NS prefix) are used throughout.
 */

/* Access Control Register */
#define FLASH_ACR_REG 0x000

/* Non-Secure Key Register - unlock sequence */
#define FLASH_NSKEYR_REG 0x004

/* Non-Secure Status Register */
#define FLASH_NSSR_REG 0x020
#define FLASH_NSSR_BSY_Pos 0
#define FLASH_NSSR_BSY_Msk (1UL << FLASH_NSSR_BSY_Pos)

#define FLASH_NSSR_WBNE_Pos 1
#define FLASH_NSSR_WBNE_Msk (1UL << FLASH_NSSR_WBNE_Pos)

#define FLASH_NSSR_EOP_Pos 16
#define FLASH_NSSR_EOP_Msk (1UL << FLASH_NSSR_EOP_Pos)

#define FLASH_NSSR_WRPERR_Pos 17
#define FLASH_NSSR_WRPERR_Msk (1UL << FLASH_NSSR_WRPERR_Pos)

#define FLASH_NSSR_PGSERR_Pos 18
#define FLASH_NSSR_PGSERR_Msk (1UL << FLASH_NSSR_PGSERR_Pos)

#define FLASH_NSSR_STRBERR_Pos 19
#define FLASH_NSSR_STRBERR_Msk (1UL << FLASH_NSSR_STRBERR_Pos)

#define FLASH_NSSR_INCERR_Pos 20
#define FLASH_NSSR_INCERR_Msk (1UL << FLASH_NSSR_INCERR_Pos)

#define FLASH_NSSR_ALL_ERR (FLASH_NSSR_WRPERR_Msk | FLASH_NSSR_PGSERR_Msk | \
                            FLASH_NSSR_STRBERR_Msk | FLASH_NSSR_INCERR_Msk)

/* Non-Secure Control Register */
#define FLASH_NSCR_REG 0x028
#define FLASH_NSCR_LOCK_Pos 0
#define FLASH_NSCR_LOCK_Msk (1UL << FLASH_NSCR_LOCK_Pos)

#define FLASH_NSCR_PG_Pos 1
#define FLASH_NSCR_PG_Msk (1UL << FLASH_NSCR_PG_Pos)

#define FLASH_NSCR_SER_Pos 2
#define FLASH_NSCR_SER_Msk (1UL << FLASH_NSCR_SER_Pos)

#define FLASH_NSCR_BER_Pos 3
#define FLASH_NSCR_BER_Msk (1UL << FLASH_NSCR_BER_Pos)

#define FLASH_NSCR_FW_Pos 4
#define FLASH_NSCR_FW_Msk (1UL << FLASH_NSCR_FW_Pos)

#define FLASH_NSCR_STRT_Pos 5
#define FLASH_NSCR_STRT_Msk (1UL << FLASH_NSCR_STRT_Pos)

#define FLASH_NSCR_SNB_Pos 6
#define FLASH_NSCR_SNB_Msk (WHAL_BITMASK(7) << FLASH_NSCR_SNB_Pos)

#define FLASH_NSCR_BKSEL_Pos 31
#define FLASH_NSCR_BKSEL_Msk (1UL << FLASH_NSCR_BKSEL_Pos)

/* Non-Secure Clear Control Register - write 1 to clear status flags */
#define FLASH_NSCCR_REG 0x030
#define FLASH_NSCCR_CLR_EOP_Pos 16
#define FLASH_NSCCR_CLR_EOP_Msk (1UL << FLASH_NSCCR_CLR_EOP_Pos)

#define FLASH_NSCCR_CLR_WRPERR_Pos 17
#define FLASH_NSCCR_CLR_WRPERR_Msk (1UL << FLASH_NSCCR_CLR_WRPERR_Pos)

#define FLASH_NSCCR_CLR_PGSERR_Pos 18
#define FLASH_NSCCR_CLR_PGSERR_Msk (1UL << FLASH_NSCCR_CLR_PGSERR_Pos)

#define FLASH_NSCCR_CLR_STRBERR_Pos 19
#define FLASH_NSCCR_CLR_STRBERR_Msk (1UL << FLASH_NSCCR_CLR_STRBERR_Pos)

#define FLASH_NSCCR_CLR_INCERR_Pos 20
#define FLASH_NSCCR_CLR_INCERR_Msk (1UL << FLASH_NSCCR_CLR_INCERR_Pos)

#define FLASH_NSCCR_CLR_ALL (FLASH_NSCCR_CLR_EOP_Msk | \
                             FLASH_NSCCR_CLR_WRPERR_Msk | \
                             FLASH_NSCCR_CLR_PGSERR_Msk | \
                             FLASH_NSCCR_CLR_STRBERR_Msk | \
                             FLASH_NSCCR_CLR_INCERR_Msk)

/* Unlock keys (same as STM32WB) */
#define FLASH_KEY1 0x45670123UL
#define FLASH_KEY2 0xCDEF89ABUL

/* Sector size: 8 KB */
#define FLASH_SECTOR_SIZE 0x2000
#define FLASH_SECTOR_SHIFT 13

/* Sectors per bank */
#define FLASH_SECTORS_PER_BANK 128

/* Bank size: 1 MB */
#define FLASH_BANK_SIZE (FLASH_SECTORS_PER_BANK * FLASH_SECTOR_SIZE)

whal_Error whal_Stm32h5Flash_Init(whal_Flash *flashDev)
{
    (void)flashDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Flash_Deinit(whal_Flash *flashDev)
{
    (void)flashDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Flash_Lock(whal_Flash *flashDev, size_t addr,
                                   size_t len)
{
    const whal_Regmap *regmap;

    (void)addr;
    (void)len;

    if (!flashDev)
        return WHAL_EINVAL;

    regmap = &flashDev->regmap;

    whal_Reg_Update(regmap->base, FLASH_NSCR_REG, FLASH_NSCR_LOCK_Msk,
                    whal_SetBits(FLASH_NSCR_LOCK_Msk, FLASH_NSCR_LOCK_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Flash_Unlock(whal_Flash *flashDev, size_t addr,
                                     size_t len)
{
    const whal_Regmap *regmap;

    (void)addr;
    (void)len;

    if (!flashDev)
        return WHAL_EINVAL;

    regmap = &flashDev->regmap;

    whal_Reg_Write(regmap->base, FLASH_NSKEYR_REG, FLASH_KEY1);
    whal_Reg_Write(regmap->base, FLASH_NSKEYR_REG, FLASH_KEY2);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Flash_Read(whal_Flash *flashDev, size_t addr,
                                   uint8_t *data, size_t dataSz)
{
    whal_Stm32h5Flash_Cfg *cfg;

    if (!flashDev || !flashDev->cfg || !data)
        return WHAL_EINVAL;

    cfg = flashDev->cfg;

    if (addr < cfg->startAddr || addr + dataSz > cfg->startAddr + cfg->size)
        return WHAL_EINVAL;

    uint8_t *flashAddr = (uint8_t *)addr;
    for (size_t i = 0; i < dataSz; ++i)
        data[i] = flashAddr[i];

    return WHAL_SUCCESS;
}

static whal_Error WaitNotBusy(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, FLASH_NSSR_REG, FLASH_NSSR_BSY_Msk,
                             0, timeout);
}

static whal_Error CheckErrors(size_t base)
{
    size_t sr = whal_Reg_Read(base, FLASH_NSSR_REG);
    if (sr & FLASH_NSSR_ALL_ERR) {
        /* Clear all error flags */
        whal_Reg_Write(base, FLASH_NSCCR_REG, FLASH_NSCCR_CLR_ALL);
        return WHAL_EHARDWARE;
    }
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Flash_Write(whal_Flash *flashDev, size_t addr,
                                    const uint8_t *data, size_t dataSz)
{
    whal_Stm32h5Flash_Cfg *cfg;
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

    /* Wait for any ongoing operation */
    err = WaitNotBusy(regmap->base, cfg->timeout);
    if (err)
        return err;

    /* Clear all error and status flags */
    whal_Reg_Write(regmap->base, FLASH_NSCCR_REG, FLASH_NSCCR_CLR_ALL);

    /* Enable programming */
    whal_Reg_Update(regmap->base, FLASH_NSCR_REG, FLASH_NSCR_PG_Msk,
                    whal_SetBits(FLASH_NSCR_PG_Msk, FLASH_NSCR_PG_Pos, 1));

    /* Program data in 128-bit (16 byte) flash-word chunks */
    for (size_t i = 0; i < dataSz; i += 16) {
        uint32_t *flashAddr = (uint32_t *)(addr + i);
        const uint32_t *dataAddr = (const uint32_t *)(data + i);

        flashAddr[0] = dataAddr[0];
        flashAddr[1] = dataAddr[1];
        flashAddr[2] = dataAddr[2];
        flashAddr[3] = dataAddr[3];

        err = WaitNotBusy(regmap->base, cfg->timeout);
        if (err)
            goto cleanup;

        err = CheckErrors(regmap->base);
        if (err)
            goto cleanup;
    }

cleanup:
    /* Disable programming */
    whal_Reg_Update(regmap->base, FLASH_NSCR_REG, FLASH_NSCR_PG_Msk, 0);

    return err;
}

whal_Error whal_Stm32h5Flash_Erase(whal_Flash *flashDev, size_t addr,
                                    size_t dataSz)
{
    whal_Stm32h5Flash_Cfg *cfg;
    const whal_Regmap *regmap;
    whal_Error err;
    size_t offset;
    size_t startSector, endSector;

    if (!flashDev || !flashDev->cfg)
        return WHAL_EINVAL;

    cfg = flashDev->cfg;
    regmap = &flashDev->regmap;

    if (dataSz == 0)
        return WHAL_SUCCESS;

    if (addr < cfg->startAddr || addr + dataSz > cfg->startAddr + cfg->size)
        return WHAL_EINVAL;

    /* Wait for any ongoing operation */
    err = WaitNotBusy(regmap->base, cfg->timeout);
    if (err)
        return err;

    /* Clear all error and status flags */
    whal_Reg_Write(regmap->base, FLASH_NSCCR_REG, FLASH_NSCCR_CLR_ALL);

    offset = addr - cfg->startAddr;
    startSector = offset >> FLASH_SECTOR_SHIFT;
    endSector = (offset + dataSz - 1) >> FLASH_SECTOR_SHIFT;

    for (size_t sector = startSector; sector <= endSector; sector++) {
        size_t bank = (sector >= FLASH_SECTORS_PER_BANK) ? 1 : 0;
        size_t sectorInBank = sector - (bank * FLASH_SECTORS_PER_BANK);

        /* Configure: sector erase, sector number, bank select */
        whal_Reg_Update(regmap->base, FLASH_NSCR_REG,
                        FLASH_NSCR_SER_Msk | FLASH_NSCR_SNB_Msk |
                        FLASH_NSCR_BKSEL_Msk,
                        whal_SetBits(FLASH_NSCR_SER_Msk, FLASH_NSCR_SER_Pos, 1) |
                        whal_SetBits(FLASH_NSCR_SNB_Msk, FLASH_NSCR_SNB_Pos,
                                     sectorInBank) |
                        whal_SetBits(FLASH_NSCR_BKSEL_Msk, FLASH_NSCR_BKSEL_Pos,
                                     bank));

        /* Start erase */
        whal_Reg_Update(regmap->base, FLASH_NSCR_REG, FLASH_NSCR_STRT_Msk,
                        whal_SetBits(FLASH_NSCR_STRT_Msk, FLASH_NSCR_STRT_Pos, 1));

        err = WaitNotBusy(regmap->base, cfg->timeout);
        if (err)
            goto cleanup;

        err = CheckErrors(regmap->base);
        if (err)
            goto cleanup;
    }

cleanup:
    /* Clear sector erase mode */
    whal_Reg_Update(regmap->base, FLASH_NSCR_REG, FLASH_NSCR_SER_Msk, 0);

    return err;
}

const whal_FlashDriver whal_Stm32h5Flash_Driver = {
    .Init = whal_Stm32h5Flash_Init,
    .Deinit = whal_Stm32h5Flash_Deinit,
    .Lock = whal_Stm32h5Flash_Lock,
    .Unlock = whal_Stm32h5Flash_Unlock,
    .Read = whal_Stm32h5Flash_Read,
    .Write = whal_Stm32h5Flash_Write,
    .Erase = whal_Stm32h5Flash_Erase,
};
