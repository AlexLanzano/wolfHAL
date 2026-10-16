/* test_stm32wba_dma.c
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

#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/dma/stm32wba_gpdma.h>
#include "wolfHAL_board.h"
#include "test.h"

#ifdef BOARD_DMA

static uint8_t g_src[16];
static uint8_t g_dst[16];

/* Every case is rejected before any channel register is written */
static void Test_Gpdma_Reject(void)
{
    whal_Stm32wba_Gpdma_ChCfg cfg = {
        .dir = WHAL_STM32WBA_GPDMA_DIR_MEM_TO_MEM,
        .srcAddr = (size_t)g_src,
        .dstAddr = (size_t)g_dst,
        .nbytes = sizeof(g_src),
        .srcWidth = WHAL_STM32WBA_GPDMA_WIDTH_8BIT,
        .dstWidth = WHAL_STM32WBA_GPDMA_WIDTH_8BIT,
        .srcInc = WHAL_STM32WBA_GPDMA_INC_ENABLE,
        .dstInc = WHAL_STM32WBA_GPDMA_INC_ENABLE,
    };
    size_t numCh = ((whal_Stm32wba_Gpdma_Cfg *)g_whalDma1.cfg)->numChannels;

    WHAL_ASSERT_EQ(whal_Dma_Configure(&g_whalDma1, numCh, &cfg), WHAL_EINVAL);

    cfg.dir = (whal_Stm32wba_Gpdma_Dir)(WHAL_STM32WBA_GPDMA_DIR_MEM_TO_MEM + 1);
    WHAL_ASSERT_EQ(whal_Dma_Configure(&g_whalDma1, 0, &cfg), WHAL_EINVAL);
    cfg.dir = WHAL_STM32WBA_GPDMA_DIR_MEM_TO_MEM;

    cfg.nbytes = 0;
    WHAL_ASSERT_EQ(whal_Dma_Configure(&g_whalDma1, 0, &cfg), WHAL_EINVAL);

    cfg.nbytes = 0x10000;
    WHAL_ASSERT_EQ(whal_Dma_Configure(&g_whalDma1, 0, &cfg), WHAL_EINVAL);
}

#else

static void Test_Gpdma_NoBoardDma(void)
{
    WHAL_SKIP();
}

#endif /* BOARD_DMA */

void whal_Test_Dma_Platform(void)
{
    WHAL_TEST_SUITE_START("dma_platform");
#ifdef BOARD_DMA
    WHAL_TEST(Test_Gpdma_Reject);
#else
    WHAL_TEST(Test_Gpdma_NoBoardDma);
#endif
    WHAL_TEST_SUITE_END();
}
