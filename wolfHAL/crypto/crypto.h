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

/* ---- Per-algorithm argument structs ---- */

/*
 * @brief Arguments for AES-ECB.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const uint8_t  *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const uint8_t  *in;
    uint8_t        *out;
    size_t          sz;       /* Must be a multiple of 16 */
} whal_Crypto_AesEcbArgs;

/*
 * @brief Arguments for AES-CBC.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const uint8_t  *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const uint8_t  *iv;       /* 16 bytes */
    const uint8_t  *in;
    uint8_t        *out;
    size_t          sz;       /* Must be a multiple of 16 */
} whal_Crypto_AesCbcArgs;

/*
 * @brief Arguments for AES-CTR.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const uint8_t  *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const uint8_t  *iv;       /* 16 bytes (initial counter block) */
    const uint8_t  *in;
    uint8_t        *out;
    size_t          sz;
} whal_Crypto_AesCtrArgs;

/*
 * @brief Arguments for AES-GCM.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const uint8_t  *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const uint8_t  *iv;
    size_t          ivSz;     /* Typically 12 */
    const uint8_t  *in;
    uint8_t        *out;
    size_t          sz;
    const uint8_t  *aad;
    size_t          aadSz;
    uint8_t        *tag;
    size_t          tagSz;    /* Up to 16 */
} whal_Crypto_AesGcmArgs;

/*
 * @brief Arguments for AES-CCM.
 */
typedef struct {
    whal_Crypto_Dir dir;
    const uint8_t  *key;
    size_t          keySz;    /* 16, 24, or 32 */
    const uint8_t  *nonce;
    size_t          nonceSz;  /* 7-13 */
    const uint8_t  *in;
    uint8_t        *out;
    size_t          sz;
    const uint8_t  *aad;
    size_t          aadSz;
    uint8_t        *tag;
    size_t          tagSz;    /* 4, 6, 8, 10, 12, 14, or 16 */
} whal_Crypto_AesCcmArgs;

/*
 * @brief Arguments for AES-GMAC (authentication only, no payload).
 */
typedef struct {
    const uint8_t *key;
    size_t         keySz;    /* 16, 24, or 32 */
    const uint8_t *iv;
    size_t         ivSz;     /* Typically 12 */
    const uint8_t *aad;
    size_t         aadSz;
    uint8_t       *tag;
    size_t         tagSz;    /* Up to 16 */
} whal_Crypto_AesGmacArgs;

/* ---- Device and driver ---- */

typedef struct whal_Crypto whal_Crypto;

/*
 * @brief Operation function pointer type for per-device ops tables.
 */
typedef whal_Error (*whal_Crypto_OpFunc)(whal_Crypto *cryptoDev, void *opArgs);

/*
 * @brief Driver vtable for crypto devices.
 */
typedef struct {
    /* Initialize the crypto hardware. */
    whal_Error (*Init)(whal_Crypto *cryptoDev);
    /* Deinitialize the crypto hardware. */
    whal_Error (*Deinit)(whal_Crypto *cryptoDev);
} whal_CryptoDriver;

/*
 * @brief Crypto device instance tying a register map, driver, and ops table.
 */
struct whal_Crypto {
    const whal_Regmap regmap;
    const whal_CryptoDriver *driver;
    const whal_Crypto_OpFunc *ops;
    size_t opsCount;
    const void *cfg;
};

#ifdef WHAL_CFG_DIRECT_CALLBACKS
#define whal_Crypto_Init(cryptoDev) ((cryptoDev)->driver->Init((cryptoDev)))
#define whal_Crypto_Deinit(cryptoDev) ((cryptoDev)->driver->Deinit((cryptoDev)))
#define whal_Crypto_Op(cryptoDev, op, opArgs) ((cryptoDev)->ops[(op)]((cryptoDev), (opArgs)))
#else
/*
 * @brief Initializes a crypto device and its driver.
 *
 * @param cryptoDev Pointer to the crypto instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or driver rejected configuration.
 */
whal_Error whal_Crypto_Init(whal_Crypto *cryptoDev);

/*
 * @brief Deinitializes a crypto device and releases resources.
 *
 * @param cryptoDev Pointer to the crypto instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer or driver refused to deinit.
 */
whal_Error whal_Crypto_Deinit(whal_Crypto *cryptoDev);

/*
 * @brief Perform a crypto operation.
 *
 * @param cryptoDev Pointer to the crypto instance.
 * @param op        Operation index into the platform-specific ops table.
 * @param opArgs    Platform-specific operation arguments.
 *
 * @retval WHAL_SUCCESS Operation completed.
 * @retval WHAL_EINVAL  Null pointer, invalid op, or driver failed.
 */
whal_Error whal_Crypto_Op(whal_Crypto *cryptoDev, size_t op, void *opArgs);
#endif

#endif /* WHAL_CRYPTO_H */
