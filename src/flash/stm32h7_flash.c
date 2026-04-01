#include <stdint.h>
#include <wolfHAL/flash/stm32h7_flash.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/*
 * STM32H7 Flash Register Definitions (RM0433)
 *
 * The STM32H7 flash controller manages dual-bank embedded flash with 128 KB
 * sectors and 256-bit (flash-word = 32 bytes) programming. Bank 1 and Bank 2
 * each have their own set of control/status registers at different offsets.
 *
 * Flash register base: 0x52002000
 * Bank 1 registers start at offset 0x000
 * Bank 2 registers start at offset 0x100
 */

/* Access Control Register (offset 0x000) */
#define FLASH_ACR_REG 0x000

/* Key Register - Bank 1 (offset 0x004) */
#define FLASH_KEYR1_REG 0x004

/* Key Register - Bank 2 (offset 0x104) */
#define FLASH_KEYR2_REG 0x104

/* Status Register - Bank 1 (offset 0x010) */
#define FLASH_SR1_REG 0x010

/* Status Register - Bank 2 (offset 0x110) */
#define FLASH_SR2_REG 0x110

#define FLASH_SR_BSY_Pos 0
#define FLASH_SR_BSY_Msk (1UL << FLASH_SR_BSY_Pos)

#define FLASH_SR_WBNE_Pos 1
#define FLASH_SR_WBNE_Msk (1UL << FLASH_SR_WBNE_Pos)

#define FLASH_SR_QW_Pos 2
#define FLASH_SR_QW_Msk (1UL << FLASH_SR_QW_Pos)

#define FLASH_SR_EOP_Pos 16
#define FLASH_SR_EOP_Msk (1UL << FLASH_SR_EOP_Pos)

#define FLASH_SR_WRPERR_Pos 17
#define FLASH_SR_WRPERR_Msk (1UL << FLASH_SR_WRPERR_Pos)

#define FLASH_SR_PGSERR_Pos 18
#define FLASH_SR_PGSERR_Msk (1UL << FLASH_SR_PGSERR_Pos)

#define FLASH_SR_STRBERR_Pos 19
#define FLASH_SR_STRBERR_Msk (1UL << FLASH_SR_STRBERR_Pos)

#define FLASH_SR_INCERR_Pos 21
#define FLASH_SR_INCERR_Msk (1UL << FLASH_SR_INCERR_Pos)

#define FLASH_SR_OPERR_Pos 22
#define FLASH_SR_OPERR_Msk (1UL << FLASH_SR_OPERR_Pos)

#define FLASH_SR_SNECCERR_Pos 25
#define FLASH_SR_SNECCERR_Msk (1UL << FLASH_SR_SNECCERR_Pos)

#define FLASH_SR_DBECCERR_Pos 26
#define FLASH_SR_DBECCERR_Msk (1UL << FLASH_SR_DBECCERR_Pos)

#define FLASH_SR_ALL_ERR (FLASH_SR_WRPERR_Msk | FLASH_SR_PGSERR_Msk | \
                          FLASH_SR_STRBERR_Msk | FLASH_SR_INCERR_Msk | \
                          FLASH_SR_OPERR_Msk | FLASH_SR_SNECCERR_Msk | \
                          FLASH_SR_DBECCERR_Msk)

/* Control Register - Bank 1 (offset 0x00C) */
#define FLASH_CR1_REG 0x00C

/* Control Register - Bank 2 (offset 0x10C) */
#define FLASH_CR2_REG 0x10C

#define FLASH_CR_LOCK_Pos 0
#define FLASH_CR_LOCK_Msk (1UL << FLASH_CR_LOCK_Pos)

#define FLASH_CR_PG_Pos 1
#define FLASH_CR_PG_Msk (1UL << FLASH_CR_PG_Pos)

#define FLASH_CR_SER_Pos 2
#define FLASH_CR_SER_Msk (1UL << FLASH_CR_SER_Pos)

#define FLASH_CR_BER_Pos 3
#define FLASH_CR_BER_Msk (1UL << FLASH_CR_BER_Pos)

#define FLASH_CR_PSIZE_Pos 4
#define FLASH_CR_PSIZE_Msk (WHAL_BITMASK(2) << FLASH_CR_PSIZE_Pos)

#define FLASH_CR_FW_Pos 6
#define FLASH_CR_FW_Msk (1UL << FLASH_CR_FW_Pos)

#define FLASH_CR_START_Pos 7
#define FLASH_CR_START_Msk (1UL << FLASH_CR_START_Pos)

#define FLASH_CR_SNB_Pos 8
#define FLASH_CR_SNB_Msk (WHAL_BITMASK(3) << FLASH_CR_SNB_Pos)

/* Clear Control Register - Bank 1 (offset 0x014) */
#define FLASH_CCR1_REG 0x014

/* Clear Control Register - Bank 2 (offset 0x114) */
#define FLASH_CCR2_REG 0x114

#define FLASH_CCR_CLR_EOP_Pos 16
#define FLASH_CCR_CLR_EOP_Msk (1UL << FLASH_CCR_CLR_EOP_Pos)

#define FLASH_CCR_CLR_WRPERR_Pos 17
#define FLASH_CCR_CLR_WRPERR_Msk (1UL << FLASH_CCR_CLR_WRPERR_Pos)

#define FLASH_CCR_CLR_PGSERR_Pos 18
#define FLASH_CCR_CLR_PGSERR_Msk (1UL << FLASH_CCR_CLR_PGSERR_Pos)

#define FLASH_CCR_CLR_STRBERR_Pos 19
#define FLASH_CCR_CLR_STRBERR_Msk (1UL << FLASH_CCR_CLR_STRBERR_Pos)

#define FLASH_CCR_CLR_INCERR_Pos 21
#define FLASH_CCR_CLR_INCERR_Msk (1UL << FLASH_CCR_CLR_INCERR_Pos)

#define FLASH_CCR_CLR_OPERR_Pos 22
#define FLASH_CCR_CLR_OPERR_Msk (1UL << FLASH_CCR_CLR_OPERR_Pos)

#define FLASH_CCR_CLR_SNECCERR_Pos 25
#define FLASH_CCR_CLR_SNECCERR_Msk (1UL << FLASH_CCR_CLR_SNECCERR_Pos)

#define FLASH_CCR_CLR_DBECCERR_Pos 26
#define FLASH_CCR_CLR_DBECCERR_Msk (1UL << FLASH_CCR_CLR_DBECCERR_Pos)

#define FLASH_CCR_CLR_ALL (FLASH_CCR_CLR_EOP_Msk | \
                           FLASH_CCR_CLR_WRPERR_Msk | \
                           FLASH_CCR_CLR_PGSERR_Msk | \
                           FLASH_CCR_CLR_STRBERR_Msk | \
                           FLASH_CCR_CLR_INCERR_Msk | \
                           FLASH_CCR_CLR_OPERR_Msk | \
                           FLASH_CCR_CLR_SNECCERR_Msk | \
                           FLASH_CCR_CLR_DBECCERR_Msk)

/* Unlock keys */
#define FLASH_KEY1 0x45670123UL
#define FLASH_KEY2 0xCDEF89ABUL

/* Sector size: 128 KB */
#define FLASH_SECTOR_SIZE 0x20000
#define FLASH_SECTOR_SHIFT 17

/* Sectors per bank */
#define FLASH_SECTORS_PER_BANK 8

/* Bank size: 1 MB */
#define FLASH_BANK_SIZE (FLASH_SECTORS_PER_BANK * FLASH_SECTOR_SIZE)

/* Flash-word size: 256 bits = 32 bytes */
#define FLASH_WORD_SIZE 32

/* Parallelism size: 3 = x64 (256-bit) programming */
#define FLASH_PSIZE_X64 3

/*
 * Get the register offsets for the appropriate bank.
 */
static inline void GetBankRegs(size_t bank, size_t *crOff, size_t *srOff,
                                size_t *ccrOff, size_t *keyrOff)
{
    if (bank == 0) {
        *crOff = FLASH_CR1_REG;
        *srOff = FLASH_SR1_REG;
        *ccrOff = FLASH_CCR1_REG;
        *keyrOff = FLASH_KEYR1_REG;
    }
    else {
        *crOff = FLASH_CR2_REG;
        *srOff = FLASH_SR2_REG;
        *ccrOff = FLASH_CCR2_REG;
        *keyrOff = FLASH_KEYR2_REG;
    }
}

static whal_Error WaitNotBusy(size_t base, size_t srOff, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, srOff,
                             FLASH_SR_BSY_Msk | FLASH_SR_QW_Msk,
                             0, timeout);
}

static whal_Error CheckErrors(size_t base, size_t srOff, size_t ccrOff)
{
    size_t sr = whal_Reg_Read(base, srOff);
    if (sr & FLASH_SR_ALL_ERR) {
        /* Clear all error flags */
        whal_Reg_Write(base, ccrOff, FLASH_CCR_CLR_ALL);
        return WHAL_EHARDWARE;
    }
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Flash_Init(whal_Flash *flashDev)
{
    (void)flashDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Flash_Deinit(whal_Flash *flashDev)
{
    (void)flashDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Flash_Lock(whal_Flash *flashDev, size_t addr,
                                   size_t len)
{
    const whal_Regmap *regmap;

    (void)addr;
    (void)len;

    if (!flashDev)
        return WHAL_EINVAL;

    regmap = &flashDev->regmap;

    /* Lock both banks */
    whal_Reg_Update(regmap->base, FLASH_CR1_REG, FLASH_CR_LOCK_Msk,
                    whal_SetBits(FLASH_CR_LOCK_Msk, FLASH_CR_LOCK_Pos, 1));
    whal_Reg_Update(regmap->base, FLASH_CR2_REG, FLASH_CR_LOCK_Msk,
                    whal_SetBits(FLASH_CR_LOCK_Msk, FLASH_CR_LOCK_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Flash_Unlock(whal_Flash *flashDev, size_t addr,
                                     size_t len)
{
    const whal_Regmap *regmap;

    (void)addr;
    (void)len;

    if (!flashDev)
        return WHAL_EINVAL;

    regmap = &flashDev->regmap;

    /* Unlock Bank 1 */
    whal_Reg_Write(regmap->base, FLASH_KEYR1_REG, FLASH_KEY1);
    whal_Reg_Write(regmap->base, FLASH_KEYR1_REG, FLASH_KEY2);

    /* Unlock Bank 2 */
    whal_Reg_Write(regmap->base, FLASH_KEYR2_REG, FLASH_KEY1);
    whal_Reg_Write(regmap->base, FLASH_KEYR2_REG, FLASH_KEY2);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Flash_Read(whal_Flash *flashDev, size_t addr,
                                   void *data, size_t dataSz)
{
    uint8_t *dataBuf = (uint8_t *)data;
    whal_Stm32h7Flash_Cfg *cfg;

    if (!flashDev || !flashDev->cfg || !data)
        return WHAL_EINVAL;

    cfg = flashDev->cfg;

    if (addr < cfg->startAddr || addr + dataSz > cfg->startAddr + cfg->size)
        return WHAL_EINVAL;

    /* Flash is memory-mapped */
    uint8_t *flashAddr = (uint8_t *)addr;
    for (size_t i = 0; i < dataSz; ++i)
        dataBuf[i] = flashAddr[i];

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Flash_Write(whal_Flash *flashDev, size_t addr,
                                    const void *data, size_t dataSz)
{
    const uint8_t *dataBuf = (const uint8_t *)data;
    whal_Stm32h7Flash_Cfg *cfg;
    const whal_Regmap *regmap;
    whal_Error err;

    if (!flashDev || !flashDev->cfg || !data)
        return WHAL_EINVAL;

    cfg = flashDev->cfg;
    regmap = &flashDev->regmap;

    /* Address and size must be 32-byte aligned (256-bit flash-word) */
    if ((addr & (FLASH_WORD_SIZE - 1)) || (dataSz & (FLASH_WORD_SIZE - 1)))
        return WHAL_EINVAL;

    if (addr < cfg->startAddr || addr + dataSz > cfg->startAddr + cfg->size)
        return WHAL_EINVAL;

    /* Determine which bank this address falls in */
    size_t offset = addr - cfg->startAddr;
    size_t bank = (offset >= FLASH_BANK_SIZE) ? 1 : 0;
    size_t crOff, srOff, ccrOff, keyrOff;
    GetBankRegs(bank, &crOff, &srOff, &ccrOff, &keyrOff);

    /* Wait for any ongoing operation */
    err = WaitNotBusy(regmap->base, srOff, cfg->timeout);
    if (err)
        return err;

    /* Clear all error and status flags */
    whal_Reg_Write(regmap->base, ccrOff, FLASH_CCR_CLR_ALL);

    /* Set parallelism size and enable programming */
    whal_Reg_Update(regmap->base, crOff,
                    FLASH_CR_PG_Msk | FLASH_CR_PSIZE_Msk,
                    whal_SetBits(FLASH_CR_PG_Msk, FLASH_CR_PG_Pos, 1) |
                    whal_SetBits(FLASH_CR_PSIZE_Msk, FLASH_CR_PSIZE_Pos,
                                 FLASH_PSIZE_X64));

    /* Program data in 256-bit (32 byte) flash-word chunks */
    for (size_t i = 0; i < dataSz; i += FLASH_WORD_SIZE) {
        /* Check if we crossed a bank boundary */
        size_t curOffset = (addr + i) - cfg->startAddr;
        size_t curBank = (curOffset >= FLASH_BANK_SIZE) ? 1 : 0;
        if (curBank != bank) {
            /* Disable programming on old bank */
            whal_Reg_Update(regmap->base, crOff, FLASH_CR_PG_Msk, 0);

            bank = curBank;
            GetBankRegs(bank, &crOff, &srOff, &ccrOff, &keyrOff);

            /* Wait for new bank, clear errors, enable programming */
            err = WaitNotBusy(regmap->base, srOff, cfg->timeout);
            if (err)
                return err;
            whal_Reg_Write(regmap->base, ccrOff, FLASH_CCR_CLR_ALL);
            whal_Reg_Update(regmap->base, crOff,
                            FLASH_CR_PG_Msk | FLASH_CR_PSIZE_Msk,
                            whal_SetBits(FLASH_CR_PG_Msk, FLASH_CR_PG_Pos, 1) |
                            whal_SetBits(FLASH_CR_PSIZE_Msk, FLASH_CR_PSIZE_Pos,
                                         FLASH_PSIZE_X64));
        }

        uint32_t *flashAddr = (uint32_t *)(addr + i);
        const uint32_t *dataAddr = (const uint32_t *)(dataBuf + i);

        /* Write 8 x 32-bit words to trigger the 256-bit programming */
        flashAddr[0] = dataAddr[0];
        flashAddr[1] = dataAddr[1];
        flashAddr[2] = dataAddr[2];
        flashAddr[3] = dataAddr[3];
        flashAddr[4] = dataAddr[4];
        flashAddr[5] = dataAddr[5];
        flashAddr[6] = dataAddr[6];
        flashAddr[7] = dataAddr[7];

        err = WaitNotBusy(regmap->base, srOff, cfg->timeout);
        if (err)
            goto cleanup;

        err = CheckErrors(regmap->base, srOff, ccrOff);
        if (err)
            goto cleanup;
    }

cleanup:
    /* Disable programming */
    whal_Reg_Update(regmap->base, crOff, FLASH_CR_PG_Msk, 0);

    return err;
}

whal_Error whal_Stm32h7Flash_Erase(whal_Flash *flashDev, size_t addr,
                                    size_t dataSz)
{
    whal_Stm32h7Flash_Cfg *cfg;
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

    offset = addr - cfg->startAddr;
    startSector = offset >> FLASH_SECTOR_SHIFT;
    endSector = (offset + dataSz - 1) >> FLASH_SECTOR_SHIFT;

    for (size_t sector = startSector; sector <= endSector; sector++) {
        size_t bank = (sector >= FLASH_SECTORS_PER_BANK) ? 1 : 0;
        size_t sectorInBank = sector - (bank * FLASH_SECTORS_PER_BANK);
        size_t crOff, srOff, ccrOff, keyrOff;

        GetBankRegs(bank, &crOff, &srOff, &ccrOff, &keyrOff);

        /* Wait for any ongoing operation */
        err = WaitNotBusy(regmap->base, srOff, cfg->timeout);
        if (err)
            return err;

        /* Clear all error and status flags */
        whal_Reg_Write(regmap->base, ccrOff, FLASH_CCR_CLR_ALL);

        /* Configure: sector erase, sector number, parallelism */
        whal_Reg_Update(regmap->base, crOff,
                        FLASH_CR_SER_Msk | FLASH_CR_SNB_Msk |
                        FLASH_CR_PSIZE_Msk,
                        whal_SetBits(FLASH_CR_SER_Msk, FLASH_CR_SER_Pos, 1) |
                        whal_SetBits(FLASH_CR_SNB_Msk, FLASH_CR_SNB_Pos,
                                     sectorInBank) |
                        whal_SetBits(FLASH_CR_PSIZE_Msk, FLASH_CR_PSIZE_Pos,
                                     FLASH_PSIZE_X64));

        /* Start erase */
        whal_Reg_Update(regmap->base, crOff, FLASH_CR_START_Msk,
                        whal_SetBits(FLASH_CR_START_Msk, FLASH_CR_START_Pos, 1));

        err = WaitNotBusy(regmap->base, srOff, cfg->timeout);
        if (err)
            goto cleanup;

        err = CheckErrors(regmap->base, srOff, ccrOff);
        if (err)
            goto cleanup;

        /* Clear sector erase mode */
        whal_Reg_Update(regmap->base, crOff, FLASH_CR_SER_Msk, 0);
    }

    return WHAL_SUCCESS;

cleanup:
    {
        size_t bank = (endSector >= FLASH_SECTORS_PER_BANK) ? 1 : 0;
        size_t crOff2, srOff2, ccrOff2, keyrOff2;
        GetBankRegs(bank, &crOff2, &srOff2, &ccrOff2, &keyrOff2);
        whal_Reg_Update(regmap->base, crOff2, FLASH_CR_SER_Msk, 0);
    }
    return err;
}

const whal_FlashDriver whal_Stm32h7Flash_Driver = {
    .Init = whal_Stm32h7Flash_Init,
    .Deinit = whal_Stm32h7Flash_Deinit,
    .Lock = whal_Stm32h7Flash_Lock,
    .Unlock = whal_Stm32h7Flash_Unlock,
    .Read = whal_Stm32h7Flash_Read,
    .Write = whal_Stm32h7Flash_Write,
    .Erase = whal_Stm32h7Flash_Erase,
};
