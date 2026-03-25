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

inline whal_Error whal_Spi_StartCom(whal_Spi *spiDev, whal_Spi_ComCfg *comCfg)
{
    if (!spiDev || !spiDev->driver || !spiDev->driver->StartCom || !comCfg) {
        return WHAL_EINVAL;
    }

    return spiDev->driver->StartCom(spiDev, comCfg);
}

inline whal_Error whal_Spi_EndCom(whal_Spi *spiDev)
{
    if (!spiDev || !spiDev->driver || !spiDev->driver->EndCom) {
        return WHAL_EINVAL;
    }

    return spiDev->driver->EndCom(spiDev);
}

inline whal_Error whal_Spi_SendRecv(whal_Spi *spiDev, const uint8_t *tx, size_t txLen, uint8_t *rx, size_t rxLen)
{
    if (!spiDev || !spiDev->driver || !spiDev->driver->SendRecv) {
        return WHAL_EINVAL;
    }

    return spiDev->driver->SendRecv(spiDev, tx, txLen, rx, rxLen);
}
