#include "peripheral.h"

#ifdef PERIPHERAL_SDHC_SPI_SDCARD32GB
#include "block/sdhc_spi_sdcard32gb.h"
#endif

#ifdef PERIPHERAL_SPI_NOR_W25Q64
#include "flash/spi_nor_w25q64.h"
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

whal_PeripheralFlash_Cfg g_peripheralFlash[] = {
#ifdef PERIPHERAL_SPI_NOR_W25Q64
    {
        .name = "spi_nor_w25q64",
        .dev = &g_whalSpiNorW25q64,
        .sectorSz = 4096,
    },
#endif
    {0}, /* sentinel */
};

whal_Error Peripheral_Init(void)
{
    whal_Error err;

    for (size_t i = 0; g_peripheralBlock[i].dev; i++) {
        err = whal_Block_Init(g_peripheralBlock[i].dev);
        if (err)
            return err;
    }

    for (size_t i = 0; g_peripheralFlash[i].dev; i++) {
        err = whal_Flash_Init(g_peripheralFlash[i].dev);
        if (err)
            return err;
    }

    return WHAL_SUCCESS;
}

whal_Error Peripheral_Deinit(void)
{
    whal_Error err;

    for (size_t i = 0; g_peripheralFlash[i].dev; i++) {
        err = whal_Flash_Deinit(g_peripheralFlash[i].dev);
        if (err)
            return err;
    }

    for (size_t i = 0; g_peripheralBlock[i].dev; i++) {
        err = whal_Block_Deinit(g_peripheralBlock[i].dev);
        if (err)
            return err;
    }

    return WHAL_SUCCESS;
}
