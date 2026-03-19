#include <stdint.h>
#include <wolfHAL/flash/pic32cz_flash.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/*
 * PIC32CZ FCW (Flash Controller Write) Register Definitions
 *
 * The PIC32CZ uses FCW for flash write/erase operations. Each operation
 * requires writing an unlock key to FCW_KEY before triggering via FCW_CTRLA.
 */

/* Register offsets */
#define FCW_CTRLA_REG       0x00
#define FCW_MUTEX_REG       0x08
#define FCW_INTFLAG_REG     0x14
#define FCW_STATUS_REG      0x18
#define FCW_KEY_REG         0x1C
#define FCW_ADDR_REG        0x20
#define FCW_SRCADDR_REG        0x24
#define FCW_DATA_REG(n)     (0x28 + ((n) * 4))

/* CTRLA: NVM operation command and pre-program bit */
#define FCW_CTRLA_NVMOP_Pos               0
#define FCW_CTRLA_NVMOP_Msk               (WHAL_BITMASK(4) << FCW_CTRLA_NVMOP_Pos)
#define FCW_CTRLA_NVMOP_SINGLE_DWORD       0x1
#define FCW_CTRLA_NVMOP_QUAD_DWORD         0x2
#define FCW_CTRLA_NVMOP_PAGE_ERASE         0x4

#define FCW_CTRLA_PREPG_Pos                     7
#define FCW_CTRLA_PREPG_Msk                     (1UL << FCW_CTRLA_PREPG_Pos)

/* MUTEX: NVM locking and ownership values */
#define FCW_MUTEX_LOCK_Pos 0
#define FCW_MUTEX_LOCK_Msk (1UL << FCW_MUTEX_LOCK_Pos)

#define FCW_MUTEX_OWNER_Pos 1
#define FCW_MUTEX_OWNER_Msk (WHAL_BITMASK(2) << FCW_MUTEX_OWNER_Pos)

/* INTFLAG: operation completion and error flags (write-1-to-clear) */
#define FCW_INTFLAG_DONE_Pos        0
#define FCW_INTFLAG_DONE_Msk        (1UL << FCW_INTFLAG_DONE_Pos)

#define FCW_INTFLAG_KEYERR_Pos      1
#define FCW_INTFLAG_KEYERR_Msk      (1UL << FCW_INTFLAG_KEYERR_Pos)

#define FCW_INTFLAG_CFGERR_Pos      2
#define FCW_INTFLAG_CFGERR_Msk      (1UL << FCW_INTFLAG_CFGERR_Pos)

#define FCW_INTFLAG_FIFOERR_Pos     3
#define FCW_INTFLAG_FIFOERR_Msk     (1UL << FCW_INTFLAG_FIFOERR_Pos)

#define FCW_INTFLAG_BUSERR_Pos      4
#define FCW_INTFLAG_BUSERR_Msk      (1UL << FCW_INTFLAG_BUSERR_Pos)

#define FCW_INTFLAG_WPERR_Pos       5
#define FCW_INTFLAG_WPERR_Msk       (1UL << FCW_INTFLAG_WPERR_Pos)

#define FCW_INTFLAG_OPERR_Pos       6
#define FCW_INTFLAG_OPERR_Msk       (1UL << FCW_INTFLAG_OPERR_Pos)

#define FCW_INTFLAG_SECERR_Pos      7
#define FCW_INTFLAG_SECERR_Msk      (1UL << FCW_INTFLAG_SECERR_Pos)

#define FCW_INTFLAG_HTDPGM_Pos      8
#define FCW_INTFLAG_HTDPGM_Msk      (1UL << FCW_INTFLAG_HTDPGM_Pos)

#define FCW_INTFLAG_BORERR_Pos      12
#define FCW_INTFLAG_BORERR_Msk      (1UL << FCW_INTFLAG_BORERR_Pos)

#define FCW_INTFLAG_WRERR_Pos       13
#define FCW_INTFLAG_WRERR_Msk       (1UL << FCW_INTFLAG_WRERR_Pos)

#define FCW_INTFLAG_ALL_ERR     (FCW_INTFLAG_KEYERR_Msk | FCW_INTFLAG_CFGERR_Msk | \
                                 FCW_INTFLAG_FIFOERR_Msk | FCW_INTFLAG_BUSERR_Msk | \
                                 FCW_INTFLAG_WPERR_Msk | FCW_INTFLAG_OPERR_Msk | \
                                 FCW_INTFLAG_SECERR_Msk | FCW_INTFLAG_HTDPGM_Msk | \
                                 FCW_INTFLAG_BORERR_Msk | FCW_INTFLAG_WRERR_Msk)

#define FCW_INTFLAG_ALL         (FCW_INTFLAG_DONE_Msk | FCW_INTFLAG_ALL_ERR)

/* STATUS: busy flag */
#define FCW_STATUS_BUSY_Pos         0
#define FCW_STATUS_BUSY_Msk         (1UL << FCW_STATUS_BUSY_Pos)

/* KEY: unlock value for write/erase operations */
#define FCW_UNLOCK_WRKEY        0x91C32C01

/* Flash geometry */
#define FCW_PAGE_SIZE           4096
#define FCW_DWORD_SIZE          8
#define FCW_QDWORD_SIZE         32

static whal_Error whal_Pic32czFlash_MutexLock(const whal_Regmap *reg,
                                              whal_Timeout *timeout)
{
    WHAL_TIMEOUT_START(timeout);
    while (whal_Reg_Read(reg->base, FCW_MUTEX_REG) & FCW_MUTEX_LOCK_Msk) {
        if (WHAL_TIMEOUT_EXPIRED(timeout))
            return WHAL_ETIMEOUT;
    }

    whal_Reg_Update(reg->base, FCW_MUTEX_REG, FCW_MUTEX_LOCK_Msk | FCW_MUTEX_OWNER_Msk,
                    whal_SetBits(FCW_MUTEX_LOCK_Msk, FCW_MUTEX_LOCK_Pos, 1) |
                    whal_SetBits(FCW_MUTEX_OWNER_Msk, FCW_MUTEX_OWNER_Pos, 1));

    return WHAL_SUCCESS;
}

static void whal_Pic32czFlash_MutexUnlock(const whal_Regmap *reg)
{
    whal_Reg_Update(reg->base, FCW_MUTEX_REG, FCW_MUTEX_LOCK_Msk | FCW_MUTEX_OWNER_Msk,
                    whal_SetBits(FCW_MUTEX_LOCK_Msk, FCW_MUTEX_LOCK_Pos, 0) |
                    whal_SetBits(FCW_MUTEX_OWNER_Msk, FCW_MUTEX_OWNER_Pos, 1));
}

static whal_Error whal_Pic32czFlash_WaitBusy(const whal_Regmap *reg,
                                              whal_Timeout *timeout)
{
    WHAL_TIMEOUT_START(timeout);
    while (whal_Reg_Read(reg->base, FCW_STATUS_REG) & FCW_STATUS_BUSY_Msk) {
        if (WHAL_TIMEOUT_EXPIRED(timeout))
            return WHAL_ETIMEOUT;
    }

    return WHAL_SUCCESS;
}

/*
 * Execute an FCW command: unlock, trigger, wait, and check for errors.
 * Caller must set up FCW_ADDR and FCW_DATA registers before calling.
 */
static whal_Error whal_Pic32czFlash_ExecCmd(const whal_Regmap *reg, size_t cmd,
                                            whal_Timeout *timeout)
{
    whal_Error err;
    size_t errFlags;

    /* Write unlock key */
    whal_Reg_Update(reg->base, FCW_KEY_REG, 0xFFFFFFFF, FCW_UNLOCK_WRKEY);

    /* Trigger operation with pre-program enabled */
    whal_Reg_Update(reg->base, FCW_CTRLA_REG,
                    FCW_CTRLA_NVMOP_Msk | FCW_CTRLA_PREPG_Msk,
                    whal_SetBits(FCW_CTRLA_NVMOP_Msk, FCW_CTRLA_NVMOP_Pos, cmd) | FCW_CTRLA_PREPG_Msk);

    /* Wait for completion */
    err = whal_Pic32czFlash_WaitBusy(reg, timeout);
    if (err)
        return err;

    /* Check for errors */
    whal_Reg_Get(reg->base, FCW_INTFLAG_REG, FCW_INTFLAG_ALL_ERR, 0, &errFlags);

    /* Clear DONE flag */
    whal_Reg_Update(reg->base, FCW_INTFLAG_REG, FCW_INTFLAG_DONE_Msk,
                    FCW_INTFLAG_DONE_Msk);

    if (errFlags) {
        /* Clear error flags */
        whal_Reg_Update(reg->base, FCW_INTFLAG_REG, FCW_INTFLAG_ALL_ERR,
                        FCW_INTFLAG_ALL_ERR);
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czFlash_Init(whal_Flash *flashDev)
{
    const whal_Regmap *reg;

    if (!flashDev) {
        return WHAL_EINVAL;
    }

    reg = &flashDev->regmap;

    whal_Pic32czFlash_MutexUnlock(reg);
    whal_Reg_Update(reg->base, FCW_KEY_REG, 0xFFFFFFFF, 0);
    whal_Reg_Update(reg->base, FCW_ADDR_REG, 0xFFFFFFFF, 0);
    whal_Reg_Update(reg->base, FCW_SRCADDR_REG, 0xFFFFFFFF, 0);

    /* Clear all pending interrupt flags */
    whal_Reg_Update(reg->base, FCW_INTFLAG_REG, FCW_INTFLAG_ALL,
                    FCW_INTFLAG_ALL);

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czFlash_Deinit(whal_Flash *flashDev)
{
    const whal_Regmap *reg;

    if (!flashDev) {
        return WHAL_EINVAL;
    }

    reg = &flashDev->regmap;

    whal_Pic32czFlash_MutexUnlock(reg);
    whal_Reg_Update(reg->base, FCW_KEY_REG, 0xFFFFFFFF, 0);
    whal_Reg_Update(reg->base, FCW_ADDR_REG, 0xFFFFFFFF, 0);
    whal_Reg_Update(reg->base, FCW_SRCADDR_REG, 0xFFFFFFFF, 0);

    /* Clear all pending interrupt flags */
    whal_Reg_Update(reg->base, FCW_INTFLAG_REG, FCW_INTFLAG_ALL,
                    FCW_INTFLAG_ALL);

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czFlash_Lock(whal_Flash *flashDev, size_t addr, size_t len)
{
    /*
     * TODO: Implement using FCW_PWP[0..7] region write-protect registers.
     * Each FCW operation already requires the unlock key, providing
     * inherent per-operation protection.
     */
    (void)addr;
    (void)len;

    if (!flashDev) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czFlash_Unlock(whal_Flash *flashDev, size_t addr, size_t len)
{
    (void)addr;
    (void)len;

    if (!flashDev) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czFlash_Read(whal_Flash *flashDev, size_t addr, uint8_t *data,
                             size_t dataSz)
{
    const whal_Regmap *reg;
    whal_Pic32czFlash_Cfg *cfg;
    uint8_t *flashAddr = (uint8_t *)addr;
    whal_Error err;
    size_t i;

    if (!flashDev || !flashDev->cfg || !data) {
        return WHAL_EINVAL;
    }

    reg = &flashDev->regmap;
    cfg = flashDev->cfg;


    err = whal_Pic32czFlash_MutexLock(reg, cfg->timeout);
    if (err)
        return err;

    /* Flash is memory-mapped; read directly */
    for (i = 0; i < dataSz; i++) {
        data[i] = flashAddr[i];
    }

    whal_Pic32czFlash_MutexUnlock(reg);

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czFlash_Write(whal_Flash *flashDev, size_t addr, const uint8_t *data,
                              size_t dataSz)
{
    const whal_Regmap *reg;
    whal_Pic32czFlash_Cfg *cfg;
    const uint32_t *src;
    whal_Error err;
    size_t offset = 0;

    if (!flashDev || !flashDev->cfg || !data) {
        return WHAL_EINVAL;
    }

    cfg = flashDev->cfg;

    /* Require double-word alignment */
    if ((addr & 0x7) || (dataSz & 0x7)) {
        return WHAL_EINVAL;
    }

    reg = &flashDev->regmap;
    src = (const uint32_t *)data;


    err = whal_Pic32czFlash_MutexLock(reg, cfg->timeout);
    if (err)
        return err;

    while (offset < dataSz) {
        size_t curAddr = addr + offset;
        size_t remaining = dataSz - offset;

        err = whal_Pic32czFlash_WaitBusy(reg, cfg->timeout);
        if (err) {
            whal_Pic32czFlash_MutexUnlock(reg);
            return err;
        }

        if (!(curAddr & 0x1F) && remaining >= FCW_QDWORD_SIZE) {
            /* Quad double word write (32 bytes) */
            size_t j;
            for (j = 0; j < 8; j++) {
                whal_Reg_Update(reg->base, FCW_DATA_REG(j),
                                0xFFFFFFFF, src[offset / 4 + j]);
            }
            whal_Reg_Update(reg->base, FCW_ADDR_REG, 0xFFFFFFFF, curAddr);

            err = whal_Pic32czFlash_ExecCmd(reg,
                                            FCW_CTRLA_NVMOP_QUAD_DWORD,
                                            cfg->timeout);
            if (err) {
                whal_Pic32czFlash_MutexUnlock(reg);
                return err;
            }
            offset += FCW_QDWORD_SIZE;
        } else {
            /* Single double word write (8 bytes) */
            whal_Reg_Update(reg->base, FCW_DATA_REG(0),
                            0xFFFFFFFF, src[offset / 4]);
            whal_Reg_Update(reg->base, FCW_DATA_REG(1),
                            0xFFFFFFFF, src[offset / 4 + 1]);
            whal_Reg_Update(reg->base, FCW_ADDR_REG, 0xFFFFFFFF, curAddr);

            err = whal_Pic32czFlash_ExecCmd(reg,
                                            FCW_CTRLA_NVMOP_SINGLE_DWORD,
                                            cfg->timeout);
            if (err) {
                whal_Pic32czFlash_MutexUnlock(reg);
                return err;
            }
            offset += FCW_DWORD_SIZE;
        }

    }

    whal_Pic32czFlash_MutexUnlock(reg);

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czFlash_Erase(whal_Flash *flashDev, size_t addr, size_t dataSz)
{
    const whal_Regmap *reg;
    whal_Pic32czFlash_Cfg *cfg;
    whal_Error err;
    size_t pageAddr;
    size_t endAddr;

    if (!flashDev || !flashDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = flashDev->cfg;
    reg = &flashDev->regmap;

    /* Align down to page boundary */
    pageAddr = addr & ~(FCW_PAGE_SIZE - 1);
    endAddr = addr + dataSz;


    err = whal_Pic32czFlash_MutexLock(reg, cfg->timeout);
    if (err)
        return err;

    while (pageAddr < endAddr) {
        err = whal_Pic32czFlash_WaitBusy(reg, cfg->timeout);
        if (err) {
            whal_Pic32czFlash_MutexUnlock(reg);
            return err;
        }

        whal_Reg_Update(reg->base, FCW_ADDR_REG, 0xFFFFFFFF, pageAddr);

        err = whal_Pic32czFlash_ExecCmd(reg, FCW_CTRLA_NVMOP_PAGE_ERASE,
                                        cfg->timeout);
        if (err) {
            whal_Pic32czFlash_MutexUnlock(reg);
            return err;
        }

        pageAddr += FCW_PAGE_SIZE;
    }

    whal_Pic32czFlash_MutexUnlock(reg);

    return WHAL_SUCCESS;
}

const whal_FlashDriver whal_Pic32czFlash_Driver = {
    .Init = whal_Pic32czFlash_Init,
    .Deinit = whal_Pic32czFlash_Deinit,
    .Lock = whal_Pic32czFlash_Lock,
    .Unlock = whal_Pic32czFlash_Unlock,
    .Read = whal_Pic32czFlash_Read,
    .Write = whal_Pic32czFlash_Write,
    .Erase = whal_Pic32czFlash_Erase,
};
