#ifndef WHAL_STM32N6_SPI_H
#define WHAL_STM32N6_SPI_H

/**
 * @file stm32n6_spi.h
 * @brief STM32N6 SPI driver (alias for STM32H5 SPI).
 *
 * The STM32N6 SPI peripheral is register-compatible with the STM32H5 SPI
 * (V2 SPI: CR1/CR2/CFG1/CFG2/IER/SR/IFCR/TXDR/RXDR at identical offsets).
 * This header re-exports under STM32N6-specific names.
 */

#include <wolfHAL/spi/stm32h5_spi.h>

typedef whal_Stm32h5Spi_Cfg whal_Stm32n6Spi_Cfg;

#ifndef WHAL_CFG_SPI_API_MAPPING_STM32N6
#define whal_Stm32n6Spi_Driver   whal_Stm32h5Spi_Driver
#define whal_Stm32n6Spi_Init     whal_Stm32h5Spi_Init
#define whal_Stm32n6Spi_Deinit   whal_Stm32h5Spi_Deinit
#define whal_Stm32n6Spi_StartCom whal_Stm32h5Spi_StartCom
#define whal_Stm32n6Spi_EndCom   whal_Stm32h5Spi_EndCom
#define whal_Stm32n6Spi_SendRecv whal_Stm32h5Spi_SendRecv
#endif /* !WHAL_CFG_SPI_API_MAPPING_STM32N6 */

#endif /* WHAL_STM32N6_SPI_H */
