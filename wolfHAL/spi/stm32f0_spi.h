#ifndef WHAL_STM32F0_SPI_H
#define WHAL_STM32F0_SPI_H

/*
 * @file stm32f0_spi.h
 * @brief STM32F0 SPI driver (alias for STM32WB SPI).
 *
 * The STM32F0 SPI peripheral is register-compatible with the STM32WB SPI
 * (SPI v2 with DS[3:0] data size field in CR2). This header re-exports
 * the STM32WB SPI driver types and symbols under STM32F0-specific names.
 */

#include <wolfHAL/spi/stm32wb_spi.h>

typedef whal_Stm32wbSpi_Cfg whal_Stm32f0Spi_Cfg;

#ifndef WHAL_CFG_SPI_API_MAPPING_STM32F0
#define whal_Stm32f0Spi_Driver   whal_Stm32wbSpi_Driver
#define whal_Stm32f0Spi_Init     whal_Stm32wbSpi_Init
#define whal_Stm32f0Spi_Deinit   whal_Stm32wbSpi_Deinit
#define whal_Stm32f0Spi_StartCom whal_Stm32wbSpi_StartCom
#define whal_Stm32f0Spi_EndCom   whal_Stm32wbSpi_EndCom
#define whal_Stm32f0Spi_SendRecv whal_Stm32wbSpi_SendRecv
#endif /* !WHAL_CFG_SPI_API_MAPPING_STM32F0 */

#endif /* WHAL_STM32F0_SPI_H */
