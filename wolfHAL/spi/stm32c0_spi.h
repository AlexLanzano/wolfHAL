#ifndef WHAL_STM32C0_SPI_H
#define WHAL_STM32C0_SPI_H

/*
 * @file stm32c0_spi.h
 * @brief STM32C0 SPI driver (alias for STM32WB SPI).
 *
 * The STM32C0 SPI peripheral is register-compatible with the STM32WB SPI.
 * This header re-exports the STM32WB SPI driver types and symbols under
 * STM32C0-specific names. The underlying implementation is shared.
 */

#include <wolfHAL/spi/stm32wb_spi.h>

typedef whal_Stm32wbSpi_Cfg whal_Stm32c0Spi_Cfg;

#define whal_Stm32c0Spi_Driver   whal_Stm32wbSpi_Driver
#define whal_Stm32c0Spi_Init     whal_Stm32wbSpi_Init
#define whal_Stm32c0Spi_Deinit   whal_Stm32wbSpi_Deinit
#define whal_Stm32c0Spi_StartCom whal_Stm32wbSpi_StartCom
#define whal_Stm32c0Spi_EndCom   whal_Stm32wbSpi_EndCom
#define whal_Stm32c0Spi_SendRecv whal_Stm32wbSpi_SendRecv

#endif /* WHAL_STM32C0_SPI_H */
