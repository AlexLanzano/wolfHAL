/* sdmmc_block.c
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include <stdint.h>
#ifdef WHAL_CFG_SDMMC_SINGLE_INSTANCE
#include "board.h"  /* provides whal_Sdmmc_Dev singleton */
#endif
#include <wolfHAL/block/sdmmc_block.h>
#include <wolfHAL/block/block.h>
#include <wolfHAL/sdhc/sdhc.h>
#include <wolfHAL/error.h>
#include <wolfHAL/timeout.h>

/* SD native-mode command indices (built into the CMD register by the transport). */
#define SD_CMD_GO_IDLE_STATE      0
#define SD_CMD_ALL_SEND_CID       2
#define SD_CMD_SEND_RELATIVE_ADDR 3
#define SD_CMD_SELECT_CARD        7
#define SD_CMD_SEND_IF_COND       8
#define SD_CMD_SEND_CSD           9
#define SD_CMD_STOP_TRANSMISSION  12
#define SD_CMD_SEND_STATUS        13
#define SD_CMD_SET_BLOCKLEN       16
#define SD_CMD_READ_SINGLE_BLOCK  17
#define SD_CMD_READ_MULTIPLE_BLOCK 18
#define SD_CMD_WRITE_BLOCK        24
#define SD_CMD_WRITE_MULTIPLE_BLOCK 25
#define SD_CMD_ERASE_WR_BLK_START 32
#define SD_CMD_ERASE_WR_BLK_END   33
#define SD_CMD_ERASE              38
#define SD_ACMD_SEND_OP_COND      41
#define SD_CMD_APP_CMD            55

/* Card status (R1) current-state field value: TRAN = ready for next transfer. */
#define SD_STATE_TRAN             4

/* CMD8 argument: 2.7-3.6V (VHS=1) plus the recommended 0xAA check pattern. */
#define SD_CMD8_ARG               0x1AA
#define SD_CMD8_CHECK_PATTERN     0xAA

/* OCR / ACMD41 fields. */
#define SD_OCR_VOLTAGE_WINDOW     0x00FF8000UL /* 2.7-3.6V window (argument) */
#define SD_OCR_HCS                0x40000000UL /* host capacity support (argument) */
#define SD_OCR_BUSY               0x80000000UL /* power-up complete (response) */
#define SD_OCR_CCS                0x40000000UL /* card capacity status (response) */

#ifdef WHAL_CFG_SDMMC_BLOCK_DIRECT_API_MAPPING
#define whal_Sdmmc_Init   whal_Block_Init
#define whal_Sdmmc_Deinit whal_Block_Deinit
#define whal_Sdmmc_Read   whal_Block_Read
#define whal_Sdmmc_Write  whal_Block_Write
#define whal_Sdmmc_Erase  whal_Block_Erase
#endif /* WHAL_CFG_SDMMC_BLOCK_DIRECT_API_MAPPING */

#ifdef WHAL_CFG_SDMMC_SINGLE_INSTANCE
const whal_Block whal_Sdmmc_Dev = WHAL_CFG_SDMMC_DEV;
#endif

whal_Error whal_Sdmmc_Init(whal_Block *blockDev)
{
    whal_Sdhc_ComCfg comCfg;
    whal_Sdhc_Command cmd;
    whal_Sdhc *sdhc;
    whal_Error err;
    uint32_t ocrArg, cSize, cSizeMult, readBlLen;
    int sdVer2;
#ifdef WHAL_CFG_SDMMC_SINGLE_INSTANCE
    whal_Sdmmc_Cfg *cfg = (whal_Sdmmc_Cfg *)whal_Sdmmc_Dev.cfg;
    (void)blockDev;
#else
    whal_Sdmmc_Cfg *cfg;

    if (!blockDev || !blockDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Sdmmc_Cfg *)blockDev->cfg;
#endif

    sdhc = cfg->sdhc;

    /* Identification clock: 400 kHz, 1-bit, 3.3 V, card powered. */
    comCfg.clockHz = 400000;
    comCfg.busWidth = WHAL_SDHC_BUS_WIDTH_1;
    comCfg.timing = WHAL_SDHC_TIMING_LEGACY;
    comCfg.signalVoltage = WHAL_SDHC_VOLTAGE_330;
    comCfg.powerMode = WHAL_SDHC_POWER_ON;
    comCfg.busMode = WHAL_SDHC_BUSMODE_PUSHPULL;
    err = whal_Sdhc_ConfigureCom(sdhc, &comCfg);
    if (err)
        return err;

    /* CMD0: software-reset all cards to idle state. */
    cmd.opcode = SD_CMD_GO_IDLE_STATE;
    cmd.arg = 0;
    cmd.responseType = WHAL_SDHC_RSP_NONE;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err)
        return err;

    /* CMD8: validate the interface condition; no response means a v1.x card
     * (the host must then clear HCS in ACMD41). */
    cmd.opcode = SD_CMD_SEND_IF_COND;
    cmd.arg = SD_CMD8_ARG;
    cmd.responseType = WHAL_SDHC_RSP_R7;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err == WHAL_SUCCESS) {
        if ((cmd.response[0] & 0xFF) != SD_CMD8_CHECK_PATTERN)
            return WHAL_EHARDWARE;
        sdVer2 = 1;
    } else if (err == WHAL_ETIMEOUT) {
        sdVer2 = 0;
    } else {
        return err;
    }

    /* CMD55 + ACMD41: negotiate the OCR voltage window and poll until the
     * card finishes power-up (OCR busy bit set). */
    ocrArg = SD_OCR_VOLTAGE_WINDOW | (sdVer2 ? SD_OCR_HCS : 0);
    WHAL_TIMEOUT_START(cfg->timeout);
    for (;;) {
        cmd.opcode = SD_CMD_APP_CMD;
        cmd.arg = 0;
        cmd.responseType = WHAL_SDHC_RSP_R1;
        err = whal_Sdhc_Request(sdhc, &cmd, NULL);
        if (err)
            return err;

        cmd.opcode = SD_ACMD_SEND_OP_COND;
        cmd.arg = ocrArg;
        cmd.responseType = WHAL_SDHC_RSP_R3;
        err = whal_Sdhc_Request(sdhc, &cmd, NULL);
        if (err)
            return err;

        if (cmd.response[0] & SD_OCR_BUSY)
            break;
        if (WHAL_TIMEOUT_EXPIRED(cfg->timeout))
            return WHAL_ETIMEOUT;
    }
    cfg->_highCapacity = (cmd.response[0] & SD_OCR_CCS) ? 1 : 0;

    /* CMD2: request the CID; card moves to the identification state. */
    cmd.opcode = SD_CMD_ALL_SEND_CID;
    cmd.arg = 0;
    cmd.responseType = WHAL_SDHC_RSP_R2;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err)
        return err;

    /* CMD3: card publishes its relative address (RCA), moves to stand-by. */
    cmd.opcode = SD_CMD_SEND_RELATIVE_ADDR;
    cmd.arg = 0;
    cmd.responseType = WHAL_SDHC_RSP_R6;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err)
        return err;
    cfg->_rca = (uint16_t)(cmd.response[0] >> 16);

    /* Identification done: leave fOD and move the bus up to 25 MHz. */
    comCfg.clockHz = 25000000;
    err = whal_Sdhc_ConfigureCom(sdhc, &comCfg);
    if (err)
        return err;

    /* CMD9: read the CSD and derive the capacity in 512-byte blocks. */
    cmd.opcode = SD_CMD_SEND_CSD;
    cmd.arg = (uint32_t)cfg->_rca << 16;
    cmd.responseType = WHAL_SDHC_RSP_R2;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err)
        return err;

    if (((cmd.response[3] >> 30) & 0x3) == 1) {
        /* CSD v2: C_SIZE = CSD[69:48], capacity = (C_SIZE+1) * 512 KB. */
        cSize = ((cmd.response[2] & 0x3F) << 16) | (cmd.response[1] >> 16);
        cfg->_blockCount = (cSize + 1) * 1024;
    } else {
        /* CSD v1: C_SIZE=[73:62], C_SIZE_MULT=[49:47], RD_BL=[83:80]. */
        cSize = ((cmd.response[2] & 0x3FF) << 2) | (cmd.response[1] >> 30);
        cSizeMult = (cmd.response[1] >> 15) & 0x7;
        readBlLen = (cmd.response[2] >> 16) & 0xF;
        cfg->_blockCount = (cSize + 1) << (cSizeMult + readBlLen - 7);
    }

    /* CMD7: select the card by RCA, moving it into the transfer state. */
    cmd.opcode = SD_CMD_SELECT_CARD;
    cmd.arg = (uint32_t)cfg->_rca << 16;
    cmd.responseType = WHAL_SDHC_RSP_R1B;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err)
        return err;

    /* CMD16: SDSC is byte-addressed, so pin the block length at 512. */
    if (!cfg->_highCapacity) {
        cmd.opcode = SD_CMD_SET_BLOCKLEN;
        cmd.arg = WHAL_SDMMC_BLOCK_SZ;
        cmd.responseType = WHAL_SDHC_RSP_R1;
        err = whal_Sdhc_Request(sdhc, &cmd, NULL);
        if (err)
            return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Sdmmc_Deinit(whal_Block *blockDev)
{
    whal_Sdhc_ComCfg comCfg;
#ifdef WHAL_CFG_SDMMC_SINGLE_INSTANCE
    whal_Sdmmc_Cfg *cfg = (whal_Sdmmc_Cfg *)whal_Sdmmc_Dev.cfg;
    (void)blockDev;
#else
    whal_Sdmmc_Cfg *cfg;

    if (!blockDev || !blockDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Sdmmc_Cfg *)blockDev->cfg;
#endif

    /* Stop the card clock and power the card off. */
    comCfg.clockHz = 0;
    comCfg.busWidth = WHAL_SDHC_BUS_WIDTH_1;
    comCfg.timing = WHAL_SDHC_TIMING_LEGACY;
    comCfg.signalVoltage = WHAL_SDHC_VOLTAGE_330;
    comCfg.powerMode = WHAL_SDHC_POWER_OFF;
    comCfg.busMode = WHAL_SDHC_BUSMODE_PUSHPULL;
    return whal_Sdhc_ConfigureCom(cfg->sdhc, &comCfg);
}

whal_Error whal_Sdmmc_Read(whal_Block *blockDev, uint32_t block,
                           void *data, uint32_t blockCount)
{
    whal_Sdhc_Command cmd;
    whal_Sdhc_Data dataDesc;
    whal_Sdhc *sdhc;
    whal_Error err, stopErr;
    int multi;
#ifdef WHAL_CFG_SDMMC_SINGLE_INSTANCE
    whal_Sdmmc_Cfg *cfg = (whal_Sdmmc_Cfg *)whal_Sdmmc_Dev.cfg;
    (void)blockDev;
#else
    whal_Sdmmc_Cfg *cfg;

    if (!blockDev || !blockDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Sdmmc_Cfg *)blockDev->cfg;
#endif

    if (!data || blockCount == 0)
        return WHAL_EINVAL;
    if (block > cfg->_blockCount || blockCount > cfg->_blockCount - block)
        return WHAL_EINVAL;

    sdhc = cfg->sdhc;
    multi = (blockCount > 1);

    dataDesc.data = data;
    dataDesc.blockSize = WHAL_SDMMC_BLOCK_SZ;
    dataDesc.blockCount = blockCount;
    dataDesc.dir = WHAL_SDHC_DATA_READ;
    dataDesc.bytesXfered = 0;

    /* CMD17/CMD18: read one or many 512-byte blocks. SDHC/SDXC address by
     * block; SDSC addresses by byte. */
    cmd.opcode = multi ? SD_CMD_READ_MULTIPLE_BLOCK : SD_CMD_READ_SINGLE_BLOCK;
    cmd.arg = cfg->_highCapacity ? block : block * WHAL_SDMMC_BLOCK_SZ;
    cmd.responseType = WHAL_SDHC_RSP_R1;
    err = whal_Sdhc_Request(sdhc, &cmd, &dataDesc);

    /* CMD12: end the open-ended multi-block read, even if the read failed. */
    if (multi) {
        cmd.opcode = SD_CMD_STOP_TRANSMISSION;
        cmd.arg = 0;
        cmd.responseType = WHAL_SDHC_RSP_R1B;
        stopErr = whal_Sdhc_Request(sdhc, &cmd, NULL);
        if (err == WHAL_SUCCESS)
            err = stopErr;
    }

    return err;
}

whal_Error whal_Sdmmc_Write(whal_Block *blockDev, uint32_t block,
                            const void *data, uint32_t blockCount)
{
    whal_Sdhc_Command cmd;
    whal_Sdhc_Data dataDesc;
    whal_Sdhc *sdhc;
    whal_Error err, stopErr;
    int multi;
#ifdef WHAL_CFG_SDMMC_SINGLE_INSTANCE
    whal_Sdmmc_Cfg *cfg = (whal_Sdmmc_Cfg *)whal_Sdmmc_Dev.cfg;
    (void)blockDev;
#else
    whal_Sdmmc_Cfg *cfg;

    if (!blockDev || !blockDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Sdmmc_Cfg *)blockDev->cfg;
#endif

    if (!data || blockCount == 0)
        return WHAL_EINVAL;
    if (block > cfg->_blockCount || blockCount > cfg->_blockCount - block)
        return WHAL_EINVAL;

    sdhc = cfg->sdhc;
    multi = (blockCount > 1);

    dataDesc.data = (void *)data;
    dataDesc.blockSize = WHAL_SDMMC_BLOCK_SZ;
    dataDesc.blockCount = blockCount;
    dataDesc.dir = WHAL_SDHC_DATA_WRITE;
    dataDesc.bytesXfered = 0;

    /* CMD24/CMD25: write one or many 512-byte blocks. SDHC/SDXC address by
     * block; SDSC addresses by byte. */
    cmd.opcode = multi ? SD_CMD_WRITE_MULTIPLE_BLOCK : SD_CMD_WRITE_BLOCK;
    cmd.arg = cfg->_highCapacity ? block : block * WHAL_SDMMC_BLOCK_SZ;
    cmd.responseType = WHAL_SDHC_RSP_R1;
    err = whal_Sdhc_Request(sdhc, &cmd, &dataDesc);

    /* CMD12: end the open-ended multi-block write, even if the write failed. */
    if (multi) {
        cmd.opcode = SD_CMD_STOP_TRANSMISSION;
        cmd.arg = 0;
        cmd.responseType = WHAL_SDHC_RSP_R1B;
        stopErr = whal_Sdhc_Request(sdhc, &cmd, NULL);
        if (err == WHAL_SUCCESS)
            err = stopErr;
    }
    if (err)
        return err;

    /* CMD13: poll the card status until it leaves the programming state,
     * i.e. the data is committed to flash. */
    WHAL_TIMEOUT_START(cfg->timeout);
    for (;;) {
        cmd.opcode = SD_CMD_SEND_STATUS;
        cmd.arg = (uint32_t)cfg->_rca << 16;
        cmd.responseType = WHAL_SDHC_RSP_R1;
        err = whal_Sdhc_Request(sdhc, &cmd, NULL);
        if (err)
            return err;
        if (((cmd.response[0] >> 9) & 0xF) == SD_STATE_TRAN)
            break;
        if (WHAL_TIMEOUT_EXPIRED(cfg->timeout))
            return WHAL_ETIMEOUT;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Sdmmc_Erase(whal_Block *blockDev, uint32_t block,
                            uint32_t blockCount)
{
    whal_Sdhc_Command cmd;
    whal_Sdhc *sdhc;
    whal_Error err;
    uint32_t lastBlock, startAddr, endAddr;
#ifdef WHAL_CFG_SDMMC_SINGLE_INSTANCE
    whal_Sdmmc_Cfg *cfg = (whal_Sdmmc_Cfg *)whal_Sdmmc_Dev.cfg;
    (void)blockDev;
#else
    whal_Sdmmc_Cfg *cfg;

    if (!blockDev || !blockDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Sdmmc_Cfg *)blockDev->cfg;
#endif

    if (blockCount == 0)
        return WHAL_EINVAL;
    if (block > cfg->_blockCount || blockCount > cfg->_blockCount - block)
        return WHAL_EINVAL;

    sdhc = cfg->sdhc;
    lastBlock = block + blockCount - 1;
    startAddr = cfg->_highCapacity ? block : block * WHAL_SDMMC_BLOCK_SZ;
    endAddr = cfg->_highCapacity ? lastBlock : lastBlock * WHAL_SDMMC_BLOCK_SZ;

    /* CMD32: set the first block of the erase range. */
    cmd.opcode = SD_CMD_ERASE_WR_BLK_START;
    cmd.arg = startAddr;
    cmd.responseType = WHAL_SDHC_RSP_R1;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err)
        return err;

    /* CMD33: set the last block of the erase range. */
    cmd.opcode = SD_CMD_ERASE_WR_BLK_END;
    cmd.arg = endAddr;
    cmd.responseType = WHAL_SDHC_RSP_R1;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err)
        return err;

    /* CMD38: erase the selected range. */
    cmd.opcode = SD_CMD_ERASE;
    cmd.arg = 0;
    cmd.responseType = WHAL_SDHC_RSP_R1B;
    err = whal_Sdhc_Request(sdhc, &cmd, NULL);
    if (err)
        return err;

    /* CMD13: poll the card status until the erase completes (leaves the
     * programming state). */
    WHAL_TIMEOUT_START(cfg->timeout);
    for (;;) {
        cmd.opcode = SD_CMD_SEND_STATUS;
        cmd.arg = (uint32_t)cfg->_rca << 16;
        cmd.responseType = WHAL_SDHC_RSP_R1;
        err = whal_Sdhc_Request(sdhc, &cmd, NULL);
        if (err)
            return err;
        if (((cmd.response[0] >> 9) & 0xF) == SD_STATE_TRAN)
            break;
        if (WHAL_TIMEOUT_EXPIRED(cfg->timeout))
            return WHAL_ETIMEOUT;
    }

    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_SDMMC_BLOCK_DIRECT_API_MAPPING
const whal_BlockDriver whal_Sdmmc_Driver = {
    .Init   = whal_Sdmmc_Init,
    .Deinit = whal_Sdmmc_Deinit,
    .Read   = whal_Sdmmc_Read,
    .Write  = whal_Sdmmc_Write,
    .Erase  = whal_Sdmmc_Erase,
};
#endif /* !WHAL_CFG_SDMMC_BLOCK_DIRECT_API_MAPPING */
