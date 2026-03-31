#ifndef WHAL_STM32H5_SPI_H
#define WHAL_STM32H5_SPI_H

#include <wolfHAL/spi/spi.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32h5_spi.h
 * @brief STM32H5 SPI driver configuration.
 *
 * The STM32H5 SPI peripheral differs significantly from the STM32WB SPI:
 * - Separate configuration registers (SPI_CFG1, SPI_CFG2) instead of CR1/CR2
 * - Configurable data size up to 32 bits (DSIZE field)
 * - TX/RX FIFOs with configurable threshold (FTHLV)
 * - Transfer size counter (TSIZE in SPI_CR2)
 * - Explicit CSTART bit to begin master transfers
 * - Byte-accessible TXDR/RXDR at different offsets (0x020, 0x030)
 */

/*
 * @brief STM32H5 SPI configuration parameters.
 */
typedef struct whal_Stm32h5Spi_Cfg {
    size_t pclk;            /* Peripheral clock frequency in Hz */
    whal_Timeout *timeout;
} whal_Stm32h5Spi_Cfg;

/*
 * @brief Driver instance for STM32H5 SPI peripheral.
 */
extern const whal_SpiDriver whal_Stm32h5Spi_Driver;

/*
 * @brief Initialize the STM32H5 SPI peripheral.
 *
 * Configures the SPI as master with software slave management.
 *
 * @param spiDev SPI device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Spi_Init(whal_Spi *spiDev);

/*
 * @brief Deinitialize the STM32H5 SPI peripheral.
 *
 * Disables the SPI peripheral.
 *
 * @param spiDev SPI device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Spi_Deinit(whal_Spi *spiDev);

/*
 * @brief Begin a communication session with the given parameters.
 *
 * Configures CPOL, CPHA, baud rate, and data size. Enables the SPI.
 *
 * @param spiDev SPI device instance.
 * @param comCfg Per-session communication parameters.
 *
 * @retval WHAL_SUCCESS Session started.
 * @retval WHAL_EINVAL  Invalid parameters.
 */
whal_Error whal_Stm32h5Spi_StartCom(whal_Spi *spiDev, whal_Spi_ComCfg *comCfg);

/*
 * @brief End the current communication session.
 *
 * Disables the SPI peripheral.
 *
 * @param spiDev SPI device instance.
 *
 * @retval WHAL_SUCCESS Session ended.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Spi_EndCom(whal_Spi *spiDev);

/*
 * @brief Perform a bidirectional SPI transfer.
 *
 * Clocks max(txLen, rxLen) bytes. Pads TX with 0xFF when exhausted,
 * discards RX when exhausted or NULL.
 *
 * @param spiDev SPI device instance.
 * @param tx     Buffer to transmit (NULL to send 0xFF).
 * @param txLen  Number of bytes to transmit.
 * @param rx     Buffer to receive (NULL to discard).
 * @param rxLen  Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS Transfer completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Spi_SendRecv(whal_Spi *spiDev, const void *tx,
                                     size_t txLen, void *rx, size_t rxLen);

#endif /* WHAL_STM32H5_SPI_H */
