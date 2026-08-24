/* lpc55s6x_spi.h
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

#ifndef WHAL_LPC55S6X_SPI_H
#define WHAL_LPC55S6X_SPI_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/timeout.h>
#include <wolfHAL/bitops.h>

/*
 * @file lpc55s6x_spi.h
 * @brief LPC55S6x FLEXCOMM SPI driver configuration.
 *
 * Chip-select is caller-managed (software slave management); the driver drives
 * no hardware SSEL, so a session may span several SendRecv calls with the
 * caller holding CS (e.g. a GPIO) across them.
 */

/*
 * SPI CFG register (offset 0x400) bit fields.
 */
#define WHAL_LPC55S6X_SPI_CFG_ENABLE_Pos    0
#define WHAL_LPC55S6X_SPI_CFG_ENABLE_Msk    (1UL << WHAL_LPC55S6X_SPI_CFG_ENABLE_Pos)
#define WHAL_LPC55S6X_SPI_CFG_MASTER_Pos    2
#define WHAL_LPC55S6X_SPI_CFG_MASTER_Msk    (1UL << WHAL_LPC55S6X_SPI_CFG_MASTER_Pos)
#define WHAL_LPC55S6X_SPI_CFG_LSBF_Pos      3
#define WHAL_LPC55S6X_SPI_CFG_LSBF_Msk      (1UL << WHAL_LPC55S6X_SPI_CFG_LSBF_Pos)
#define WHAL_LPC55S6X_SPI_CFG_CPHA_Pos      4
#define WHAL_LPC55S6X_SPI_CFG_CPHA_Msk      (1UL << WHAL_LPC55S6X_SPI_CFG_CPHA_Pos)
#define WHAL_LPC55S6X_SPI_CFG_CPOL_Pos      5
#define WHAL_LPC55S6X_SPI_CFG_CPOL_Msk      (1UL << WHAL_LPC55S6X_SPI_CFG_CPOL_Pos)
#define WHAL_LPC55S6X_SPI_CFG_LOOP_Pos      7
#define WHAL_LPC55S6X_SPI_CFG_LOOP_Msk      (1UL << WHAL_LPC55S6X_SPI_CFG_LOOP_Pos)
#define WHAL_LPC55S6X_SPI_CFG_SPOL0_Pos     8
#define WHAL_LPC55S6X_SPI_CFG_SPOL0_Msk     (1UL << WHAL_LPC55S6X_SPI_CFG_SPOL0_Pos)
#define WHAL_LPC55S6X_SPI_CFG_SPOL1_Pos     9
#define WHAL_LPC55S6X_SPI_CFG_SPOL1_Msk     (1UL << WHAL_LPC55S6X_SPI_CFG_SPOL1_Pos)
#define WHAL_LPC55S6X_SPI_CFG_SPOL2_Pos     10
#define WHAL_LPC55S6X_SPI_CFG_SPOL2_Msk     (1UL << WHAL_LPC55S6X_SPI_CFG_SPOL2_Pos)
#define WHAL_LPC55S6X_SPI_CFG_SPOL3_Pos     11
#define WHAL_LPC55S6X_SPI_CFG_SPOL3_Msk     (1UL << WHAL_LPC55S6X_SPI_CFG_SPOL3_Pos)

/*
 * @brief Build an SPI CFG register word.
 *
 * ENABLE, CPOL, and CPHA are left clear; the driver sets ENABLE in StartCom
 * and CPOL/CPHA from whal_Spi_ComCfg.mode each session.
 *
 * @param master Mode select: 0 slave, 1 master.
 * @param lsbf   Data order: 0 MSB first, 1 LSB first.
 * @param loop   Loopback mode enable.
 * @param spol0  SSEL0 polarity: 0 active low, 1 active high.
 * @param spol1  SSEL1 polarity: 0 active low, 1 active high.
 * @param spol2  SSEL2 polarity: 0 active low, 1 active high.
 * @param spol3  SSEL3 polarity: 0 active low, 1 active high.
 */
#define WHAL_LPC55S6X_SPI_CFG(master, lsbf, loop, \
                              spol0, spol1, spol2, spol3) ( \
    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_MASTER_Msk, WHAL_LPC55S6X_SPI_CFG_MASTER_Pos, (master)) | \
    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_LSBF_Msk,   WHAL_LPC55S6X_SPI_CFG_LSBF_Pos,   (lsbf))   | \
    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_LOOP_Msk,   WHAL_LPC55S6X_SPI_CFG_LOOP_Pos,   (loop))   | \
    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_SPOL0_Msk,  WHAL_LPC55S6X_SPI_CFG_SPOL0_Pos,  (spol0))  | \
    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_SPOL1_Msk,  WHAL_LPC55S6X_SPI_CFG_SPOL1_Pos,  (spol1))  | \
    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_SPOL2_Msk,  WHAL_LPC55S6X_SPI_CFG_SPOL2_Pos,  (spol2))  | \
    whal_SetBits(WHAL_LPC55S6X_SPI_CFG_SPOL3_Msk,  WHAL_LPC55S6X_SPI_CFG_SPOL3_Pos,  (spol3)))

/*
 * @brief LPC55S6x SPI configuration parameters.
 */
typedef struct whal_Lpc55s6x_Spi_Cfg {
    uint32_t fclkHz;      /* FLEXCOMM function clock (FCLK) frequency, for DIV */
    uint32_t cfgReg;      /* Packed CFG register word (WHAL_LPC55S6X_SPI_CFG) */
    uint8_t  _wordSz;      /* Session frame size in bits, set by StartCom */
    whal_Timeout *timeout;
} whal_Lpc55s6x_Spi_Cfg;

/*
 * @brief Single-instance device struct. Defined in the driver TU
 * from the WHAL_CFG_LPC55S6X_SPI_DEV initializer in board.h.
 */
#if defined(WHAL_CFG_LPC55S6X_SPI_SINGLE_INSTANCE)
extern const whal_Spi whal_Lpc55s6x_Spi_Dev;
#endif

#ifndef WHAL_CFG_LPC55S6X_SPI_DIRECT_API_MAPPING
/*
 * @brief Driver instance for LPC55S6x SPI peripheral.
 */
extern const whal_SpiDriver whal_Lpc55s6x_Spi_Driver;

/*
 * @brief Initialize the LPC55S6x SPI peripheral.
 *
 * @param spiDev SPI device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Spi_Init(whal_Spi *spiDev);

/*
 * @brief Deinitialize the LPC55S6x SPI peripheral.
 *
 * @param spiDev SPI device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Spi_Deinit(whal_Spi *spiDev);

/*
 * @brief Begin a communication session on the LPC55S6x SPI peripheral.
 *
 * @param spiDev SPI device instance.
 * @param comCfg Per-session communication parameters.
 *
 * @retval WHAL_SUCCESS Communication session started.
 * @retval WHAL_EINVAL  Invalid arguments.
 * @retval WHAL_ENOTSUP Word size, data-line count, or frequency not achievable by this driver.
 */
whal_Error whal_Lpc55s6x_Spi_StartCom(whal_Spi *spiDev, whal_Spi_ComCfg *comCfg);

/*
 * @brief End the current communication session on the LPC55S6x SPI peripheral.
 *
 * @param spiDev SPI device instance.
 *
 * @retval WHAL_SUCCESS Communication session ended.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Spi_EndCom(whal_Spi *spiDev);

/*
 * @brief Perform a bidirectional SPI transfer.
 *
 * @param spiDev SPI device instance.
 * @param tx     Data to transmit (may be NULL).
 * @param txLen  Number of bytes to transmit.
 * @param rx     Receive buffer (may be NULL).
 * @param rxLen  Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS  Transfer completed.
 * @retval WHAL_EINVAL   Invalid arguments.
 * @retval WHAL_ETIMEOUT Timed out waiting on the FIFO or master-idle status.
 */
whal_Error whal_Lpc55s6x_Spi_SendRecv(whal_Spi *spiDev,
                                      const void *tx, size_t txLen,
                                      void *rx, size_t rxLen);
#endif /* !WHAL_CFG_LPC55S6X_SPI_DIRECT_API_MAPPING */

#endif /* WHAL_LPC55S6X_SPI_H */
