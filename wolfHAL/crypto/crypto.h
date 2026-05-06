#ifndef WHAL_CRYPTO_H
#define WHAL_CRYPTO_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/error.h>

/**
 * @file crypto.h
 * @brief Crypto accelerator device handle and per-algorithm API.
 *
 * whal_Crypto is a platform driver representing a crypto hardware peripheral.
 * It provides Init/Deinit for hardware lifecycle. Each algorithm gets its own
 * typed device struct (whal_AesGcm, whal_AesEcb, etc.) that references the
 * underlying whal_Crypto device and a per-algorithm driver vtable.
 */

/* ---- Common enums ---- */

typedef enum {
    WHAL_CRYPTO_ENCRYPT,
    WHAL_CRYPTO_DECRYPT,
} whal_Crypto_Dir;

/* ---- Hardware device (platform driver) ---- */

typedef struct whal_Crypto whal_Crypto;

/**
 * @brief Crypto hardware driver vtable (Init/Deinit only).
 */
typedef struct {
    whal_Error (*Init)(whal_Crypto *dev);
    whal_Error (*Deinit)(whal_Crypto *dev);
} whal_CryptoDriver;

/**
 * @brief Crypto hardware device instance.
 */
struct whal_Crypto {
    const size_t base;
    const whal_CryptoDriver *driver;
    const void *cfg;
};

/**
 * @brief Initialize a crypto hardware device.
 *
 * @param dev Crypto device instance.
 */
whal_Error whal_Crypto_Init(whal_Crypto *dev);

/**
 * @brief Deinitialize a crypto hardware device.
 *
 * @param dev Crypto device instance.
 */
whal_Error whal_Crypto_Deinit(whal_Crypto *dev);

/* ---- Per-algorithm device structs and vtables ---- */

/* AES-ECB */

typedef struct whal_AesEcb whal_AesEcb;

typedef struct {
    whal_Error (*Oneshot)(whal_AesEcb *dev, whal_Crypto_Dir dir,
                          const void *key, size_t keySz,
                          const void *in, void *out, size_t sz);
    whal_Error (*Start)(whal_AesEcb *dev, whal_Crypto_Dir dir,
                        const void *key, size_t keySz);
    whal_Error (*Process)(whal_AesEcb *dev,
                          const void *in, void *out, size_t sz);
} whal_AesEcbDriver;

struct whal_AesEcb {
    whal_Crypto *crypto;
    const whal_AesEcbDriver *driver;
};

whal_Error whal_AesEcb_Oneshot(whal_AesEcb *dev, whal_Crypto_Dir dir,
                               const void *key, size_t keySz,
                               const void *in, void *out, size_t sz);
whal_Error whal_AesEcb_Start(whal_AesEcb *dev, whal_Crypto_Dir dir,
                             const void *key, size_t keySz);
whal_Error whal_AesEcb_Process(whal_AesEcb *dev,
                               const void *in, void *out, size_t sz);

/* AES-CBC */

typedef struct whal_AesCbc whal_AesCbc;

typedef struct {
    whal_Error (*Oneshot)(whal_AesCbc *dev, whal_Crypto_Dir dir,
                          const void *key, size_t keySz,
                          const void *iv,
                          const void *in, void *out, size_t sz);
    whal_Error (*Start)(whal_AesCbc *dev, whal_Crypto_Dir dir,
                        const void *key, size_t keySz,
                        const void *iv);
    whal_Error (*Process)(whal_AesCbc *dev,
                          const void *in, void *out, size_t sz);
} whal_AesCbcDriver;

struct whal_AesCbc {
    whal_Crypto *crypto;
    const whal_AesCbcDriver *driver;
};

whal_Error whal_AesCbc_Oneshot(whal_AesCbc *dev, whal_Crypto_Dir dir,
                               const void *key, size_t keySz,
                               const void *iv,
                               const void *in, void *out, size_t sz);
whal_Error whal_AesCbc_Start(whal_AesCbc *dev, whal_Crypto_Dir dir,
                             const void *key, size_t keySz,
                             const void *iv);
whal_Error whal_AesCbc_Process(whal_AesCbc *dev,
                               const void *in, void *out, size_t sz);

/* AES-CTR */

typedef struct whal_AesCtr whal_AesCtr;

typedef struct {
    whal_Error (*Oneshot)(whal_AesCtr *dev, whal_Crypto_Dir dir,
                          const void *key, size_t keySz,
                          const void *iv,
                          const void *in, void *out, size_t sz);
    whal_Error (*Start)(whal_AesCtr *dev, whal_Crypto_Dir dir,
                        const void *key, size_t keySz,
                        const void *iv);
    whal_Error (*Process)(whal_AesCtr *dev,
                          const void *in, void *out, size_t sz);
} whal_AesCtrDriver;

struct whal_AesCtr {
    whal_Crypto *crypto;
    const whal_AesCtrDriver *driver;
};

whal_Error whal_AesCtr_Oneshot(whal_AesCtr *dev, whal_Crypto_Dir dir,
                               const void *key, size_t keySz,
                               const void *iv,
                               const void *in, void *out, size_t sz);
whal_Error whal_AesCtr_Start(whal_AesCtr *dev, whal_Crypto_Dir dir,
                             const void *key, size_t keySz,
                             const void *iv);
whal_Error whal_AesCtr_Process(whal_AesCtr *dev,
                               const void *in, void *out, size_t sz);

/* AES-GCM */

typedef struct whal_AesGcm whal_AesGcm;

typedef struct {
    whal_Error (*Oneshot)(whal_AesGcm *dev, whal_Crypto_Dir dir,
                          const void *key, size_t keySz,
                          const void *iv, size_t ivSz,
                          const void *aad, size_t aadSz,
                          const void *in, void *out, size_t sz,
                          void *tag, size_t tagSz);
    whal_Error (*Start)(whal_AesGcm *dev, whal_Crypto_Dir dir,
                        const void *key, size_t keySz,
                        const void *iv, size_t ivSz,
                        const void *aad, size_t aadSz);
    whal_Error (*Process)(whal_AesGcm *dev,
                          const void *in, void *out, size_t sz);
    whal_Error (*Finalize)(whal_AesGcm *dev,
                           void *tag, size_t tagSz);
} whal_AesGcmDriver;

struct whal_AesGcm {
    whal_Crypto *crypto;
    const whal_AesGcmDriver *driver;
    void *state;
};

whal_Error whal_AesGcm_Oneshot(whal_AesGcm *dev, whal_Crypto_Dir dir,
                               const void *key, size_t keySz,
                               const void *iv, size_t ivSz,
                               const void *aad, size_t aadSz,
                               const void *in, void *out, size_t sz,
                               void *tag, size_t tagSz);
whal_Error whal_AesGcm_Start(whal_AesGcm *dev, whal_Crypto_Dir dir,
                             const void *key, size_t keySz,
                             const void *iv, size_t ivSz,
                             const void *aad, size_t aadSz);
whal_Error whal_AesGcm_Process(whal_AesGcm *dev,
                               const void *in, void *out, size_t sz);
whal_Error whal_AesGcm_Finalize(whal_AesGcm *dev,
                                void *tag, size_t tagSz);

/* AES-GMAC */

typedef struct whal_AesGmac whal_AesGmac;

typedef struct {
    whal_Error (*Oneshot)(whal_AesGmac *dev,
                          const void *key, size_t keySz,
                          const void *iv, size_t ivSz,
                          const void *aad, size_t aadSz,
                          void *tag, size_t tagSz);
} whal_AesGmacDriver;

struct whal_AesGmac {
    whal_Crypto *crypto;
    const whal_AesGmacDriver *driver;
};

whal_Error whal_AesGmac_Oneshot(whal_AesGmac *dev,
                                const void *key, size_t keySz,
                                const void *iv, size_t ivSz,
                                const void *aad, size_t aadSz,
                                void *tag, size_t tagSz);

/* AES-CCM */

typedef struct whal_AesCcm whal_AesCcm;

typedef struct {
    whal_Error (*Oneshot)(whal_AesCcm *dev, whal_Crypto_Dir dir,
                          const void *key, size_t keySz,
                          const void *nonce, size_t nonceSz,
                          const void *aad, size_t aadSz,
                          const void *in, void *out, size_t sz,
                          void *tag, size_t tagSz);
    whal_Error (*Start)(whal_AesCcm *dev, whal_Crypto_Dir dir,
                        const void *key, size_t keySz,
                        const void *nonce, size_t nonceSz,
                        const void *aad, size_t aadSz,
                        size_t tagSz, size_t sz);
    whal_Error (*Process)(whal_AesCcm *dev,
                          const void *in, void *out, size_t sz);
    whal_Error (*Finalize)(whal_AesCcm *dev,
                           void *tag, size_t tagSz);
} whal_AesCcmDriver;

struct whal_AesCcm {
    whal_Crypto *crypto;
    const whal_AesCcmDriver *driver;
    void *state;
};

whal_Error whal_AesCcm_Oneshot(whal_AesCcm *dev, whal_Crypto_Dir dir,
                               const void *key, size_t keySz,
                               const void *nonce, size_t nonceSz,
                               const void *aad, size_t aadSz,
                               const void *in, void *out, size_t sz,
                               void *tag, size_t tagSz);
whal_Error whal_AesCcm_Start(whal_AesCcm *dev, whal_Crypto_Dir dir,
                             const void *key, size_t keySz,
                             const void *nonce, size_t nonceSz,
                             const void *aad, size_t aadSz,
                             size_t tagSz, size_t sz);
whal_Error whal_AesCcm_Process(whal_AesCcm *dev,
                               const void *in, void *out, size_t sz);
whal_Error whal_AesCcm_Finalize(whal_AesCcm *dev,
                                void *tag, size_t tagSz);

/* SHA-1 */

typedef struct whal_Sha1 whal_Sha1;

typedef struct {
    whal_Error (*Oneshot)(whal_Sha1 *dev,
                          const void *in, size_t inSz,
                          void *digest, size_t digestSz);
    whal_Error (*Start)(whal_Sha1 *dev);
    whal_Error (*Process)(whal_Sha1 *dev, const void *in, size_t inSz);
    whal_Error (*Finalize)(whal_Sha1 *dev, void *digest, size_t digestSz);
} whal_Sha1Driver;

struct whal_Sha1 {
    whal_Crypto *crypto;
    const whal_Sha1Driver *driver;
};

whal_Error whal_Sha1_Oneshot(whal_Sha1 *dev,
                             const void *in, size_t inSz,
                             void *digest, size_t digestSz);
whal_Error whal_Sha1_Start(whal_Sha1 *dev);
whal_Error whal_Sha1_Process(whal_Sha1 *dev, const void *in, size_t inSz);
whal_Error whal_Sha1_Finalize(whal_Sha1 *dev, void *digest, size_t digestSz);

/* SHA-224 */

typedef struct whal_Sha224 whal_Sha224;

typedef struct {
    whal_Error (*Oneshot)(whal_Sha224 *dev,
                          const void *in, size_t inSz,
                          void *digest, size_t digestSz);
    whal_Error (*Start)(whal_Sha224 *dev);
    whal_Error (*Process)(whal_Sha224 *dev, const void *in, size_t inSz);
    whal_Error (*Finalize)(whal_Sha224 *dev, void *digest, size_t digestSz);
} whal_Sha224Driver;

struct whal_Sha224 {
    whal_Crypto *crypto;
    const whal_Sha224Driver *driver;
};

whal_Error whal_Sha224_Oneshot(whal_Sha224 *dev,
                               const void *in, size_t inSz,
                               void *digest, size_t digestSz);
whal_Error whal_Sha224_Start(whal_Sha224 *dev);
whal_Error whal_Sha224_Process(whal_Sha224 *dev, const void *in, size_t inSz);
whal_Error whal_Sha224_Finalize(whal_Sha224 *dev,
                                void *digest, size_t digestSz);

/* SHA-256 */

typedef struct whal_Sha256 whal_Sha256;

typedef struct {
    whal_Error (*Oneshot)(whal_Sha256 *dev,
                          const void *in, size_t inSz,
                          void *digest, size_t digestSz);
    whal_Error (*Start)(whal_Sha256 *dev);
    whal_Error (*Process)(whal_Sha256 *dev, const void *in, size_t inSz);
    whal_Error (*Finalize)(whal_Sha256 *dev, void *digest, size_t digestSz);
} whal_Sha256Driver;

struct whal_Sha256 {
    whal_Crypto *crypto;
    const whal_Sha256Driver *driver;
};

whal_Error whal_Sha256_Oneshot(whal_Sha256 *dev,
                               const void *in, size_t inSz,
                               void *digest, size_t digestSz);
whal_Error whal_Sha256_Start(whal_Sha256 *dev);
whal_Error whal_Sha256_Process(whal_Sha256 *dev, const void *in, size_t inSz);
whal_Error whal_Sha256_Finalize(whal_Sha256 *dev,
                                void *digest, size_t digestSz);

/* HMAC-SHA-1 */

typedef struct whal_HmacSha1 whal_HmacSha1;

typedef struct {
    whal_Error (*Oneshot)(whal_HmacSha1 *dev,
                          const void *key, size_t keySz,
                          const void *in, size_t inSz,
                          void *digest, size_t digestSz);
    whal_Error (*Start)(whal_HmacSha1 *dev,
                        const void *key, size_t keySz);
    whal_Error (*Process)(whal_HmacSha1 *dev,
                          const void *in, size_t inSz);
    whal_Error (*Finalize)(whal_HmacSha1 *dev,
                           void *digest, size_t digestSz);
} whal_HmacSha1Driver;

struct whal_HmacSha1 {
    whal_Crypto *crypto;
    const whal_HmacSha1Driver *driver;
    void *state;
};

whal_Error whal_HmacSha1_Oneshot(whal_HmacSha1 *dev,
                                 const void *key, size_t keySz,
                                 const void *in, size_t inSz,
                                 void *digest, size_t digestSz);
whal_Error whal_HmacSha1_Start(whal_HmacSha1 *dev,
                               const void *key, size_t keySz);
whal_Error whal_HmacSha1_Process(whal_HmacSha1 *dev,
                                 const void *in, size_t inSz);
whal_Error whal_HmacSha1_Finalize(whal_HmacSha1 *dev,
                                  void *digest, size_t digestSz);

/* HMAC-SHA-224 */

typedef struct whal_HmacSha224 whal_HmacSha224;

typedef struct {
    whal_Error (*Oneshot)(whal_HmacSha224 *dev,
                          const void *key, size_t keySz,
                          const void *in, size_t inSz,
                          void *digest, size_t digestSz);
    whal_Error (*Start)(whal_HmacSha224 *dev,
                        const void *key, size_t keySz);
    whal_Error (*Process)(whal_HmacSha224 *dev,
                          const void *in, size_t inSz);
    whal_Error (*Finalize)(whal_HmacSha224 *dev,
                           void *digest, size_t digestSz);
} whal_HmacSha224Driver;

struct whal_HmacSha224 {
    whal_Crypto *crypto;
    const whal_HmacSha224Driver *driver;
    void *state;
};

whal_Error whal_HmacSha224_Oneshot(whal_HmacSha224 *dev,
                                   const void *key, size_t keySz,
                                   const void *in, size_t inSz,
                                   void *digest, size_t digestSz);
whal_Error whal_HmacSha224_Start(whal_HmacSha224 *dev,
                                 const void *key, size_t keySz);
whal_Error whal_HmacSha224_Process(whal_HmacSha224 *dev,
                                   const void *in, size_t inSz);
whal_Error whal_HmacSha224_Finalize(whal_HmacSha224 *dev,
                                    void *digest, size_t digestSz);

/* HMAC-SHA-256 */

typedef struct whal_HmacSha256 whal_HmacSha256;

typedef struct {
    whal_Error (*Oneshot)(whal_HmacSha256 *dev,
                          const void *key, size_t keySz,
                          const void *in, size_t inSz,
                          void *digest, size_t digestSz);
    whal_Error (*Start)(whal_HmacSha256 *dev,
                        const void *key, size_t keySz);
    whal_Error (*Process)(whal_HmacSha256 *dev,
                          const void *in, size_t inSz);
    whal_Error (*Finalize)(whal_HmacSha256 *dev,
                           void *digest, size_t digestSz);
} whal_HmacSha256Driver;

struct whal_HmacSha256 {
    whal_Crypto *crypto;
    const whal_HmacSha256Driver *driver;
    void *state;
};

whal_Error whal_HmacSha256_Oneshot(whal_HmacSha256 *dev,
                                   const void *key, size_t keySz,
                                   const void *in, size_t inSz,
                                   void *digest, size_t digestSz);
whal_Error whal_HmacSha256_Start(whal_HmacSha256 *dev,
                                 const void *key, size_t keySz);
whal_Error whal_HmacSha256_Process(whal_HmacSha256 *dev,
                                   const void *in, size_t inSz);
whal_Error whal_HmacSha256_Finalize(whal_HmacSha256 *dev,
                                    void *digest, size_t digestSz);

#endif /* WHAL_CRYPTO_H */
