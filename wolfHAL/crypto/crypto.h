#ifndef WHAL_CRYPTO_H
#define WHAL_CRYPTO_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/error.h>

/*
 * @file crypto.h
 * @brief Generic crypto accelerator abstraction and driver interface.
 */

/* ---- Common enums ---- */

typedef enum {
    WHAL_CRYPTO_ENCRYPT,
    WHAL_CRYPTO_DECRYPT,
} whal_Crypto_Dir;

enum {
    WHAL_CRYPTO_AES_ECB,
    WHAL_CRYPTO_AES_CBC,
    WHAL_CRYPTO_AES_CTR,
    WHAL_CRYPTO_AES_GCM,
    WHAL_CRYPTO_AES_GMAC,
    WHAL_CRYPTO_AES_CCM,
    WHAL_CRYPTO_SHA1,
    WHAL_CRYPTO_SHA224,
    WHAL_CRYPTO_SHA256,
    WHAL_CRYPTO_HMAC_SHA1,
    WHAL_CRYPTO_HMAC_SHA224,
    WHAL_CRYPTO_HMAC_SHA256,
};

/* ---- Per-algorithm argument structs ---- */

/*
 * @brief Arguments for AES-ECB.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const void     *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const void     *in;
    void           *out;
    size_t          sz;       /* Must be a multiple of 16 */
} whal_Crypto_AesEcbArgs;

/*
 * @brief Arguments for AES-CBC.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const void     *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const void     *iv;       /* 16 bytes */
    const void     *in;
    void           *out;
    size_t          sz;       /* Must be a multiple of 16 */
} whal_Crypto_AesCbcArgs;

/*
 * @brief Arguments for AES-CTR.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const void     *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const void     *iv;       /* 16 bytes (initial counter block) */
    const void     *in;
    void           *out;
    size_t          sz;
} whal_Crypto_AesCtrArgs;

/*
 * @brief Arguments for AES-GCM.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const void     *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const void     *iv;
    size_t          ivSz;     /* Typically 12 */
    const void     *in;
    void           *out;
    size_t          sz;
    const void     *aad;
    size_t          aadSz;
    void           *tag;
    size_t          tagSz;    /* Up to 16 */
} whal_Crypto_AesGcmArgs;

/*
 * @brief Arguments for AES-CCM.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const void     *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const void     *nonce;
    size_t          nonceSz;  /* 7-13 */
    const void     *in;
    void           *out;
    size_t          sz;
    const void     *aad;
    size_t          aadSz;
    void           *tag;
    size_t          tagSz;    /* 4, 6, 8, 10, 12, 14, or 16 */
} whal_Crypto_AesCcmArgs;

/*
 * @brief Arguments for AES-GMAC (authentication only, no payload).
 */
typedef struct {
    const void *key;
    size_t      keySz;    /* 16, 24, or 32 */
    const void *iv;
    size_t      ivSz;     /* Typically 12 */
    const void *aad;
    size_t      aadSz;
    void       *tag;
    size_t      tagSz;    /* Up to 16 */
} whal_Crypto_AesGmacArgs;

/*
 * @brief Arguments for one-shot hash (SHA-1/224/256/...).
 */
typedef struct {
    const void *in;
    size_t      inSz;
    void       *digest;
    size_t      digestSz;  /* Algorithm-specific (20/28/32/48/64) */
} whal_Crypto_HashArgs;

/*
 * @brief Arguments for one-shot HMAC (HMAC-SHA-1/224/256/...).
 */
typedef struct {
    const void *key;
    size_t      keySz;
    const void *in;
    size_t      inSz;
    void       *digest;
    size_t      digestSz;  /* Algorithm-specific (20/28/32/48/64) */
} whal_Crypto_HmacArgs;

/* ---- Device and driver ---- */

typedef struct whal_Crypto whal_Crypto;

/*
 * @brief Driver vtable for crypto devices.
 */
typedef struct {
    /* Initialize the crypto hardware. */
    whal_Error (*Init)(whal_Crypto *cryptoDev);
    /* Deinitialize the crypto hardware. */
    whal_Error (*Deinit)(whal_Crypto *cryptoDev);
    /* Start a crypto operation: configure hardware, load key/IV, process AAD. */
    whal_Error (*StartOp)(whal_Crypto *cryptoDev, size_t opId, void *opArgs);
    /* Process data through the crypto engine. */
    whal_Error (*Process)(whal_Crypto *cryptoDev, size_t opId, void *opArgs);
    /* End a crypto operation: finalize, release hardware. */
    whal_Error (*EndOp)(whal_Crypto *cryptoDev, size_t opId, void *opArgs);
} whal_CryptoDriver;

/*
 * @brief Crypto device instance tying a register map, driver, and configuration.
 */
struct whal_Crypto {
    const whal_Regmap regmap;
    const whal_CryptoDriver *driver;
    const void *cfg;
};

/*
 * @brief Initialize a crypto device and its driver.
 *
 * @param cryptoDev Pointer to the crypto instance to initialize.
 *
 * @retval WHAL_SUCCESS  Driver-specific init completed.
 * @retval WHAL_EINVAL   Null pointer.
 * @retval WHAL_ENOTSUP  Operation not implemented by this driver.
 */
#ifdef WHAL_CFG_DIRECT_CALLBACKS
#define whal_Crypto_Init(cryptoDev) ((cryptoDev)->driver->Init((cryptoDev)))
#define whal_Crypto_Deinit(cryptoDev) ((cryptoDev)->driver->Deinit((cryptoDev)))
#define whal_Crypto_StartOp(cryptoDev, opId, opArgs) ((cryptoDev)->driver->StartOp((cryptoDev), (opId), (opArgs)))
#define whal_Crypto_Process(cryptoDev, opId, opArgs) ((cryptoDev)->driver->Process((cryptoDev), (opId), (opArgs)))
#define whal_Crypto_EndOp(cryptoDev, opId, opArgs) ((cryptoDev)->driver->EndOp((cryptoDev), (opId), (opArgs)))
#else
/*
 * @brief Initialize a crypto device and its driver.
 *
 * @param cryptoDev Pointer to the crypto instance to initialize.
 *
 * @retval WHAL_SUCCESS  Driver-specific init completed.
 * @retval WHAL_EINVAL   Null pointer.
 * @retval WHAL_ENOTSUP  Operation not implemented by this driver.
 */
whal_Error whal_Crypto_Init(whal_Crypto *cryptoDev);

/*
 * @brief Deinitialize a crypto device and release resources.
 *
 * @param cryptoDev Pointer to the crypto instance to deinitialize.
 *
 * @retval WHAL_SUCCESS  Driver-specific deinit completed.
 * @retval WHAL_EINVAL   Null pointer.
 * @retval WHAL_ENOTSUP  Operation not implemented by this driver.
 */
whal_Error whal_Crypto_Deinit(whal_Crypto *cryptoDev);

/*
 * @brief Start a crypto operation.
 *
 * Configures the hardware for the requested algorithm, loads keys and
 * parameters, and performs any setup (e.g. AAD processing for AEAD modes).
 *
 * @param cryptoDev Pointer to the crypto instance.
 * @param opId      Operation identifier (e.g. WHAL_CRYPTO_AES_GCM).
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Operation started.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Operation not implemented by this driver.
 * @retval WHAL_EHARDWARE Hardware error during setup.
 */
whal_Error whal_Crypto_StartOp(whal_Crypto *cryptoDev, size_t opId,
                               void *opArgs);

/*
 * @brief Process data through an active crypto operation.
 *
 * Feeds data through the hardware. May be called multiple times for
 * streaming. Optional for single-shot operations.
 *
 * @param cryptoDev Pointer to the crypto instance.
 * @param opId      Operation identifier.
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Data processed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Operation not implemented by this driver.
 * @retval WHAL_EHARDWARE Hardware error during processing.
 */
whal_Error whal_Crypto_Process(whal_Crypto *cryptoDev, size_t opId,
                               void *opArgs);

/*
 * @brief End a crypto operation.
 *
 * Finalizes the operation, reads output (tag, digest, signature), and
 * releases the hardware.
 *
 * @param cryptoDev Pointer to the crypto instance.
 * @param opId      Operation identifier.
 * @param opArgs    Pointer to the algorithm-specific arguments struct.
 *
 * @retval WHAL_SUCCESS   Operation finalized.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTSUP   Operation not implemented by this driver.
 * @retval WHAL_EHARDWARE Hardware error during finalization.
 */
whal_Error whal_Crypto_EndOp(whal_Crypto *cryptoDev, size_t opId,
                             void *opArgs);
#endif

/* ---- Per-algorithm inline wrappers ---- */
#ifdef WHAL_CFG_CRYPTO_AES_ECB

/* AES-ECB */

static inline whal_Error whal_Crypto_AesEcb(whal_Crypto *dev,
                                            whal_Crypto_AesEcbArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_ECB, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_AES_ECB, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_ECB, args);
}

static inline whal_Error whal_Crypto_AesEcb_Start(whal_Crypto *dev,
                                                  whal_Crypto_Dir dir,
                                                  const void *key, size_t keySz)
{
    whal_Crypto_AesEcbArgs args = {0};

    args.dir = dir;
    args.key = key;
    args.keySz = keySz;

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_ECB, &args);
}

static inline whal_Error whal_Crypto_AesEcb_Update(whal_Crypto *dev,
                                                   const void *in, void *out,
                                                   size_t sz)
{
    whal_Crypto_AesEcbArgs args = {0};

    args.in = in;
    args.out = out;
    args.sz = sz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_AES_ECB, &args);
}

static inline whal_Error whal_Crypto_AesEcb_Finalize(whal_Crypto *dev)
{
    whal_Crypto_AesEcbArgs args = {0};

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_ECB, &args);
}
#endif /* WHAL_CFG_CRYPTO_AES_ECB */

#ifdef WHAL_CFG_CRYPTO_AES_CBC

/* AES-CBC */

static inline whal_Error whal_Crypto_AesCbc(whal_Crypto *dev,
                                            whal_Crypto_AesCbcArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_CBC, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_AES_CBC, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_CBC, args);
}

static inline whal_Error whal_Crypto_AesCbc_Start(whal_Crypto *dev,
                                                  whal_Crypto_Dir dir,
                                                  const void *key, size_t keySz,
                                                  const void *iv)
{
    whal_Crypto_AesCbcArgs args = {0};

    args.dir = dir;
    args.key = key;
    args.keySz = keySz;
    args.iv = iv;

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_CBC, &args);
}

static inline whal_Error whal_Crypto_AesCbc_Update(whal_Crypto *dev,
                                                   const void *in, void *out,
                                                   size_t sz)
{
    whal_Crypto_AesCbcArgs args = {0};

    args.in = in;
    args.out = out;
    args.sz = sz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_AES_CBC, &args);
}

static inline whal_Error whal_Crypto_AesCbc_Finalize(whal_Crypto *dev)
{
    whal_Crypto_AesCbcArgs args = {0};

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_CBC, &args);
}
#endif /* WHAL_CFG_CRYPTO_AES_CBC */

#ifdef WHAL_CFG_CRYPTO_AES_CTR

/* AES-CTR */

static inline whal_Error whal_Crypto_AesCtr(whal_Crypto *dev,
                                            whal_Crypto_AesCtrArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_CTR, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_AES_CTR, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_CTR, args);
}

static inline whal_Error whal_Crypto_AesCtr_Start(whal_Crypto *dev,
                                                  whal_Crypto_Dir dir,
                                                  const void *key, size_t keySz,
                                                  const void *iv)
{
    whal_Crypto_AesCtrArgs args = {0};

    args.dir = dir;
    args.key = key;
    args.keySz = keySz;
    args.iv = iv;

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_CTR, &args);
}

static inline whal_Error whal_Crypto_AesCtr_Update(whal_Crypto *dev,
                                                   const void *in, void *out,
                                                   size_t sz)
{
    whal_Crypto_AesCtrArgs args = {0};

    args.in = in;
    args.out = out;
    args.sz = sz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_AES_CTR, &args);
}

static inline whal_Error whal_Crypto_AesCtr_Finalize(whal_Crypto *dev)
{
    whal_Crypto_AesCtrArgs args = {0};

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_CTR, &args);
}
#endif /* WHAL_CFG_CRYPTO_AES_CTR */

#ifdef WHAL_CFG_CRYPTO_AES_GCM

/* AES-GCM */

static inline whal_Error whal_Crypto_AesGcm(whal_Crypto *dev,
                                            whal_Crypto_AesGcmArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_GCM, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_AES_GCM, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_GCM, args);
}

static inline whal_Error whal_Crypto_AesGcm_Start(whal_Crypto *dev,
                                                  whal_Crypto_Dir dir,
                                                  const void *key, size_t keySz,
                                                  const void *iv, size_t ivSz,
                                                  const void *aad, size_t aadSz)
{
    whal_Crypto_AesGcmArgs args = {0};

    args.dir = dir;
    args.key = key;
    args.keySz = keySz;
    args.iv = iv;
    args.ivSz = ivSz;
    args.aad = aad;
    args.aadSz = aadSz;

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_GCM, &args);
}

static inline whal_Error whal_Crypto_AesGcm_Update(whal_Crypto *dev,
                                                   whal_Crypto_Dir dir,
                                                   const void *in, void *out,
                                                   size_t sz, size_t aadSz)
{
    whal_Crypto_AesGcmArgs args = {0};

    args.dir = dir;
    args.in = in;
    args.out = out;
    args.sz = sz;
    args.aadSz = aadSz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_AES_GCM, &args);
}

static inline whal_Error whal_Crypto_AesGcm_Finalize(whal_Crypto *dev,
                                                     void *tag, size_t tagSz,
                                                     size_t aadSz, size_t sz)
{
    whal_Crypto_AesGcmArgs args = {0};

    args.tag = tag;
    args.tagSz = tagSz;
    args.aadSz = aadSz;
    args.sz = sz;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_GCM, &args);
}
#endif /* WHAL_CFG_CRYPTO_AES_GCM */

#ifdef WHAL_CFG_CRYPTO_AES_GMAC

/* AES-GMAC */

static inline whal_Error whal_Crypto_AesGmac(whal_Crypto *dev,
                                             whal_Crypto_AesGmacArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_GMAC, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_AES_GMAC, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_GMAC, args);
}
#endif /* WHAL_CFG_CRYPTO_AES_GMAC */

#ifdef WHAL_CFG_CRYPTO_AES_CCM

/* AES-CCM */

static inline whal_Error whal_Crypto_AesCcm(whal_Crypto *dev,
                                            whal_Crypto_AesCcmArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_CCM, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_AES_CCM, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_CCM, args);
}

static inline whal_Error whal_Crypto_AesCcm_Start(whal_Crypto *dev,
                                                  whal_Crypto_Dir dir,
                                                  const void *key, size_t keySz,
                                                  const void *nonce,
                                                  size_t nonceSz,
                                                  const void *aad, size_t aadSz,
                                                  size_t tagSz, size_t sz)
{
    whal_Crypto_AesCcmArgs args = {0};

    args.dir = dir;
    args.key = key;
    args.keySz = keySz;
    args.nonce = nonce;
    args.nonceSz = nonceSz;
    args.aad = aad;
    args.aadSz = aadSz;
    args.tagSz = tagSz;
    args.sz = sz;

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_AES_CCM, &args);
}

static inline whal_Error whal_Crypto_AesCcm_Update(whal_Crypto *dev,
                                                   whal_Crypto_Dir dir,
                                                   const void *in, void *out,
                                                   size_t sz, size_t aadSz)
{
    whal_Crypto_AesCcmArgs args = {0};

    args.dir = dir;
    args.in = in;
    args.out = out;
    args.sz = sz;
    args.aadSz = aadSz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_AES_CCM, &args);
}

static inline whal_Error whal_Crypto_AesCcm_Finalize(whal_Crypto *dev,
                                                     void *tag, size_t tagSz,
                                                     size_t aadSz, size_t sz)
{
    whal_Crypto_AesCcmArgs args = {0};

    args.tag = tag;
    args.tagSz = tagSz;
    args.aadSz = aadSz;
    args.sz = sz;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_AES_CCM, &args);
}
#endif /* WHAL_CFG_CRYPTO_AES_CCM */

#ifdef WHAL_CFG_CRYPTO_SHA1

/* SHA-1 */

static inline whal_Error whal_Crypto_Sha1(whal_Crypto *dev,
                                          whal_Crypto_HashArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_SHA1, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_SHA1, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_SHA1, args);
}

static inline whal_Error whal_Crypto_Sha1_Start(whal_Crypto *dev)
{
    whal_Crypto_HashArgs args = {0};

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_SHA1, &args);
}

static inline whal_Error whal_Crypto_Sha1_Update(whal_Crypto *dev,
                                                 const void *in, size_t inSz)
{
    whal_Crypto_HashArgs args = {0};

    args.in = in;
    args.inSz = inSz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_SHA1, &args);
}

static inline whal_Error whal_Crypto_Sha1_Finalize(whal_Crypto *dev,
                                                   void *digest,
                                                   size_t digestSz)
{
    whal_Crypto_HashArgs args = {0};

    args.digest = digest;
    args.digestSz = digestSz;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_SHA1, &args);
}
#endif /* WHAL_CFG_CRYPTO_SHA1 */

#ifdef WHAL_CFG_CRYPTO_SHA224

/* SHA-224 */

static inline whal_Error whal_Crypto_Sha224(whal_Crypto *dev,
                                            whal_Crypto_HashArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_SHA224, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_SHA224, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_SHA224, args);
}

static inline whal_Error whal_Crypto_Sha224_Start(whal_Crypto *dev)
{
    whal_Crypto_HashArgs args = {0};

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_SHA224, &args);
}

static inline whal_Error whal_Crypto_Sha224_Update(whal_Crypto *dev,
                                                   const void *in, size_t inSz)
{
    whal_Crypto_HashArgs args = {0};

    args.in = in;
    args.inSz = inSz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_SHA224, &args);
}

static inline whal_Error whal_Crypto_Sha224_Finalize(whal_Crypto *dev,
                                                     void *digest,
                                                     size_t digestSz)
{
    whal_Crypto_HashArgs args = {0};

    args.digest = digest;
    args.digestSz = digestSz;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_SHA224, &args);
}
#endif /* WHAL_CFG_CRYPTO_SHA224 */

#ifdef WHAL_CFG_CRYPTO_SHA256

/* SHA-256 */

static inline whal_Error whal_Crypto_Sha256(whal_Crypto *dev,
                                            whal_Crypto_HashArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_SHA256, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_SHA256, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_SHA256, args);
}

static inline whal_Error whal_Crypto_Sha256_Start(whal_Crypto *dev)
{
    whal_Crypto_HashArgs args = {0};

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_SHA256, &args);
}

static inline whal_Error whal_Crypto_Sha256_Update(whal_Crypto *dev,
                                                   const void *in, size_t inSz)
{
    whal_Crypto_HashArgs args = {0};

    args.in = in;
    args.inSz = inSz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_SHA256, &args);
}

static inline whal_Error whal_Crypto_Sha256_Finalize(whal_Crypto *dev,
                                                     void *digest,
                                                     size_t digestSz)
{
    whal_Crypto_HashArgs args = {0};

    args.digest = digest;
    args.digestSz = digestSz;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_SHA256, &args);
}
#endif /* WHAL_CFG_CRYPTO_SHA256 */

#ifdef WHAL_CFG_CRYPTO_HMAC_SHA1

/* HMAC-SHA-1 */

static inline whal_Error whal_Crypto_HmacSha1(whal_Crypto *dev,
                                              whal_Crypto_HmacArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_HMAC_SHA1, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_HMAC_SHA1, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_HMAC_SHA1, args);
}

static inline whal_Error whal_Crypto_HmacSha1_Start(whal_Crypto *dev,
                                                    const void *key,
                                                    size_t keySz)
{
    whal_Crypto_HmacArgs args = {0};

    args.key = key;
    args.keySz = keySz;

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_HMAC_SHA1, &args);
}

static inline whal_Error whal_Crypto_HmacSha1_Update(whal_Crypto *dev,
                                                     const void *in,
                                                     size_t inSz)
{
    whal_Crypto_HmacArgs args = {0};

    args.in = in;
    args.inSz = inSz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_HMAC_SHA1, &args);
}

static inline whal_Error whal_Crypto_HmacSha1_Finalize(whal_Crypto *dev,
                                                       const void *key,
                                                       size_t keySz,
                                                       void *digest,
                                                       size_t digestSz)
{
    whal_Crypto_HmacArgs args = {0};

    args.key = key;
    args.keySz = keySz;
    args.digest = digest;
    args.digestSz = digestSz;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_HMAC_SHA1, &args);
}
#endif /* WHAL_CFG_CRYPTO_HMAC_SHA1 */

#ifdef WHAL_CFG_CRYPTO_HMAC_SHA224

/* HMAC-SHA-224 */

static inline whal_Error whal_Crypto_HmacSha224(whal_Crypto *dev,
                                                whal_Crypto_HmacArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_HMAC_SHA224, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_HMAC_SHA224, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_HMAC_SHA224, args);
}

static inline whal_Error whal_Crypto_HmacSha224_Start(whal_Crypto *dev,
                                                      const void *key,
                                                      size_t keySz)
{
    whal_Crypto_HmacArgs args = {0};

    args.key = key;
    args.keySz = keySz;

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_HMAC_SHA224, &args);
}

static inline whal_Error whal_Crypto_HmacSha224_Update(whal_Crypto *dev,
                                                       const void *in,
                                                       size_t inSz)
{
    whal_Crypto_HmacArgs args = {0};

    args.in = in;
    args.inSz = inSz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_HMAC_SHA224, &args);
}

static inline whal_Error whal_Crypto_HmacSha224_Finalize(whal_Crypto *dev,
                                                         const void *key,
                                                         size_t keySz,
                                                         void *digest,
                                                         size_t digestSz)
{
    whal_Crypto_HmacArgs args = {0};

    args.key = key;
    args.keySz = keySz;
    args.digest = digest;
    args.digestSz = digestSz;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_HMAC_SHA224, &args);
}
#endif /* WHAL_CFG_CRYPTO_HMAC_SHA224 */

#ifdef WHAL_CFG_CRYPTO_HMAC_SHA256

/* HMAC-SHA-256 */

static inline whal_Error whal_Crypto_HmacSha256(whal_Crypto *dev,
                                                whal_Crypto_HmacArgs *args)
{
    whal_Error err;

    err = whal_Crypto_StartOp(dev, WHAL_CRYPTO_HMAC_SHA256, args);
    if (err)
        return err;

    err = whal_Crypto_Process(dev, WHAL_CRYPTO_HMAC_SHA256, args);
    if (err)
        return err;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_HMAC_SHA256, args);
}

static inline whal_Error whal_Crypto_HmacSha256_Start(whal_Crypto *dev,
                                                      const void *key,
                                                      size_t keySz)
{
    whal_Crypto_HmacArgs args = {0};

    args.key = key;
    args.keySz = keySz;

    return whal_Crypto_StartOp(dev, WHAL_CRYPTO_HMAC_SHA256, &args);
}

static inline whal_Error whal_Crypto_HmacSha256_Update(whal_Crypto *dev,
                                                       const void *in,
                                                       size_t inSz)
{
    whal_Crypto_HmacArgs args = {0};

    args.in = in;
    args.inSz = inSz;

    return whal_Crypto_Process(dev, WHAL_CRYPTO_HMAC_SHA256, &args);
}

static inline whal_Error whal_Crypto_HmacSha256_Finalize(whal_Crypto *dev,
                                                         const void *key,
                                                         size_t keySz,
                                                         void *digest,
                                                         size_t digestSz)
{
    whal_Crypto_HmacArgs args = {0};

    args.key = key;
    args.keySz = keySz;
    args.digest = digest;
    args.digestSz = digestSz;

    return whal_Crypto_EndOp(dev, WHAL_CRYPTO_HMAC_SHA256, &args);
}

#endif /* WHAL_CFG_CRYPTO_HMAC_SHA256 */

#endif /* WHAL_CRYPTO_H */
