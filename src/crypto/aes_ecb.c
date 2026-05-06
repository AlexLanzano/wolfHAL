#include <wolfHAL/crypto/crypto.h>

whal_Error whal_AesEcb_Oneshot(whal_AesEcb *dev, whal_Crypto_Dir dir,
                               const void *key, size_t keySz,
                               const void *in, void *out, size_t sz)
{
    if (!dev || !dev->driver || !dev->driver->Oneshot)
        return WHAL_ENOTSUP;
    return dev->driver->Oneshot(dev, dir, key, keySz, in, out, sz);
}

whal_Error whal_AesEcb_Start(whal_AesEcb *dev, whal_Crypto_Dir dir,
                             const void *key, size_t keySz)
{
    if (!dev || !dev->driver || !dev->driver->Start)
        return WHAL_ENOTSUP;
    return dev->driver->Start(dev, dir, key, keySz);
}

whal_Error whal_AesEcb_Process(whal_AesEcb *dev,
                               const void *in, void *out, size_t sz)
{
    if (!dev || !dev->driver || !dev->driver->Process)
        return WHAL_ENOTSUP;
    return dev->driver->Process(dev, in, out, sz);
}
