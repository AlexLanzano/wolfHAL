#include <stdint.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Spi_Init(whal_Spi *spiDev)
{
    if (!spiDev || !spiDev->driver || !spiDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return spiDev->driver->Init(spiDev);
}

inline whal_Error whal_Spi_Deinit(whal_Spi *spiDev)
{
    if (!spiDev || !spiDev->driver || !spiDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return spiDev->driver->Deinit(spiDev);
}

inline whal_Error whal_Spi_SendRecv(whal_Spi *spiDev, void *spiComCfg, const uint8_t *tx, size_t txLen, uint8_t *rx, size_t rxLen)
{
    if (!spiDev || !spiDev->driver || !spiDev->driver->SendRecv || !spiComCfg) {
        return WHAL_EINVAL;
    }

    return spiDev->driver->SendRecv(spiDev, spiComCfg, tx, txLen, rx, rxLen);
}

inline whal_Error whal_Spi_Send(whal_Spi *spiDev, void *spiComCfg, const uint8_t *data, size_t dataSz)
{
    if (!spiDev || !spiDev->driver || !spiDev->driver->Send || !spiComCfg || !data) {
        return WHAL_EINVAL;
    }

    return spiDev->driver->Send(spiDev, spiComCfg, data, dataSz);
}

inline whal_Error whal_Spi_Recv(whal_Spi *spiDev, void *spiComCfg, uint8_t *data, size_t dataSz)
{
    if (!spiDev || !spiDev->driver || !spiDev->driver->Recv || !spiComCfg || !data) {
        return WHAL_EINVAL;
    }

    return spiDev->driver->Recv(spiDev, spiComCfg, data, dataSz);
}

