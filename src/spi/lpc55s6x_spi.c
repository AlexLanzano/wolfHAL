/* lpc55s6x_spi.c
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
#ifdef WHAL_CFG_LPC55S6X_SPI_SINGLE_INSTANCE
#include "board.h"  /* provides whal_Lpc55s6x_Spi_Dev device instance (possibly via platform alias macro) */
#endif
#include <wolfHAL/spi/lpc55s6x_spi.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>

/* FLEXCOMM peripheral-select register (selects USART/SPI/I2C for this instance). */
#define FLEXCOMM_PSELID_REG        0xFF8
#define FLEXCOMM_PSELID_PERSEL_Pos 0
#define FLEXCOMM_PSELID_PERSEL_Msk (WHAL_BITMASK(3) << FLEXCOMM_PSELID_PERSEL_Pos)
#define FLEXCOMM_PSELID_PERSEL_SPI 0x2

/* SPI registers (offsets within the FLEXCOMM address space). */
#define SPI_CFG_REG 0x400

#define SPI_STAT_REG         0x408
#define SPI_STAT_MSTIDLE_Msk (1UL << 8)

#define SPI_DIV_REG        0x424
#define SPI_DIV_DIVVAL_Pos 0
#define SPI_DIV_DIVVAL_Msk (WHAL_BITMASK(16) << SPI_DIV_DIVVAL_Pos)

#define SPI_FIFOCFG_REG          0xE00
#define SPI_FIFOCFG_ENABLETX_Msk (1UL << 0)
#define SPI_FIFOCFG_ENABLERX_Msk (1UL << 1)
#define SPI_FIFOCFG_EMPTYTX_Msk  (1UL << 16)
#define SPI_FIFOCFG_EMPTYRX_Msk  (1UL << 17)

#define SPI_FIFOSTAT_REG            0xE04
#define SPI_FIFOSTAT_TXNOTFULL_Msk  (1UL << 5)
#define SPI_FIFOSTAT_RXNOTEMPTY_Msk (1UL << 6)

#define SPI_FIFOWR_REG          0xE20
#define SPI_FIFOWR_TXDATA_Pos   0
#define SPI_FIFOWR_TXDATA_Msk   (WHAL_BITMASK(16) << SPI_FIFOWR_TXDATA_Pos)
#define SPI_FIFOWR_TXSSEL_N_Pos 16
#define SPI_FIFOWR_TXSSEL_N_Msk (WHAL_BITMASK(4) << SPI_FIFOWR_TXSSEL_N_Pos)
#define SPI_FIFOWR_EOT_Msk      (1UL << 20)
#define SPI_FIFOWR_LEN_Pos      24
#define SPI_FIFOWR_LEN_Msk      (WHAL_BITMASK(4) << SPI_FIFOWR_LEN_Pos)

#define SPI_FIFORD_REG        0xE30
#define SPI_FIFORD_RXDATA_Pos 0
#define SPI_FIFORD_RXDATA_Msk (WHAL_BITMASK(16) << SPI_FIFORD_RXDATA_Pos)

#ifdef WHAL_CFG_LPC55S6X_SPI_DIRECT_API_MAPPING
#define whal_Lpc55s6x_Spi_Init     whal_Spi_Init
#define whal_Lpc55s6x_Spi_Deinit   whal_Spi_Deinit
#define whal_Lpc55s6x_Spi_StartCom whal_Spi_StartCom
#define whal_Lpc55s6x_Spi_EndCom   whal_Spi_EndCom
#define whal_Lpc55s6x_Spi_SendRecv whal_Spi_SendRecv
#endif /* WHAL_CFG_LPC55S6X_SPI_DIRECT_API_MAPPING */

#ifdef WHAL_CFG_LPC55S6X_SPI_SINGLE_INSTANCE
const whal_Spi whal_Lpc55s6x_Spi_Dev = WHAL_CFG_LPC55S6X_SPI_DEV;
#endif

whal_Error whal_Lpc55s6x_Spi_Init(whal_Spi *spiDev)
{
#ifdef WHAL_CFG_LPC55S6X_SPI_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Spi_Dev.base;
    whal_Lpc55s6x_Spi_Cfg *cfg =
        (whal_Lpc55s6x_Spi_Cfg *)whal_Lpc55s6x_Spi_Dev.cfg;
    (void)spiDev;
#else
    size_t base;
    whal_Lpc55s6x_Spi_Cfg *cfg;

    if (!spiDev || !spiDev->cfg)
        return WHAL_EINVAL;

    base = spiDev->base;
    cfg = (whal_Lpc55s6x_Spi_Cfg *)spiDev->cfg;
#endif

    whal_Reg_Update(base, FLEXCOMM_PSELID_REG, FLEXCOMM_PSELID_PERSEL_Msk,
                    whal_SetBits(FLEXCOMM_PSELID_PERSEL_Msk,
                                 FLEXCOMM_PSELID_PERSEL_Pos,
                                 FLEXCOMM_PSELID_PERSEL_SPI));

    whal_Reg_Write(base, SPI_CFG_REG, cfg->cfgReg);

    whal_Reg_Update(base, SPI_FIFOCFG_REG,
                    SPI_FIFOCFG_ENABLETX_Msk | SPI_FIFOCFG_ENABLERX_Msk,
                    SPI_FIFOCFG_ENABLETX_Msk | SPI_FIFOCFG_ENABLERX_Msk);

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Spi_Deinit(whal_Spi *spiDev)
{
#ifdef WHAL_CFG_LPC55S6X_SPI_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Spi_Dev.base;
    (void)spiDev;
#else
    size_t base;

    if (!spiDev)
        return WHAL_EINVAL;

    base = spiDev->base;
#endif

    whal_Reg_Write(base, SPI_CFG_REG, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Spi_StartCom(whal_Spi *spiDev, whal_Spi_ComCfg *comCfg)
{
    uint32_t cpol, cpha, divval;
#ifdef WHAL_CFG_LPC55S6X_SPI_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Spi_Dev.base;
    whal_Lpc55s6x_Spi_Cfg *cfg =
        (whal_Lpc55s6x_Spi_Cfg *)whal_Lpc55s6x_Spi_Dev.cfg;
    (void)spiDev;

    if (!comCfg)
        return WHAL_EINVAL;
#else
    size_t base;
    whal_Lpc55s6x_Spi_Cfg *cfg;

    if (!spiDev || !spiDev->cfg || !comCfg)
        return WHAL_EINVAL;

    base = spiDev->base;
    cfg = (whal_Lpc55s6x_Spi_Cfg *)spiDev->cfg;
#endif

    if (comCfg->mode > 3 || comCfg->freq == 0)
        return WHAL_EINVAL;

    if (comCfg->dataLines != 1 || comCfg->wordSz < 4 || comCfg->wordSz > 16 ||
        comCfg->freq > cfg->fclkHz)
        return WHAL_ENOTSUP;

    divval = (cfg->fclkHz + comCfg->freq - 1) / comCfg->freq - 1;
    if (divval > (SPI_DIV_DIVVAL_Msk >> SPI_DIV_DIVVAL_Pos))
        return WHAL_ENOTSUP;

    cfg->_wordSz = comCfg->wordSz;
    cpol = (comCfg->mode >> 1) & 1;
    cpha = comCfg->mode & 1;

    whal_Reg_Update(base, SPI_CFG_REG, WHAL_LPC55S6X_SPI_CFG_ENABLE_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_ENABLE_Msk,
                                 WHAL_LPC55S6X_SPI_CFG_ENABLE_Pos, 0));

    whal_Reg_Update(base, SPI_FIFOCFG_REG,
                    SPI_FIFOCFG_EMPTYTX_Msk | SPI_FIFOCFG_EMPTYRX_Msk,
                    SPI_FIFOCFG_EMPTYTX_Msk | SPI_FIFOCFG_EMPTYRX_Msk);

    whal_Reg_Update(base, SPI_CFG_REG,
                    WHAL_LPC55S6X_SPI_CFG_CPOL_Msk | WHAL_LPC55S6X_SPI_CFG_CPHA_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_CPOL_Msk,
                                 WHAL_LPC55S6X_SPI_CFG_CPOL_Pos, cpol) |
                    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_CPHA_Msk,
                                 WHAL_LPC55S6X_SPI_CFG_CPHA_Pos, cpha));

    whal_Reg_Update(base, SPI_DIV_REG, SPI_DIV_DIVVAL_Msk,
                    whal_SetBits(SPI_DIV_DIVVAL_Msk, SPI_DIV_DIVVAL_Pos, divval));

    whal_Reg_Update(base, SPI_CFG_REG, WHAL_LPC55S6X_SPI_CFG_ENABLE_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_ENABLE_Msk,
                                 WHAL_LPC55S6X_SPI_CFG_ENABLE_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Spi_EndCom(whal_Spi *spiDev)
{
#ifdef WHAL_CFG_LPC55S6X_SPI_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Spi_Dev.base;
    (void)spiDev;
#else
    size_t base;

    if (!spiDev)
        return WHAL_EINVAL;

    base = spiDev->base;
#endif

    whal_Reg_Update(base, SPI_CFG_REG, WHAL_LPC55S6X_SPI_CFG_ENABLE_Msk,
                    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_ENABLE_Msk,
                                 WHAL_LPC55S6X_SPI_CFG_ENABLE_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Lpc55s6x_Spi_SendRecv(whal_Spi *spiDev,
                                      const void *tx, size_t txLen,
                                      void *rx, size_t rxLen)
{
    const uint8_t *txBuf = (const uint8_t *)tx;
    uint8_t *rxBuf = (uint8_t *)rx;
    size_t totalLen;
    uint8_t frameBytes;
    uint32_t len;
    whal_Error err;
#ifdef WHAL_CFG_LPC55S6X_SPI_SINGLE_INSTANCE
    size_t base = whal_Lpc55s6x_Spi_Dev.base;
    whal_Lpc55s6x_Spi_Cfg *cfg =
        (whal_Lpc55s6x_Spi_Cfg *)whal_Lpc55s6x_Spi_Dev.cfg;
    (void)spiDev;

    if ((!tx && txLen) || (!rx && rxLen))
        return WHAL_EINVAL;
#else
    size_t base;
    whal_Lpc55s6x_Spi_Cfg *cfg;

    if (!spiDev || !spiDev->cfg || (!tx && txLen) || (!rx && rxLen))
        return WHAL_EINVAL;

    base = spiDev->base;
    cfg = (whal_Lpc55s6x_Spi_Cfg *)spiDev->cfg;
#endif

    frameBytes = (cfg->_wordSz > 8) ? 2 : 1;
    len = (uint32_t)cfg->_wordSz - 1;
    totalLen = txLen > rxLen ? txLen : rxLen;

    if (frameBytes == 2 && ((txLen & 1) || (rxLen & 1)))
        return WHAL_EINVAL;

    for (size_t i = 0; i < totalLen; i += frameBytes) {
        uint16_t txData;
        uint32_t rd;
        uint32_t wr;

        if (frameBytes == 2) {
            uint8_t lo = (txBuf && i < txLen) ? txBuf[i] : 0xFF;
            uint8_t hi = (txBuf && (i + 1) < txLen) ? txBuf[i + 1] : 0xFF;
            txData = (uint16_t)((uint16_t)hi << 8 | lo);
        } else {
            txData = (txBuf && i < txLen) ? txBuf[i] : 0xFF;
        }

        wr = whal_SetBits(SPI_FIFOWR_TXDATA_Msk, SPI_FIFOWR_TXDATA_Pos, txData) |
             SPI_FIFOWR_TXSSEL_N_Msk |
             whal_SetBits(SPI_FIFOWR_LEN_Msk, SPI_FIFOWR_LEN_Pos, len);

        if (i + frameBytes >= totalLen)
            wr |= SPI_FIFOWR_EOT_Msk;

        err = whal_Reg_ReadPoll(base, SPI_FIFOSTAT_REG,
                                SPI_FIFOSTAT_TXNOTFULL_Msk,
                                SPI_FIFOSTAT_TXNOTFULL_Msk, cfg->timeout);
        if (err)
            return err;

        whal_Reg_Write(base, SPI_FIFOWR_REG, wr);

        err = whal_Reg_ReadPoll(base, SPI_FIFOSTAT_REG,
                                SPI_FIFOSTAT_RXNOTEMPTY_Msk,
                                SPI_FIFOSTAT_RXNOTEMPTY_Msk, cfg->timeout);
        if (err)
            return err;

        rd = whal_GetBits(SPI_FIFORD_RXDATA_Msk, SPI_FIFORD_RXDATA_Pos,
                          whal_Reg_Read(base, SPI_FIFORD_REG));

        if (rxBuf && i < rxLen) {
            rxBuf[i] = (uint8_t)rd;
            if (frameBytes == 2)
                rxBuf[i + 1] = (uint8_t)(rd >> 8);
        }
    }

    return whal_Reg_ReadPoll(base, SPI_STAT_REG, SPI_STAT_MSTIDLE_Msk,
                             SPI_STAT_MSTIDLE_Msk, cfg->timeout);
}

#ifndef WHAL_CFG_LPC55S6X_SPI_DIRECT_API_MAPPING
const whal_SpiDriver whal_Lpc55s6x_Spi_Driver = {
    .Init = whal_Lpc55s6x_Spi_Init,
    .Deinit = whal_Lpc55s6x_Spi_Deinit,
    .StartCom = whal_Lpc55s6x_Spi_StartCom,
    .EndCom = whal_Lpc55s6x_Spi_EndCom,
    .SendRecv = whal_Lpc55s6x_Spi_SendRecv,
};
#endif /* !WHAL_CFG_LPC55S6X_SPI_DIRECT_API_MAPPING */
