#ifndef WHAL_STM32WB55XX_H
#define WHAL_STM32WB55XX_H

#include <wolfHAL/platform/arm/cortex_m4.h>

#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/gpio/stm32wb_gpio.h>
#include <wolfHAL/uart/stm32wb_uart.h>
#include <wolfHAL/uart/stm32wb_uart_dma.h>
#include <wolfHAL/spi/stm32wb_spi.h>
#include <wolfHAL/flash/stm32wb_flash.h>
#include <wolfHAL/rng/stm32wb_rng.h>
#include <wolfHAL/crypto/stm32wb_aes.h>
#include <wolfHAL/dma/stm32wb_dma.h>
#include <wolfHAL/i2c/stm32wb_i2c.h>

/*
 * @file stm32wb55xx.h
 * @brief Convenience initializers for STM32WB55xx device instances.
 */

#define WHAL_STM32WB55_LPUART1_DEVICE   \
    .regmap = {                         \
        .base = 0x40008000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbUart_Driver

#define WHAL_STM32WB55_SPI1_DEVICE      \
    .regmap = {                         \
        .base = 0x40013000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbSpi_Driver

#define WHAL_STM32WB55_UART1_DEVICE     \
    .regmap = {                         \
        .base = 0x40013800,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbUart_Driver

#define WHAL_STM32WB55_GPIO_DEVICE      \
    .regmap = {                         \
        .base = 0x48000000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbGpio_Driver

#define WHAL_STM32WB55_RCC_PLL_DEVICE   \
    .regmap = {                         \
        .base = 0x58000000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbRccPll_Driver

#define WHAL_STM32WB55_RCC_MSI_DEVICE   \
    .regmap = {                         \
        .base = 0x58000000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbRccPll_Driver

#define WHAL_STM32WB55_RNG_DEVICE       \
    .regmap = {                         \
        .base = 0x58001000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbRng_Driver

#define WHAL_STM32WB55_AES1_DEVICE      \
    .regmap = {                         \
        .base = 0x50060000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbAes_Driver

#define WHAL_STM32WB55_I2C1_DEVICE      \
    .regmap = {                         \
        .base = 0x40005400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbI2c_Driver

#define WHAL_STM32WB55_I2C3_DEVICE      \
    .regmap = {                         \
        .base = 0x40005C00,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbI2c_Driver

#define WHAL_STM32WB55_FLASH_DEVICE     \
    .regmap = {                         \
        .base = 0x58004000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbFlash_Driver

#define WHAL_STM32WB55_DMA1_DEVICE      \
    .regmap = {                         \
        .base = 0x40020000,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbDma_Driver

#define WHAL_STM32WB55_DMA2_DEVICE      \
    .regmap = {                         \
        .base = 0x40020400,             \
        .size = 0x400,                  \
    },                                  \
    .driver = &whal_Stm32wbDma_Driver

#define WHAL_STM32WB55_DMA1_CFG         \
    .dmamuxBase = 0x40020800,           \
    .dmamuxChOffset = 0,                \
    .numChannels = 7

#define WHAL_STM32WB55_DMA2_CFG         \
    .dmamuxBase = 0x40020800,           \
    .dmamuxChOffset = 7,                \
    .numChannels = 5


#define WHAL_STM32WB55_PLL_CLOCK    \
    .regOffset = 0x00,              \
    .enableMask = (1UL << 24),      \
    .enablePos = 24

#define WHAL_STM32WB55_GPIOA_CLOCK  \
    .regOffset = 0x4C,              \
    .enableMask = (1UL << 0),       \
    .enablePos = 0

#define WHAL_STM32WB55_GPIOB_CLOCK  \
    .regOffset = 0x4C,              \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#define WHAL_STM32WB55_AES1_CLOCK    \
    .regOffset = 0x4C,               \
    .enableMask = (1UL << 16),       \
    .enablePos = 16

#define WHAL_STM32WB55_RNG_CLOCK    \
    .regOffset = 0x50,              \
    .enableMask = (1UL << 18),      \
    .enablePos = 18

#define WHAL_STM32WB55_FLASH_CLOCK  \
    .regOffset = 0x50,              \
    .enableMask = (1UL << 25),      \
    .enablePos = 25

#define WHAL_STM32WB55_I2C1_CLOCK   \
    .regOffset = 0x58,              \
    .enableMask = (1UL << 21),      \
    .enablePos = 21

#define WHAL_STM32WB55_I2C3_CLOCK   \
    .regOffset = 0x58,              \
    .enableMask = (1UL << 23),      \
    .enablePos = 23

#define WHAL_STM32WB55_LPUART1_CLOCK    \
    .regOffset = 0x5C,                  \
    .enableMask = (1UL << 0),           \
    .enablePos = 0

#define WHAL_STM32WB55_UART1_CLOCK  \
    .regOffset = 0x60,              \
    .enableMask = (1UL << 14),      \
    .enablePos = 14

#define WHAL_STM32WB55_SPI1_CLOCK   \
    .regOffset = 0x60,              \
    .enableMask = (1UL << 12),      \
    .enablePos = 12

#define WHAL_STM32WB55_DMA1_CLOCK   \
    .regOffset = 0x48,              \
    .enableMask = (1UL << 0),       \
    .enablePos = 0

#define WHAL_STM32WB55_DMA2_CLOCK   \
    .regOffset = 0x48,              \
    .enableMask = (1UL << 1),       \
    .enablePos = 1

#define WHAL_STM32WB55_UART1_TX_DMA_CFG     \
    .dir = WHAL_STM32WB_DMA_DIR_MEM_TO_PERIPH, \
    .width = WHAL_STM32WB_DMA_WIDTH_8BIT,      \
    .srcInc = WHAL_STM32WB_DMA_INC_ENABLE,     \
    .dstInc = WHAL_STM32WB_DMA_INC_DISABLE,    \
    .dmamuxReqId = 15

#define WHAL_STM32WB55_UART1_RX_DMA_CFG     \
    .dir = WHAL_STM32WB_DMA_DIR_PERIPH_TO_MEM, \
    .width = WHAL_STM32WB_DMA_WIDTH_8BIT,      \
    .srcInc = WHAL_STM32WB_DMA_INC_DISABLE,    \
    .dstInc = WHAL_STM32WB_DMA_INC_ENABLE,     \
    .dmamuxReqId = 14

#define WHAL_STM32WB55_DMAMUX1_CLOCK \
    .regOffset = 0x48,               \
    .enableMask = (1UL << 2),        \
    .enablePos = 2


#endif /* WHAL_STM32WB55XX_H */
