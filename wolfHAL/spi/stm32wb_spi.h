#ifndef WHAL_STM32WB_SPI_H
#define WHAL_STM32WB_SPI_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32wb_spi.h
 * @brief STM32WB SPI driver configuration.
 *
 * The STM32WB SPI peripheral provides:
 * - Full-duplex synchronous serial communication
 * - Master and slave modes (this driver supports master only)
 * - Configurable clock polarity and phase (SPI modes 0-3)
 * - Programmable baud rate prescaler (fPCLK/2 to fPCLK/256)
 * - 4 to 16-bit data frame (this driver uses 8-bit)
 * - Software slave management (chip select via GPIO)
 */

/*
 * @brief SPI clock polarity/phase mode selection.
 */
typedef enum {
    WHAL_STM32WB_SPI_MODE_0, /* CPOL=0, CPHA=0 */
    WHAL_STM32WB_SPI_MODE_1, /* CPOL=0, CPHA=1 */
    WHAL_STM32WB_SPI_MODE_2, /* CPOL=1, CPHA=0 */
    WHAL_STM32WB_SPI_MODE_3, /* CPOL=1, CPHA=1 */
} whal_Stm32wbSpi_Mode;

/*
 * @brief SPI device configuration.
 */
typedef struct whal_Stm32wbSpi_Cfg {
    uint32_t pclk;        /* Peripheral clock frequency in Hz */
    whal_Timeout *timeout;
} whal_Stm32wbSpi_Cfg;

/*
 * @brief Per-transaction SPI communication parameters.
 */
typedef struct whal_Stm32wbSpi_ComCfg {
    uint32_t mode;       /* SPI mode (WHAL_STM32WB_SPI_MODE_x) */
    uint32_t baud;       /* Baud rate in Hz */
} whal_Stm32wbSpi_ComCfg;

/*
 * @brief Driver instance for STM32 SPI peripheral.
 */
extern const whal_SpiDriver whal_Stm32wbSpi_Driver;

/*
 * @brief Initialize the STM32 SPI peripheral.
 *
 * @param spiDev SPI device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbSpi_Init(whal_Spi *spiDev);
/*
 * @brief Deinitialize the STM32 SPI peripheral.
 *
 * @param spiDev SPI device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbSpi_Deinit(whal_Spi *spiDev);
/*
 * @brief Perform a full-duplex SPI transfer.
 *
 * @param spiDev    SPI device instance.
 * @param spiComCfg Per-transfer configuration.
 * @param tx        Data to transmit (may be NULL).
 * @param txLen     Number of bytes to transmit.
 * @param rx        Receive buffer (may be NULL).
 * @param rxLen     Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS Transfer completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbSpi_SendRecv(whal_Spi *spiDev, void *spiComCfg, const uint8_t *tx,
                               size_t txLen, uint8_t *rx, size_t rxLen);
/*
 * @brief Transmit a buffer over SPI.
 *
 * @param spiDev    SPI device instance.
 * @param spiComCfg Per-transfer configuration.
 * @param data      Buffer to transmit.
 * @param dataSz    Number of bytes to transmit.
 *
 * @retval WHAL_SUCCESS Transfer completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbSpi_Send(whal_Spi *spiDev, void *spiComCfg, const uint8_t *data,
                           size_t dataSz);
/*
 * @brief Receive a buffer over SPI.
 *
 * @param spiDev    SPI device instance.
 * @param spiComCfg Per-transfer configuration.
 * @param data      Receive buffer.
 * @param dataSz    Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS Transfer completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32wbSpi_Recv(whal_Spi *spiDev, void *spiComCfg, uint8_t *data,
                           size_t dataSz);

#endif /* WHAL_STM32WB_SPI_H */
