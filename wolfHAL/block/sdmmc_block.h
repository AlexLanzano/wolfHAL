/* sdmmc_block.h
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

#ifndef WHAL_SDMMC_H
#define WHAL_SDMMC_H

#include <stddef.h>
#include <stdint.h>
#include <wolfHAL/block/block.h>
#include <wolfHAL/sdhc/sdhc.h>
#include <wolfHAL/timeout.h>

/*
 * @file sdmmc_block.h
 * @brief SD card driver over the native SDMMC host controller.
 *
 * Implements the whal_Block interface for SD cards using the native SD
 * protocol over an sdhc transport (1/4-bit bus). Supports SDHC and SDXC
 * cards with 512-byte block addressing. This is the native-mode sibling of
 * the SD/SPI block driver (sdhc_spi_block).
 *
 * The driver handles:
 * - Card enumeration (CMD0, CMD8, ACMD41, CMD2, CMD3, CMD7)
 * - Single and multi-block reads (CMD17, CMD18)
 * - Single and multi-block writes (CMD24, CMD25)
 * - Block range erase (CMD32, CMD33, CMD38)
 */

#define WHAL_SDMMC_BLOCK_SZ 512

/*
 * @brief Configuration for the SDMMC block driver.
 */
typedef struct whal_Sdmmc_Cfg {
    whal_Sdhc *sdhc;         /* SDHC transport device */
    whal_Timeout *timeout;   /* Timeout for the ACMD41 ready poll */
    uint16_t _rca;           /* driver: card relative address (CMD3) */
    uint8_t _highCapacity;   /* driver: 1 if SDHC/SDXC (block addressing) */
    uint32_t _blockCount;    /* driver: capacity in 512-byte blocks (CSD) */
} whal_Sdmmc_Cfg;

/*
 * @brief Single-instance device struct. Defined in the driver TU
 * from the WHAL_CFG_SDMMC_DEV initializer in board.h.
 */
#ifdef WHAL_CFG_SDMMC_SINGLE_INSTANCE
extern const whal_Block whal_Sdmmc_Dev;
#endif

#ifndef WHAL_CFG_SDMMC_BLOCK_DIRECT_API_MAPPING
/*
 * @brief Driver instance for SD cards over the native SDMMC controller.
 */
extern const whal_BlockDriver whal_Sdmmc_Driver;

/*
 * @brief Enumerate and initialize the SD card over the SDMMC transport
 *        (CMD0, CMD8, ACMD41, CMD2, CMD3, CMD7). Caller must have already
 *        initialized the sdhc transport.
 *
 * @param blockDev Block device instance.
 *
 * @retval WHAL_SUCCESS   Card initialized and ready.
 * @retval WHAL_EINVAL    Null pointer or missing cfg.
 * @retval WHAL_ETIMEOUT  Card did not become ready within the configured timeout.
 * @retval WHAL_EHARDWARE Card returned an unexpected response or is unsupported.
 */
whal_Error whal_Sdmmc_Init(whal_Block *blockDev);

/*
 * @brief Deinitialize the SD card driver.
 *
 * @param blockDev Block device instance.
 *
 * @retval WHAL_SUCCESS Driver is deinitialized.
 * @retval WHAL_EINVAL  Null pointer.
 */
whal_Error whal_Sdmmc_Deinit(whal_Block *blockDev);

/*
 * @brief Read `blockCount` 512-byte blocks starting at `block` into `data`
 *        (CMD17 single, CMD18 multi).
 *
 * @param blockDev   Block device instance.
 * @param block      First block index.
 * @param data       Destination buffer (`blockCount * 512` bytes).
 * @param blockCount Number of 512-byte blocks to read.
 *
 * @retval WHAL_SUCCESS   Read completed.
 * @retval WHAL_EINVAL    Null pointer.
 * @retval WHAL_ETIMEOUT  Card did not respond within the configured timeout.
 * @retval WHAL_EHARDWARE CRC or read-error response.
 */
whal_Error whal_Sdmmc_Read(whal_Block *blockDev, uint32_t block,
                           void *data, uint32_t blockCount);

/*
 * @brief Write `blockCount` 512-byte blocks starting at `block` from `data`
 *        (CMD24 single, CMD25 multi).
 *
 * @param blockDev   Block device instance.
 * @param block      First block index.
 * @param data       Source buffer (`blockCount * 512` bytes).
 * @param blockCount Number of 512-byte blocks to write.
 *
 * @retval WHAL_SUCCESS   Write completed.
 * @retval WHAL_EINVAL    Null pointer.
 * @retval WHAL_ETIMEOUT  Card did not respond within the configured timeout.
 * @retval WHAL_EHARDWARE Programming error response.
 */
whal_Error whal_Sdmmc_Write(whal_Block *blockDev, uint32_t block,
                            const void *data, uint32_t blockCount);

/*
 * @brief Erase a range of blocks via CMD32/CMD33/CMD38.
 *
 * @param blockDev   Block device instance.
 * @param block      First block index.
 * @param blockCount Number of 512-byte blocks to erase.
 *
 * @retval WHAL_SUCCESS   Erase completed.
 * @retval WHAL_EINVAL    Null pointer.
 * @retval WHAL_ETIMEOUT  Erase did not complete within the configured timeout.
 * @retval WHAL_EHARDWARE Erase error response.
 */
whal_Error whal_Sdmmc_Erase(whal_Block *blockDev, uint32_t block,
                            uint32_t blockCount);
#endif /* !WHAL_CFG_SDMMC_BLOCK_DIRECT_API_MAPPING */

#endif /* WHAL_SDMMC_H */
