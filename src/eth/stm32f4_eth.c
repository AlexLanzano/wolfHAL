/* stm32f4_eth.c
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
#include "board.h"  /* provides WHAL_CFG_STM32F4_ETH_DEV initializer */
#include <wolfHAL/eth/stm32f4_eth.h>
#include <wolfHAL/eth/eth.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>

const whal_Eth whal_Stm32f4_Eth_Dev = WHAL_CFG_STM32F4_ETH_DEV;

/*
 * STM32F4 ETH register map (RM0090 §33.8). Offsets are absolute from
 * ETH_BASE — the MAC block starts at 0x0000, the DMA block at 0x1000.
 */

/* --- MAC registers --- */
#define ETH_MACCR_REG          0x0000
#define ETH_MACCR_RE_Pos       2     /* receive enable */
#define ETH_MACCR_RE_Msk       (1UL << ETH_MACCR_RE_Pos)
#define ETH_MACCR_TE_Pos       3     /* transmit enable */
#define ETH_MACCR_TE_Msk       (1UL << ETH_MACCR_TE_Pos)
#define ETH_MACCR_DM_Pos       11    /* duplex mode */
#define ETH_MACCR_DM_Msk       (1UL << ETH_MACCR_DM_Pos)
#define ETH_MACCR_LM_Pos       12    /* loopback */
#define ETH_MACCR_LM_Msk       (1UL << ETH_MACCR_LM_Pos)
#define ETH_MACCR_FES_Pos      14    /* fast Ethernet speed (100 Mbps) */
#define ETH_MACCR_FES_Msk      (1UL << ETH_MACCR_FES_Pos)

#define ETH_MACFFR_REG         0x0004

#define ETH_MACMIIAR_REG       0x0010
#define ETH_MACMIIAR_MB_Pos    0     /* MII busy */
#define ETH_MACMIIAR_MB_Msk    (1UL << ETH_MACMIIAR_MB_Pos)
#define ETH_MACMIIAR_MW_Pos    1     /* MII write */
#define ETH_MACMIIAR_MW_Msk    (1UL << ETH_MACMIIAR_MW_Pos)
#define ETH_MACMIIAR_CR_Pos    2     /* clock range */
#define ETH_MACMIIAR_CR_Msk    (7UL << ETH_MACMIIAR_CR_Pos)
#define ETH_MACMIIAR_MR_Pos    6     /* MII register */
#define ETH_MACMIIAR_MR_Msk    (0x1FUL << ETH_MACMIIAR_MR_Pos)
#define ETH_MACMIIAR_PA_Pos    11    /* PHY address */
#define ETH_MACMIIAR_PA_Msk    (0x1FUL << ETH_MACMIIAR_PA_Pos)

#define ETH_MACMIIDR_REG       0x0014
#define ETH_MACMIIDR_MD_Msk    0xFFFFUL

#define ETH_MACA0HR_REG        0x0040
#define ETH_MACA0LR_REG        0x0044

/* --- DMA registers --- */
#define ETH_DMABMR_REG         0x1000
#define ETH_DMABMR_SR_Pos      0
#define ETH_DMABMR_SR_Msk      (1UL << ETH_DMABMR_SR_Pos)
#define ETH_DMABMR_DA_Pos      1
#define ETH_DMABMR_PBL_Pos     8
#define ETH_DMABMR_PBL_Msk     (0x3FUL << ETH_DMABMR_PBL_Pos)
#define ETH_DMABMR_FB_Pos      16
#define ETH_DMABMR_FB_Msk      (1UL << ETH_DMABMR_FB_Pos)
#define ETH_DMABMR_AAB_Pos     25
#define ETH_DMABMR_AAB_Msk     (1UL << ETH_DMABMR_AAB_Pos)

#define ETH_DMATPDR_REG        0x1004  /* transmit poll demand */
#define ETH_DMARPDR_REG        0x1008  /* receive poll demand */
#define ETH_DMARDLAR_REG       0x100C
#define ETH_DMATDLAR_REG       0x1010

#define ETH_DMASR_REG          0x1014

#define ETH_DMAOMR_REG         0x1018
#define ETH_DMAOMR_SR_Pos      1
#define ETH_DMAOMR_SR_Msk      (1UL << ETH_DMAOMR_SR_Pos)
#define ETH_DMAOMR_ST_Pos      13
#define ETH_DMAOMR_ST_Msk      (1UL << ETH_DMAOMR_ST_Pos)
#define ETH_DMAOMR_TSF_Pos     21
#define ETH_DMAOMR_TSF_Msk     (1UL << ETH_DMAOMR_TSF_Pos)
#define ETH_DMAOMR_RSF_Pos     25
#define ETH_DMAOMR_RSF_Msk     (1UL << ETH_DMAOMR_RSF_Pos)
#define ETH_DMAOMR_FTF_Pos     20
#define ETH_DMAOMR_FTF_Msk     (1UL << ETH_DMAOMR_FTF_Pos)

#define ETH_DMAIER_REG         0x101C

/* --- TX descriptor TDES0/TDES1/TDES2/TDES3 (RM0090 §33.6.7) --- */
#define TDES0_OWN              (1UL << 31)
#define TDES0_IC               (1UL << 30)  /* interrupt on completion */
#define TDES0_LS               (1UL << 29)  /* last segment */
#define TDES0_FS               (1UL << 28)  /* first segment */
#define TDES0_TCH              (1UL << 20)  /* second address chained */
#define TDES0_TER              (1UL << 21)  /* end of ring */

#define TDES1_TBS1_Msk         0x1FFFUL     /* buffer 1 size (bits 12:0) */

/* --- RX descriptor RDES0/RDES1/RDES2/RDES3 (RM0090 §33.6.8) --- */
#define RDES0_OWN              (1UL << 31)
#define RDES0_ES               (1UL << 15)  /* error summary */
#define RDES0_FL_Pos           16           /* frame length (bits 29:16) */
#define RDES0_FL_Msk           (0x3FFFUL << RDES0_FL_Pos)

#define RDES1_RCH              (1UL << 14)
#define RDES1_RER              (1UL << 15)
#define RDES1_RBS1_Msk         0x1FFFUL

/* Maps an AHB clock frequency to the MACMIIAR.CR encoding (RM0090 §33.8.1).
 *
 *   000: HCLK = 60–100 MHz   (MDC = HCLK / 42)
 *   001: HCLK = 100–150 MHz  (MDC = HCLK / 62)
 *   010: HCLK = 20–35 MHz    (MDC = HCLK / 16)
 *   011: HCLK = 35–60 MHz    (MDC = HCLK / 26)
 *   100: HCLK = 150–180 MHz  (MDC = HCLK / 102)
 *
 * Boards at 100 MHz fall in the 60-100 MHz band → CR=0; the F439 NUCLEO at
 * 100 MHz fits cleanly. */
static uint32_t MdioClockRange(uint32_t hclkHz)
{
    if (hclkHz >= 150000000U)
        return 4;
    if (hclkHz >= 100000000U)
        return 1;
    if (hclkHz >= 60000000U)
        return 0;
    if (hclkHz >= 35000000U)
        return 3;
    return 2;
}

/* Driver-internal ring tracking state. The const cfg lives in the singleton
 * above (initialized from board.h); the mutable head indices live here. */
static struct {
    size_t txHead;
    size_t rxHead;
} eth_state;

#ifdef WHAL_CFG_STM32F4_ETH_DIRECT_API_MAPPING
#define whal_Stm32f4_Eth_Init      whal_Eth_Init
#define whal_Stm32f4_Eth_Deinit    whal_Eth_Deinit
#define whal_Stm32f4_Eth_Start     whal_Eth_Start
#define whal_Stm32f4_Eth_Stop      whal_Eth_Stop
#define whal_Stm32f4_Eth_Send      whal_Eth_Send
#define whal_Stm32f4_Eth_Recv      whal_Eth_Recv
#define whal_Stm32f4_Eth_MdioRead  whal_Eth_MdioRead
#define whal_Stm32f4_Eth_MdioWrite whal_Eth_MdioWrite
#endif /* WHAL_CFG_STM32F4_ETH_DIRECT_API_MAPPING */

static whal_Error MdioPoll(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, ETH_MACMIIAR_REG,
                             ETH_MACMIIAR_MB_Msk, 0, timeout);
}

whal_Error whal_Stm32f4_Eth_Init(whal_Eth *ethDev)
{
    const whal_Stm32f4_Eth_Cfg *cfg =
        (const whal_Stm32f4_Eth_Cfg *)whal_Stm32f4_Eth_Dev.cfg;
    size_t base = whal_Stm32f4_Eth_Dev.base;
    whal_Error err;
    size_t i;
    (void)ethDev;

    /* DMA software reset (TRM §33.8.4) — self-clears once internal logic
     * has been reset across all clock domains. */
    whal_Reg_Update(base, ETH_DMABMR_REG, ETH_DMABMR_SR_Msk,
                    ETH_DMABMR_SR_Msk);
    err = whal_Reg_ReadPoll(base, ETH_DMABMR_REG, ETH_DMABMR_SR_Msk, 0,
                            cfg->timeout);
    if (err)
        return err;

    /* Bus mode: fixed burst, address-aligned, PBL=32. */
    whal_Reg_Write(base, ETH_DMABMR_REG,
                   ETH_DMABMR_FB_Msk | ETH_DMABMR_AAB_Msk |
                   whal_SetBits(ETH_DMABMR_PBL_Msk, ETH_DMABMR_PBL_Pos, 32));

    /* TX descriptor ring (non-enhanced, ring mode). Each TDES2 points at
     * its dedicated TX buffer; TDES3 is unused in ring mode. The last
     * descriptor gets TER (end of ring) so DMA wraps. OWN=0 — host-owned. */
    for (i = 0; i < cfg->txDescCount; i++) {
        cfg->txDescs[i].des[0] = (i == cfg->txDescCount - 1)
                                 ? TDES0_TER : 0;
        cfg->txDescs[i].des[1] = 0;
        cfg->txDescs[i].des[2] = (uintptr_t)(cfg->txBufs + i * cfg->txBufSize);
        cfg->txDescs[i].des[3] = 0;
    }

    /* RX descriptor ring. Each RDES2 points at its RX buffer; RDES1 sets
     * RBS1 to the buffer size and RER on the last. OWN=1 — DMA-owned, ready
     * to receive. */
    for (i = 0; i < cfg->rxDescCount; i++) {
        cfg->rxDescs[i].des[0] = RDES0_OWN;
        cfg->rxDescs[i].des[1] = (cfg->rxBufSize & RDES1_RBS1_Msk) |
                                 ((i == cfg->rxDescCount - 1) ? RDES1_RER : 0);
        cfg->rxDescs[i].des[2] = (uintptr_t)(cfg->rxBufs + i * cfg->rxBufSize);
        cfg->rxDescs[i].des[3] = 0;
    }

    /* Hand the rings off to the DMA. */
    whal_Reg_Write(base, ETH_DMATDLAR_REG, (uintptr_t)cfg->txDescs);
    whal_Reg_Write(base, ETH_DMARDLAR_REG, (uintptr_t)cfg->rxDescs);

    /* DMA operation mode: store-and-forward for both directions. */
    whal_Reg_Update(base, ETH_DMAOMR_REG,
                    ETH_DMAOMR_TSF_Msk | ETH_DMAOMR_RSF_Msk,
                    ETH_DMAOMR_TSF_Msk | ETH_DMAOMR_RSF_Msk);

    /* MAC frame filter: leave at reset (promiscuous off, no PAUSE handling
     * here — sufficient for unicast reception by MAC address). */
    whal_Reg_Write(base, ETH_MACFFR_REG, 0);

    /* MAC address register: MACA0LR holds bytes 0..3, MACA0HR holds 4..5.
     * Programming the MAC address in Init (per repo convention) and not on
     * each Send. */
    whal_Reg_Write(base, ETH_MACA0LR_REG,
                   ((uint32_t)whal_Stm32f4_Eth_Dev.macAddr[3] << 24) |
                   ((uint32_t)whal_Stm32f4_Eth_Dev.macAddr[2] << 16) |
                   ((uint32_t)whal_Stm32f4_Eth_Dev.macAddr[1] <<  8) |
                   ((uint32_t)whal_Stm32f4_Eth_Dev.macAddr[0]));
    whal_Reg_Write(base, ETH_MACA0HR_REG,
                   ((uint32_t)whal_Stm32f4_Eth_Dev.macAddr[5] << 8) |
                   ((uint32_t)whal_Stm32f4_Eth_Dev.macAddr[4]));

    /* Pre-load CR (MDIO clock range) so MDIO accesses work before Start. */
    whal_Reg_Update(base, ETH_MACMIIAR_REG, ETH_MACMIIAR_CR_Msk,
                    whal_SetBits(ETH_MACMIIAR_CR_Msk, ETH_MACMIIAR_CR_Pos,
                                 MdioClockRange(cfg->hclkHz)));

    /* Clear DMASR by writing 1 to all RW1C bits we care about — keeps the
     * status register starting from a clean slate so future polls work. */
    whal_Reg_Write(base, ETH_DMASR_REG, 0xFFFFFFFFUL);

    eth_state.txHead = 0;
    eth_state.rxHead = 0;

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Eth_Deinit(whal_Eth *ethDev)
{
    (void)ethDev;
    whal_Reg_Update(whal_Stm32f4_Eth_Dev.base, ETH_DMABMR_REG,
                    ETH_DMABMR_SR_Msk, ETH_DMABMR_SR_Msk);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Eth_Start(whal_Eth *ethDev, uint8_t speed,
                                  uint8_t duplex)
{
    size_t base = whal_Stm32f4_Eth_Dev.base;
    (void)ethDev;

    /* Apply link parameters to MACCR — FES=1 selects 100 Mbps,
     * DM=1 selects full duplex. */
    whal_Reg_Update(base, ETH_MACCR_REG,
                    ETH_MACCR_FES_Msk | ETH_MACCR_DM_Msk,
                    ((speed == WHAL_ETH_SPEED_100) ? ETH_MACCR_FES_Msk : 0) |
                    (duplex ? ETH_MACCR_DM_Msk : 0));

    /* Enable MAC TX and RX. */
    whal_Reg_Update(base, ETH_MACCR_REG,
                    ETH_MACCR_TE_Msk | ETH_MACCR_RE_Msk,
                    ETH_MACCR_TE_Msk | ETH_MACCR_RE_Msk);

    /* Flush TX FIFO before going Running — self-clears when complete. */
    whal_Reg_Update(base, ETH_DMAOMR_REG,
                    ETH_DMAOMR_FTF_Msk, ETH_DMAOMR_FTF_Msk);

    /* Start TX and RX DMA. DMAOMR is documented (§33.8.4) as the last
     * CSR to write during DMA bring-up. */
    whal_Reg_Update(base, ETH_DMAOMR_REG,
                    ETH_DMAOMR_ST_Msk | ETH_DMAOMR_SR_Msk,
                    ETH_DMAOMR_ST_Msk | ETH_DMAOMR_SR_Msk);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Eth_Stop(whal_Eth *ethDev)
{
    size_t base = whal_Stm32f4_Eth_Dev.base;
    (void)ethDev;

    whal_Reg_Update(base, ETH_DMAOMR_REG,
                    ETH_DMAOMR_ST_Msk | ETH_DMAOMR_SR_Msk, 0);

    whal_Reg_Update(base, ETH_MACCR_REG,
                    ETH_MACCR_TE_Msk | ETH_MACCR_RE_Msk, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Eth_Send(whal_Eth *ethDev, const void *frame,
                                 size_t len)
{
    const whal_Stm32f4_Eth_Cfg *cfg =
        (const whal_Stm32f4_Eth_Cfg *)whal_Stm32f4_Eth_Dev.cfg;
    size_t base = whal_Stm32f4_Eth_Dev.base;
    whal_Stm32f4_Eth_TxDesc *desc;
    const uint8_t *src = (const uint8_t *)frame;
    uint8_t *dst;
    size_t idx;
    uint32_t des0;
    size_t i;
    (void)ethDev;

    if (!frame || len == 0 || len > cfg->txBufSize)
        return WHAL_EINVAL;

    idx = eth_state.txHead;
    desc = &cfg->txDescs[idx];

    /* Descriptor still owned by DMA — no slot available. */
    if (desc->des[0] & TDES0_OWN)
        return WHAL_ENOTREADY;

    /* Copy into the per-descriptor TX buffer (already wired via des[2] in
     * Init). */
    dst = cfg->txBufs + idx * cfg->txBufSize;
    for (i = 0; i < len; i++)
        dst[i] = src[i];

    /* TBS1 = len; TBS2 = 0. */
    desc->des[1] = (uint32_t)(len & TDES1_TBS1_Msk);

    /* des[0]: keep TER bit on the last descriptor, set FS+LS+IC+OWN. */
    des0 = (idx == cfg->txDescCount - 1) ? TDES0_TER : 0;
    des0 |= TDES0_OWN | TDES0_FS | TDES0_LS | TDES0_IC;
    desc->des[0] = des0;

    eth_state.txHead = (idx + 1) % cfg->txDescCount;

    /* Kick TX poll demand — the DMA may have parked in suspended state on a
     * previous empty descriptor; writing any value to DMATPDR resumes it. */
    whal_Reg_Write(base, ETH_DMATPDR_REG, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Eth_Recv(whal_Eth *ethDev, void *frame, size_t *len)
{
    const whal_Stm32f4_Eth_Cfg *cfg =
        (const whal_Stm32f4_Eth_Cfg *)whal_Stm32f4_Eth_Dev.cfg;
    size_t base = whal_Stm32f4_Eth_Dev.base;
    whal_Stm32f4_Eth_RxDesc *desc;
    uint8_t *dst = (uint8_t *)frame;
    const uint8_t *src;
    size_t idx;
    uint32_t rdes0;
    size_t pktLen;
    size_t i;
    (void)ethDev;

    if (!frame || !len)
        return WHAL_EINVAL;

    idx = eth_state.rxHead;
    desc = &cfg->rxDescs[idx];
    rdes0 = desc->des[0];

    if (rdes0 & RDES0_OWN)
        return WHAL_ENOTREADY;

    if (rdes0 & RDES0_ES) {
        /* Error frame — drop and re-arm the descriptor for DMA. */
        desc->des[0] = RDES0_OWN;
        eth_state.rxHead = (idx + 1) % cfg->rxDescCount;
        whal_Reg_Write(base, ETH_DMARPDR_REG, 0);
        return WHAL_EHARDWARE;
    }

    /* RDES0.FL is the byte length including CRC. */
    pktLen = (rdes0 & RDES0_FL_Msk) >> RDES0_FL_Pos;
    if (pktLen > *len)
        pktLen = *len;

    src = cfg->rxBufs + idx * cfg->rxBufSize;
    for (i = 0; i < pktLen; i++)
        dst[i] = src[i];
    *len = pktLen;

    /* Return the descriptor to DMA. */
    desc->des[0] = RDES0_OWN;

    eth_state.rxHead = (idx + 1) % cfg->rxDescCount;

    /* If the RX DMA was suspended waiting for descriptors, the poll demand
     * write resumes it. */
    whal_Reg_Write(base, ETH_DMARPDR_REG, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Eth_MdioRead(whal_Eth *ethDev, uint8_t phyAddr,
                                     uint8_t reg, uint16_t *val)
{
    const whal_Stm32f4_Eth_Cfg *cfg =
        (const whal_Stm32f4_Eth_Cfg *)whal_Stm32f4_Eth_Dev.cfg;
    size_t base = whal_Stm32f4_Eth_Dev.base;
    whal_Error err;
    uint32_t miiar;
    (void)ethDev;

    if (!val)
        return WHAL_EINVAL;

    err = MdioPoll(base, cfg->timeout);
    if (err)
        return err;

    /* Compose MACMIIAR: PA, MR, CR preserved, MW=0 (read), MB=1 (start). */
    miiar = whal_SetBits(ETH_MACMIIAR_PA_Msk, ETH_MACMIIAR_PA_Pos,
                         phyAddr & 0x1F) |
            whal_SetBits(ETH_MACMIIAR_MR_Msk, ETH_MACMIIAR_MR_Pos,
                         reg & 0x1F) |
            whal_SetBits(ETH_MACMIIAR_CR_Msk, ETH_MACMIIAR_CR_Pos,
                         MdioClockRange(cfg->hclkHz)) |
            ETH_MACMIIAR_MB_Msk;
    whal_Reg_Write(base, ETH_MACMIIAR_REG, miiar);

    err = MdioPoll(base, cfg->timeout);
    if (err)
        return err;

    *val = (uint16_t)(whal_Reg_Read(base, ETH_MACMIIDR_REG) &
                      ETH_MACMIIDR_MD_Msk);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Eth_MdioWrite(whal_Eth *ethDev, uint8_t phyAddr,
                                      uint8_t reg, uint16_t val)
{
    const whal_Stm32f4_Eth_Cfg *cfg =
        (const whal_Stm32f4_Eth_Cfg *)whal_Stm32f4_Eth_Dev.cfg;
    size_t base = whal_Stm32f4_Eth_Dev.base;
    whal_Error err;
    uint32_t miiar;
    (void)ethDev;

    err = MdioPoll(base, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Write(base, ETH_MACMIIDR_REG, val);

    miiar = whal_SetBits(ETH_MACMIIAR_PA_Msk, ETH_MACMIIAR_PA_Pos,
                         phyAddr & 0x1F) |
            whal_SetBits(ETH_MACMIIAR_MR_Msk, ETH_MACMIIAR_MR_Pos,
                         reg & 0x1F) |
            whal_SetBits(ETH_MACMIIAR_CR_Msk, ETH_MACMIIAR_CR_Pos,
                         MdioClockRange(cfg->hclkHz)) |
            ETH_MACMIIAR_MW_Msk |
            ETH_MACMIIAR_MB_Msk;
    whal_Reg_Write(base, ETH_MACMIIAR_REG, miiar);

    err = MdioPoll(base, cfg->timeout);
    if (err)
        return err;

    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_STM32F4_ETH_DIRECT_API_MAPPING
const whal_EthDriver whal_Stm32f4_Eth_Driver = {
    .Init      = whal_Stm32f4_Eth_Init,
    .Deinit    = whal_Stm32f4_Eth_Deinit,
    .Start     = whal_Stm32f4_Eth_Start,
    .Stop      = whal_Stm32f4_Eth_Stop,
    .Send      = whal_Stm32f4_Eth_Send,
    .Recv      = whal_Stm32f4_Eth_Recv,
    .MdioRead  = whal_Stm32f4_Eth_MdioRead,
    .MdioWrite = whal_Stm32f4_Eth_MdioWrite,
};
#endif /* !WHAL_CFG_STM32F4_ETH_DIRECT_API_MAPPING */
