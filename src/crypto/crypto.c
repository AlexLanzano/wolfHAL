#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/error.h>

#ifndef WHAL_CFG_STM32WB_AES_DIRECT_API_MAPPING
whal_Error whal_Crypto_Init(whal_Crypto *cryptoDev)
{
    if (!cryptoDev)
        return WHAL_EINVAL;
    if (!cryptoDev->driver || !cryptoDev->driver->Init)
        return WHAL_ENOTSUP;

    return cryptoDev->driver->Init(cryptoDev);
}

whal_Error whal_Crypto_Deinit(whal_Crypto *cryptoDev)
{
    if (!cryptoDev)
        return WHAL_EINVAL;
    if (!cryptoDev->driver || !cryptoDev->driver->Deinit)
        return WHAL_ENOTSUP;

    return cryptoDev->driver->Deinit(cryptoDev);
}

whal_Error whal_Crypto_StartOp(whal_Crypto *cryptoDev, size_t opId,
                               void *opArgs)
{
    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;
    if (!cryptoDev->driver || !cryptoDev->driver->StartOp)
        return WHAL_ENOTSUP;

    return cryptoDev->driver->StartOp(cryptoDev, opId, opArgs);
}

whal_Error whal_Crypto_Process(whal_Crypto *cryptoDev, size_t opId,
                               void *opArgs)
{
    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;
    if (!cryptoDev->driver || !cryptoDev->driver->Process)
        return WHAL_ENOTSUP;

    return cryptoDev->driver->Process(cryptoDev, opId, opArgs);
}

whal_Error whal_Crypto_EndOp(whal_Crypto *cryptoDev, size_t opId,
                             void *opArgs)
{
    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;
    if (!cryptoDev->driver || !cryptoDev->driver->EndOp)
        return WHAL_ENOTSUP;

    return cryptoDev->driver->EndOp(cryptoDev, opId, opArgs);
}
#endif
