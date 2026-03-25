#include "peripheral.h"

#ifdef PERIPHERAL_SDHC_SPI_SDCARD32GB
#include "block/sdhc_spi_sdcard32gb.h"
#endif

whal_PeripheralBlock_Cfg g_peripheralBlock[] = {
#ifdef PERIPHERAL_SDHC_SPI_SDCARD32GB
    {
        .name = "sdhc_spi_sdcard32gb",
        .dev = &g_whalSdhcSpiSdcard32gb,
        .blockSz = WHAL_SDHC_SPI_BLOCK_SZ,
        .blockBuf = (uint8_t[WHAL_SDHC_SPI_BLOCK_SZ * 2]) {0},
        .blockBufSz = WHAL_SDHC_SPI_BLOCK_SZ * 2,
        .erasedByte = 0x00,
    },
#endif
    {0}, /* sentinel */
};
