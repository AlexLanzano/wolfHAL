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
};

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

#endif /* WHAL_CRYPTO_H */
