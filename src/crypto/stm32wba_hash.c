#include <stdint.h>
#include <wolfHAL/crypto/stm32wba_hash.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/endian.h>

/* Control Register */
#define HASH_CR_REG           0x00

#define HASH_CR_INIT_Pos      2
#define HASH_CR_INIT_Msk      (1UL << HASH_CR_INIT_Pos)

#define HASH_CR_DATATYPE_Pos  4
#define HASH_CR_DATATYPE_Msk  (3UL << HASH_CR_DATATYPE_Pos)

#define HASH_CR_MODE_Pos      6
#define HASH_CR_MODE_Msk      (1UL << HASH_CR_MODE_Pos)

#define HASH_CR_LKEY_Pos      16
#define HASH_CR_LKEY_Msk      (1UL << HASH_CR_LKEY_Pos)

#define HASH_CR_ALGO_Pos      17
#define HASH_CR_ALGO_Msk      (3UL << HASH_CR_ALGO_Pos)

/* Data Input Register */
#define HASH_DIN_REG          0x04

/* Start Register */
#define HASH_STR_REG          0x08

#define HASH_STR_NBLW_Pos     0
#define HASH_STR_NBLW_Msk     (0x1FUL << HASH_STR_NBLW_Pos)

#define HASH_STR_DCAL_Pos     8
#define HASH_STR_DCAL_Msk     (1UL << HASH_STR_DCAL_Pos)

/* Status Register */
#define HASH_SR_REG           0x24

#define HASH_SR_BUSY_Pos      3
#define HASH_SR_BUSY_Msk      (1UL << HASH_SR_BUSY_Pos)

/* Digest Result Registers */
#define HASH_HR_BASE_REG      0x310

/* Algorithm encoding: ALGO[1:0] at bits 18:17 */
#define HASH_ALGO_SHA1        0
#define HASH_ALGO_SHA224      2
#define HASH_ALGO_SHA256      3

#define HASH_MODE_HASH        0
#define HASH_MODE_HMAC        1

static uint32_t AlgoBits(size_t algo)
{
    return whal_SetBits(HASH_CR_ALGO_Msk, HASH_CR_ALGO_Pos, algo);
}

static whal_Error WaitForReady(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, HASH_SR_REG, HASH_SR_BUSY_Msk, 0, timeout);
}

static void WriteData(size_t base, const uint8_t *in, size_t inSz)
{
    while (inSz >= 4) {
        whal_Reg_Write(base, HASH_DIN_REG, whal_LoadBe32(in));
        in += 4;
        inSz -= 4;
    }

    if (inSz > 0) {
        whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk,
                        whal_SetBits(HASH_STR_NBLW_Msk, HASH_STR_NBLW_Pos,
                                     inSz * 8));
        whal_Reg_Write(base, HASH_DIN_REG, whal_LoadBe32Partial(in, inSz));
    }
}

static void ReadDigest(size_t base, uint8_t *digest, size_t digestSz)
{
    size_t words = digestSz / 4;
    size_t i;

    for (i = 0; i < words; i++)
        whal_StoreBe32(digest + i * 4,
                       whal_Reg_Read(base, HASH_HR_BASE_REG + i * 4));
}

static whal_Error Process_Hash(whal_Crypto *cryptoDev, const void *in,
                               size_t inSz)
{
    if (inSz == 0)
        return WHAL_SUCCESS;

    if (!in)
        return WHAL_EINVAL;

    WriteData(cryptoDev->regmap.base, (const uint8_t *)in, inSz);

    return WHAL_SUCCESS;
}


#ifdef WHAL_CFG_CRYPTO_API_MAPPING_STM32WBA_HASH
#define whal_Stm32wbaHash_Init    whal_Crypto_Init
#define whal_Stm32wbaHash_Deinit  whal_Crypto_Deinit
#define whal_Stm32wbaHash_StartOp whal_Crypto_StartOp
#define whal_Stm32wbaHash_Process whal_Crypto_Process
#define whal_Stm32wbaHash_EndOp   whal_Crypto_EndOp
#endif

whal_Error whal_Stm32wbaHash_Init(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg)
        return WHAL_EINVAL;

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaHash_Deinit(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg)
        return WHAL_EINVAL;

    return WHAL_SUCCESS;
}

/* --- SHA-1 / SHA-224 / SHA-256 --- */

#if defined(WHAL_CFG_CRYPTO_SHA1) || defined(WHAL_CFG_CRYPTO_SHA224) || \
    defined(WHAL_CFG_CRYPTO_SHA256)
static whal_Error StartOp_Hash(whal_Crypto *cryptoDev, size_t algo)
{
    size_t base = cryptoDev->regmap.base;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(algo) | HASH_CR_INIT_Msk);

    return WHAL_SUCCESS;
}

static whal_Error EndOp_Hash(whal_Crypto *cryptoDev, size_t expectedDigestSz,
                             void *opArgs)
{
    whal_Crypto_HashArgs *args = (whal_Crypto_HashArgs *)opArgs;
    const whal_Stm32wbaHash_Cfg *cfg =
        (const whal_Stm32wbaHash_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    whal_Error err;

    if (!args->digest || args->digestSz != expectedDigestSz)
        return WHAL_EINVAL;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)args->digest, args->digestSz);

    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_SHA1 || SHA224 || SHA256 */

/* --- HMAC-SHA-1 / HMAC-SHA-224 / HMAC-SHA-256 --- */

#if defined(WHAL_CFG_CRYPTO_HMAC_SHA1) || defined(WHAL_CFG_CRYPTO_HMAC_SHA224) || \
    defined(WHAL_CFG_CRYPTO_HMAC_SHA256)
static whal_Error StartOp_Hmac(whal_Crypto *cryptoDev, size_t algo,
                               void *opArgs)
{
    whal_Crypto_HmacArgs *args = (whal_Crypto_HmacArgs *)opArgs;
    const whal_Stm32wbaHash_Cfg *cfg =
        (const whal_Stm32wbaHash_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    size_t lkey;
    whal_Error err;

    if (!args->key)
        return WHAL_EINVAL;

    lkey = (args->keySz > 64) ? 1 : 0;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(algo) |
                    whal_SetBits(HASH_CR_MODE_Msk, HASH_CR_MODE_Pos,
                                 HASH_MODE_HMAC) |
                    whal_SetBits(HASH_CR_LKEY_Msk, HASH_CR_LKEY_Pos, lkey) |
                    HASH_CR_INIT_Msk);

    WriteData(base, (const uint8_t *)args->key, args->keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    return WHAL_SUCCESS;
}

static whal_Error EndOp_Hmac(whal_Crypto *cryptoDev, size_t expectedDigestSz,
                             void *opArgs)
{
    whal_Crypto_HmacArgs *args = (whal_Crypto_HmacArgs *)opArgs;
    const whal_Stm32wbaHash_Cfg *cfg =
        (const whal_Stm32wbaHash_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    whal_Error err;

    if (!args->key || !args->digest || args->digestSz != expectedDigestSz)
        return WHAL_EINVAL;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);
    WriteData(base, (const uint8_t *)args->key, args->keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)args->digest, args->digestSz);

    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_HMAC_SHA1 || HMAC_SHA224 || HMAC_SHA256 */

/* --- Dispatch --- */

whal_Error whal_Stm32wbaHash_StartOp(whal_Crypto *cryptoDev, size_t opId,
                                     void *opArgs)
{
    if (!cryptoDev)
        return WHAL_EINVAL;

    switch (opId) {
#ifdef WHAL_CFG_CRYPTO_SHA1
    case WHAL_CRYPTO_SHA1:
        return StartOp_Hash(cryptoDev, HASH_ALGO_SHA1);
#endif
#ifdef WHAL_CFG_CRYPTO_SHA224
    case WHAL_CRYPTO_SHA224:
        return StartOp_Hash(cryptoDev, HASH_ALGO_SHA224);
#endif
#ifdef WHAL_CFG_CRYPTO_SHA256
    case WHAL_CRYPTO_SHA256:
        return StartOp_Hash(cryptoDev, HASH_ALGO_SHA256);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA1
    case WHAL_CRYPTO_HMAC_SHA1:
        return StartOp_Hmac(cryptoDev, HASH_ALGO_SHA1, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA224
    case WHAL_CRYPTO_HMAC_SHA224:
        return StartOp_Hmac(cryptoDev, HASH_ALGO_SHA224, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA256
    case WHAL_CRYPTO_HMAC_SHA256:
        return StartOp_Hmac(cryptoDev, HASH_ALGO_SHA256, opArgs);
#endif
    default:
        return WHAL_ENOTSUP;
    }
}

whal_Error whal_Stm32wbaHash_Process(whal_Crypto *cryptoDev, size_t opId,
                                     void *opArgs)
{
    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    switch (opId) {
#ifdef WHAL_CFG_CRYPTO_SHA1
    case WHAL_CRYPTO_SHA1:
#endif
#ifdef WHAL_CFG_CRYPTO_SHA224
    case WHAL_CRYPTO_SHA224:
#endif
#ifdef WHAL_CFG_CRYPTO_SHA256
    case WHAL_CRYPTO_SHA256:
#endif
#if defined(WHAL_CFG_CRYPTO_SHA1) || defined(WHAL_CFG_CRYPTO_SHA224) || \
    defined(WHAL_CFG_CRYPTO_SHA256)
    {
        whal_Crypto_HashArgs *args = (whal_Crypto_HashArgs *)opArgs;
        return Process_Hash(cryptoDev, args->in, args->inSz);
    }
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA1
    case WHAL_CRYPTO_HMAC_SHA1:
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA224
    case WHAL_CRYPTO_HMAC_SHA224:
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA256
    case WHAL_CRYPTO_HMAC_SHA256:
#endif
#if defined(WHAL_CFG_CRYPTO_HMAC_SHA1) || defined(WHAL_CFG_CRYPTO_HMAC_SHA224) || \
    defined(WHAL_CFG_CRYPTO_HMAC_SHA256)
    {
        whal_Crypto_HmacArgs *args = (whal_Crypto_HmacArgs *)opArgs;
        return Process_Hash(cryptoDev, args->in, args->inSz);
    }
#endif
    default:
        return WHAL_ENOTSUP;
    }
}

whal_Error whal_Stm32wbaHash_EndOp(whal_Crypto *cryptoDev, size_t opId,
                                   void *opArgs)
{
    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    switch (opId) {
#ifdef WHAL_CFG_CRYPTO_SHA1
    case WHAL_CRYPTO_SHA1:
        return EndOp_Hash(cryptoDev, 20, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_SHA224
    case WHAL_CRYPTO_SHA224:
        return EndOp_Hash(cryptoDev, 28, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_SHA256
    case WHAL_CRYPTO_SHA256:
        return EndOp_Hash(cryptoDev, 32, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA1
    case WHAL_CRYPTO_HMAC_SHA1:
        return EndOp_Hmac(cryptoDev, 20, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA224
    case WHAL_CRYPTO_HMAC_SHA224:
        return EndOp_Hmac(cryptoDev, 28, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA256
    case WHAL_CRYPTO_HMAC_SHA256:
        return EndOp_Hmac(cryptoDev, 32, opArgs);
#endif
    default:
        return WHAL_ENOTSUP;
    }
}

#ifndef WHAL_CFG_CRYPTO_API_MAPPING_STM32WBA_HASH
const whal_CryptoDriver whal_Stm32wbaHash_Driver = {
    .Init = whal_Stm32wbaHash_Init,
    .Deinit = whal_Stm32wbaHash_Deinit,
    .StartOp = whal_Stm32wbaHash_StartOp,
    .Process = whal_Stm32wbaHash_Process,
    .EndOp = whal_Stm32wbaHash_EndOp,
};
#endif /* !WHAL_CFG_CRYPTO_API_MAPPING_STM32WBA_HASH */
