/* stm32f4_dma.h
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

#ifndef WHAL_STM32F4_DMA_H
#define WHAL_STM32F4_DMA_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/dma/dma.h>
#include <wolfHAL/timeout.h>

/**
 * @file stm32f4_dma.h
 * @brief STM32F4 DMA driver (classic, stream-based, two controllers).
 *
 * The STM32F4 series uses the legacy stream-based DMA: DMA1 and DMA2, each
 * with eight streams. Each stream multiplexes one of eight peripheral request
 * channels via the SxCR.CHSEL[2:0] field. This is *not* DMAMUX (G0/G4/U5/H5)
 * nor GPDMA (U5/H5/WBA/N6) — it is its own architecture documented in
 * RM0090 chapter 10.
 *
 * A whal_Dma instance represents one DMA controller; the stream index passed
 * to Configure/Start/Stop selects the stream (0-7). Use
 * WHAL_CFG_STM32F4_DMA_SINGLE_INSTANCE to opt into the singleton form;
 * otherwise the driver supports both DMA1 and DMA2 as separate devices.
 */

/**
 * @brief Transfer direction (encodes DIR[1:0] in SxCR).
 *
 * The hardware does not have an explicit mem-to-mem direction code distinct
 * from periph-to-mem; mem-to-mem is signalled by DIR=10. The driver hides
 * that detail.
 */
typedef enum {
    WHAL_STM32F4_DMA_DIR_PERIPH_TO_MEM = 0, /* DIR=00 */
    WHAL_STM32F4_DMA_DIR_MEM_TO_PERIPH = 1, /* DIR=01 */
    WHAL_STM32F4_DMA_DIR_MEM_TO_MEM    = 2, /* DIR=10 */
} whal_Stm32f4_Dma_Dir;

/**
 * @brief Data width (PSIZE/MSIZE encoding).
 */
typedef enum {
    WHAL_STM32F4_DMA_WIDTH_8BIT  = 0,
    WHAL_STM32F4_DMA_WIDTH_16BIT = 1,
    WHAL_STM32F4_DMA_WIDTH_32BIT = 2,
} whal_Stm32f4_Dma_Width;

/**
 * @brief Address increment mode.
 */
typedef enum {
    WHAL_STM32F4_DMA_INC_DISABLE = 0,
    WHAL_STM32F4_DMA_INC_ENABLE  = 1,
} whal_Stm32f4_Dma_Inc;

/**
 * @brief Per-stream transfer configuration.
 *
 * @p channel selects the peripheral request mapped to the stream
 * (SxCR.CHSEL[2:0]). @p length is the number of items (NDTR) in @p width
 * units. F4 DMA NDTR is 16-bit.
 */
typedef struct {
    whal_Stm32f4_Dma_Dir   dir;
    uint32_t               srcAddr;
    uint32_t               dstAddr;
    uint16_t               length;     /* number of items (NDT) */
    whal_Stm32f4_Dma_Width width;
    whal_Stm32f4_Dma_Inc   srcInc;
    whal_Stm32f4_Dma_Inc   dstInc;
    uint8_t                channel;    /* CHSEL[2:0] — 0..7 */
    uint8_t                circular;   /* non-zero for circular mode */
} whal_Stm32f4_Dma_ChCfg;

/**
 * @brief Controller-level configuration.
 *
 * @p numChannels is the stream count (always 8 on F4). @p timeout bounds the
 * Stop polling on EN clear.
 */
typedef struct {
    uint8_t       numChannels;
    whal_Timeout *timeout;
} whal_Stm32f4_Dma_Cfg;

#ifdef WHAL_CFG_STM32F4_DMA_SINGLE_INSTANCE
/**
 * @brief Single-instance device. Defined in the driver TU from
 *        WHAL_CFG_STM32F4_DMA_DEV in board.h.
 */
extern const whal_Dma whal_Stm32f4_Dma_Dev;
#endif

#ifndef WHAL_CFG_STM32F4_DMA_DIRECT_API_MAPPING
/**
 * @brief Driver vtable.
 */
extern const whal_DmaDriver whal_Stm32f4_Dma_Driver;

/**
 * @brief Initialize the DMA controller (clear stale flags).
 */
whal_Error whal_Stm32f4_Dma_Init(whal_Dma *dmaDev);

/**
 * @brief Disable all streams and clear stale flags.
 */
whal_Error whal_Stm32f4_Dma_Deinit(whal_Dma *dmaDev);

/**
 * @brief Configure a DMA stream.
 *
 * @param dmaDev DMA controller instance.
 * @param ch     Stream index (0-7).
 * @param chCfg  Pointer to whal_Stm32f4_Dma_ChCfg.
 */
whal_Error whal_Stm32f4_Dma_Configure(whal_Dma *dmaDev, size_t ch,
                                      const void *chCfg);

/**
 * @brief Enable a previously configured stream.
 */
whal_Error whal_Stm32f4_Dma_Start(whal_Dma *dmaDev, size_t ch);

/**
 * @brief Disable a stream and clear its flags.
 */
whal_Error whal_Stm32f4_Dma_Stop(whal_Dma *dmaDev, size_t ch);
#endif /* !WHAL_CFG_STM32F4_DMA_DIRECT_API_MAPPING */

/**
 * @brief DMA completion callback type (invoked from the stream IRQ handler).
 *
 * @param ctx User context pointer.
 * @param err WHAL_SUCCESS on transfer complete; WHAL_EHARDWARE on transfer
 *            error or FIFO error.
 */
typedef void (*whal_Stm32f4_Dma_Callback)(void *ctx, whal_Error err);

/**
 * @brief Handle a DMA stream interrupt.
 *
 * Reads LISR/HISR, dispatches on TCIF/TEIF/FEIF, clears the matching flag in
 * LIFCR/HIFCR, and invokes the callback exactly once with the resolved
 * status.
 *
 * @param dmaDev DMA controller instance.
 * @param ch     Stream index (0-7).
 * @param cb     Callback to invoke (or NULL to just clear flags).
 * @param ctx    Context pointer passed to the callback.
 */
void whal_Stm32f4_Dma_IRQHandler(whal_Dma *dmaDev, size_t ch,
                                 whal_Stm32f4_Dma_Callback cb, void *ctx);

#endif /* WHAL_STM32F4_DMA_H */
