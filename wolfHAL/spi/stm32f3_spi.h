#ifndef WHAL_STM32F3_SPI_H
#define WHAL_STM32F3_SPI_H

/*
 * @file stm32f3_spi.h
 * @brief STM32F3 SPI driver (alias for STM32WB SPI).
 *
 * The STM32F3 SPI peripheral is register-compatible with the STM32WB SPI
 * (SPI v2 with DS[3:0] data size field in CR2).
 */

#include <wolfHAL/spi/stm32wb_spi.h>

typedef whal_Stm32wbSpi_Cfg whal_Stm32f3Spi_Cfg;

#ifndef WHAL_CFG_SPI_API_MAPPING_STM32F3
#define whal_Stm32f3Spi_Driver   whal_Stm32wbSpi_Driver
#define whal_Stm32f3Spi_Init     whal_Stm32wbSpi_Init
#define whal_Stm32f3Spi_Deinit   whal_Stm32wbSpi_Deinit
#define whal_Stm32f3Spi_StartCom whal_Stm32wbSpi_StartCom
#define whal_Stm32f3Spi_EndCom   whal_Stm32wbSpi_EndCom
#define whal_Stm32f3Spi_SendRecv whal_Stm32wbSpi_SendRecv
#endif /* !WHAL_CFG_SPI_API_MAPPING_STM32F3 */

#endif /* WHAL_STM32F3_SPI_H */
