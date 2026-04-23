#ifndef WHAL_STM32L1_SPI_H
#define WHAL_STM32L1_SPI_H

/*
 * @file stm32l1_spi.h
 * @brief STM32L1 SPI driver (alias for STM32F4 SPI).
 *
 * The STM32L1 SPI peripheral uses the same V1 SPI register layout as the
 * STM32F4 (CR1 with DFF bit at 0x00, CR2 at 0x04, SR at 0x08, DR at 0x0C).
 */

#include <wolfHAL/spi/stm32f4_spi.h>

typedef whal_Stm32f4Spi_Cfg whal_Stm32l1Spi_Cfg;

#ifndef WHAL_CFG_SPI_API_MAPPING_STM32L1
#define whal_Stm32l1Spi_Driver   whal_Stm32f4Spi_Driver
#define whal_Stm32l1Spi_Init     whal_Stm32f4Spi_Init
#define whal_Stm32l1Spi_Deinit   whal_Stm32f4Spi_Deinit
#define whal_Stm32l1Spi_StartCom whal_Stm32f4Spi_StartCom
#define whal_Stm32l1Spi_EndCom   whal_Stm32f4Spi_EndCom
#define whal_Stm32l1Spi_SendRecv whal_Stm32f4Spi_SendRecv
#endif /* !WHAL_CFG_SPI_API_MAPPING_STM32L1 */

#endif /* WHAL_STM32L1_SPI_H */
