#ifndef WHAL_STM32WB_DMA_H
#define WHAL_STM32WB_DMA_H

#include <stddef.h>
#include <stdint.h>
#include <wolfHAL/dma/dma.h>
#include <wolfHAL/regmap.h>

/*
 * @file stm32wb_dma.h
 * @brief STM32WB DMA driver configuration types.
 *
 * The STM32WB has two DMA controllers:
 * - DMA1 with 7 channels
 * - DMA2 with 5 channels
 *
 * Each controller is represented by a single whal_Dma instance. Channels are
 * identified by a 0-based index (0-6 for DMA1, 0-4 for DMA2).
 *
 * A DMAMUX peripheral maps peripheral request lines to DMA channels.
 * DMAMUX channels 0-6 correspond to DMA1 channels 1-7. DMAMUX channels 7-11
 * correspond to DMA2 channels 1-5. The driver handles this mapping internally;
 * the caller only needs to provide the DMAREQ_ID for the desired peripheral.
 */

/*
 * @brief Transfer direction.
 */
typedef enum {
    WHAL_STM32WB_DMA_DIR_PERIPH_TO_MEM, /* Peripheral to memory */
    WHAL_STM32WB_DMA_DIR_MEM_TO_PERIPH, /* Memory to peripheral */
    WHAL_STM32WB_DMA_DIR_MEM_TO_MEM,    /* Memory to memory */
} whal_Stm32wbDma_Dir;

/*
 * @brief Data width for transfers.
 */
typedef enum {
    WHAL_STM32WB_DMA_WIDTH_8BIT,  /* 8-bit (byte) */
    WHAL_STM32WB_DMA_WIDTH_16BIT, /* 16-bit (half-word) */
    WHAL_STM32WB_DMA_WIDTH_32BIT, /* 32-bit (word) */
} whal_Stm32wbDma_Width;

/*
 * @brief Address increment mode.
 */
typedef enum {
    WHAL_STM32WB_DMA_INC_DISABLE, /* Fixed address */
    WHAL_STM32WB_DMA_INC_ENABLE,  /* Increment address after each transfer */
} whal_Stm32wbDma_Inc;

/*
 * @brief Per-channel DMA configuration.
 */
typedef struct {
    whal_Stm32wbDma_Dir dir;       /* Transfer direction */
    uint32_t srcAddr;              /* Source address */
    uint32_t dstAddr;              /* Destination address */
    uint16_t length;               /* Number of data items to transfer */
    whal_Stm32wbDma_Width width;   /* Data width (8, 16, or 32 bit) */
    whal_Stm32wbDma_Inc srcInc;    /* Source address increment mode */
    whal_Stm32wbDma_Inc dstInc;    /* Destination address increment mode */
    uint8_t dmamuxReqId;           /* DMAMUX request ID for the peripheral */
    uint8_t circular;              /* Non-zero to enable circular mode */
} whal_Stm32wbDma_ChCfg;

/*
 * @brief Controller-level DMA configuration.
 *
 * Contains the DMAMUX base address and the DMAMUX channel offset for this
 * controller. DMA1 uses DMAMUX channels starting at 0; DMA2 starts at 7.
 */
typedef struct {
    size_t dmamuxBase;       /* DMAMUX1 register base address */
    uint8_t dmamuxChOffset;  /* First DMAMUX channel index for this controller */
    uint8_t numChannels;     /* Number of channels (7 for DMA1, 5 for DMA2) */
} whal_Stm32wbDma_Cfg;

/*
 * @brief Driver instance for STM32WB DMA.
 */
extern const whal_DmaDriver whal_Stm32wbDma_Driver;

/*
 * @brief DMA callback type.
 *
 * Called from ISR context when a transfer completes or an error occurs.
 *
 * @param ctx User context pointer.
 * @param err WHAL_SUCCESS on transfer complete, WHAL_EHARDWARE on error.
 */
typedef void (*whal_Stm32wbDma_Callback)(void *ctx, whal_Error err);

/*
 * @brief Handle a DMA channel interrupt.
 *
 * Checks TCIF/TEIF flags, clears them, and invokes the callback.
 * Should be called from the board's ISR entry point.
 *
 * @param dmaDev DMA controller instance.
 * @param ch     Channel index (0-based).
 * @param cb     Callback to invoke (or NULL to just clear flags).
 * @param ctx    Context pointer passed to callback.
 */
void whal_Stm32wbDma_IRQHandler(whal_Dma *dmaDev, size_t ch,
                                 whal_Stm32wbDma_Callback cb, void *ctx);

#endif /* WHAL_STM32WB_DMA_H */
