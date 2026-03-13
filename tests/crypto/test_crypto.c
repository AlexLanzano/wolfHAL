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

/* NIST SP 800-38A F.1.5 AES-256-ECB expected ciphertext */
static const uint8_t nistEcbCt[16] = {
    0xF3, 0xEE, 0xD1, 0xBD, 0xB5, 0xD2, 0xA0, 0x3C,
    0x06, 0x4B, 0x5A, 0x7E, 0x3D, 0xB1, 0x81, 0xF8,
};

/* NIST SP 800-38A F.2.5 AES-256-CBC expected ciphertext */
static const uint8_t nistCbcIv[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};

static const uint8_t nistCbcCt[16] = {
    0xF5, 0x8C, 0x4C, 0x04, 0xD6, 0xE5, 0xF1, 0xBA,
    0x77, 0x9E, 0xAB, 0xFB, 0x5F, 0x7B, 0xFB, 0xD6,
};

/* NIST SP 800-38A F.5.5 AES-256-CTR expected ciphertext */
static const uint8_t nistCtrIv[16] = {
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static const uint8_t nistCtrCt[16] = {
    0x60, 0x1E, 0xC3, 0x13, 0x77, 0x57, 0x89, 0xA5,
    0xB7, 0xA7, 0xF5, 0x04, 0xBB, 0xF3, 0xD2, 0x28,
};

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

static int BoardHasOp(size_t op)
{
    return op < g_whalCrypto.opsCount && g_whalCrypto.ops[op] != 0;
}

static void Test_Crypto_AesEcb_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_ECB))
        WHAL_SKIP();

    whal_Crypto_AesEcbArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .in = plaintext, .out = ct, .sz = sizeof(plaintext),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_ECB, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesEcbArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .in = ct, .out = pt, .sz = sizeof(ct),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_ECB, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
}

static void Test_Crypto_AesCbc_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_CBC))
        WHAL_SKIP();

    whal_Crypto_AesCbcArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .iv = iv, .in = plaintext, .out = ct, .sz = sizeof(plaintext),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CBC, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesCbcArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .iv = iv, .in = ct, .out = pt, .sz = sizeof(ct),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CBC, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
}

static void Test_Crypto_AesCtr_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_CTR))
        WHAL_SKIP();

    whal_Crypto_AesCtrArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .iv = iv, .in = plaintext, .out = ct, .sz = sizeof(plaintext),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CTR, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesCtrArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .iv = iv, .in = ct, .out = pt, .sz = sizeof(ct),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CTR, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
}

static void Test_Crypto_AesGcm_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};
    uint8_t encTag[16] = {0};
    uint8_t decTag[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_GCM))
        WHAL_SKIP();

    whal_Crypto_AesGcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .iv = nonce, .ivSz = sizeof(nonce),
        .in = plaintext, .out = ct, .sz = sizeof(plaintext),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = encTag, .tagSz = sizeof(encTag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_GCM, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesGcmArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .iv = nonce, .ivSz = sizeof(nonce),
        .in = ct, .out = pt, .sz = sizeof(ct),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = decTag, .tagSz = sizeof(decTag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_GCM, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
    WHAL_ASSERT_MEM_EQ(decTag, encTag, sizeof(encTag));
}

static void Test_Crypto_AesGmac_Basic(void)
{
    uint8_t tag1[16] = {0};
    uint8_t tag2[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_GMAC))
        WHAL_SKIP();

    whal_Crypto_AesGmacArgs args1 = {
        .key = key, .keySz = 32,
        .iv = nonce, .ivSz = sizeof(nonce),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = tag1, .tagSz = sizeof(tag1),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_GMAC, &args1),
                   WHAL_SUCCESS);

    whal_Crypto_AesGmacArgs args2 = {
        .key = key, .keySz = 32,
        .iv = nonce, .ivSz = sizeof(nonce),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = tag2, .tagSz = sizeof(tag2),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_GMAC, &args2),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(tag1, tag2, sizeof(tag1));
}

static void Test_Crypto_AesCcm_Basic(void)
{
    uint8_t ct[32] = {0};
    uint8_t pt[32] = {0};
    uint8_t encTag[16] = {0};
    uint8_t decTag[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_CCM))
        WHAL_SKIP();

    whal_Crypto_AesCcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = key, .keySz = 32,
        .nonce = nonce, .nonceSz = sizeof(nonce),
        .in = plaintext, .out = ct, .sz = sizeof(plaintext),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = encTag, .tagSz = sizeof(encTag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CCM, &enc),
                   WHAL_SUCCESS);

    whal_Crypto_AesCcmArgs dec = {
        .dir = WHAL_CRYPTO_DECRYPT, .key = key, .keySz = 32,
        .nonce = nonce, .nonceSz = sizeof(nonce),
        .in = ct, .out = pt, .sz = sizeof(ct),
        .aad = aad, .aadSz = sizeof(aad),
        .tag = decTag, .tagSz = sizeof(decTag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CCM, &dec),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pt, plaintext, sizeof(plaintext));
    WHAL_ASSERT_MEM_EQ(decTag, encTag, sizeof(encTag));
}

static void Test_Crypto_AesEcb_KnownAnswer(void)
{
    uint8_t ct[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_ECB))
        WHAL_SKIP();

    whal_Crypto_AesEcbArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = nistKey, .keySz = 32,
        .in = nistPt, .out = ct, .sz = sizeof(nistPt),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_ECB, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, nistEcbCt, sizeof(nistEcbCt));
}

static void Test_Crypto_AesCbc_KnownAnswer(void)
{
    uint8_t ct[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_CBC))
        WHAL_SKIP();

    whal_Crypto_AesCbcArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = nistKey, .keySz = 32,
        .iv = nistCbcIv, .in = nistPt, .out = ct, .sz = sizeof(nistPt),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CBC, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, nistCbcCt, sizeof(nistCbcCt));
}

static void Test_Crypto_AesCtr_KnownAnswer(void)
{
    uint8_t ct[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_CTR))
        WHAL_SKIP();

    whal_Crypto_AesCtrArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = nistKey, .keySz = 32,
        .iv = nistCtrIv, .in = nistPt, .out = ct, .sz = sizeof(nistPt),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CTR, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, nistCtrCt, sizeof(nistCtrCt));
}

static void Test_Crypto_AesGcm_KnownAnswer(void)
{
    uint8_t ct[64] = {0};
    uint8_t tag[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_GCM))
        WHAL_SKIP();

    whal_Crypto_AesGcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = gcmKey, .keySz = 32,
        .iv = gcmIv, .ivSz = sizeof(gcmIv),
        .in = gcmPt, .out = ct, .sz = sizeof(gcmPt),
        .aad = NULL, .aadSz = 0,
        .tag = tag, .tagSz = sizeof(tag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_GCM, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, gcmCt, sizeof(gcmCt));
    WHAL_ASSERT_MEM_EQ(tag, gcmTag, sizeof(gcmTag));
}

static void Test_Crypto_AesGmac_KnownAnswer(void)
{
    uint8_t tag[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_GMAC))
        WHAL_SKIP();

    whal_Crypto_AesGmacArgs args = {
        .key = gmacKey, .keySz = 32,
        .iv = gmacIv, .ivSz = sizeof(gmacIv),
        .aad = gmacAad, .aadSz = sizeof(gmacAad),
        .tag = tag, .tagSz = sizeof(tag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_GMAC, &args),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(tag, gmacTag, sizeof(gmacTag));
}

static void Test_Crypto_AesCcm_KnownAnswer(void)
{
    uint8_t ct[24] = {0};
    uint8_t tag[16] = {0};

    if (!BoardHasOp(BOARD_CRYPTO_AES_CCM))
        WHAL_SKIP();

    whal_Crypto_AesCcmArgs enc = {
        .dir = WHAL_CRYPTO_ENCRYPT, .key = ccmKey, .keySz = 32,
        .nonce = ccmNonce, .nonceSz = sizeof(ccmNonce),
        .in = ccmPt, .out = ct, .sz = sizeof(ccmPt),
        .aad = ccmAad, .aadSz = sizeof(ccmAad),
        .tag = tag, .tagSz = sizeof(tag),
    };
    WHAL_ASSERT_EQ(whal_Crypto_Op(&g_whalCrypto, BOARD_CRYPTO_AES_CCM, &enc),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(ct, ccmCt, sizeof(ccmCt));
    WHAL_ASSERT_MEM_EQ(tag, ccmTag, sizeof(ccmTag));
}

void whal_Test_Crypto(void)
{
    WHAL_TEST_SUITE_START("crypto");
    WHAL_TEST(Test_Crypto_AesEcb_Basic);
    WHAL_TEST(Test_Crypto_AesEcb_KnownAnswer);
    WHAL_TEST(Test_Crypto_AesCbc_Basic);
    WHAL_TEST(Test_Crypto_AesCbc_KnownAnswer);
    WHAL_TEST(Test_Crypto_AesCtr_Basic);
    WHAL_TEST(Test_Crypto_AesCtr_KnownAnswer);
    WHAL_TEST(Test_Crypto_AesGcm_Basic);
    WHAL_TEST(Test_Crypto_AesGcm_KnownAnswer);
    WHAL_TEST(Test_Crypto_AesGmac_Basic);
    WHAL_TEST(Test_Crypto_AesGmac_KnownAnswer);
    WHAL_TEST(Test_Crypto_AesCcm_Basic);
    WHAL_TEST(Test_Crypto_AesCcm_KnownAnswer);
    WHAL_TEST_SUITE_END();
}
