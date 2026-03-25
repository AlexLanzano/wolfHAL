#include <stdint.h>
#include <wolfHAL/block/block.h>
#include <wolfHAL/error.h>

inline whal_Error whal_Block_Init(whal_Block *blockDev)
{
    if (!blockDev || !blockDev->driver || !blockDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return blockDev->driver->Init(blockDev);
}

inline whal_Error whal_Block_Deinit(whal_Block *blockDev)
{
    if (!blockDev || !blockDev->driver || !blockDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return blockDev->driver->Deinit(blockDev);
}

inline whal_Error whal_Block_Read(whal_Block *blockDev, uint32_t block,
                                  uint8_t *data, uint32_t blockCount)
{
    if (!blockDev || !blockDev->driver || !blockDev->driver->Read || !data) {
        return WHAL_EINVAL;
    }

    return blockDev->driver->Read(blockDev, block, data, blockCount);
}

inline whal_Error whal_Block_Write(whal_Block *blockDev, uint32_t block,
                                   const uint8_t *data, uint32_t blockCount)
{
    if (!blockDev || !blockDev->driver || !blockDev->driver->Write || !data) {
        return WHAL_EINVAL;
    }

    return blockDev->driver->Write(blockDev, block, data, blockCount);
}

inline whal_Error whal_Block_Erase(whal_Block *blockDev, uint32_t block,
                                   uint32_t blockCount)
{
    if (!blockDev || !blockDev->driver || !blockDev->driver->Erase) {
        return WHAL_EINVAL;
    }

    return blockDev->driver->Erase(blockDev, block, blockCount);
}
