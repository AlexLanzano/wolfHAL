#ifndef WHAL_STM32H7_SPI_H
#define WHAL_STM32H7_SPI_H

/*
 * @file stm32h7_spi.h
 * @brief STM32H7 SPI driver (alias for STM32H5 SPI).
 *
 * The STM32H7 SPI peripheral is register-compatible with the STM32H5 SPI.
 * Both use the newer SPI architecture with CFG1/CFG2 configuration registers,
 * separate TXDR/RXDR data registers, and CSTART-based master transfers.
 * This header re-exports the STM32H5 SPI driver types and symbols under
 * STM32H7-specific names.
 */

#include <wolfHAL/spi/stm32h5_spi.h>

typedef whal_Stm32h5Spi_Cfg whal_Stm32h7Spi_Cfg;

#define whal_Stm32h7Spi_Driver   whal_Stm32h5Spi_Driver
#define whal_Stm32h7Spi_Init     whal_Stm32h5Spi_Init
#define whal_Stm32h7Spi_Deinit   whal_Stm32h5Spi_Deinit
#define whal_Stm32h7Spi_StartCom whal_Stm32h5Spi_StartCom
#define whal_Stm32h7Spi_EndCom   whal_Stm32h5Spi_EndCom
#define whal_Stm32h7Spi_SendRecv whal_Stm32h5Spi_SendRecv

#endif /* WHAL_STM32H7_SPI_H */
