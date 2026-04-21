#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static const uint8_t key[32] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
};

static const uint8_t iv[16] = {
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
};

static const uint8_t nonce[12] = {
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
    0xB8, 0xB9, 0xBA, 0xBB,
};

static const uint8_t aad[16] = {
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
};

static const uint8_t plaintext[32] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE,
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
};

/* NIST SP 800-38A test vectors (AES-256, single block) */
static const uint8_t nistKey[32] = {
    0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
    0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
    0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
    0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4,
};

static const uint8_t nistPt[16] = {
    0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
    0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
};

#ifdef WHAL_CFG_CRYPTO_AES_ECB
/* NIST SP 800-38A F.1.5 AES-256-ECB expected ciphertext */
static const uint8_t nistEcbCt[16] = {
    0xF3, 0xEE, 0xD1, 0xBD, 0xB5, 0xD2, 0xA0, 0x3C,
    0x06, 0x4B, 0x5A, 0x7E, 0x3D, 0xB1, 0x81, 0xF8,
};

#endif /* WHAL_CFG_CRYPTO_AES_ECB */

#ifdef WHAL_CFG_CRYPTO_AES_CBC
/* NIST SP 800-38A F.2.5 AES-256-CBC expected ciphertext */
static const uint8_t nistCbcIv[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};

static const uint8_t nistCbcCt[16] = {
    0xF5, 0x8C, 0x4C, 0x04, 0xD6, 0xE5, 0xF1, 0xBA,
    0x77, 0x9E, 0xAB, 0xFB, 0x5F, 0x7B, 0xFB, 0xD6,
};

#endif /* WHAL_CFG_CRYPTO_AES_CBC */

#ifdef WHAL_CFG_CRYPTO_AES_CTR
/* NIST SP 800-38A F.5.5 AES-256-CTR expected ciphertext */
static const uint8_t nistCtrIv[16] = {
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static const uint8_t nistCtrCt[16] = {
    0x60, 0x1E, 0xC3, 0x13, 0x77, 0x57, 0x89, 0xA5,
    0xB7, 0xA7, 0xF5, 0x04, 0xBB, 0xF3, 0xD2, 0x28,
};

#endif /* WHAL_CFG_CRYPTO_AES_CTR */

#ifdef WHAL_CFG_CRYPTO_AES_GCM
/* GCM spec Test Case 15: AES-256-GCM, 64-byte payload, no AAD */
static const uint8_t gcmKey[32] = {
    0xFE, 0xFF, 0xE9, 0x92, 0x86, 0x65, 0x73, 0x1C,
    0x6D, 0x6A, 0x8F, 0x94, 0x67, 0x30, 0x83, 0x08,
    0xFE, 0xFF, 0xE9, 0x92, 0x86, 0x65, 0x73, 0x1C,
    0x6D, 0x6A, 0x8F, 0x94, 0x67, 0x30, 0x83, 0x08,
};

static const uint8_t gcmIv[12] = {
    0xCA, 0xFE, 0xBA, 0xBE, 0xFA, 0xCE, 0xDB, 0xAD,
    0xDE, 0xCA, 0xF8, 0x88,
};

static const uint8_t gcmPt[64] = {
    0xD9, 0x31, 0x32, 0x25, 0xF8, 0x84, 0x06, 0xE5,
    0xA5, 0x59, 0x09, 0xC5, 0xAF, 0xF5, 0x26, 0x9A,
    0x86, 0xA7, 0xA9, 0x53, 0x15, 0x34, 0xF7, 0xDA,
    0x2E, 0x4C, 0x30, 0x3D, 0x8A, 0x31, 0x8A, 0x72,
    0x1C, 0x3C, 0x0C, 0x95, 0x95, 0x68, 0x09, 0x53,
    0x2F, 0xCF, 0x0E, 0x24, 0x49, 0xA6, 0xB5, 0x25,
    0xB1, 0x6A, 0xED, 0xF5, 0xAA, 0x0D, 0xE6, 0x57,
    0xBA, 0x63, 0x7B, 0x39, 0x1A, 0xAF, 0xD2, 0x55,
};

static const uint8_t gcmCt[64] = {
    0x52, 0x2D, 0xC1, 0xF0, 0x99, 0x56, 0x7D, 0x07,
    0xF4, 0x7F, 0x37, 0xA3, 0x2A, 0x84, 0x42, 0x7D,
    0x64, 0x3A, 0x8C, 0xDC, 0xBF, 0xE5, 0xC0, 0xC9,
    0x75, 0x98, 0xA2, 0xBD, 0x25, 0x55, 0xD1, 0xAA,
    0x8C, 0xB0, 0x8E, 0x48, 0x59, 0x0D, 0xBB, 0x3D,
    0xA7, 0xB0, 0x8B, 0x10, 0x56, 0x82, 0x88, 0x38,
    0xC5, 0xF6, 0x1E, 0x63, 0x93, 0xBA, 0x7A, 0x0A,
    0xBC, 0xC9, 0xF6, 0x62, 0x89, 0x80, 0x15, 0xAD,
};

static const uint8_t gcmTag[16] = {
    0xB0, 0x94, 0xDA, 0xC5, 0xD9, 0x34, 0x71, 0xBD,
    0xEC, 0x1A, 0x50, 0x22, 0x70, 0xE3, 0xCC, 0x6C,
};

/* GCM spec Test Case 16: AES-256-GCM, 60-byte payload, 20-byte AAD.
 * Exercises partial last blocks on both AAD and payload. Reuses gcmKey/gcmIv. */
static const uint8_t gcm16Pt[60] = {
    0xD9, 0x31, 0x32, 0x25, 0xF8, 0x84, 0x06, 0xE5,
    0xA5, 0x59, 0x09, 0xC5, 0xAF, 0xF5, 0x26, 0x9A,
    0x86, 0xA7, 0xA9, 0x53, 0x15, 0x34, 0xF7, 0xDA,
    0x2E, 0x4C, 0x30, 0x3D, 0x8A, 0x31, 0x8A, 0x72,
    0x1C, 0x3C, 0x0C, 0x95, 0x95, 0x68, 0x09, 0x53,
    0x2F, 0xCF, 0x0E, 0x24, 0x49, 0xA6, 0xB5, 0x25,
    0xB1, 0x6A, 0xED, 0xF5, 0xAA, 0x0D, 0xE6, 0x57,
    0xBA, 0x63, 0x7B, 0x39,
};

static const uint8_t gcm16Aad[20] = {
    0xFE, 0xED, 0xFA, 0xCE, 0xDE, 0xAD, 0xBE, 0xEF,
    0xFE, 0xED, 0xFA, 0xCE, 0xDE, 0xAD, 0xBE, 0xEF,
    0xAB, 0xAD, 0xDA, 0xD2,
};

static const uint8_t gcm16Ct[60] = {
    0x52, 0x2D, 0xC1, 0xF0, 0x99, 0x56, 0x7D, 0x07,
    0xF4, 0x7F, 0x37, 0xA3, 0x2A, 0x84, 0x42, 0x7D,
    0x64, 0x3A, 0x8C, 0xDC, 0xBF, 0xE5, 0xC0, 0xC9,
    0x75, 0x98, 0xA2, 0xBD, 0x25, 0x55, 0xD1, 0xAA,
    0x8C, 0xB0, 0x8E, 0x48, 0x59, 0x0D, 0xBB, 0x3D,
    0xA7, 0xB0, 0x8B, 0x10, 0x56, 0x82, 0x88, 0x38,
    0xC5, 0xF6, 0x1E, 0x63, 0x93, 0xBA, 0x7A, 0x0A,
    0xBC, 0xC9, 0xF6, 0x62,
};

static const uint8_t gcm16Tag[16] = {
    0x76, 0xFC, 0x6E, 0xCE, 0x0F, 0x4E, 0x17, 0x68,
    0xCD, 0xDF, 0x88, 0x53, 0xBB, 0x2D, 0x55, 0x1B,
};

#endif /* WHAL_CFG_CRYPTO_AES_GCM */

#ifdef WHAL_CFG_CRYPTO_AES_GMAC
/* NIST CAVP gcmEncryptExtIV256.rsp: Keylen=256, IVlen=96, PTlen=0, AADlen=128, Taglen=128, Count=0 */
static const uint8_t gmacKey[32] = {
    0x78, 0xDC, 0x4E, 0x0A, 0xAF, 0x52, 0xD9, 0x35,
    0xC3, 0xC0, 0x1E, 0xEA, 0x57, 0x42, 0x8F, 0x00,
    0xCA, 0x1F, 0xD4, 0x75, 0xF5, 0xDA, 0x86, 0xA4,
    0x9C, 0x8D, 0xD7, 0x3D, 0x68, 0xC8, 0xE2, 0x23,
};

static const uint8_t gmacIv[12] = {
    0xD7, 0x9C, 0xF2, 0x2D, 0x50, 0x4C, 0xC7, 0x93,
    0xC3, 0xFB, 0x6C, 0x8A,
};

static const uint8_t gmacAad[16] = {
    0xB9, 0x6B, 0xAA, 0x8C, 0x1C, 0x75, 0xA6, 0x71,
    0xBF, 0xB2, 0xD0, 0x8D, 0x06, 0xBE, 0x5F, 0x36,
};

static const uint8_t gmacTag[16] = {
    0x3E, 0x5D, 0x48, 0x6A, 0xA2, 0xE3, 0x0B, 0x22,
    0xE0, 0x40, 0xB8, 0x57, 0x23, 0xA0, 0x6E, 0x76,
};

#endif /* WHAL_CFG_CRYPTO_AES_GMAC */

#ifdef WHAL_CFG_CRYPTO_AES_CCM
/* NIST CAVP DVPT256.rsp: Alen=32, Plen=24, Nlen=13, Tlen=16, Count=225 */
static const uint8_t ccmKey[32] = {
    0x31, 0x4A, 0x20, 0x2F, 0x83, 0x6F, 0x9F, 0x25,
    0x7E, 0x22, 0xD8, 0xC1, 0x17, 0x57, 0x83, 0x2A,
    0xE5, 0x13, 0x1D, 0x35, 0x7A, 0x72, 0xDF, 0x88,
    0xF3, 0xEF, 0xF0, 0xFF, 0xCE, 0xE0, 0xDA, 0x4E,
};

static const uint8_t ccmNonce[13] = {
    0xA5, 0x44, 0x21, 0x8D, 0xAD, 0xD3, 0xC1, 0x05,
    0x83, 0xDB, 0x49, 0xCF, 0x39,
};

static const uint8_t ccmAad[32] = {
    0x3C, 0x0E, 0x28, 0x15, 0xD3, 0x7D, 0x84, 0x4F,
    0x7A, 0xC2, 0x40, 0xBA, 0x9D, 0x6E, 0x3A, 0x0B,
    0x2A, 0x86, 0xF7, 0x06, 0xE8, 0x85, 0x95, 0x9E,
    0x09, 0xA1, 0x00, 0x5E, 0x02, 0x4F, 0x69, 0x07,
};

static const uint8_t ccmPt[24] = {
    0xE8, 0xDE, 0x97, 0x0F, 0x6E, 0xE8, 0xE8, 0x0E,
    0xDE, 0x93, 0x35, 0x81, 0xB5, 0xBC, 0xF4, 0xD8,
    0x37, 0xE2, 0xB7, 0x2B, 0xAA, 0x8B, 0x00, 0xC3,
};

static const uint8_t ccmCt[24] = {
    0x8D, 0x34, 0xCD, 0xCA, 0x37, 0xCE, 0x77, 0xBE,
    0x68, 0xF6, 0x5B, 0xAF, 0x33, 0x82, 0xE3, 0x1E,
    0xFA, 0x69, 0x3E, 0x63, 0xF9, 0x14, 0xA7, 0x81,
};

static const uint8_t ccmTag[16] = {
    0x36, 0x7F, 0x30, 0xF2, 0xEA, 0xAD, 0x8C, 0x06,
    0x3C, 0xA5, 0x07, 0x95, 0xAC, 0xD9, 0x02, 0x03,
};

#endif /* WHAL_CFG_CRYPTO_AES_CCM */

#ifdef WHAL_CFG_CRYPTO_AES_ECB
static void Test_Crypto_AesEcb_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};

    whal_Crypto_AesEcbArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .in = plaintext, .out = ct, .sz = sizeof(plaintext),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesEcb(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesEcbArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .in = ct, .out = pt, .sz = sizeof(ct),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesEcb(&g_whalCrypto, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
}

static void Test_Crypto_AesEcb_KnownAnswer(void)
{
    uint8_t ct[16] = {0};

    whal_Crypto_AesEcbArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = nistKey, .keySz = 32,
        .in = nistPt, .out = ct, .sz = sizeof(nistPt),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesEcb(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, nistEcbCt, sizeof(nistEcbCt));
}
#endif /* WHAL_CFG_CRYPTO_AES_ECB */

#ifdef WHAL_CFG_CRYPTO_AES_CBC
static void Test_Crypto_AesCbc_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};

    whal_Crypto_AesCbcArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .iv = iv, .in = plaintext, .out = ct, .sz = sizeof(plaintext),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCbc(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesCbcArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .iv = iv, .in = ct, .out = pt, .sz = sizeof(ct),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCbc(&g_whalCrypto, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
}

static void Test_Crypto_AesCbc_KnownAnswer(void)
{
    uint8_t ct[16] = {0};

    whal_Crypto_AesCbcArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = nistKey, .keySz = 32,
        .iv = nistCbcIv, .in = nistPt, .out = ct, .sz = sizeof(nistPt),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCbc(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, nistCbcCt, sizeof(nistCbcCt));
}
#endif /* WHAL_CFG_CRYPTO_AES_CBC */

#ifdef WHAL_CFG_CRYPTO_AES_CTR
static void Test_Crypto_AesCtr_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};

    whal_Crypto_AesCtrArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .iv = iv, .in = plaintext, .out = ct, .sz = sizeof(plaintext),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCtr(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesCtrArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .iv = iv, .in = ct, .out = pt, .sz = sizeof(ct),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCtr(&g_whalCrypto, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
}

static void Test_Crypto_AesCtr_KnownAnswer(void)
{
    uint8_t ct[16] = {0};

    whal_Crypto_AesCtrArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = nistKey, .keySz = 32,
        .iv = nistCtrIv, .in = nistPt, .out = ct, .sz = sizeof(nistPt),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCtr(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, nistCtrCt, sizeof(nistCtrCt));
}
#endif /* WHAL_CFG_CRYPTO_AES_CTR */

#ifdef WHAL_CFG_CRYPTO_AES_GCM
static void Test_Crypto_AesGcm_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};
    uint8_t encTag[16] = {0};
    uint8_t decTag[16] = {0};

    whal_Crypto_AesGcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .iv = nonce, .ivSz = sizeof(nonce),
        .in = plaintext, .out = ct, .sz = sizeof(plaintext),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = encTag, .tagSz = sizeof(encTag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesGcm(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesGcmArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .iv = nonce, .ivSz = sizeof(nonce),
        .in = ct, .out = pt, .sz = sizeof(ct),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = decTag, .tagSz = sizeof(decTag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesGcm(&g_whalCrypto, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
    WHAL_ASSERT_MEM_EQ(decTag, encTag, sizeof(encTag));
}

static void Test_Crypto_AesGcm_KnownAnswer(void)
{
    uint8_t ct[64] = {0};
    uint8_t tag[16] = {0};

    whal_Crypto_AesGcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = gcmKey, .keySz = 32,
        .iv = gcmIv, .ivSz = sizeof(gcmIv),
        .in = gcmPt, .out = ct, .sz = sizeof(gcmPt),
        .aad = NULL, .aadSz = 0,
        .tag = tag, .tagSz = sizeof(tag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesGcm(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, gcmCt, sizeof(gcmCt));
    WHAL_ASSERT_MEM_EQ(tag, gcmTag, sizeof(gcmTag));
}

static void Test_Crypto_AesGcm_KnownAnswer_PartialBlocks(void)
{
    uint8_t ct[sizeof(gcm16Pt)] = {0};
    uint8_t tag[16] = {0};

    whal_Crypto_AesGcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = gcmKey, .keySz = 32,
        .iv = gcmIv, .ivSz = sizeof(gcmIv),
        .in = gcm16Pt, .out = ct, .sz = sizeof(gcm16Pt),
        .aad = gcm16Aad, .aadSz = sizeof(gcm16Aad),
        .tag = tag, .tagSz = sizeof(tag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesGcm(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, gcm16Ct, sizeof(gcm16Ct));
    WHAL_ASSERT_MEM_EQ(tag, gcm16Tag, sizeof(gcm16Tag));
}
#endif /* WHAL_CFG_CRYPTO_AES_GCM */

#ifdef WHAL_CFG_CRYPTO_AES_GMAC
static void Test_Crypto_AesGmac_Basic(void)
{
    uint8_t tag1[16] = {0};
    uint8_t tag2[16] = {0};

    whal_Crypto_AesGmacArgs args1 = {
        .key = key, .keySz = 32,
        .iv = nonce, .ivSz = sizeof(nonce),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = tag1, .tagSz = sizeof(tag1),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesGmac(&g_whalCrypto, &args1),
                   WHAL_SUCCESS);

    whal_Crypto_AesGmacArgs args2 = {
        .key = key, .keySz = 32,
        .iv = nonce, .ivSz = sizeof(nonce),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = tag2, .tagSz = sizeof(tag2),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesGmac(&g_whalCrypto, &args2),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(tag1, tag2, sizeof(tag1));
}

static void Test_Crypto_AesGmac_KnownAnswer(void)
{
    uint8_t tag[16] = {0};

    whal_Crypto_AesGmacArgs args = {
        .key = gmacKey, .keySz = 32,
        .iv = gmacIv, .ivSz = sizeof(gmacIv),
        .aad = gmacAad, .aadSz = sizeof(gmacAad),
        .tag = tag, .tagSz = sizeof(tag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesGmac(&g_whalCrypto, &args),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(tag, gmacTag, sizeof(gmacTag));
}
#endif /* WHAL_CFG_CRYPTO_AES_GMAC */

#ifdef WHAL_CFG_CRYPTO_AES_CCM
static void Test_Crypto_AesCcm_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};
    uint8_t encTag[16] = {0};
    uint8_t decTag[16] = {0};

    whal_Crypto_AesCcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .nonce = nonce, .nonceSz = sizeof(nonce),
        .in = plaintext, .out = ct, .sz = sizeof(plaintext),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = encTag, .tagSz = sizeof(encTag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCcm(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesCcmArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .nonce = nonce, .nonceSz = sizeof(nonce),
        .in = ct, .out = pt, .sz = sizeof(ct),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = decTag, .tagSz = sizeof(decTag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCcm(&g_whalCrypto, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
    WHAL_ASSERT_MEM_EQ(decTag, encTag, sizeof(encTag));
}

static void Test_Crypto_AesCcm_KnownAnswer(void)
{
    uint8_t ct[24] = {0};
    uint8_t tag[16] = {0};

    whal_Crypto_AesCcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = ccmKey, .keySz = 32,
        .nonce = ccmNonce, .nonceSz = sizeof(ccmNonce),
        .in = ccmPt, .out = ct, .sz = sizeof(ccmPt),
        .aad = ccmAad, .aadSz = sizeof(ccmAad),
        .tag = tag, .tagSz = sizeof(tag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_AesCcm(&g_whalCrypto, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, ccmCt, sizeof(ccmCt));
    WHAL_ASSERT_MEM_EQ(tag, ccmTag, sizeof(ccmTag));
}
#endif /* WHAL_CFG_CRYPTO_AES_CCM */

/* --- Hash / HMAC known-answer vectors --- */

#if defined(WHAL_CFG_CRYPTO_SHA1) || defined(WHAL_CFG_CRYPTO_SHA224) || \
    defined(WHAL_CFG_CRYPTO_SHA256)
static const uint8_t hashInput[] = { 'a', 'b', 'c' };
#endif

#ifdef WHAL_CFG_CRYPTO_SHA1
/* NIST FIPS 180-4: SHA-1("abc") */
static const uint8_t sha1Digest[20] = {
    0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A,
    0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C,
    0x9C, 0xD0, 0xD8, 0x9D,
};

static void Test_Crypto_Sha1_KnownAnswer(void)
{
    uint8_t digest[20] = {0};

    whal_Crypto_HashArgs args = {
        .in = hashInput, .inSz = sizeof(hashInput),
        .digest = digest, .digestSz = sizeof(digest),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Sha1(&g_whalHash, &args),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(digest, sha1Digest, sizeof(sha1Digest));
}
#endif /* WHAL_CFG_CRYPTO_SHA1 */

#ifdef WHAL_CFG_CRYPTO_SHA224
/* NIST FIPS 180-4: SHA-224("abc") */
static const uint8_t sha224Digest[28] = {
    0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22,
    0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3,
    0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7,
    0xE3, 0x6C, 0x9D, 0xA7,
};

static void Test_Crypto_Sha224_KnownAnswer(void)
{
    uint8_t digest[28] = {0};

    whal_Crypto_HashArgs args = {
        .in = hashInput, .inSz = sizeof(hashInput),
        .digest = digest, .digestSz = sizeof(digest),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Sha224(&g_whalHash, &args),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(digest, sha224Digest, sizeof(sha224Digest));
}
#endif /* WHAL_CFG_CRYPTO_SHA224 */

#ifdef WHAL_CFG_CRYPTO_SHA256
/* NIST FIPS 180-4: SHA-256("abc") */
static const uint8_t sha256Digest[32] = {
    0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
    0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
    0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
    0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD,
};

static void Test_Crypto_Sha256_KnownAnswer(void)
{
    uint8_t digest[32] = {0};

    whal_Crypto_HashArgs args = {
        .in = hashInput, .inSz = sizeof(hashInput),
        .digest = digest, .digestSz = sizeof(digest),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Sha256(&g_whalHash, &args),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(digest, sha256Digest, sizeof(sha256Digest));
}
#endif /* WHAL_CFG_CRYPTO_SHA256 */

#if defined(WHAL_CFG_CRYPTO_HMAC_SHA1) || defined(WHAL_CFG_CRYPTO_HMAC_SHA224) || \
    defined(WHAL_CFG_CRYPTO_HMAC_SHA256)
/* RFC 2202 / RFC 4231 Test Case 1: HMAC with 20-byte 0x0b key, "Hi There" */
static const uint8_t hmacKey[20] = {
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B,
};

static const uint8_t hmacInput[] = {
    'H', 'i', ' ', 'T', 'h', 'e', 'r', 'e',
};
#endif

#ifdef WHAL_CFG_CRYPTO_HMAC_SHA1
/* RFC 2202 section 3, Test Case 1 */
static const uint8_t hmacSha1Digest[20] = {
    0xB6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64,
    0xE2, 0x8B, 0xC0, 0xB6, 0xFB, 0x37, 0x8C, 0x8E,
    0xF1, 0x46, 0xBE, 0x00,
};

static void Test_Crypto_HmacSha1_KnownAnswer(void)
{
    uint8_t digest[20] = {0};

    whal_Crypto_HmacArgs args = {
        .key = hmacKey, .keySz = sizeof(hmacKey),
        .in = hmacInput, .inSz = sizeof(hmacInput),
        .digest = digest, .digestSz = sizeof(digest),
    };
    WHAL_ASSERT_EQ(whal_Crypto_HmacSha1(&g_whalHash, &args),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(digest, hmacSha1Digest, sizeof(hmacSha1Digest));
}
#endif /* WHAL_CFG_CRYPTO_HMAC_SHA1 */

#ifdef WHAL_CFG_CRYPTO_HMAC_SHA224
/* RFC 4231 section 4, Test Case 1 */
static const uint8_t hmacSha224Digest[28] = {
    0x89, 0x6F, 0xB1, 0x12, 0x8A, 0xBB, 0xDF, 0x19,
    0x68, 0x32, 0x10, 0x7C, 0xD4, 0x9D, 0xF3, 0x3F,
    0x47, 0xB4, 0xB1, 0x16, 0x99, 0x12, 0xBA, 0x4F,
    0x53, 0x68, 0x4B, 0x22,
};

static void Test_Crypto_HmacSha224_KnownAnswer(void)
{
    uint8_t digest[28] = {0};

    whal_Crypto_HmacArgs args = {
        .key = hmacKey, .keySz = sizeof(hmacKey),
        .in = hmacInput, .inSz = sizeof(hmacInput),
        .digest = digest, .digestSz = sizeof(digest),
    };
    WHAL_ASSERT_EQ(whal_Crypto_HmacSha224(&g_whalHash, &args),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(digest, hmacSha224Digest, sizeof(hmacSha224Digest));
}
#endif /* WHAL_CFG_CRYPTO_HMAC_SHA224 */

#ifdef WHAL_CFG_CRYPTO_HMAC_SHA256
/* RFC 4231 section 4, Test Case 1 */
static const uint8_t hmacSha256Digest[32] = {
    0xB0, 0x34, 0x4C, 0x61, 0xD8, 0xDB, 0x38, 0x53,
    0x5C, 0xA8, 0xAF, 0xCE, 0xAF, 0x0B, 0xF1, 0x2B,
    0x88, 0x1D, 0xC2, 0x00, 0xC9, 0x83, 0x3D, 0xA7,
    0x26, 0xE9, 0x37, 0x6C, 0x2E, 0x32, 0xCF, 0xF7,
};

static void Test_Crypto_HmacSha256_KnownAnswer(void)
{
    uint8_t digest[32] = {0};

    whal_Crypto_HmacArgs args = {
        .key = hmacKey, .keySz = sizeof(hmacKey),
        .in = hmacInput, .inSz = sizeof(hmacInput),
        .digest = digest, .digestSz = sizeof(digest),
    };
    WHAL_ASSERT_EQ(whal_Crypto_HmacSha256(&g_whalHash, &args),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(digest, hmacSha256Digest, sizeof(hmacSha256Digest));
}
#endif /* WHAL_CFG_CRYPTO_HMAC_SHA256 */

static void Test_Crypto_Api(void)
{
    whal_Crypto_AesEcbArgs args = {0};

    WHAL_ASSERT_EQ(whal_Crypto_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Crypto_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Crypto_StartOp(NULL, 0, &args), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Crypto_Process(NULL, 0, &args), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Crypto_EndOp(NULL, 0, &args), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Crypto_StartOp(&g_whalCrypto, 0, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Crypto_Process(&g_whalCrypto, 0, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Crypto_EndOp(&g_whalCrypto, 0, NULL), WHAL_EINVAL);
}

void whal_Test_Crypto(void)
{
    WHAL_TEST_SUITE_START("cipher");
    WHAL_TEST(Test_Crypto_Api);
#ifdef WHAL_CFG_CRYPTO_AES_ECB
    WHAL_TEST(Test_Crypto_AesEcb_Basic);
    WHAL_TEST(Test_Crypto_AesEcb_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CBC
    WHAL_TEST(Test_Crypto_AesCbc_Basic);
    WHAL_TEST(Test_Crypto_AesCbc_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CTR
    WHAL_TEST(Test_Crypto_AesCtr_Basic);
    WHAL_TEST(Test_Crypto_AesCtr_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_GCM
    WHAL_TEST(Test_Crypto_AesGcm_Basic);
    WHAL_TEST(Test_Crypto_AesGcm_KnownAnswer);
    WHAL_TEST(Test_Crypto_AesGcm_KnownAnswer_PartialBlocks);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_GMAC
    WHAL_TEST(Test_Crypto_AesGmac_Basic);
    WHAL_TEST(Test_Crypto_AesGmac_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CCM
    WHAL_TEST(Test_Crypto_AesCcm_Basic);
    WHAL_TEST(Test_Crypto_AesCcm_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_SHA1
    WHAL_TEST(Test_Crypto_Sha1_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_SHA224
    WHAL_TEST(Test_Crypto_Sha224_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_SHA256
    WHAL_TEST(Test_Crypto_Sha256_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA1
    WHAL_TEST(Test_Crypto_HmacSha1_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA224
    WHAL_TEST(Test_Crypto_HmacSha224_KnownAnswer);
#endif
#ifdef WHAL_CFG_CRYPTO_HMAC_SHA256
    WHAL_TEST(Test_Crypto_HmacSha256_KnownAnswer);
#endif
    WHAL_TEST_SUITE_END();
}
