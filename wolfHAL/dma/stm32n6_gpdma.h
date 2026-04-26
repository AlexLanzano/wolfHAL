#ifndef WHAL_STM32N6_GPDMA_H
#define WHAL_STM32N6_GPDMA_H

/**
 * @file stm32n6_gpdma.h
 * @brief STM32N6 GPDMA driver (alias for STM32WBA GPDMA).
 *
 * The STM32N6 GPDMA peripheral is register-compatible with the STM32WBA GPDMA.
 * This header re-exports under STM32N6-specific names.
 */

#include <wolfHAL/dma/stm32wba_gpdma.h>

typedef whal_Stm32wbaGpdma_Cfg   whal_Stm32n6Gpdma_Cfg;
typedef whal_Stm32wbaGpdma_ChCfg whal_Stm32n6Gpdma_ChCfg;

#ifndef WHAL_CFG_DMA_API_MAPPING_STM32N6
#define whal_Stm32n6Gpdma_Driver     whal_Stm32wbaGpdma_Driver
#define whal_Stm32n6Gpdma_Init       whal_Stm32wbaGpdma_Init
#define whal_Stm32n6Gpdma_Deinit     whal_Stm32wbaGpdma_Deinit
#define whal_Stm32n6Gpdma_Transfer   whal_Stm32wbaGpdma_Transfer
#define whal_Stm32n6Gpdma_IRQHandler whal_Stm32wbaGpdma_IRQHandler
#endif /* !WHAL_CFG_DMA_API_MAPPING_STM32N6 */

#define WHAL_STM32N6_GPDMA_DIR_PERIPH_TO_MEM WHAL_STM32WBA_GPDMA_DIR_PERIPH_TO_MEM
#define WHAL_STM32N6_GPDMA_DIR_MEM_TO_PERIPH WHAL_STM32WBA_GPDMA_DIR_MEM_TO_PERIPH
#define WHAL_STM32N6_GPDMA_DIR_MEM_TO_MEM    WHAL_STM32WBA_GPDMA_DIR_MEM_TO_MEM

#define WHAL_STM32N6_GPDMA_WIDTH_8BIT  WHAL_STM32WBA_GPDMA_WIDTH_8BIT
#define WHAL_STM32N6_GPDMA_WIDTH_16BIT WHAL_STM32WBA_GPDMA_WIDTH_16BIT
#define WHAL_STM32N6_GPDMA_WIDTH_32BIT WHAL_STM32WBA_GPDMA_WIDTH_32BIT

#define WHAL_STM32N6_GPDMA_INC_DISABLE WHAL_STM32WBA_GPDMA_INC_DISABLE
#define WHAL_STM32N6_GPDMA_INC_ENABLE  WHAL_STM32WBA_GPDMA_INC_ENABLE

#endif /* WHAL_STM32N6_GPDMA_H */
