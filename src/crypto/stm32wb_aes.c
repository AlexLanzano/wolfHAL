#include <stdint.h>
#include <wolfHAL/crypto/stm32wb_aes.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/endian.h>

/* Control Register */
#define AES_CR_REG        0x00
#define AES_CR_EN_Pos     0
#define AES_CR_EN_Msk     (1UL << AES_CR_EN_Pos)

#define AES_CR_DATATYPE_Pos 1
#define AES_CR_DATATYPE_Msk (3UL << AES_CR_DATATYPE_Pos)

#define AES_CR_MODE_Pos   3
#define AES_CR_MODE_Msk   (3UL << AES_CR_MODE_Pos)

#define AES_CR_CHMOD_Pos  5
#define AES_CR_CHMOD_Msk  (3UL << AES_CR_CHMOD_Pos)

#define AES_CR_CHMOD2_Pos 16
#define AES_CR_CHMOD2_Msk (1UL << AES_CR_CHMOD2_Pos)

#define AES_CR_CCFC_Pos   7
#define AES_CR_CCFC_Msk   (1UL << AES_CR_CCFC_Pos)

#define AES_CR_KEYSIZE_Pos 18
#define AES_CR_KEYSIZE_Msk (1UL << AES_CR_KEYSIZE_Pos)

#define AES_CR_GCMPH_Pos  13
#define AES_CR_GCMPH_Msk  (3UL << AES_CR_GCMPH_Pos)

#define AES_CR_NPBLB_Pos  20
#define AES_CR_NPBLB_Msk  (0xF << AES_CR_NPBLB_Pos)

/* Status Register */
#define AES_SR_REG        0x04
#define AES_SR_CCF_Pos    0
#define AES_SR_CCF_Msk    (1UL << AES_SR_CCF_Pos)

#define AES_SR_RDERR_Pos  1
#define AES_SR_RDERR_Msk  (1UL << AES_SR_RDERR_Pos)

#define AES_SR_WRERR_Pos  2
#define AES_SR_WRERR_Msk  (1UL << AES_SR_WRERR_Pos)

/* Data Registers */
#define AES_DINR_REG      0x08
#define AES_DOUTR_REG     0x0C

/* Key Registers */
#define AES_KEYR0_REG     0x10
#define AES_KEYR1_REG     0x14
#define AES_KEYR2_REG     0x18
#define AES_KEYR3_REG     0x1C
#define AES_KEYR4_REG     0x30
#define AES_KEYR5_REG     0x34
#define AES_KEYR6_REG     0x38
#define AES_KEYR7_REG     0x3C

/* Initialization Vector Registers */
#define AES_IVR0_REG      0x20
#define AES_IVR1_REG      0x24
#define AES_IVR2_REG      0x28
#define AES_IVR3_REG      0x2C

/* Chaining modes */
#define AES_CHMOD_ECB     0x0
#define AES_CHMOD_CBC     0x1
#define AES_CHMOD_CTR     0x2
#define AES_CHMOD_GCM     0x3
#define AES_CHMOD_CCM     0x4

/* Operating modes */
#define AES_MODE_ENCRYPT  0x0
#define AES_MODE_KEYDERIV 0x1
#define AES_MODE_DECRYPT  0x2
#define AES_MODE_KEYDERIV_DECRYPT 0x3

/* Data types (swap modes) */
#define AES_DATATYPE_NONE     0x0
#define AES_DATATYPE_HALFWORD 0x1
#define AES_DATATYPE_BYTE     0x2
#define AES_DATATYPE_BIT      0x3

/* GCM phases */
#define AES_GCMPH_INIT    0x0
#define AES_GCMPH_HEADER  0x1
#define AES_GCMPH_PAYLOAD 0x2
#define AES_GCMPH_FINAL   0x3

static void WriteKey(size_t base, const uint8_t *key, size_t keySz)
{
    const uint8_t *k = key;
    if (keySz == 32) {
        whal_Reg_Write(base, AES_KEYR7_REG, whal_LoadBe32(k));
        whal_Reg_Write(base, AES_KEYR6_REG, whal_LoadBe32(k + 4));
        whal_Reg_Write(base, AES_KEYR5_REG, whal_LoadBe32(k + 8));
        whal_Reg_Write(base, AES_KEYR4_REG, whal_LoadBe32(k + 12));
        k += 16;
    }
    whal_Reg_Write(base, AES_KEYR3_REG, whal_LoadBe32(k));
    whal_Reg_Write(base, AES_KEYR2_REG, whal_LoadBe32(k + 4));
    whal_Reg_Write(base, AES_KEYR1_REG, whal_LoadBe32(k + 8));
    whal_Reg_Write(base, AES_KEYR0_REG, whal_LoadBe32(k + 12));
}

static void WriteIv(size_t base, const uint8_t *iv)
{
    whal_Reg_Write(base, AES_IVR3_REG, whal_LoadBe32(iv));
    whal_Reg_Write(base, AES_IVR2_REG, whal_LoadBe32(iv + 4));
    whal_Reg_Write(base, AES_IVR1_REG, whal_LoadBe32(iv + 8));
    whal_Reg_Write(base, AES_IVR0_REG, whal_LoadBe32(iv + 12));
}

static void WriteBlock(size_t base, const uint8_t *in)
{
    whal_Reg_Write(base, AES_DINR_REG, whal_LoadBe32(in));
    whal_Reg_Write(base, AES_DINR_REG, whal_LoadBe32(in + 4));
    whal_Reg_Write(base, AES_DINR_REG, whal_LoadBe32(in + 8));
    whal_Reg_Write(base, AES_DINR_REG, whal_LoadBe32(in + 12));
}

static void ReadBlock(size_t base, uint8_t *out)
{
    whal_StoreBe32(out,      whal_Reg_Read(base, AES_DOUTR_REG));
    whal_StoreBe32(out + 4,  whal_Reg_Read(base, AES_DOUTR_REG));
    whal_StoreBe32(out + 8,  whal_Reg_Read(base, AES_DOUTR_REG));
    whal_StoreBe32(out + 12, whal_Reg_Read(base, AES_DOUTR_REG));
}

static whal_Error WaitForCCF(size_t base, whal_Timeout *timeout)
{
    whal_Error err;
    err = whal_Reg_ReadPoll(base, AES_SR_REG, AES_SR_CCF_Msk,
                            AES_SR_CCF_Msk, timeout);
    if (err)
        return err;
    whal_Reg_Update(base, AES_CR_REG, AES_CR_CCFC_Msk, AES_CR_CCFC_Msk);
    return WHAL_SUCCESS;
}


#ifdef WHAL_CFG_STM32WB_AES_DIRECT_API_MAPPING
#define whal_Stm32wb_Aes_Init    whal_Crypto_Init
#define whal_Stm32wb_Aes_Deinit  whal_Crypto_Deinit
#define whal_Stm32wb_Aes_StartOp whal_Crypto_StartOp
#define whal_Stm32wb_Aes_Process whal_Crypto_Process
#define whal_Stm32wb_Aes_EndOp   whal_Crypto_EndOp
#endif /* WHAL_CFG_STM32WB_AES_DIRECT_API_MAPPING */

whal_Error whal_Stm32wb_Aes_Init(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wb_Aes_Deinit(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg) {
        return WHAL_EINVAL;
    }

    whal_Reg_Update(cryptoDev->regmap.base, AES_CR_REG, AES_CR_EN_Msk,
                    whal_SetBits(AES_CR_EN_Msk, AES_CR_EN_Pos, 0));

    return WHAL_SUCCESS;
}

#ifdef WHAL_CFG_CRYPTO_AES_ECB
static whal_Error StartOp_AesEcb(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesEcbArgs *args = (whal_Crypto_AesEcbArgs *)opArgs;
    size_t base = cryptoDev->regmap.base;
    size_t mode;
    size_t keySizeBit;

    if (!args->key)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_ENOTSUP;

    keySizeBit = (args->keySz == 32) ? 1 : 0;
    mode = (args->dir == WHAL_CRYPTO_ENCRYPT)
        ? AES_MODE_ENCRYPT : AES_MODE_KEYDERIV_DECRYPT;

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos, mode) |
                    whal_SetBits(AES_CR_CHMOD_Msk, AES_CR_CHMOD_Pos,
                                 AES_CHMOD_ECB) |
                    whal_SetBits(AES_CR_DATATYPE_Msk, AES_CR_DATATYPE_Pos,
                                 AES_DATATYPE_NONE) |
                    whal_SetBits(AES_CR_KEYSIZE_Msk, AES_CR_KEYSIZE_Pos,
                                 keySizeBit));

    WriteKey(base, args->key, args->keySz);

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_ECB */

#ifdef WHAL_CFG_CRYPTO_AES_CBC
static whal_Error StartOp_AesCbc(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCbcArgs *args = (whal_Crypto_AesCbcArgs *)opArgs;
    size_t base = cryptoDev->regmap.base;
    size_t mode;
    size_t keySizeBit;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_ENOTSUP;

    keySizeBit = (args->keySz == 32) ? 1 : 0;
    mode = (args->dir == WHAL_CRYPTO_ENCRYPT)
        ? AES_MODE_ENCRYPT : AES_MODE_KEYDERIV_DECRYPT;

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos, mode) |
                    whal_SetBits(AES_CR_CHMOD_Msk, AES_CR_CHMOD_Pos,
                                 AES_CHMOD_CBC) |
                    whal_SetBits(AES_CR_DATATYPE_Msk, AES_CR_DATATYPE_Pos,
                                 AES_DATATYPE_NONE) |
                    whal_SetBits(AES_CR_KEYSIZE_Msk, AES_CR_KEYSIZE_Pos,
                                 keySizeBit));

    WriteKey(base, args->key, args->keySz);
    WriteIv(base, args->iv);

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_CBC */

#ifdef WHAL_CFG_CRYPTO_AES_CTR
static whal_Error StartOp_AesCtr(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCtrArgs *args = (whal_Crypto_AesCtrArgs *)opArgs;
    size_t base = cryptoDev->regmap.base;
    size_t keySizeBit;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_ENOTSUP;

    keySizeBit = (args->keySz == 32) ? 1 : 0;

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_CHMOD_Msk, AES_CR_CHMOD_Pos,
                                 AES_CHMOD_CTR) |
                    whal_SetBits(AES_CR_DATATYPE_Msk, AES_CR_DATATYPE_Pos,
                                 AES_DATATYPE_NONE) |
                    whal_SetBits(AES_CR_KEYSIZE_Msk, AES_CR_KEYSIZE_Pos,
                                 keySizeBit));

    WriteKey(base, args->key, args->keySz);
    WriteIv(base, args->iv);

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_CTR */

#if defined(WHAL_CFG_CRYPTO_AES_ECB) || defined(WHAL_CFG_CRYPTO_AES_CBC) || \
    defined(WHAL_CFG_CRYPTO_AES_CTR)
static whal_Error Process_BlockCipher(whal_Crypto *cryptoDev,
                                      const uint8_t *in, uint8_t *out,
                                      size_t sz)
{
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    whal_Error err;
    size_t i;

    if (sz == 0)
        return WHAL_SUCCESS;

    if (!in || !out)
        return WHAL_EINVAL;

    if ((sz & 0xF) != 0)
        return WHAL_EINVAL;

    for (i = 0; i < sz; i += 16) {
        WriteBlock(base, in + i);
        err = WaitForCCF(base, cfg->timeout);
        if (err) {
            whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);
            return err;
        }
        ReadBlock(base, out + i);
    }

    return WHAL_SUCCESS;
}

static whal_Error EndOp_BlockCipher(whal_Crypto *cryptoDev)
{
    whal_Reg_Update(cryptoDev->regmap.base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_ECB || CBC || CTR */

#ifdef WHAL_CFG_CRYPTO_AES_GCM
static whal_Error StartOp_AesGcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err;
    whal_Crypto_AesGcmArgs *args = (whal_Crypto_AesGcmArgs *)opArgs;
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    const uint8_t *iv;
    const uint8_t *aad;
    size_t keySizeBit;
    size_t i;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;

    iv = (const uint8_t *)args->iv;
    aad = (const uint8_t *)args->aad;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_ENOTSUP;

    if (args->ivSz != 12)
        return WHAL_ENOTSUP;

    if (args->sz > 0 && (!args->in || !args->out))
        return WHAL_EINVAL;

    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;

    keySizeBit = (args->keySz == 32) ? 1 : 0;

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk |
                    AES_CR_GCMPH_Msk | AES_CR_NPBLB_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_CHMOD_Msk, AES_CR_CHMOD_Pos,
                                 AES_CHMOD_GCM) |
                    whal_SetBits(AES_CR_DATATYPE_Msk, AES_CR_DATATYPE_Pos,
                                 AES_DATATYPE_NONE) |
                    whal_SetBits(AES_CR_KEYSIZE_Msk, AES_CR_KEYSIZE_Pos,
                                 keySizeBit) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_INIT));

    WriteKey(base, args->key, args->keySz);

    whal_Reg_Write(base, AES_IVR3_REG, whal_LoadBe32(iv));
    whal_Reg_Write(base, AES_IVR2_REG, whal_LoadBe32(iv + 4));
    whal_Reg_Write(base, AES_IVR1_REG, whal_LoadBe32(iv + 8));
    whal_Reg_Write(base, AES_IVR0_REG, 0x00000002);

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    if (args->aadSz > 0) {
        whal_Reg_Update(base, AES_CR_REG,
                        AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                        whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                     AES_MODE_ENCRYPT) |
                        whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                     AES_GCMPH_HEADER));

        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

        for (i = 0; i < args->aadSz; i += 16) {
            const uint8_t *blkAad = aad + i;
            size_t remain = args->aadSz - i;
            uint8_t block[16] = {0};
            size_t j;

            if (remain >= 16) {
                WriteBlock(base, blkAad);
            } else {
                for (j = 0; j < remain; j++)
                    block[j] = blkAad[j];
                WriteBlock(base, block);
            }

            err = WaitForCCF(base, cfg->timeout);
            if (err)
                goto cleanup;
        }
    }

    return WHAL_SUCCESS;

cleanup:
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);
    return err;
}

static whal_Error Process_AesGcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesGcmArgs *args = (whal_Crypto_AesGcmArgs *)opArgs;
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    size_t mode;
    whal_Error err;
    size_t i;

    if (args->sz == 0)
        return WHAL_SUCCESS;

    if (!args->in || !args->out)
        return WHAL_EINVAL;

    mode = (args->dir == WHAL_CRYPTO_ENCRYPT)
        ? AES_MODE_ENCRYPT : AES_MODE_DECRYPT;

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos, mode) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_PAYLOAD));

    if (args->aadSz == 0)
        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    for (i = 0; i < args->sz; i += 16) {
        const uint8_t *in = (const uint8_t *)args->in + i;
        uint8_t *out = (uint8_t *)args->out + i;
        size_t remain = args->sz - i;
        uint8_t block[16] = {0};
        size_t j;

        if (remain >= 16) {
            WriteBlock(base, in);
        } else {
            if (mode == AES_MODE_ENCRYPT) {
                whal_Reg_Update(base, AES_CR_REG, AES_CR_NPBLB_Msk,
                                whal_SetBits(AES_CR_NPBLB_Msk, AES_CR_NPBLB_Pos, 16 - remain));
            }
            for (j = 0; j < remain; j++)
                block[j] = in[j];
            WriteBlock(base, block);
        }

        err = WaitForCCF(base, cfg->timeout);
        if (err) {
            whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);
            return err;
        }

        if (remain >= 16) {
            ReadBlock(base, out);
        } else {
            ReadBlock(base, block);
            for (j = 0; j < remain; j++)
                out[j] = block[j];
        }
    }

    return WHAL_SUCCESS;
}

static whal_Error EndOp_AesGcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesGcmArgs *args = (whal_Crypto_AesGcmArgs *)opArgs;
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint8_t *tag;
    whal_Error err;
    uint8_t tagBuf[16];
    size_t i;

    if (!args->tag || args->tagSz == 0 || args->tagSz > 16)
        return WHAL_EINVAL;

    tag = (uint8_t *)args->tag;

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_FINAL));

    if (args->aadSz == 0 && args->sz == 0)
        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, (uint32_t)(args->aadSz * 8));
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, (uint32_t)(args->sz * 8));

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    ReadBlock(base, tagBuf);

    for (i = 0; i < args->tagSz; i++)
        tag[i] = tagBuf[i];

cleanup:
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}
#endif /* WHAL_CFG_CRYPTO_AES_GCM */

#ifdef WHAL_CFG_CRYPTO_AES_GMAC
static whal_Error StartOp_AesGmac(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err;
    whal_Crypto_AesGmacArgs *args = (whal_Crypto_AesGmacArgs *)opArgs;
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    const uint8_t *iv;
    const uint8_t *aad;
    size_t keySizeBit;
    size_t i;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;

    iv = (const uint8_t *)args->iv;
    aad = (const uint8_t *)args->aad;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_ENOTSUP;

    if (args->ivSz != 12)
        return WHAL_ENOTSUP;

    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;

    keySizeBit = (args->keySz == 32) ? 1 : 0;

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk |
                    AES_CR_GCMPH_Msk | AES_CR_NPBLB_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_CHMOD_Msk, AES_CR_CHMOD_Pos,
                                 AES_CHMOD_GCM) |
                    whal_SetBits(AES_CR_DATATYPE_Msk, AES_CR_DATATYPE_Pos,
                                 AES_DATATYPE_NONE) |
                    whal_SetBits(AES_CR_KEYSIZE_Msk, AES_CR_KEYSIZE_Pos,
                                 keySizeBit) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_INIT));

    WriteKey(base, args->key, args->keySz);

    whal_Reg_Write(base, AES_IVR3_REG, whal_LoadBe32(iv));
    whal_Reg_Write(base, AES_IVR2_REG, whal_LoadBe32(iv + 4));
    whal_Reg_Write(base, AES_IVR1_REG, whal_LoadBe32(iv + 8));
    whal_Reg_Write(base, AES_IVR0_REG, 0x00000002);

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    if (args->aadSz > 0) {
        whal_Reg_Update(base, AES_CR_REG,
                        AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                        whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                     AES_MODE_ENCRYPT) |
                        whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                     AES_GCMPH_HEADER));

        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

        for (i = 0; i < args->aadSz; i += 16) {
            const uint8_t *blkAad = aad + i;
            size_t remain = args->aadSz - i;
            uint8_t block[16] = {0};
            size_t j;

            if (remain >= 16) {
                WriteBlock(base, blkAad);
            } else {
                for (j = 0; j < remain; j++)
                    block[j] = blkAad[j];
                WriteBlock(base, block);
            }

            err = WaitForCCF(base, cfg->timeout);
            if (err)
                goto cleanup;
        }
    }

    return WHAL_SUCCESS;

cleanup:
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);
    return err;
}

static whal_Error EndOp_AesGmac(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesGmacArgs *args = (whal_Crypto_AesGmacArgs *)opArgs;
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint8_t *tag;
    whal_Error err;
    uint8_t tagBuf[16];
    size_t i;

    if (!args->tag || args->tagSz == 0 || args->tagSz > 16)
        return WHAL_EINVAL;

    tag = (uint8_t *)args->tag;

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_FINAL));

    if (args->aadSz == 0)
        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, (uint32_t)(args->aadSz * 8));
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, 0);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    ReadBlock(base, tagBuf);

    for (i = 0; i < args->tagSz; i++)
        tag[i] = tagBuf[i];

cleanup:
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}
#endif /* WHAL_CFG_CRYPTO_AES_GMAC */

#ifdef WHAL_CFG_CRYPTO_AES_CCM
static whal_Error StartOp_AesCcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err;
    whal_Crypto_AesCcmArgs *args = (whal_Crypto_AesCcmArgs *)opArgs;
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    const uint8_t *nonce = (const uint8_t *)args->nonce;
    const uint8_t *aad = (const uint8_t *)args->aad;
    size_t keySizeBit;
    size_t i;
    uint8_t b0[16];

    if (!args->key || !args->nonce)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_ENOTSUP;

    if (args->nonceSz < 7 || args->nonceSz > 13)
        return WHAL_EINVAL;

    if (args->sz > 0 && (!args->in || !args->out))
        return WHAL_EINVAL;

    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;

    if (args->tagSz < 4 || args->tagSz > 16 || (args->tagSz & 1) != 0)
        return WHAL_EINVAL;

    keySizeBit = (args->keySz == 32) ? 1 : 0;

    {
        size_t q = 15 - args->nonceSz;
        size_t t = args->tagSz;
        uint8_t flags = (uint8_t)(((args->aadSz > 0) ? 0x40 : 0) |
                                  (((t - 2) / 2) << 3) |
                                  (q - 1));
        b0[0] = flags;
        for (i = 0; i < args->nonceSz; i++)
            b0[1 + i] = nonce[i];

        {
            size_t msgLen = args->sz;
            size_t j;
            for (j = 0; j < q; j++) {
                b0[15 - j] = (uint8_t)(msgLen & 0xFF);
                msgLen >>= 8;
            }
        }
    }

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk |
                    AES_CR_GCMPH_Msk | AES_CR_NPBLB_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_CHMOD_Msk, AES_CR_CHMOD_Pos, 0) |
                    AES_CR_CHMOD2_Msk |
                    whal_SetBits(AES_CR_DATATYPE_Msk, AES_CR_DATATYPE_Pos,
                                 AES_DATATYPE_NONE) |
                    whal_SetBits(AES_CR_KEYSIZE_Msk, AES_CR_KEYSIZE_Pos,
                                 keySizeBit) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_INIT));

    WriteKey(base, args->key, args->keySz);
    WriteIv(base, b0);

    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    if (args->aadSz > 0) {
        whal_Reg_Update(base, AES_CR_REG,
                        AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                        whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                     AES_MODE_ENCRYPT) |
                        whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                     AES_GCMPH_HEADER));

        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

        {
            uint8_t hdrBuf[16] = {0};
            size_t hdrOff = 0;
            size_t aadOff = 0;
            size_t j;

            hdrBuf[0] = (uint8_t)(args->aadSz >> 8);
            hdrBuf[1] = (uint8_t)(args->aadSz);
            hdrOff = 2;

            while (hdrOff < 16 && aadOff < args->aadSz) {
                hdrBuf[hdrOff++] = aad[aadOff++];
            }

            WriteBlock(base, hdrBuf);
            err = WaitForCCF(base, cfg->timeout);
            if (err)
                goto cleanup;

            while (aadOff < args->aadSz) {
                uint8_t block[16] = {0};

                for (j = 0; j < 16 && aadOff < args->aadSz; j++)
                    block[j] = aad[aadOff++];

                WriteBlock(base, block);
                err = WaitForCCF(base, cfg->timeout);
                if (err)
                    goto cleanup;
            }
        }
    }

    return WHAL_SUCCESS;

cleanup:
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);
    return err;
}

static whal_Error Process_AesCcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCcmArgs *args = (whal_Crypto_AesCcmArgs *)opArgs;
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    size_t mode;
    whal_Error err;
    size_t i;

    if (args->sz == 0)
        return WHAL_SUCCESS;

    if (!args->in || !args->out)
        return WHAL_EINVAL;

    mode = (args->dir == WHAL_CRYPTO_ENCRYPT)
        ? AES_MODE_ENCRYPT : AES_MODE_DECRYPT;

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos, mode) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_PAYLOAD));

    if (args->aadSz == 0)
        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    for (i = 0; i < args->sz; i += 16) {
        const uint8_t *in = (const uint8_t *)args->in + i;
        uint8_t *out = (uint8_t *)args->out + i;
        size_t remain = args->sz - i;
        uint8_t block[16] = {0};
        size_t j;

        if (remain >= 16) {
            WriteBlock(base, in);
        } else {
            if (mode == AES_MODE_DECRYPT) {
                whal_Reg_Update(base, AES_CR_REG, AES_CR_NPBLB_Msk,
                                whal_SetBits(AES_CR_NPBLB_Msk, AES_CR_NPBLB_Pos, 16 - remain));
            }
            for (j = 0; j < remain; j++)
                block[j] = in[j];
            WriteBlock(base, block);
        }

        err = WaitForCCF(base, cfg->timeout);
        if (err) {
            whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);
            return err;
        }

        if (remain >= 16) {
            ReadBlock(base, out);
        } else {
            ReadBlock(base, block);
            for (j = 0; j < remain; j++)
                out[j] = block[j];
        }
    }

    return WHAL_SUCCESS;
}

static whal_Error EndOp_AesCcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCcmArgs *args = (whal_Crypto_AesCcmArgs *)opArgs;
    const whal_Stm32wb_Aes_Cfg *cfg =
        (const whal_Stm32wb_Aes_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint8_t *tag;
    whal_Error err;
    uint8_t tagBuf[16];
    size_t i;

    if (!args->tag || args->tagSz < 4 || args->tagSz > 16 ||
        (args->tagSz & 1) != 0)
        return WHAL_EINVAL;

    tag = (uint8_t *)args->tag;

    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_FINAL));

    if (args->aadSz == 0 && args->sz == 0)
        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, 0);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    ReadBlock(base, tagBuf);

    for (i = 0; i < args->tagSz; i++)
        tag[i] = tagBuf[i];

cleanup:
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}
#endif /* WHAL_CFG_CRYPTO_AES_CCM */


whal_Error whal_Stm32wb_Aes_StartOp(whal_Crypto *cryptoDev, size_t opId,
                                   void *opArgs)
{
    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    switch (opId) {
#ifdef WHAL_CFG_CRYPTO_AES_ECB
    case WHAL_CRYPTO_AES_ECB:
        return StartOp_AesEcb(cryptoDev, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CBC
    case WHAL_CRYPTO_AES_CBC:
        return StartOp_AesCbc(cryptoDev, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CTR
    case WHAL_CRYPTO_AES_CTR:
        return StartOp_AesCtr(cryptoDev, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_GCM
    case WHAL_CRYPTO_AES_GCM:
        return StartOp_AesGcm(cryptoDev, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_GMAC
    case WHAL_CRYPTO_AES_GMAC:
        return StartOp_AesGmac(cryptoDev, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CCM
    case WHAL_CRYPTO_AES_CCM:
        return StartOp_AesCcm(cryptoDev, opArgs);
#endif
    default:
        return WHAL_ENOTSUP;
    }
}

whal_Error whal_Stm32wb_Aes_Process(whal_Crypto *cryptoDev, size_t opId,
                                   void *opArgs)
{
    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    switch (opId) {
#ifdef WHAL_CFG_CRYPTO_AES_ECB
    case WHAL_CRYPTO_AES_ECB: {
        whal_Crypto_AesEcbArgs *args = (whal_Crypto_AesEcbArgs *)opArgs;
        return Process_BlockCipher(cryptoDev, args->in, args->out, args->sz);
    }
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CBC
    case WHAL_CRYPTO_AES_CBC: {
        whal_Crypto_AesCbcArgs *args = (whal_Crypto_AesCbcArgs *)opArgs;
        return Process_BlockCipher(cryptoDev, args->in, args->out, args->sz);
    }
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CTR
    case WHAL_CRYPTO_AES_CTR: {
        whal_Crypto_AesCtrArgs *args = (whal_Crypto_AesCtrArgs *)opArgs;
        return Process_BlockCipher(cryptoDev, args->in, args->out, args->sz);
    }
#endif
#ifdef WHAL_CFG_CRYPTO_AES_GCM
    case WHAL_CRYPTO_AES_GCM:
        return Process_AesGcm(cryptoDev, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_GMAC
    case WHAL_CRYPTO_AES_GMAC:
        return WHAL_SUCCESS;
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CCM
    case WHAL_CRYPTO_AES_CCM:
        return Process_AesCcm(cryptoDev, opArgs);
#endif
    default:
        return WHAL_ENOTSUP;
    }
}

whal_Error whal_Stm32wb_Aes_EndOp(whal_Crypto *cryptoDev, size_t opId,
                                 void *opArgs)
{
    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    switch (opId) {
#ifdef WHAL_CFG_CRYPTO_AES_ECB
    case WHAL_CRYPTO_AES_ECB:
        return EndOp_BlockCipher(cryptoDev);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CBC
    case WHAL_CRYPTO_AES_CBC:
        return EndOp_BlockCipher(cryptoDev);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CTR
    case WHAL_CRYPTO_AES_CTR:
        return EndOp_BlockCipher(cryptoDev);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_GCM
    case WHAL_CRYPTO_AES_GCM:
        return EndOp_AesGcm(cryptoDev, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_GMAC
    case WHAL_CRYPTO_AES_GMAC:
        return EndOp_AesGmac(cryptoDev, opArgs);
#endif
#ifdef WHAL_CFG_CRYPTO_AES_CCM
    case WHAL_CRYPTO_AES_CCM:
        return EndOp_AesCcm(cryptoDev, opArgs);
#endif
    default:
        return WHAL_ENOTSUP;
    }
}

#ifndef WHAL_CFG_STM32WB_AES_DIRECT_API_MAPPING
const whal_CryptoDriver whal_Stm32wb_Aes_Driver = {
    .Init = whal_Stm32wb_Aes_Init,
    .Deinit = whal_Stm32wb_Aes_Deinit,
    .StartOp = whal_Stm32wb_Aes_StartOp,
    .Process = whal_Stm32wb_Aes_Process,
    .EndOp = whal_Stm32wb_Aes_EndOp,
};
#endif /* !WHAL_CFG_STM32WB_AES_DIRECT_API_MAPPING */
