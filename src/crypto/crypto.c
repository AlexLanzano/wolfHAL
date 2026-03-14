#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/error.h>

whal_Error whal_Crypto_Init(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->driver || !cryptoDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return cryptoDev->driver->Init(cryptoDev);
}

whal_Error whal_Crypto_Deinit(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->driver || !cryptoDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return cryptoDev->driver->Deinit(cryptoDev);
}

whal_Error whal_Crypto_Op(whal_Crypto *cryptoDev, size_t op, void *opArgs)
{
    if (!cryptoDev || !cryptoDev->ops || !opArgs) {
        return WHAL_EINVAL;
    }

    if (op >= cryptoDev->opsCount || !cryptoDev->ops[op]) {
        return WHAL_EINVAL;
    }

    return cryptoDev->ops[op](cryptoDev, opArgs);
}
