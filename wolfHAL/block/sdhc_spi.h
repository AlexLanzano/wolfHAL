#ifndef WHAL_SDHC_SPI_H
#define WHAL_SDHC_SPI_H

#include <stddef.h>
#include <stdint.h>
#include <wolfHAL/block/block.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/timeout.h>

/*
 * @file sdhc_spi.h
 * @brief SD card driver over SPI (SDHC/SDXC).
 *
 * Implements the whal_Block interface for SD cards using the SD/SPI
 * protocol. Supports SDHC and SDXC cards with 512-byte block addressing.
 *
 * The driver handles:
 * - Card initialization (CMD0, CMD8, ACMD41, CMD58)
 * - Single and multi-block reads (CMD17, CMD18)
 * - Single and multi-block writes (CMD24, CMD25)
 * - Block range erase (CMD32, CMD33, CMD38)
 */

#define WHAL_SDHC_SPI_BLOCK_SZ 512

typedef struct whal_SdhcSpi_Cfg {
    whal_Spi *spiDev;           /* SPI bus device */
    whal_Spi_ComCfg *spiComCfg; /* SPI session config for StartCom */
    whal_Gpio *gpioDev;         /* GPIO device for chip select */
    size_t csPin;               /* GPIO pin index for chip select */
    whal_Timeout *timeout;
} whal_SdhcSpi_Cfg;

extern const whal_BlockDriver whal_SdhcSpi_Driver;

whal_Error whal_SdhcSpi_Init(whal_Block *blockDev);
whal_Error whal_SdhcSpi_Deinit(whal_Block *blockDev);
whal_Error whal_SdhcSpi_Read(whal_Block *blockDev, uint32_t block,
                              uint8_t *data, uint32_t blockCount);
whal_Error whal_SdhcSpi_Write(whal_Block *blockDev, uint32_t block,
                               const uint8_t *data, uint32_t blockCount);
whal_Error whal_SdhcSpi_Erase(whal_Block *blockDev, uint32_t block,
                               uint32_t blockCount);

#endif /* WHAL_SDHC_SPI_H */
