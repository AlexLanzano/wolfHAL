#include <stdint.h>
#include <wolfHAL/crypto/stm32wb_aes.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/endian.h>

/*
 * STM32WB AES1 Register Definitions
 *
 * The AES1 peripheral provides hardware acceleration for 128/256-bit
 * AES in ECB, CBC, CTR, GCM, GMAC, and CCM modes. Data is fed through DINR and
 * read from DOUTR, 32 bits at a time.
 */

/* Control Register */
#define AES_CR_REG        0x00
#define AES_CR_EN_Pos     0                                                     /* AES enable */
#define AES_CR_EN_Msk     (1UL << AES_CR_EN_Pos)

#define AES_CR_DATATYPE_Pos 1                                                   /* Data type selection */
#define AES_CR_DATATYPE_Msk (3UL << AES_CR_DATATYPE_Pos)

#define AES_CR_MODE_Pos   3                                                     /* Operating mode */
#define AES_CR_MODE_Msk   (3UL << AES_CR_MODE_Pos)

#define AES_CR_CHMOD_Pos  5                                                     /* Chaining mode [1:0] */
#define AES_CR_CHMOD_Msk  (3UL << AES_CR_CHMOD_Pos)

#define AES_CR_CHMOD2_Pos 16                                                    /* Chaining mode [2] */
#define AES_CR_CHMOD2_Msk (1UL << AES_CR_CHMOD2_Pos)

#define AES_CR_CCFC_Pos   7                                                     /* Computation complete flag clear */
#define AES_CR_CCFC_Msk   (1UL << AES_CR_CCFC_Pos)

#define AES_CR_KEYSIZE_Pos 18                                                   /* Key size (0=128, 1=256) */
#define AES_CR_KEYSIZE_Msk (1UL << AES_CR_KEYSIZE_Pos)

#define AES_CR_GCMPH_Pos  13                                                    /* GCM phase */
#define AES_CR_GCMPH_Msk  (3UL << AES_CR_GCMPH_Pos)

/* Status Register */
#define AES_SR_REG        0x04
#define AES_SR_CCF_Pos    0                                                     /* Computation complete */
#define AES_SR_CCF_Msk    (1UL << AES_SR_CCF_Pos)

#define AES_SR_RDERR_Pos  1                                                     /* Read error */
#define AES_SR_RDERR_Msk  (1UL << AES_SR_RDERR_Pos)

#define AES_SR_WRERR_Pos  2                                                     /* Write error */
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
#define AES_CHMOD_CCM     0x4                                                   /* CHMOD[2:0] = 100 */

/* Operating modes */
#define AES_MODE_ENCRYPT  0x0
#define AES_MODE_KEYDERIV 0x1
#define AES_MODE_DECRYPT  0x2
#define AES_MODE_KEYDERIV_DECRYPT 0x3

/* Data types (swap modes) */
#define AES_DATATYPE_NONE     0x0                                                /* No swapping */
#define AES_DATATYPE_HALFWORD 0x1                                                /* 16-bit half-word swap */
#define AES_DATATYPE_BYTE     0x2                                                /* 8-bit byte swap */
#define AES_DATATYPE_BIT      0x3                                                /* 1-bit bit swap */


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


whal_Error whal_Stm32wbAes_Init(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbAes_Deinit(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg) {
        return WHAL_EINVAL;
    }

    /* Disable AES peripheral */
    whal_Reg_Update(cryptoDev->regmap.base, AES_CR_REG, AES_CR_EN_Msk,
                    whal_SetBits(AES_CR_EN_Msk, AES_CR_EN_Pos, 0));

    return WHAL_SUCCESS;
}


whal_Error whal_Stm32wbAes_AesEcb(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesEcbArgs *args;
    const whal_Stm32wbAes_Cfg *cfg;
    size_t base;
    size_t mode;
    size_t keySizeBit;
    size_t i;

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesEcbArgs *)opArgs;

    if (!args->key || !args->in || !args->out)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;

    if (args->sz == 0 || (args->sz & 0xF) != 0)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32wbAes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;
    keySizeBit = (args->keySz == 32) ? 1 : 0;

    mode = (args->dir == WHAL_CRYPTO_ENCRYPT)
        ? AES_MODE_ENCRYPT : AES_MODE_KEYDERIV_DECRYPT;

    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    /* Configure: MODE, CHMOD=ECB, DATATYPE=none, KEYSIZE */
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

    /* Enable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    /* Process each 16-byte block */
    for (i = 0; i < args->sz; i += 16) {
        const uint8_t *in = args->in + i;
        uint8_t *out = args->out + i;

        WriteBlock(base, in);
        err = WaitForCCF(base, cfg->timeout);
        if (err)
            goto cleanup;
        ReadBlock(base, out);
    }

cleanup:
    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}

whal_Error whal_Stm32wbAes_AesCbc(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesCbcArgs *args;
    const whal_Stm32wbAes_Cfg *cfg;
    size_t base;
    size_t mode;
    size_t keySizeBit;
    size_t i;

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesCbcArgs *)opArgs;

    if (!args->key || !args->iv || !args->in || !args->out)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;

    if (args->sz == 0 || (args->sz & 0xF) != 0)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32wbAes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;
    keySizeBit = (args->keySz == 32) ? 1 : 0;

    mode = (args->dir == WHAL_CRYPTO_ENCRYPT)
        ? AES_MODE_ENCRYPT : AES_MODE_KEYDERIV_DECRYPT;

    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    /* Configure: MODE, CHMOD=CBC, DATATYPE=none, KEYSIZE */
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

    /* Enable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    /* Process each 16-byte block */
    for (i = 0; i < args->sz; i += 16) {
        const uint8_t *in = args->in + i;
        uint8_t *out = args->out + i;

        WriteBlock(base, in);
        err = WaitForCCF(base, cfg->timeout);
        if (err)
            goto cleanup;
        ReadBlock(base, out);
    }

cleanup:
    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}

whal_Error whal_Stm32wbAes_AesCtr(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesCtrArgs *args;
    const whal_Stm32wbAes_Cfg *cfg;
    size_t base;
    size_t keySizeBit;
    size_t i;

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesCtrArgs *)opArgs;

    if (!args->key || !args->iv || !args->in || !args->out)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;

    if (args->sz == 0 || (args->sz & 0xF) != 0)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32wbAes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;
    keySizeBit = (args->keySz == 32) ? 1 : 0;

    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    /* Configure: MODE=encrypt (always for CTR), CHMOD=CTR, DATATYPE=none, KEYSIZE */
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

    /* Enable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    /* Process each 16-byte block */
    for (i = 0; i < args->sz; i += 16) {
        const uint8_t *in = args->in + i;
        uint8_t *out = args->out + i;

        WriteBlock(base, in);
        err = WaitForCCF(base, cfg->timeout);
        if (err)
            goto cleanup;
        ReadBlock(base, out);
    }

cleanup:
    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}

whal_Error whal_Stm32wbAes_AesGcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesGcmArgs *args;
    const whal_Stm32wbAes_Cfg *cfg;
    size_t base;
    size_t mode;
    size_t keySizeBit;
    size_t i;
    uint8_t tagBuf[16];

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesGcmArgs *)opArgs;

    if (!args->key || !args->iv || !args->tag)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;

    if (args->ivSz != 12)
        return WHAL_EINVAL;

    if (args->sz > 0 && (!args->in || !args->out))
        return WHAL_EINVAL;

    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;

    if (args->tagSz == 0 || args->tagSz > 16)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32wbAes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;
    keySizeBit = (args->keySz == 32) ? 1 : 0;

    mode = (args->dir == WHAL_CRYPTO_ENCRYPT)
        ? AES_MODE_ENCRYPT : AES_MODE_DECRYPT;

    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    /* --- Init phase: compute H from key --- */
    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk |
                    AES_CR_GCMPH_Msk,
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

    /* Write IV: 12 bytes nonce into IVR3-IVR1, counter=0x00000002 into IVR0 */
    whal_Reg_Write(base, AES_IVR3_REG, whal_LoadBe32(args->iv));
    whal_Reg_Write(base, AES_IVR2_REG, whal_LoadBe32(args->iv + 4));
    whal_Reg_Write(base, AES_IVR1_REG, whal_LoadBe32(args->iv + 8));
    whal_Reg_Write(base, AES_IVR0_REG, 0x00000002);

    /* Enable AES — init phase runs, EN auto-clears when done */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    /* --- Header phase: process AAD --- */
    if (args->aadSz > 0) {
        whal_Reg_Update(base, AES_CR_REG,
                        AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                        whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                     AES_MODE_ENCRYPT) |
                        whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                     AES_GCMPH_HEADER));

        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

        for (i = 0; i < args->aadSz; i += 16) {
            const uint8_t *aad = args->aad + i;
            size_t remain = args->aadSz - i;
            uint8_t block[16] = {0};
            size_t j;

            if (remain >= 16) {
                WriteBlock(base, aad);
            } else {
                for (j = 0; j < remain; j++)
                    block[j] = aad[j];
                WriteBlock(base, block);
            }

            err = WaitForCCF(base, cfg->timeout);
            if (err)
                goto cleanup;
        }
    }

    /* --- Payload phase --- */
    if (args->sz > 0) {
        whal_Reg_Update(base, AES_CR_REG,
                        AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                        whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos, mode) |
                        whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                     AES_GCMPH_PAYLOAD));

        if (args->aadSz == 0)
            whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

        for (i = 0; i < args->sz; i += 16) {
            const uint8_t *in = args->in + i;
            uint8_t *out = args->out + i;
            size_t remain = args->sz - i;
            uint8_t block[16] = {0};
            size_t j;

            if (remain >= 16) {
                WriteBlock(base, in);
            } else {
                for (j = 0; j < remain; j++)
                    block[j] = in[j];
                WriteBlock(base, block);
            }

            err = WaitForCCF(base, cfg->timeout);
            if (err)
                goto cleanup;

            if (remain >= 16) {
                ReadBlock(base, out);
            } else {
                ReadBlock(base, block);
                for (j = 0; j < remain; j++)
                    out[j] = block[j];
            }
        }
    }

    /* --- Final phase: compute tag --- */
    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_FINAL));

    if (args->aadSz == 0 && args->sz == 0)
        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    /* Write bit-length block: aadSz*8 (64-bit) || sz*8 (64-bit) */
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, (uint32_t)(args->aadSz * 8));
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, (uint32_t)(args->sz * 8));

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    /* Read tag from DOUTR */
    ReadBlock(base, tagBuf);

    for (i = 0; i < args->tagSz; i++)
        args->tag[i] = tagBuf[i];

cleanup:
    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}

whal_Error whal_Stm32wbAes_AesGmac(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesGmacArgs *args;
    const whal_Stm32wbAes_Cfg *cfg;
    size_t base;
    size_t keySizeBit;
    size_t i;
    uint8_t tagBuf[16];

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesGmacArgs *)opArgs;

    if (!args->key || !args->iv || !args->tag)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;

    if (args->ivSz != 12)
        return WHAL_EINVAL;

    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;

    if (args->tagSz == 0 || args->tagSz > 16)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32wbAes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;
    keySizeBit = (args->keySz == 32) ? 1 : 0;

    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    /* --- Init phase --- */
    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk |
                    AES_CR_GCMPH_Msk,
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

    /* Write IV: 12 bytes nonce, counter=0x00000002 */
    whal_Reg_Write(base, AES_IVR3_REG, whal_LoadBe32(args->iv));
    whal_Reg_Write(base, AES_IVR2_REG, whal_LoadBe32(args->iv + 4));
    whal_Reg_Write(base, AES_IVR1_REG, whal_LoadBe32(args->iv + 8));
    whal_Reg_Write(base, AES_IVR0_REG, 0x00000002);

    /* Enable — init phase runs, EN auto-clears */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    /* --- Header phase: process AAD --- */
    if (args->aadSz > 0) {
        whal_Reg_Update(base, AES_CR_REG,
                        AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                        whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                     AES_MODE_ENCRYPT) |
                        whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                     AES_GCMPH_HEADER));

        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

        for (i = 0; i < args->aadSz; i += 16) {
            const uint8_t *aad = args->aad + i;
            size_t remain = args->aadSz - i;
            uint8_t block[16] = {0};
            size_t j;

            if (remain >= 16) {
                WriteBlock(base, aad);
            } else {
                for (j = 0; j < remain; j++)
                    block[j] = aad[j];
                WriteBlock(base, block);
            }

            err = WaitForCCF(base, cfg->timeout);
            if (err)
                goto cleanup;
        }
    }

    /* --- Final phase: compute tag (skip payload for GMAC) --- */
    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_FINAL));

    if (args->aadSz == 0)
        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    /* Write bit-length block: aadSz*8 (64-bit) || 0 (64-bit) */
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, (uint32_t)(args->aadSz * 8));
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, 0);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    /* Read tag from DOUTR */
    ReadBlock(base, tagBuf);

    for (i = 0; i < args->tagSz; i++)
        args->tag[i] = tagBuf[i];

cleanup:
    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}

whal_Error whal_Stm32wbAes_AesCcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesCcmArgs *args;
    const whal_Stm32wbAes_Cfg *cfg;
    size_t base;
    size_t mode;
    size_t keySizeBit;
    size_t i;
    uint8_t b0[16];
    uint8_t block[16];
    uint8_t tagBuf[16];

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesCcmArgs *)opArgs;

    if (!args->key || !args->nonce || !args->tag)
        return WHAL_EINVAL;

    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;

    if (args->nonceSz < 7 || args->nonceSz > 13)
        return WHAL_EINVAL;

    if (args->sz > 0 && (!args->in || !args->out))
        return WHAL_EINVAL;

    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;

    if (args->tagSz < 4 || args->tagSz > 16 || (args->tagSz & 1) != 0)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32wbAes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;
    keySizeBit = (args->keySz == 32) ? 1 : 0;

    mode = (args->dir == WHAL_CRYPTO_ENCRYPT)
        ? AES_MODE_ENCRYPT : AES_MODE_DECRYPT;

    /*
     * Build B0 block per RFC 3610:
     * Flags = 8*(Adata?1:0) + 8*((t-2)/2) + (q-1)
     * where q = 15 - nonceSz, t = tagSz
     */
    {
        size_t q = 15 - args->nonceSz;
        size_t t = args->tagSz;
        uint8_t flags = (uint8_t)(((args->aadSz > 0) ? 0x40 : 0) |
                                  (((t - 2) / 2) << 3) |
                                  (q - 1));
        b0[0] = flags;
        for (i = 0; i < args->nonceSz; i++)
            b0[1 + i] = args->nonce[i];

        /* Encode message length in q bytes, big-endian */
        {
            size_t msgLen = args->sz;
            size_t j;
            for (j = 0; j < q; j++) {
                b0[15 - j] = (uint8_t)(msgLen & 0xFF);
                msgLen >>= 8;
            }
        }
    }

    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    /* --- Init phase: load B0 into IVRx --- */
    /* CCM: CHMOD[2:0] = 100, so CHMOD[1:0]=00 and CHMOD[2]=1 */
    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_CHMOD_Msk | AES_CR_CHMOD2_Msk |
                    AES_CR_DATATYPE_Msk | AES_CR_KEYSIZE_Msk |
                    AES_CR_GCMPH_Msk,
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

    /* Enable — init phase runs, EN auto-clears */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    /* --- Header phase: process AAD with length prefix --- */
    if (args->aadSz > 0) {
        whal_Reg_Update(base, AES_CR_REG,
                        AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                        whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                     AES_MODE_ENCRYPT) |
                        whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                     AES_GCMPH_HEADER));

        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

        /*
         * B1 block: 2-byte length prefix followed by AAD data.
         * For aadSz < 65280, prefix is just 2 bytes.
         * Remaining AAD continues in subsequent blocks.
         */
        {
            uint8_t hdrBuf[16] = {0};
            size_t hdrOff = 0;
            size_t aadOff = 0;
            size_t j;

            /* 2-byte length prefix */
            hdrBuf[0] = (uint8_t)(args->aadSz >> 8);
            hdrBuf[1] = (uint8_t)(args->aadSz);
            hdrOff = 2;

            /* Fill rest of first block with AAD */
            while (hdrOff < 16 && aadOff < args->aadSz) {
                hdrBuf[hdrOff++] = args->aad[aadOff++];
            }

            /* Write first block */
            WriteBlock(base, hdrBuf);
            err = WaitForCCF(base, cfg->timeout);
            if (err)
                goto cleanup;

            /* Process remaining AAD in 16-byte blocks */
            while (aadOff < args->aadSz) {
                for (j = 0; j < 16; j++)
                    block[j] = 0;

                for (j = 0; j < 16 && aadOff < args->aadSz; j++)
                    block[j] = args->aad[aadOff++];

                WriteBlock(base, block);
                err = WaitForCCF(base, cfg->timeout);
                if (err)
                    goto cleanup;
            }
        }
    }

    /* --- Payload phase --- */
    if (args->sz > 0) {
        whal_Reg_Update(base, AES_CR_REG,
                        AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                        whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos, mode) |
                        whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                     AES_GCMPH_PAYLOAD));

        if (args->aadSz == 0)
            whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

        for (i = 0; i < args->sz; i += 16) {
            const uint8_t *in = args->in + i;
            uint8_t *out = args->out + i;
            size_t remain = args->sz - i;
            size_t j;

            if (remain >= 16) {
                WriteBlock(base, in);
            } else {
                for (j = 0; j < 16; j++)
                    block[j] = 0;
                for (j = 0; j < remain; j++)
                    block[j] = in[j];
                WriteBlock(base, block);
            }

            err = WaitForCCF(base, cfg->timeout);
            if (err)
                goto cleanup;

            if (remain >= 16) {
                ReadBlock(base, out);
            } else {
                ReadBlock(base, block);
                for (j = 0; j < remain; j++)
                    out[j] = block[j];
            }
        }
    }

    /* --- Final phase: compute/verify tag --- */
    whal_Reg_Update(base, AES_CR_REG,
                    AES_CR_MODE_Msk | AES_CR_GCMPH_Msk,
                    whal_SetBits(AES_CR_MODE_Msk, AES_CR_MODE_Pos,
                                 AES_MODE_ENCRYPT) |
                    whal_SetBits(AES_CR_GCMPH_Msk, AES_CR_GCMPH_Pos,
                                 AES_GCMPH_FINAL));

    if (args->aadSz == 0 && args->sz == 0)
        whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, AES_CR_EN_Msk);

    /* Write zeros to DINR for final phase */
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, 0);
    whal_Reg_Write(base, AES_DINR_REG, 0);

    err = WaitForCCF(base, cfg->timeout);
    if (err)
        goto cleanup;

    /* Read tag from DOUTR */
    ReadBlock(base, tagBuf);

    for (i = 0; i < args->tagSz; i++)
        args->tag[i] = tagBuf[i];

cleanup:
    /* Disable AES */
    whal_Reg_Update(base, AES_CR_REG, AES_CR_EN_Msk, 0);

    return err;
}

const whal_CryptoDriver whal_Stm32wbAes_Driver = {
    .Init = whal_Stm32wbAes_Init,
    .Deinit = whal_Stm32wbAes_Deinit,
};
