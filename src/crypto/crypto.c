#include <wolfHAL/crypto/crypto.h>

whal_Error whal_Crypto_Init(whal_Crypto *dev)
{
    if (!dev)
        return WHAL_EINVAL;
    if (!dev->driver || !dev->driver->Init)
        return WHAL_ENOTSUP;
    return dev->driver->Init(dev);
}

whal_Error whal_Crypto_Deinit(whal_Crypto *dev)
{
    if (!dev)
        return WHAL_EINVAL;
    if (!dev->driver || !dev->driver->Deinit)
        return WHAL_ENOTSUP;
    return dev->driver->Deinit(dev);
}
