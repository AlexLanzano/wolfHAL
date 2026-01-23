#include <stdint.h>
#include <wolfHAL/spi/st_spi.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/error.h>

whal_Error whal_StSpi_Init(whal_Spi *spiDev)
{
    (void)spiDev;
    return WHAL_SUCCESS;
}

whal_Error whal_StSpi_Deinit(whal_Spi *spiDev)
{
    (void)spiDev;
    return WHAL_SUCCESS;
}

whal_Error whal_StSpi_SendRecv(whal_Spi *spiDev, void *spiComCfg, const uint8_t *tx,
                                 size_t txLen, uint8_t *rx, size_t rxLen)
{
    (void)spiDev;
    (void)spiComCfg;
    (void)tx;
    (void)txLen;
    (void)rx;
    (void)rxLen;
    return WHAL_SUCCESS;
}

whal_Error whal_StSpi_Send(whal_Spi *spiDev, void *spiComCfg, const uint8_t *data,
                             size_t dataSz)
{
    (void)spiDev;
    (void)spiComCfg;
    (void)data;
    (void)dataSz;
    return WHAL_SUCCESS;
}

whal_Error whal_StSpi_Recv(whal_Spi *spiDev, void *spiComCfg, uint8_t *data,
                             size_t dataSz)
{
    (void)spiDev;
    (void)spiComCfg;
    (void)data;
    (void)dataSz;
    return WHAL_SUCCESS;
}

whal_Error whal_StSpi_Cmd(whal_Spi *spiDev, size_t cmd, void *args)
{
    (void)spiDev;
    (void)cmd;
    (void)args;
    return WHAL_SUCCESS;
}

whal_SpiDriver whal_StSpi_Driver = {
    .Init = whal_StSpi_Init,
    .Deinit = whal_StSpi_Deinit,
    .SendRecv = whal_StSpi_SendRecv,
    .Send = whal_StSpi_Send,
    .Recv = whal_StSpi_Recv,
    .Cmd = whal_StSpi_Cmd,
};
