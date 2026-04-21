#ifndef WHAL_STM32WBA_SPI_H
#define WHAL_STM32WBA_SPI_H

/*
 * @file stm32wba_spi.h
 * @brief STM32WBA SPI driver (alias for STM32H5 SPI).
 *
 * The STM32WBA SPI peripheral is register-compatible with the STM32H5 SPI.
 * This header re-exports the STM32H5 SPI driver types and symbols under
 * STM32WBA-specific names.
 */

#include <wolfHAL/spi/stm32h5_spi.h>

typedef whal_Stm32h5Spi_Cfg whal_Stm32wbaSpi_Cfg;

#ifndef WHAL_CFG_SPI_API_MAPPING_STM32WBA
#define whal_Stm32wbaSpi_Driver   whal_Stm32h5Spi_Driver
#define whal_Stm32wbaSpi_Init     whal_Stm32h5Spi_Init
#define whal_Stm32wbaSpi_Deinit   whal_Stm32h5Spi_Deinit
#define whal_Stm32wbaSpi_StartCom whal_Stm32h5Spi_StartCom
#define whal_Stm32wbaSpi_EndCom   whal_Stm32h5Spi_EndCom
#define whal_Stm32wbaSpi_SendRecv whal_Stm32h5Spi_SendRecv
#endif /* !WHAL_CFG_SPI_API_MAPPING_STM32WBA */

#endif /* WHAL_STM32WBA_SPI_H */
