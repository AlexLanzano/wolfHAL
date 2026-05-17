#include <wolfHAL/crypto/crypto.h>

whal_Error whal_AesCtr_Oneshot(whal_AesCtr *dev, whal_Crypto_Dir dir,
                               const void *key, size_t keySz,
                               const void *iv,
                               const void *in, void *out, size_t sz)
{
    if (!dev || !dev->driver || !dev->driver->Oneshot)
        return WHAL_ENOTSUP;
    return dev->driver->Oneshot(dev, dir, key, keySz, iv, in, out, sz);
}

whal_Error whal_AesCtr_Start(whal_AesCtr *dev, whal_Crypto_Dir dir,
                             const void *key, size_t keySz,
                             const void *iv)
{
    if (!dev || !dev->driver || !dev->driver->Start)
        return WHAL_ENOTSUP;
    return dev->driver->Start(dev, dir, key, keySz, iv);
}

whal_Error whal_AesCtr_Process(whal_AesCtr *dev,
                               const void *in, void *out, size_t sz)
{
    if (!dev || !dev->driver || !dev->driver->Process)
        return WHAL_ENOTSUP;
    return dev->driver->Process(dev, in, out, sz);
}
