/* stm32f4_eth.h
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

#ifndef WHAL_STM32F4_ETH_H
#define WHAL_STM32F4_ETH_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/eth/eth.h>
#include <wolfHAL/timeout.h>

/**
 * @file stm32f4_eth.h
 * @brief STM32F4 Ethernet MAC driver (legacy 10/100 Synopsys MAC).
 *
 * The STM32F4 Ethernet peripheral is a 10/100 Mbps Synopsys MAC with an
 * integrated DMA controller and separate MAC / DMA register blocks at fixed
 * offsets within the peripheral (RM0090 §33.8):
 *   MAC block @ ETH_BASE + 0x0000
 *   MMC block @ ETH_BASE + 0x0100
 *   PTP block @ ETH_BASE + 0x0700
 *   DMA block @ ETH_BASE + 0x1000
 *
 * This is the *legacy* Synopsys IP — not register-compatible with the H5/N6
 * Ethernet MAC. It uses 16-byte (non-enhanced) DMA descriptors in ring mode:
 * TDES0/1/2/3 for TX, RDES0/1/2/3 for RX.
 *
 * The driver brings the MAC up at 10/100 Mbps full or half duplex over RMII
 * or MII. RMII vs MII selection is done at the SYSCFG.PMC level by the board
 * before calling Init. Optional features (IEEE 1588 PTP timestamping, MMC
 * counter readout, hardware checksum offload, double-buffer rings) are not
 * implemented in this first port.
 */

/** Non-enhanced TX descriptor (4 words = 16 bytes). */
typedef struct {
    volatile uint32_t des[4];
} whal_Stm32f4_Eth_TxDesc;

/** Non-enhanced RX descriptor (4 words = 16 bytes). */
typedef struct {
    volatile uint32_t des[4];
} whal_Stm32f4_Eth_RxDesc;

/**
 * @brief STM32F4 Ethernet MAC configuration.
 *
 * The board owns the descriptor rings and frame buffers and passes them in
 * through this config. MDIO clock divider selection is driven by @p hclkHz
 * (the AHB1 frequency that clocks the ETH MAC) — the driver maps it to the
 * CR field per RM0090 §33.8.1.
 */
typedef struct whal_Stm32f4_Eth_Cfg {
    whal_Stm32f4_Eth_TxDesc *txDescs;
    uint8_t                 *txBufs;
    size_t                   txDescCount;
    size_t                   txBufSize;
    whal_Stm32f4_Eth_RxDesc *rxDescs;
    uint8_t                 *rxBufs;
    size_t                   rxDescCount;
    size_t                   rxBufSize;
    uint32_t                 hclkHz;       /* AHB1 clock in Hz, drives CR */
    whal_Timeout            *timeout;
} whal_Stm32f4_Eth_Cfg;

/**
 * @brief Platform-owned Ethernet device singleton. Defined in the driver TU
 *        from the WHAL_CFG_STM32F4_ETH_DEV initializer in board.h.
 */
extern const whal_Eth whal_Stm32f4_Eth_Dev;

#ifndef WHAL_CFG_STM32F4_ETH_DIRECT_API_MAPPING
/** Driver vtable. */
extern const whal_EthDriver whal_Stm32f4_Eth_Driver;

/**
 * @brief Initialize the STM32F4 Ethernet MAC and DMA.
 *
 * Performs DMA software reset, programs DMABMR (burst length, fixed-burst,
 * address-aligned), initializes the TX/RX descriptor rings in ring mode
 * (TER/RER set on the last descriptor), programs MACA0HR/MACA0LR with the
 * MAC address from @p ethDev->macAddr, and clears stale DMASR bits. Does
 * not start TX/RX.
 */
whal_Error whal_Stm32f4_Eth_Init(whal_Eth *ethDev);

/** Reset the DMA controller and leave the MAC idle. */
whal_Error whal_Stm32f4_Eth_Deinit(whal_Eth *ethDev);

/**
 * @brief Apply link speed/duplex to MACCR and start the MAC + DMA engines.
 *
 * @param ethDev Eth device.
 * @param speed  WHAL_ETH_SPEED_10 or WHAL_ETH_SPEED_100.
 * @param duplex WHAL_ETH_DUPLEX_HALF or WHAL_ETH_DUPLEX_FULL.
 */
whal_Error whal_Stm32f4_Eth_Start(whal_Eth *ethDev, uint8_t speed,
                                  uint8_t duplex);

/** Stop TX/RX in MACCR and clear ST/SR in DMAOMR. */
whal_Error whal_Stm32f4_Eth_Stop(whal_Eth *ethDev);

/** Copy @p frame into the next free TX descriptor and kick the TX poll demand. */
whal_Error whal_Stm32f4_Eth_Send(whal_Eth *ethDev, const void *frame,
                                 size_t len);

/** Pull one received frame off the RX ring (returns WHAL_ENOTREADY if none). */
whal_Error whal_Stm32f4_Eth_Recv(whal_Eth *ethDev, void *frame, size_t *len);

/** MDIO read via MACMIIAR/MIIDR. */
whal_Error whal_Stm32f4_Eth_MdioRead(whal_Eth *ethDev, uint8_t phyAddr,
                                     uint8_t reg, uint16_t *val);

/** MDIO write via MACMIIAR/MIIDR. */
whal_Error whal_Stm32f4_Eth_MdioWrite(whal_Eth *ethDev, uint8_t phyAddr,
                                      uint8_t reg, uint16_t val);
#endif /* !WHAL_CFG_STM32F4_ETH_DIRECT_API_MAPPING */

#endif /* WHAL_STM32F4_ETH_H */
