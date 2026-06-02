/* stm32f4_dma.c
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
#ifdef WHAL_CFG_STM32F4_DMA_SINGLE_INSTANCE
#include "board.h"  /* provides WHAL_CFG_STM32F4_DMA_DEV initializer */
#endif
#include <wolfHAL/dma/dma.h>
#include <wolfHAL/dma/stm32f4_dma.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>

/*
 * STM32F4 DMA register layout (RM0090 §10.5).
 *
 *   LISR  @ 0x00 — int status, streams 0..3
 *   HISR  @ 0x04 — int status, streams 4..7
 *   LIFCR @ 0x08 — int flag clear, streams 0..3
 *   HIFCR @ 0x0C — int flag clear, streams 4..7
 *   stream block @ 0x10 + 0x18 * n (n = 0..7):
 *     SxCR   +0x00
 *     SxNDTR +0x04
 *     SxPAR  +0x08
 *     SxM0AR +0x0C
 *     SxM1AR +0x10
 *     SxFCR  +0x14
 */

#define DMA_LISR_REG    0x00
#define DMA_HISR_REG    0x04
#define DMA_LIFCR_REG   0x08
#define DMA_HIFCR_REG   0x0C

#define DMA_STREAM_BASE(s)  (0x10 + 0x18 * (s))
#define DMA_SCR_REG(s)      (DMA_STREAM_BASE(s) + 0x00)
#define DMA_SNDTR_REG(s)    (DMA_STREAM_BASE(s) + 0x04)
#define DMA_SPAR_REG(s)     (DMA_STREAM_BASE(s) + 0x08)
#define DMA_SM0AR_REG(s)    (DMA_STREAM_BASE(s) + 0x0C)
#define DMA_SM1AR_REG(s)    (DMA_STREAM_BASE(s) + 0x10)
#define DMA_SFCR_REG(s)     (DMA_STREAM_BASE(s) + 0x14)

/* SxCR bit positions (verified against RM0090 §10.5.5 textual description). */
#define DMA_SCR_EN_Pos      0
#define DMA_SCR_EN_Msk      (1UL << DMA_SCR_EN_Pos)

#define DMA_SCR_DMEIE_Pos   1
#define DMA_SCR_DMEIE_Msk   (1UL << DMA_SCR_DMEIE_Pos)

#define DMA_SCR_TEIE_Pos    2
#define DMA_SCR_TEIE_Msk    (1UL << DMA_SCR_TEIE_Pos)

#define DMA_SCR_HTIE_Pos    3
#define DMA_SCR_HTIE_Msk    (1UL << DMA_SCR_HTIE_Pos)

#define DMA_SCR_TCIE_Pos    4
#define DMA_SCR_TCIE_Msk    (1UL << DMA_SCR_TCIE_Pos)

#define DMA_SCR_DIR_Pos     6
#define DMA_SCR_DIR_Msk     (3UL << DMA_SCR_DIR_Pos)

#define DMA_SCR_CIRC_Pos    8
#define DMA_SCR_CIRC_Msk    (1UL << DMA_SCR_CIRC_Pos)

#define DMA_SCR_PINC_Pos    9
#define DMA_SCR_PINC_Msk    (1UL << DMA_SCR_PINC_Pos)

#define DMA_SCR_MINC_Pos    10
#define DMA_SCR_MINC_Msk    (1UL << DMA_SCR_MINC_Pos)

#define DMA_SCR_PSIZE_Pos   11
#define DMA_SCR_PSIZE_Msk   (3UL << DMA_SCR_PSIZE_Pos)

#define DMA_SCR_MSIZE_Pos   13
#define DMA_SCR_MSIZE_Msk   (3UL << DMA_SCR_MSIZE_Pos)

#define DMA_SCR_PL_Pos      16
#define DMA_SCR_PL_Msk      (3UL << DMA_SCR_PL_Pos)

#define DMA_SCR_CHSEL_Pos   25
#define DMA_SCR_CHSEL_Msk   (7UL << DMA_SCR_CHSEL_Pos)

/*
 * Within LISR (streams 0..3) or HISR (streams 4..7), each stream uses a
 * 6-bit lane. Streams 0/1 (or 4/5) occupy bits 0..11; streams 2/3 (or 6/7)
 * occupy bits 16..27 (gap at bits 12..15). Within a stream's lane:
 *   FEIF  = lane_base + 0
 *   DMEIF = lane_base + 2
 *   TEIF  = lane_base + 3
 *   HTIF  = lane_base + 4
 *   TCIF  = lane_base + 5
 *
 * Stream 0 base = 0, stream 1 base = 6, stream 2 base = 16, stream 3 base = 22.
 * RM0090 §10.5.1 lists TCIF at bits 5, 11, 21, 27 — matching these bases + 5.
 */
static size_t LaneBase(size_t stream)
{
    size_t lane = stream % 4;
    static const uint8_t bases[4] = {0, 6, 16, 22};
    return bases[lane];
}

static size_t IsrReg(size_t stream)
{
    return (stream < 4) ? DMA_LISR_REG : DMA_HISR_REG;
}

static size_t IfcrReg(size_t stream)
{
    return (stream < 4) ? DMA_LIFCR_REG : DMA_HIFCR_REG;
}

#define LANE_FEIF_Pos(b)   ((b) + 0)
#define LANE_DMEIF_Pos(b)  ((b) + 2)
#define LANE_TEIF_Pos(b)   ((b) + 3)
#define LANE_HTIF_Pos(b)   ((b) + 4)
#define LANE_TCIF_Pos(b)   ((b) + 5)

/* Clear all five flags (FEIF/DMEIF/TEIF/HTIF/TCIF) within a lane. */
static size_t LaneClearAll(size_t base)
{
    return  (1UL << LANE_FEIF_Pos(base))
          | (1UL << LANE_DMEIF_Pos(base))
          | (1UL << LANE_TEIF_Pos(base))
          | (1UL << LANE_HTIF_Pos(base))
          | (1UL << LANE_TCIF_Pos(base));
}

#ifdef WHAL_CFG_STM32F4_DMA_DIRECT_API_MAPPING
#define whal_Stm32f4_Dma_Init      whal_Dma_Init
#define whal_Stm32f4_Dma_Deinit    whal_Dma_Deinit
#define whal_Stm32f4_Dma_Configure whal_Dma_Configure
#define whal_Stm32f4_Dma_Start     whal_Dma_Start
#define whal_Stm32f4_Dma_Stop      whal_Dma_Stop
#endif

#ifdef WHAL_CFG_STM32F4_DMA_SINGLE_INSTANCE
const whal_Dma whal_Stm32f4_Dma_Dev = WHAL_CFG_STM32F4_DMA_DEV;
#endif

/* Resolve (cfg, base) from either the singleton or a runtime instance. */
#ifdef WHAL_CFG_STM32F4_DMA_SINGLE_INSTANCE
#define WHAL_F4_DMA_RESOLVE(dmaDev, cfgVar, baseVar)                          \
    do {                                                                      \
        (cfgVar) = (whal_Stm32f4_Dma_Cfg *)whal_Stm32f4_Dma_Dev.cfg;           \
        (baseVar) = whal_Stm32f4_Dma_Dev.base;                                \
        (void)(dmaDev);                                                       \
    } while (0)
#else
#define WHAL_F4_DMA_RESOLVE(dmaDev, cfgVar, baseVar)                          \
    do {                                                                      \
        if (!(dmaDev) || !(dmaDev)->cfg)                                      \
            return WHAL_EINVAL;                                               \
        (cfgVar) = (whal_Stm32f4_Dma_Cfg *)(dmaDev)->cfg;                     \
        (baseVar) = (dmaDev)->base;                                           \
    } while (0)
#endif

whal_Error whal_Stm32f4_Dma_Init(whal_Dma *dmaDev)
{
    whal_Stm32f4_Dma_Cfg *cfg;
    size_t base;
    size_t lisrClear = 0;
    size_t hisrClear = 0;
    size_t i;

    WHAL_F4_DMA_RESOLVE(dmaDev, cfg, base);

    /* Clear all pending interrupt flags across every stream this controller
     * owns. LISR/HISR each cover four streams. */
    for (i = 0; i < cfg->numChannels; ++i) {
        size_t laneBase = LaneBase(i);
        if (i < 4)
            lisrClear |= LaneClearAll(laneBase);
        else
            hisrClear |= LaneClearAll(laneBase);
    }
    if (lisrClear)
        whal_Reg_Write(base, DMA_LIFCR_REG, lisrClear);
    if (hisrClear)
        whal_Reg_Write(base, DMA_HIFCR_REG, hisrClear);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Dma_Deinit(whal_Dma *dmaDev)
{
    whal_Stm32f4_Dma_Cfg *cfg;
    size_t base;
    size_t lisrClear = 0;
    size_t hisrClear = 0;
    size_t i;

    WHAL_F4_DMA_RESOLVE(dmaDev, cfg, base);

    /* Disable every stream, then clear flags. EN write to 0 begins a graceful
     * stop; the controller may take a few cycles, but for Deinit we don't
     * need to wait — Stop() handles that case. */
    for (i = 0; i < cfg->numChannels; ++i) {
        whal_Reg_Update(base, DMA_SCR_REG(i),
                        DMA_SCR_EN_Msk,
                        whal_SetBits(DMA_SCR_EN_Msk, DMA_SCR_EN_Pos, 0));

        if (i < 4)
            lisrClear |= LaneClearAll(LaneBase(i));
        else
            hisrClear |= LaneClearAll(LaneBase(i));
    }
    if (lisrClear)
        whal_Reg_Write(base, DMA_LIFCR_REG, lisrClear);
    if (hisrClear)
        whal_Reg_Write(base, DMA_HIFCR_REG, hisrClear);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Dma_Configure(whal_Dma *dmaDev, size_t ch,
                                      const void *chCfg)
{
    const whal_Stm32f4_Dma_ChCfg *streamCfg;
    whal_Stm32f4_Dma_Cfg *cfg;
    size_t base;
    size_t scr;
    uint32_t periphAddr;
    uint32_t memAddr;
    size_t periphInc;
    size_t memInc;
    size_t dirBits;

    if (!chCfg)
        return WHAL_EINVAL;

    WHAL_F4_DMA_RESOLVE(dmaDev, cfg, base);

    if (ch >= cfg->numChannels)
        return WHAL_EINVAL;
    if (((const whal_Stm32f4_Dma_ChCfg *)chCfg)->channel > 7)
        return WHAL_EINVAL;

    streamCfg = (const whal_Stm32f4_Dma_ChCfg *)chCfg;

    /*
     * Map our direction-and-roles model to F4 DMA's "peripheral side / memory
     * side" model. SPAR is the "peripheral side" register, SM0AR is the
     * "memory side". For mem-to-mem, SPAR holds the source and SM0AR the
     * destination; DIR=10.
     */
    switch (streamCfg->dir) {
    case WHAL_STM32F4_DMA_DIR_PERIPH_TO_MEM:
        periphAddr = streamCfg->srcAddr;
        memAddr    = streamCfg->dstAddr;
        periphInc  = streamCfg->srcInc;
        memInc     = streamCfg->dstInc;
        dirBits    = 0;
        break;
    case WHAL_STM32F4_DMA_DIR_MEM_TO_PERIPH:
        periphAddr = streamCfg->dstAddr;
        memAddr    = streamCfg->srcAddr;
        periphInc  = streamCfg->dstInc;
        memInc     = streamCfg->srcInc;
        dirBits    = 1;
        break;
    case WHAL_STM32F4_DMA_DIR_MEM_TO_MEM:
        periphAddr = streamCfg->srcAddr;
        memAddr    = streamCfg->dstAddr;
        periphInc  = streamCfg->srcInc;
        memInc     = streamCfg->dstInc;
        dirBits    = 2;
        break;
    default:
        return WHAL_EINVAL;
    }

    /* SxCR can only be written when EN=0 — TRM §10.5.5. Stop first. */
    whal_Reg_Write(base, DMA_SCR_REG(ch), 0);

    scr = 0;
    scr |= whal_SetBits(DMA_SCR_DIR_Msk, DMA_SCR_DIR_Pos, dirBits);
    if (streamCfg->circular)
        scr |= whal_SetBits(DMA_SCR_CIRC_Msk, DMA_SCR_CIRC_Pos, 1);
    scr |= whal_SetBits(DMA_SCR_PINC_Msk,  DMA_SCR_PINC_Pos,  periphInc);
    scr |= whal_SetBits(DMA_SCR_MINC_Msk,  DMA_SCR_MINC_Pos,  memInc);
    scr |= whal_SetBits(DMA_SCR_PSIZE_Msk, DMA_SCR_PSIZE_Pos, streamCfg->width);
    scr |= whal_SetBits(DMA_SCR_MSIZE_Msk, DMA_SCR_MSIZE_Pos, streamCfg->width);
    scr |= whal_SetBits(DMA_SCR_CHSEL_Msk, DMA_SCR_CHSEL_Pos, streamCfg->channel);
    scr |= whal_SetBits(DMA_SCR_TCIE_Msk,  DMA_SCR_TCIE_Pos,  1);
    scr |= whal_SetBits(DMA_SCR_TEIE_Msk,  DMA_SCR_TEIE_Pos,  1);

    whal_Reg_Write(base, DMA_SNDTR_REG(ch), streamCfg->length);
    whal_Reg_Write(base, DMA_SPAR_REG(ch),  periphAddr);
    whal_Reg_Write(base, DMA_SM0AR_REG(ch), memAddr);
    whal_Reg_Write(base, DMA_SCR_REG(ch),   scr);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Dma_Start(whal_Dma *dmaDev, size_t ch)
{
    whal_Stm32f4_Dma_Cfg *cfg;
    size_t base;
    size_t laneBase;

    WHAL_F4_DMA_RESOLVE(dmaDev, cfg, base);

    if (ch >= cfg->numChannels)
        return WHAL_EINVAL;

    /* RM0090 §10.3.17: clear pending event flags for the stream before
     * setting EN, otherwise a stale TEIF/FEIF would immediately mark
     * the start as a failure. */
    laneBase = LaneBase(ch);
    whal_Reg_Write(base, IfcrReg(ch), LaneClearAll(laneBase));

    whal_Reg_Update(base, DMA_SCR_REG(ch),
                    DMA_SCR_EN_Msk,
                    whal_SetBits(DMA_SCR_EN_Msk, DMA_SCR_EN_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Dma_Stop(whal_Dma *dmaDev, size_t ch)
{
    whal_Stm32f4_Dma_Cfg *cfg;
    size_t base;
    size_t laneBase;
    whal_Error err;

    WHAL_F4_DMA_RESOLVE(dmaDev, cfg, base);

    if (ch >= cfg->numChannels)
        return WHAL_EINVAL;

    /* Write EN=0 and poll until the controller actually clears it
     * (RM0090 §10.3.14: EN read 0 means the stream is fully stopped). */
    whal_Reg_Update(base, DMA_SCR_REG(ch),
                    DMA_SCR_EN_Msk,
                    whal_SetBits(DMA_SCR_EN_Msk, DMA_SCR_EN_Pos, 0));

    err = whal_Reg_ReadPoll(base, DMA_SCR_REG(ch),
                            DMA_SCR_EN_Msk, 0, cfg->timeout);
    if (err)
        return err;

    laneBase = LaneBase(ch);
    whal_Reg_Write(base, IfcrReg(ch), LaneClearAll(laneBase));

    return WHAL_SUCCESS;
}

void whal_Stm32f4_Dma_IRQHandler(whal_Dma *dmaDev, size_t ch,
                                 whal_Stm32f4_Dma_Callback cb, void *ctx)
{
    size_t base;
    size_t isr;
    size_t laneBase;
    int reported = 0;

#ifdef WHAL_CFG_STM32F4_DMA_SINGLE_INSTANCE
    base = whal_Stm32f4_Dma_Dev.base;
    (void)dmaDev;
#else
    if (!dmaDev)
        return;
    base = dmaDev->base;
#endif

    laneBase = LaneBase(ch);
    isr = whal_Reg_Read(base, IsrReg(ch));

    /* TEIF/FEIF take priority: surface the error first, then drain TCIF
     * silently so the next transfer can start cleanly. */
    if (isr & (1UL << LANE_TEIF_Pos(laneBase))) {
        whal_Reg_Write(base, IfcrReg(ch), 1UL << LANE_TEIF_Pos(laneBase));
        if (cb && !reported) {
            cb(ctx, WHAL_EHARDWARE);
            reported = 1;
        }
    }

    if (isr & (1UL << LANE_FEIF_Pos(laneBase))) {
        whal_Reg_Write(base, IfcrReg(ch), 1UL << LANE_FEIF_Pos(laneBase));
        if (cb && !reported) {
            cb(ctx, WHAL_EHARDWARE);
            reported = 1;
        }
    }

    if (isr & (1UL << LANE_TCIF_Pos(laneBase))) {
        whal_Reg_Write(base, IfcrReg(ch), 1UL << LANE_TCIF_Pos(laneBase));
        if (cb && !reported) {
            cb(ctx, WHAL_SUCCESS);
            reported = 1;
        }
    }

    if (isr & (1UL << LANE_HTIF_Pos(laneBase)))
        whal_Reg_Write(base, IfcrReg(ch), 1UL << LANE_HTIF_Pos(laneBase));
    if (isr & (1UL << LANE_DMEIF_Pos(laneBase)))
        whal_Reg_Write(base, IfcrReg(ch), 1UL << LANE_DMEIF_Pos(laneBase));
}

#ifndef WHAL_CFG_STM32F4_DMA_DIRECT_API_MAPPING
const whal_DmaDriver whal_Stm32f4_Dma_Driver = {
    .Init      = whal_Stm32f4_Dma_Init,
    .Deinit    = whal_Stm32f4_Dma_Deinit,
    .Configure = whal_Stm32f4_Dma_Configure,
    .Start     = whal_Stm32f4_Dma_Start,
    .Stop      = whal_Stm32f4_Dma_Stop,
};
#endif /* !WHAL_CFG_STM32F4_DMA_DIRECT_API_MAPPING */
