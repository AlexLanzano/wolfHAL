#include <stdint.h>
#include <wolfHAL/spi/stm32wb_spi.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/error.h>

/*
 * STM32WB SPI Driver - Stub Implementation
 *
 * TODO: Implement SPI register configuration and data transfer.
 */

whal_Error whal_Stm32wbSpi_Init(whal_Spi *spiDev)
{
    (void)spiDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbSpi_Deinit(whal_Spi *spiDev)
{
    (void)spiDev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbSpi_SendRecv(whal_Spi *spiDev, void *spiComCfg, const uint8_t *tx,
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

whal_Error whal_Stm32wbSpi_Send(whal_Spi *spiDev, void *spiComCfg, const uint8_t *data,
                                size_t dataSz)
{
    (void)spiDev;
    (void)spiComCfg;
    (void)data;
    (void)dataSz;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbSpi_Recv(whal_Spi *spiDev, void *spiComCfg, uint8_t *data,
                                size_t dataSz)
{
    (void)spiDev;
    (void)spiComCfg;
    (void)data;
    (void)dataSz;
    return WHAL_SUCCESS;
}

whal_SpiDriver whal_Stm32wbSpi_Driver = {
    .Init = whal_Stm32wbSpi_Init,
    .Deinit = whal_Stm32wbSpi_Deinit,
    .SendRecv = whal_Stm32wbSpi_SendRecv,
    .Send = whal_Stm32wbSpi_Send,
    .Recv = whal_Stm32wbSpi_Recv,
};
