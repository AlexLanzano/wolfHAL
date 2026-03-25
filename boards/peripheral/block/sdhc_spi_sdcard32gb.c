#include "sdhc_spi_sdcard32gb.h"
#include <wolfHAL/block/sdhc_spi.h>
#include "board.h"

static whal_Spi_ComCfg g_sdcardComCfg = {
    .freq = 25000000, /* 25 MHz */
    .mode = WHAL_SPI_MODE_0,
    .wordSz = 8,
    .dataLines = 1,
};

whal_Block g_whalSdhcSpiSdcard32gb = {
    .driver = &whal_SdhcSpi_Driver,
    .cfg = &(whal_SdhcSpi_Cfg) {
        .spiDev = &g_whalSpi,
        .spiComCfg = &g_sdcardComCfg,
        .gpioDev = &g_whalGpio,
        .csPin = SPI_CS_PIN,
        .timeout = &g_whalTimeout,
    },
};
