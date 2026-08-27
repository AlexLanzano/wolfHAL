/* lpc55s6x_sdhc.c
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
#ifdef WHAL_CFG_LPC55S6X_SDHC_SINGLE_INSTANCE
#include "board.h"  /* provides whal_Lpc55s6x_Sdhc_Dev device instance (possibly via platform alias macro) */
#endif
#include <wolfHAL/sdhc/lpc55s6x_sdhc.h>
#include <wolfHAL/sdhc/sdhc.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>

/*
 * SD/MMC controller registers (offsets from the SDMMC base address).
 */
#define SDMMC_CTRL_REG                   0x000
#define SDMMC_CTRL_CONTROLLER_RESET_Msk  (1UL << 0)
#define SDMMC_CTRL_FIFO_RESET_Msk        (1UL << 1)
#define SDMMC_CTRL_DMA_RESET_Msk         (1UL << 2)

#define SDMMC_PWREN_REG                  0x004
#define SDMMC_PWREN_POWER_ENABLE0_Msk    (1UL << 0)

#define SDMMC_CLKDIV_REG                 0x008

#define SDMMC_CLKENA_REG                 0x010
#define SDMMC_CLKENA_CCLK0_ENABLE_Msk    (1UL << 0)

#define SDMMC_TMOUT_REG                  0x014

#define SDMMC_CTYPE_REG                  0x018
#define SDMMC_CTYPE_CARD0_WIDTH0_Msk     (1UL << 0)
#define SDMMC_CTYPE_CARD0_WIDTH1_Msk     (1UL << 16)

#define SDMMC_BLKSIZ_REG                 0x01C

#define SDMMC_BYTCNT_REG                 0x020

#define SDMMC_INTMASK_REG                0x024

#define SDMMC_CMDARG_REG                 0x028

#define SDMMC_CMD_REG                             0x02C
#define SDMMC_CMD_INDEX_Msk                       (WHAL_BITMASK(6) << 0)
#define SDMMC_CMD_RESPONSE_EXPECT_Msk             (1UL << 6)
#define SDMMC_CMD_RESPONSE_LENGTH_Msk             (1UL << 7)
#define SDMMC_CMD_CHECK_RESPONSE_CRC_Msk          (1UL << 8)
#define SDMMC_CMD_DATA_EXPECTED_Msk               (1UL << 9)
#define SDMMC_CMD_READ_WRITE_Msk                  (1UL << 10)
#define SDMMC_CMD_WAIT_PRVDATA_COMPLETE_Msk       (1UL << 13)
#define SDMMC_CMD_SEND_INITIALIZATION_Msk         (1UL << 15)
#define SDMMC_CMD_CARD_NUMBER_Pos                 16
#define SDMMC_CMD_UPDATE_CLOCK_REGISTERS_ONLY_Msk (1UL << 21)
#define SDMMC_CMD_START_CMD_Msk                   (1UL << 31)

#define SDMMC_RESP0_REG                  0x030
#define SDMMC_RESP1_REG                  0x034
#define SDMMC_RESP2_REG                  0x038
#define SDMMC_RESP3_REG                  0x03C

#define SDMMC_RINTSTS_REG                0x044
#define SDMMC_RINTSTS_RE_Msk             (1UL << 1)
#define SDMMC_RINTSTS_CDONE_Msk          (1UL << 2)
#define SDMMC_RINTSTS_DTO_Msk            (1UL << 3)
#define SDMMC_RINTSTS_RCRC_Msk           (1UL << 6)
#define SDMMC_RINTSTS_DCRC_Msk           (1UL << 7)
#define SDMMC_RINTSTS_RTO_Msk            (1UL << 8)
#define SDMMC_RINTSTS_DRTO_Msk           (1UL << 9)
#define SDMMC_RINTSTS_SBE_Msk            (1UL << 13)
#define SDMMC_RINTSTS_EBE_Msk            (1UL << 15)

#define SDMMC_STATUS_REG                 0x048
#define SDMMC_STATUS_FIFO_EMPTY_Msk      (1UL << 2)
#define SDMMC_STATUS_FIFO_FULL_Msk       (1UL << 3)

#define SDMMC_FIFO_REG                   0x200

#ifdef WHAL_CFG_LPC55S6X_SDHC_DIRECT_API_MAPPING
#define whal_Lpc55s6x_Sdhc_Init         whal_Sdhc_Init
#define whal_Lpc55s6x_Sdhc_Deinit       whal_Sdhc_Deinit
#define whal_Lpc55s6x_Sdhc_ConfigureCom whal_Sdhc_ConfigureCom
#define whal_Lpc55s6x_Sdhc_Request      whal_Sdhc_Request
#endif /* WHAL_CFG_LPC55S6X_SDHC_DIRECT_API_MAPPING */

#ifdef WHAL_CFG_LPC55S6X_SDHC_SINGLE_INSTANCE
const whal_Sdhc whal_Lpc55s6x_Sdhc_Dev = WHAL_CFG_LPC55S6X_SDHC_DEV;
#endif

whal_Error whal_Lpc55s6x_Sdhc_Init(whal_Sdhc *dev)
{
    whal_Error err;
#ifdef WHAL_CFG_LPC55S6X_SDHC_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Sdhc_Dev.base;
    whal_Lpc55s6x_Sdhc_Cfg *cfg =
        (whal_Lpc55s6x_Sdhc_Cfg *)whal_Lpc55s6x_Sdhc_Dev.cfg;
    (void)dev;
#else
    size_t base;
    whal_Lpc55s6x_Sdhc_Cfg *cfg;

    if (!dev || !dev->cfg)
        return WHAL_EINVAL;

    base = dev->base;
    cfg = (whal_Lpc55s6x_Sdhc_Cfg *)dev->cfg;
#endif

    whal_Reg_Write(base, SDMMC_CTRL_REG,
                   SDMMC_CTRL_CONTROLLER_RESET_Msk
                   | SDMMC_CTRL_FIFO_RESET_Msk
                   | SDMMC_CTRL_DMA_RESET_Msk);

    err = whal_Reg_ReadPoll(base, SDMMC_CTRL_REG,
                            SDMMC_CTRL_CONTROLLER_RESET_Msk
                            | SDMMC_CTRL_FIFO_RESET_Msk
                            | SDMMC_CTRL_DMA_RESET_Msk,
                            0, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Write(base, SDMMC_RINTSTS_REG, 0xFFFFFFFFu);
    whal_Reg_Write(base, SDMMC_INTMASK_REG, 0);
    whal_Reg_Write(base, SDMMC_TMOUT_REG, 0xFFFFFF40u);

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Sdhc_Deinit(whal_Sdhc *dev)
{
#ifdef WHAL_CFG_LPC55S6X_SDHC_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Sdhc_Dev.base;
    (void)dev;
#else
    size_t base;

    if (!dev)
        return WHAL_EINVAL;

    base = dev->base;
#endif

    whal_Reg_Write(base, SDMMC_CLKENA_REG, 0);
    whal_Reg_Write(base, SDMMC_PWREN_REG, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Sdhc_ConfigureCom(whal_Sdhc *dev,
                                           const whal_Sdhc_ComCfg *comCfg)
{
    uint32_t div, ctype, pwren, clkena;
    whal_Error err;
#ifdef WHAL_CFG_LPC55S6X_SDHC_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Sdhc_Dev.base;
    whal_Lpc55s6x_Sdhc_Cfg *cfg =
        (whal_Lpc55s6x_Sdhc_Cfg *)whal_Lpc55s6x_Sdhc_Dev.cfg;
    (void)dev;

    if (!comCfg)
        return WHAL_EINVAL;
#else
    size_t base;
    whal_Lpc55s6x_Sdhc_Cfg *cfg;

    if (!dev || !dev->cfg || !comCfg)
        return WHAL_EINVAL;

    base = dev->base;
    cfg = (whal_Lpc55s6x_Sdhc_Cfg *)dev->cfg;
#endif

    if (comCfg->signalVoltage != WHAL_SDHC_VOLTAGE_330)
        return WHAL_ENOTSUP;
    if (comCfg->timing != WHAL_SDHC_TIMING_LEGACY
        && comCfg->timing != WHAL_SDHC_TIMING_HS)
        return WHAL_ENOTSUP;

    switch (comCfg->busWidth) {
    case WHAL_SDHC_BUS_WIDTH_1:
        ctype = 0;
        break;
    case WHAL_SDHC_BUS_WIDTH_4:
        ctype = SDMMC_CTYPE_CARD0_WIDTH0_Msk << cfg->cardNum;
        break;
    case WHAL_SDHC_BUS_WIDTH_8:
        ctype = SDMMC_CTYPE_CARD0_WIDTH1_Msk << cfg->cardNum;
        break;
    default:
        return WHAL_EINVAL;
    }

    if (comCfg->clockHz == 0) {
        div = 0;
        clkena = 0;
    } else if (cfg->fclkHz <= comCfg->clockHz) {
        div = 0;
        clkena = SDMMC_CLKENA_CCLK0_ENABLE_Msk << cfg->cardNum;
    } else {
        div = (cfg->fclkHz + 2u * comCfg->clockHz - 1u)
              / (2u * comCfg->clockHz);
        if (div > 0xFF)
            return WHAL_ENOTSUP;
        clkena = SDMMC_CLKENA_CCLK0_ENABLE_Msk << cfg->cardNum;
    }

    pwren = (comCfg->powerMode == WHAL_SDHC_POWER_ON)
            ? (SDMMC_PWREN_POWER_ENABLE0_Msk << cfg->cardNum) : 0;

    whal_Reg_Write(base, SDMMC_PWREN_REG, pwren);

    whal_Reg_Write(base, SDMMC_CLKDIV_REG, div);
    whal_Reg_Write(base, SDMMC_CLKENA_REG, clkena);
    whal_Reg_Write(base, SDMMC_CMD_REG,
                   SDMMC_CMD_START_CMD_Msk
                   | SDMMC_CMD_UPDATE_CLOCK_REGISTERS_ONLY_Msk
                   | SDMMC_CMD_WAIT_PRVDATA_COMPLETE_Msk);
    err = whal_Reg_ReadPoll(base, SDMMC_CMD_REG,
                            SDMMC_CMD_START_CMD_Msk, 0, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Write(base, SDMMC_CTYPE_REG, ctype);

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Sdhc_Request(whal_Sdhc *dev, whal_Sdhc_Command *cmd,
                                      whal_Sdhc_Data *data)
{
    uint32_t cmdReg, sts, word, fifo;
    uint8_t *buf;
    size_t remaining, b;
    int longResp = 0;
    int isWrite;
    whal_Error err;
#ifdef WHAL_CFG_LPC55S6X_SDHC_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Sdhc_Dev.base;
    whal_Lpc55s6x_Sdhc_Cfg *cfg =
        (whal_Lpc55s6x_Sdhc_Cfg *)whal_Lpc55s6x_Sdhc_Dev.cfg;
    (void)dev;

    if (!cmd)
        return WHAL_EINVAL;
#else
    size_t base;
    whal_Lpc55s6x_Sdhc_Cfg *cfg;

    if (!dev || !dev->cfg || !cmd)
        return WHAL_EINVAL;

    base = dev->base;
    cfg = (whal_Lpc55s6x_Sdhc_Cfg *)dev->cfg;
#endif

    cmdReg = (cmd->opcode & SDMMC_CMD_INDEX_Msk)
             | ((uint32_t)cfg->cardNum << SDMMC_CMD_CARD_NUMBER_Pos)
             | SDMMC_CMD_WAIT_PRVDATA_COMPLETE_Msk
             | SDMMC_CMD_START_CMD_Msk;

    /* CMD0 is the first command after power-up: prepend the 74+ initialization
     * clocks the card needs before it will accept any command. */
    if (cmd->opcode == 0)
        cmdReg |= SDMMC_CMD_SEND_INITIALIZATION_Msk;

    switch (cmd->responseType) {
    case WHAL_SDHC_RSP_NONE:
        break;
    case WHAL_SDHC_RSP_R2:
        cmdReg |= SDMMC_CMD_RESPONSE_EXPECT_Msk
                  | SDMMC_CMD_RESPONSE_LENGTH_Msk
                  | SDMMC_CMD_CHECK_RESPONSE_CRC_Msk;
        longResp = 1;
        break;
    case WHAL_SDHC_RSP_R3:
    case WHAL_SDHC_RSP_R4:
        cmdReg |= SDMMC_CMD_RESPONSE_EXPECT_Msk;
        break;
    default:
        cmdReg |= SDMMC_CMD_RESPONSE_EXPECT_Msk
                  | SDMMC_CMD_CHECK_RESPONSE_CRC_Msk;
        break;
    }

    if (data) {
        if (!data->data || data->blockSize == 0 || data->blockCount == 0)
            return WHAL_EINVAL;

        cmdReg |= SDMMC_CMD_DATA_EXPECTED_Msk;
        if (data->dir == WHAL_SDHC_DATA_WRITE)
            cmdReg |= SDMMC_CMD_READ_WRITE_Msk;

        data->bytesXfered = 0;
        whal_Reg_Write(base, SDMMC_BLKSIZ_REG, data->blockSize);
        whal_Reg_Write(base, SDMMC_BYTCNT_REG,
                       data->blockSize * data->blockCount);
    }

    whal_Reg_Write(base, SDMMC_RINTSTS_REG, 0xFFFFFFFFu);
    whal_Reg_Write(base, SDMMC_CMDARG_REG, cmd->arg);
    whal_Reg_Write(base, SDMMC_CMD_REG, cmdReg);

    err = whal_Reg_ReadPoll(base, SDMMC_RINTSTS_REG,
                            SDMMC_RINTSTS_CDONE_Msk,
                            SDMMC_RINTSTS_CDONE_Msk, cfg->timeout);
    if (err)
        return err;

    sts = whal_Reg_Read(base, SDMMC_RINTSTS_REG);
    if (sts & SDMMC_RINTSTS_RTO_Msk)
        return WHAL_ETIMEOUT;
    if ((sts & SDMMC_RINTSTS_RE_Msk)
        || ((cmdReg & SDMMC_CMD_CHECK_RESPONSE_CRC_Msk)
            && (sts & SDMMC_RINTSTS_RCRC_Msk)))
        return WHAL_EHARDWARE;

    if (cmd->responseType != WHAL_SDHC_RSP_NONE) {
        cmd->response[0] = whal_Reg_Read(base, SDMMC_RESP0_REG);
        if (longResp) {
            cmd->response[1] = whal_Reg_Read(base, SDMMC_RESP1_REG);
            cmd->response[2] = whal_Reg_Read(base, SDMMC_RESP2_REG);
            cmd->response[3] = whal_Reg_Read(base, SDMMC_RESP3_REG);
        }
    }

    if (!data)
        return WHAL_SUCCESS;

    buf = (uint8_t *)data->data;
    remaining = (size_t)data->blockSize * data->blockCount;
    isWrite = (data->dir == WHAL_SDHC_DATA_WRITE);

    WHAL_TIMEOUT_START(cfg->timeout);
    while (remaining) {
        sts = whal_Reg_Read(base, SDMMC_RINTSTS_REG);
        if (sts & SDMMC_RINTSTS_DRTO_Msk)
            return WHAL_ETIMEOUT;
        if (sts & (SDMMC_RINTSTS_DCRC_Msk | SDMMC_RINTSTS_SBE_Msk
                   | SDMMC_RINTSTS_EBE_Msk))
            return WHAL_EHARDWARE;

        fifo = whal_Reg_Read(base, SDMMC_STATUS_REG);
        if (isWrite) {
            while (remaining && !(fifo & SDMMC_STATUS_FIFO_FULL_Msk)) {
                word = 0;
                for (b = 0; b < 4 && remaining; b++, remaining--)
                    word |= (uint32_t)(*buf++) << (8 * b);
                whal_Reg_Write(base, SDMMC_FIFO_REG, word);
                fifo = whal_Reg_Read(base, SDMMC_STATUS_REG);
            }
        } else {
            while (remaining && !(fifo & SDMMC_STATUS_FIFO_EMPTY_Msk)) {
                word = whal_Reg_Read(base, SDMMC_FIFO_REG);
                for (b = 0; b < 4 && remaining; b++, remaining--)
                    *buf++ = (uint8_t)(word >> (8 * b));
                fifo = whal_Reg_Read(base, SDMMC_STATUS_REG);
            }
        }

        if (WHAL_TIMEOUT_EXPIRED(cfg->timeout))
            return WHAL_ETIMEOUT;
    }

    err = whal_Reg_ReadPoll(base, SDMMC_RINTSTS_REG,
                            SDMMC_RINTSTS_DTO_Msk,
                            SDMMC_RINTSTS_DTO_Msk, cfg->timeout);
    if (err)
        return err;

    sts = whal_Reg_Read(base, SDMMC_RINTSTS_REG);
    if (sts & SDMMC_RINTSTS_DRTO_Msk)
        return WHAL_ETIMEOUT;
    if (sts & (SDMMC_RINTSTS_DCRC_Msk | SDMMC_RINTSTS_SBE_Msk
               | SDMMC_RINTSTS_EBE_Msk))
        return WHAL_EHARDWARE;

    data->bytesXfered = (size_t)data->blockSize * data->blockCount;
    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_LPC55S6X_SDHC_DIRECT_API_MAPPING
const whal_SdhcDriver whal_Lpc55s6x_Sdhc_Driver = {
    .Init         = whal_Lpc55s6x_Sdhc_Init,
    .Deinit       = whal_Lpc55s6x_Sdhc_Deinit,
    .ConfigureCom = whal_Lpc55s6x_Sdhc_ConfigureCom,
    .Request      = whal_Lpc55s6x_Sdhc_Request,
};
#endif /* !WHAL_CFG_LPC55S6X_SDHC_DIRECT_API_MAPPING */
