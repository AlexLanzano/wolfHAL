#ifndef WHAL_SPI_H
#define WHAL_SPI_H

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <stdint.h>
#include <stddef.h>

/*
 * @file spi.h
 * @brief Generic SPI abstraction and driver interface.
 */

typedef struct whal_Spi whal_Spi;

/*
 * @brief Driver vtable for SPI devices.
 */
typedef struct {
    /* Initialize the SPI hardware. */
    whal_Error (*Init)(whal_Spi *spiDev);
    /* Deinitialize the SPI hardware. */
    whal_Error (*Deinit)(whal_Spi *spiDev);
    /* Perform a bidirectional transfer. */
    whal_Error (*SendRecv)(whal_Spi *spiDev, void *spiComCfg, const uint8_t *tx, size_t txLen, uint8_t *rx, size_t rxLen);
    /* Transmit a buffer. */
    whal_Error (*Send)(whal_Spi *spiDev, void *spiComCfg, const uint8_t *data, size_t dataSz);
    /* Receive into a buffer. */
    whal_Error (*Recv)(whal_Spi *spiDev, void *spiComCfg, uint8_t *data, size_t dataSz);
} whal_SpiDriver;

/*
 * @brief SPI device instance tying a register map and driver.
 */
struct whal_Spi {
    const whal_Regmap regmap;
    const whal_SpiDriver *driver;
    void *cfg;
};

/*
 * @brief Initializes an SPI device and its driver.
 *
 * @param spiDev Pointer to the SPI instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or driver rejected configuration.
 */
#ifdef WHAL_CFG_NO_CALLBACKS
#define whal_Spi_Init(spiDev) ((spiDev)->driver->Init((spiDev)))
#define whal_Spi_Deinit(spiDev) ((spiDev)->driver->Deinit((spiDev)))
#define whal_Spi_SendRecv(spiDev, spiComCfg, tx, txLen, rx, rxLen) \
    ((spiDev)->driver->SendRecv((spiDev), (spiComCfg), (tx), (txLen), (rx), (rxLen)))
#define whal_Spi_Send(spiDev, spiComCfg, data, dataSz) \
    ((spiDev)->driver->Send((spiDev), (spiComCfg), (data), (dataSz)))
#define whal_Spi_Recv(spiDev, spiComCfg, data, dataSz) \
    ((spiDev)->driver->Recv((spiDev), (spiComCfg), (data), (dataSz)))
#else
/*
 * @brief Initializes an SPI device and its driver.
 *
 * @param spiDev Pointer to the SPI instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or driver rejected configuration.
 */
whal_Error whal_Spi_Init(whal_Spi *spiDev);
/*
 * @brief Deinitializes an SPI device and releases resources.
 *
 * @param spiDev Pointer to the SPI instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer or driver refused to deinit.
 */
whal_Error whal_Spi_Deinit(whal_Spi *spiDev);
/*
 * @brief Perform a bidirectional SPI transfer.
 *
 * @param spiDev Pointer to the SPI instance.
 * @param spiComCfg Communication configuration (chip select, mode, speed, etc).
 * @param tx     Buffer of bytes to transmit (may be NULL for read-only).
 * @param txLen  Number of bytes to transmit.
 * @param rx     Buffer to receive bytes into (may be NULL for write-only).
 * @param rxLen  Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS Transfer completed or queued.
 * @retval WHAL_EINVAL  Null pointer or driver failed to transfer.
 */
whal_Error whal_Spi_SendRecv(whal_Spi *spiDev, void *spiComCfg, const uint8_t *tx, size_t txLen, uint8_t *rx, size_t rxLen);
/*
 * @brief Sends a buffer over SPI.
 *
 * @param spiDev Pointer to the SPI instance.
 * @param spiComCfg Communication configuration (chip select, mode, speed, etc).
 * @param data   Buffer to transmit.
 * @param dataSz Number of bytes to send.
 *
 * @retval WHAL_SUCCESS Buffer was queued or transmitted.
 * @retval WHAL_EINVAL  Null pointer or driver failed to send.
 */
whal_Error whal_Spi_Send(whal_Spi *spiDev, void *spiComCfg, const uint8_t *data, size_t dataSz);
/*
 * @brief Receives data from SPI into a buffer.
 *
 * @param spiDev Pointer to the SPI instance.
 * @param spiComCfg Communication configuration (chip select, mode, speed, etc).
 * @param data   Destination buffer.
 * @param dataSz Maximum number of bytes to read.
 *
 * @retval WHAL_SUCCESS Buffer was filled or receive started.
 * @retval WHAL_EINVAL  Null pointer or driver failed to receive.
 */
whal_Error whal_Spi_Recv(whal_Spi *spiDev, void *spiComCfg, uint8_t *data, size_t dataSz);
#endif

#endif /* WHAL_SPI_H */
