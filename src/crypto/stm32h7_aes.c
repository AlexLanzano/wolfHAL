#include <stdint.h>
#include <wolfHAL/crypto/stm32h7_aes.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/endian.h>

/*
 * STM32H7 CRYP Register Definitions (RM0433 Section 35.7)
 *
 * The CRYP peripheral provides hardware acceleration for DES/TDES and
 * AES (128/192/256-bit) in ECB, CBC, CTR, GCM, GMAC, and CCM modes.
 * Data is fed through CRYP_DIN and read from CRYP_DOUT via input/output
 * FIFOs, 32 bits at a time (4 words per AES block).
 */

/* Control Register */
#define CRYP_CR_REG        0x00

#define CRYP_CR_ALGODIR_Pos  2
#define CRYP_CR_ALGODIR_Msk  (1UL << CRYP_CR_ALGODIR_Pos)

#define CRYP_CR_ALGOMODE_Pos 3
#define CRYP_CR_ALGOMODE_Msk (7UL << CRYP_CR_ALGOMODE_Pos)

#define CRYP_CR_DATATYPE_Pos 6
#define CRYP_CR_DATATYPE_Msk (3UL << CRYP_CR_DATATYPE_Pos)

#define CRYP_CR_KEYSIZE_Pos  8
#define CRYP_CR_KEYSIZE_Msk  (3UL << CRYP_CR_KEYSIZE_Pos)

#define CRYP_CR_FFLUSH_Pos   14
#define CRYP_CR_FFLUSH_Msk   (1UL << CRYP_CR_FFLUSH_Pos)

#define CRYP_CR_CRYPEN_Pos   15
#define CRYP_CR_CRYPEN_Msk   (1UL << CRYP_CR_CRYPEN_Pos)

#define CRYP_CR_GCM_CCMPH_Pos 16
#define CRYP_CR_GCM_CCMPH_Msk (3UL << CRYP_CR_GCM_CCMPH_Pos)

#define CRYP_CR_ALGOMODE3_Pos 19
#define CRYP_CR_ALGOMODE3_Msk (1UL << CRYP_CR_ALGOMODE3_Pos)

/* Combined ALGOMODE mask (bits 19, 5:3) */
#define CRYP_CR_ALGOMODE_ALL_Msk (CRYP_CR_ALGOMODE_Msk | CRYP_CR_ALGOMODE3_Msk)

/* Status Register */
#define CRYP_SR_REG        0x04
#define CRYP_SR_IFEM_Pos   0
#define CRYP_SR_IFEM_Msk   (1UL << CRYP_SR_IFEM_Pos)
#define CRYP_SR_IFNF_Pos   1
#define CRYP_SR_IFNF_Msk   (1UL << CRYP_SR_IFNF_Pos)
#define CRYP_SR_OFNE_Pos   2
#define CRYP_SR_OFNE_Msk   (1UL << CRYP_SR_OFNE_Pos)
#define CRYP_SR_BUSY_Pos   4
#define CRYP_SR_BUSY_Msk   (1UL << CRYP_SR_BUSY_Pos)

/* Data Registers */
#define CRYP_DIN_REG       0x08
#define CRYP_DOUT_REG      0x0C

/* Key Registers (256-bit key: K0LR has bits 255:224, K3RR has bits 31:0) */
#define CRYP_K0LR_REG      0x20
#define CRYP_K0RR_REG      0x24
#define CRYP_K1LR_REG      0x28
#define CRYP_K1RR_REG      0x2C
#define CRYP_K2LR_REG      0x30
#define CRYP_K2RR_REG      0x34
#define CRYP_K3LR_REG      0x38
#define CRYP_K3RR_REG      0x3C

/* Initialization Vector Registers */
#define CRYP_IV0LR_REG     0x40
#define CRYP_IV0RR_REG     0x44
#define CRYP_IV1LR_REG     0x48
#define CRYP_IV1RR_REG     0x4C

/* ALGOMODE values (combined ALGOMODE[3:0] = bit19 || bits5:3) */
#define CRYP_ALGOMODE_AES_ECB  0x4   /* 0100 */
#define CRYP_ALGOMODE_AES_CBC  0x5   /* 0101 */
#define CRYP_ALGOMODE_AES_CTR  0x6   /* 0110 */
#define CRYP_ALGOMODE_AES_KEY  0x7   /* 0111 - key preparation */
#define CRYP_ALGOMODE_AES_GCM  0x8   /* 1000 */
#define CRYP_ALGOMODE_AES_CCM  0x9   /* 1001 */

/* GCM/CCM phase values */
#define CRYP_PHASE_INIT    0x0
#define CRYP_PHASE_HEADER  0x1
#define CRYP_PHASE_PAYLOAD 0x2
#define CRYP_PHASE_FINAL   0x3

/* DATATYPE: byte swap (for big-endian AES data) */
#define CRYP_DATATYPE_NONE     0x0
#define CRYP_DATATYPE_BYTE     0x2

/* KEYSIZE values */
#define CRYP_KEYSIZE_128  0x0
#define CRYP_KEYSIZE_192  0x1
#define CRYP_KEYSIZE_256  0x2

static inline void SetAlgoMode(size_t base, uint32_t algomode)
{
    uint32_t low3 = algomode & 0x7;
    uint32_t bit3 = (algomode >> 3) & 0x1;

    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_ALGOMODE_ALL_Msk,
                    whal_SetBits(CRYP_CR_ALGOMODE_Msk, CRYP_CR_ALGOMODE_Pos, low3) |
                    whal_SetBits(CRYP_CR_ALGOMODE3_Msk, CRYP_CR_ALGOMODE3_Pos, bit3));
}

static void WriteKey128(size_t base, const uint8_t *key)
{
    /* 128-bit key goes into K2LR..K3RR (bits 127:0) */
    whal_Reg_Write(base, CRYP_K2LR_REG, whal_LoadBe32(key));
    whal_Reg_Write(base, CRYP_K2RR_REG, whal_LoadBe32(key + 4));
    whal_Reg_Write(base, CRYP_K3LR_REG, whal_LoadBe32(key + 8));
    whal_Reg_Write(base, CRYP_K3RR_REG, whal_LoadBe32(key + 12));
}

static void WriteKey256(size_t base, const uint8_t *key)
{
    whal_Reg_Write(base, CRYP_K0LR_REG, whal_LoadBe32(key));
    whal_Reg_Write(base, CRYP_K0RR_REG, whal_LoadBe32(key + 4));
    whal_Reg_Write(base, CRYP_K1LR_REG, whal_LoadBe32(key + 8));
    whal_Reg_Write(base, CRYP_K1RR_REG, whal_LoadBe32(key + 12));
    whal_Reg_Write(base, CRYP_K2LR_REG, whal_LoadBe32(key + 16));
    whal_Reg_Write(base, CRYP_K2RR_REG, whal_LoadBe32(key + 20));
    whal_Reg_Write(base, CRYP_K3LR_REG, whal_LoadBe32(key + 24));
    whal_Reg_Write(base, CRYP_K3RR_REG, whal_LoadBe32(key + 28));
}

static void WriteKey(size_t base, const uint8_t *key, size_t keySz)
{
    if (keySz == 32)
        WriteKey256(base, key);
    else
        WriteKey128(base, key);
}

static void WriteIv(size_t base, const uint8_t *iv)
{
    whal_Reg_Write(base, CRYP_IV0LR_REG, whal_LoadBe32(iv));
    whal_Reg_Write(base, CRYP_IV0RR_REG, whal_LoadBe32(iv + 4));
    whal_Reg_Write(base, CRYP_IV1LR_REG, whal_LoadBe32(iv + 8));
    whal_Reg_Write(base, CRYP_IV1RR_REG, whal_LoadBe32(iv + 12));
}

static void WriteBlock(size_t base, const uint8_t *in)
{
    whal_Reg_Write(base, CRYP_DIN_REG, whal_LoadBe32(in));
    whal_Reg_Write(base, CRYP_DIN_REG, whal_LoadBe32(in + 4));
    whal_Reg_Write(base, CRYP_DIN_REG, whal_LoadBe32(in + 8));
    whal_Reg_Write(base, CRYP_DIN_REG, whal_LoadBe32(in + 12));
}

static void ReadBlock(size_t base, uint8_t *out)
{
    whal_StoreBe32(out,      whal_Reg_Read(base, CRYP_DOUT_REG));
    whal_StoreBe32(out + 4,  whal_Reg_Read(base, CRYP_DOUT_REG));
    whal_StoreBe32(out + 8,  whal_Reg_Read(base, CRYP_DOUT_REG));
    whal_StoreBe32(out + 12, whal_Reg_Read(base, CRYP_DOUT_REG));
}

static whal_Error WaitOutputReady(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, CRYP_SR_REG, CRYP_SR_OFNE_Msk,
                             CRYP_SR_OFNE_Msk, timeout);
}

static whal_Error WaitNotBusy(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, CRYP_SR_REG, CRYP_SR_BUSY_Msk,
                             0, timeout);
}

static void DisableCryp(size_t base)
{
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_CRYPEN_Msk, 0);
}

static void EnableCryp(size_t base)
{
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_CRYPEN_Msk, CRYP_CR_CRYPEN_Msk);
}

static void FlushFifos(size_t base)
{
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_FFLUSH_Msk, CRYP_CR_FFLUSH_Msk);
}

static uint32_t GetKeySizeBits(size_t keySz)
{
    if (keySz == 32) return CRYP_KEYSIZE_256;
    return CRYP_KEYSIZE_128;
}

/*
 * Prepare for AES-ECB/CBC decryption by running key preparation mode.
 * The CRYP peripheral needs the decryption key schedule to be derived
 * from the encryption key before ECB/CBC decryption.
 */
static whal_Error PrepareDecryptKey(size_t base, const uint8_t *key,
                                     size_t keySz, whal_Timeout *timeout)
{
    DisableCryp(base);
    SetAlgoMode(base, CRYP_ALGOMODE_AES_KEY);
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_KEYSIZE_Msk,
                    whal_SetBits(CRYP_CR_KEYSIZE_Msk, CRYP_CR_KEYSIZE_Pos,
                                 GetKeySizeBits(keySz)));
    WriteKey(base, key, keySz);
    EnableCryp(base);

    /* Wait for key preparation to complete (BUSY goes low) */
    whal_Error err = WaitNotBusy(base, timeout);
    DisableCryp(base);
    return err;
}

whal_Error whal_Stm32h7Aes_Init(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg)
        return WHAL_EINVAL;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Aes_Deinit(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg)
        return WHAL_EINVAL;
    DisableCryp(cryptoDev->regmap.base);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h7Aes_AesEcb(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesEcbArgs *args;
    const whal_Stm32h7Aes_Cfg *cfg;
    size_t base;

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesEcbArgs *)opArgs;

    if (!args->key || !args->in || !args->out)
        return WHAL_EINVAL;
    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;
    if (args->sz == 0 || (args->sz & 0xF) != 0)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32h7Aes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;

    DisableCryp(base);

    if (args->dir == WHAL_CRYPTO_DECRYPT) {
        err = PrepareDecryptKey(base, args->key, args->keySz, cfg->timeout);
        if (err)
            return err;
    }

    /* Configure: ALGOMODE=AES-ECB, ALGODIR, KEYSIZE, DATATYPE=none */
    whal_Reg_Update(base, CRYP_CR_REG,
                    CRYP_CR_ALGODIR_Msk | CRYP_CR_DATATYPE_Msk |
                    CRYP_CR_KEYSIZE_Msk,
                    whal_SetBits(CRYP_CR_ALGODIR_Msk, CRYP_CR_ALGODIR_Pos,
                                 (args->dir == WHAL_CRYPTO_DECRYPT) ? 1 : 0) |
                    whal_SetBits(CRYP_CR_DATATYPE_Msk, CRYP_CR_DATATYPE_Pos,
                                 CRYP_DATATYPE_NONE) |
                    whal_SetBits(CRYP_CR_KEYSIZE_Msk, CRYP_CR_KEYSIZE_Pos,
                                 GetKeySizeBits(args->keySz)));
    SetAlgoMode(base, CRYP_ALGOMODE_AES_ECB);

    if (args->dir == WHAL_CRYPTO_ENCRYPT)
        WriteKey(base, args->key, args->keySz);

    FlushFifos(base);
    EnableCryp(base);

    for (size_t i = 0; i < args->sz; i += 16) {
        WriteBlock(base, args->in + i);
        err = WaitOutputReady(base, cfg->timeout);
        if (err)
            goto cleanup;
        ReadBlock(base, args->out + i);
    }

cleanup:
    DisableCryp(base);
    return err;
}

whal_Error whal_Stm32h7Aes_AesCbc(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesCbcArgs *args;
    const whal_Stm32h7Aes_Cfg *cfg;
    size_t base;

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesCbcArgs *)opArgs;

    if (!args->key || !args->iv || !args->in || !args->out)
        return WHAL_EINVAL;
    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;
    if (args->sz == 0 || (args->sz & 0xF) != 0)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32h7Aes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;

    DisableCryp(base);

    if (args->dir == WHAL_CRYPTO_DECRYPT) {
        err = PrepareDecryptKey(base, args->key, args->keySz, cfg->timeout);
        if (err)
            return err;
    }

    whal_Reg_Update(base, CRYP_CR_REG,
                    CRYP_CR_ALGODIR_Msk | CRYP_CR_DATATYPE_Msk |
                    CRYP_CR_KEYSIZE_Msk,
                    whal_SetBits(CRYP_CR_ALGODIR_Msk, CRYP_CR_ALGODIR_Pos,
                                 (args->dir == WHAL_CRYPTO_DECRYPT) ? 1 : 0) |
                    whal_SetBits(CRYP_CR_DATATYPE_Msk, CRYP_CR_DATATYPE_Pos,
                                 CRYP_DATATYPE_NONE) |
                    whal_SetBits(CRYP_CR_KEYSIZE_Msk, CRYP_CR_KEYSIZE_Pos,
                                 GetKeySizeBits(args->keySz)));
    SetAlgoMode(base, CRYP_ALGOMODE_AES_CBC);

    if (args->dir == WHAL_CRYPTO_ENCRYPT)
        WriteKey(base, args->key, args->keySz);

    WriteIv(base, args->iv);
    FlushFifos(base);
    EnableCryp(base);

    for (size_t i = 0; i < args->sz; i += 16) {
        WriteBlock(base, args->in + i);
        err = WaitOutputReady(base, cfg->timeout);
        if (err)
            goto cleanup;
        ReadBlock(base, args->out + i);
    }

cleanup:
    DisableCryp(base);
    return err;
}

whal_Error whal_Stm32h7Aes_AesCtr(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesCtrArgs *args;
    const whal_Stm32h7Aes_Cfg *cfg;
    size_t base;

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesCtrArgs *)opArgs;

    if (!args->key || !args->iv || !args->in || !args->out)
        return WHAL_EINVAL;
    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;
    if (args->sz == 0 || (args->sz & 0xF) != 0)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32h7Aes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;

    DisableCryp(base);

    /* CTR mode: direction is always encrypt (ALGODIR=0) */
    whal_Reg_Update(base, CRYP_CR_REG,
                    CRYP_CR_ALGODIR_Msk | CRYP_CR_DATATYPE_Msk |
                    CRYP_CR_KEYSIZE_Msk,
                    whal_SetBits(CRYP_CR_ALGODIR_Msk, CRYP_CR_ALGODIR_Pos, 0) |
                    whal_SetBits(CRYP_CR_DATATYPE_Msk, CRYP_CR_DATATYPE_Pos,
                                 CRYP_DATATYPE_NONE) |
                    whal_SetBits(CRYP_CR_KEYSIZE_Msk, CRYP_CR_KEYSIZE_Pos,
                                 GetKeySizeBits(args->keySz)));
    SetAlgoMode(base, CRYP_ALGOMODE_AES_CTR);

    WriteKey(base, args->key, args->keySz);
    WriteIv(base, args->iv);
    FlushFifos(base);
    EnableCryp(base);

    for (size_t i = 0; i < args->sz; i += 16) {
        WriteBlock(base, args->in + i);
        err = WaitOutputReady(base, cfg->timeout);
        if (err)
            goto cleanup;
        ReadBlock(base, args->out + i);
    }

cleanup:
    DisableCryp(base);
    return err;
}

whal_Error whal_Stm32h7Aes_AesGcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Crypto_AesGcmArgs *args;
    const whal_Stm32h7Aes_Cfg *cfg;
    size_t base;

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesGcmArgs *)opArgs;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;
    if (args->keySz != 16 && args->keySz != 32)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32h7Aes_Cfg *)cryptoDev->cfg;
    base = cryptoDev->regmap.base;

    DisableCryp(base);

    /* Configure for GCM */
    whal_Reg_Update(base, CRYP_CR_REG,
                    CRYP_CR_ALGODIR_Msk | CRYP_CR_DATATYPE_Msk |
                    CRYP_CR_KEYSIZE_Msk,
                    whal_SetBits(CRYP_CR_ALGODIR_Msk, CRYP_CR_ALGODIR_Pos,
                                 (args->dir == WHAL_CRYPTO_DECRYPT) ? 1 : 0) |
                    whal_SetBits(CRYP_CR_DATATYPE_Msk, CRYP_CR_DATATYPE_Pos,
                                 CRYP_DATATYPE_NONE) |
                    whal_SetBits(CRYP_CR_KEYSIZE_Msk, CRYP_CR_KEYSIZE_Pos,
                                 GetKeySizeBits(args->keySz)));
    SetAlgoMode(base, CRYP_ALGOMODE_AES_GCM);

    WriteKey(base, args->key, args->keySz);

    /* Phase 1: Init */
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_GCM_CCMPH_Msk,
                    whal_SetBits(CRYP_CR_GCM_CCMPH_Msk, CRYP_CR_GCM_CCMPH_Pos,
                                 CRYP_PHASE_INIT));
    WriteIv(base, args->iv);
    EnableCryp(base);
    err = WaitNotBusy(base, cfg->timeout);
    if (err)
        return err;
    DisableCryp(base);

    /* Phase 2: Header (AAD) */
    if (args->aad && args->aadSz > 0) {
        whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_GCM_CCMPH_Msk,
                        whal_SetBits(CRYP_CR_GCM_CCMPH_Msk, CRYP_CR_GCM_CCMPH_Pos,
                                     CRYP_PHASE_HEADER));
        EnableCryp(base);

        size_t fullBlocks = args->aadSz & ~0xFUL;
        for (size_t i = 0; i < fullBlocks; i += 16) {
            WriteBlock(base, args->aad + i);
            err = WaitOutputReady(base, cfg->timeout);
            if (err)
                goto cleanup;
            /* Discard output during header phase */
            (void)whal_Reg_Read(base, CRYP_DOUT_REG);
            (void)whal_Reg_Read(base, CRYP_DOUT_REG);
            (void)whal_Reg_Read(base, CRYP_DOUT_REG);
            (void)whal_Reg_Read(base, CRYP_DOUT_REG);
        }

        /* Handle partial last AAD block */
        if (args->aadSz > fullBlocks) {
            uint8_t pad[16] = {0};
            size_t rem = args->aadSz - fullBlocks;
            for (size_t j = 0; j < rem; j++)
                pad[j] = args->aad[fullBlocks + j];
            WriteBlock(base, pad);
            err = WaitOutputReady(base, cfg->timeout);
            if (err)
                goto cleanup;
            (void)whal_Reg_Read(base, CRYP_DOUT_REG);
            (void)whal_Reg_Read(base, CRYP_DOUT_REG);
            (void)whal_Reg_Read(base, CRYP_DOUT_REG);
            (void)whal_Reg_Read(base, CRYP_DOUT_REG);
        }

        DisableCryp(base);
    }

    /* Phase 3: Payload */
    if (args->in && args->out && args->sz > 0) {
        whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_GCM_CCMPH_Msk,
                        whal_SetBits(CRYP_CR_GCM_CCMPH_Msk, CRYP_CR_GCM_CCMPH_Pos,
                                     CRYP_PHASE_PAYLOAD));
        EnableCryp(base);

        for (size_t i = 0; i < args->sz; i += 16) {
            WriteBlock(base, args->in + i);
            err = WaitOutputReady(base, cfg->timeout);
            if (err)
                goto cleanup;
            ReadBlock(base, args->out + i);
        }

        DisableCryp(base);
    }

    /* Phase 4: Final - generate tag */
    if (args->tag && args->tagSz > 0) {
        whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_GCM_CCMPH_Msk,
                        whal_SetBits(CRYP_CR_GCM_CCMPH_Msk, CRYP_CR_GCM_CCMPH_Pos,
                                     CRYP_PHASE_FINAL));
        EnableCryp(base);

        /* Write bit lengths: aadSz*8 (64-bit) || sz*8 (64-bit) */
        uint64_t aadBits = (uint64_t)args->aadSz * 8;
        uint64_t dataBits = (uint64_t)args->sz * 8;
        whal_Reg_Write(base, CRYP_DIN_REG, (uint32_t)(aadBits >> 32));
        whal_Reg_Write(base, CRYP_DIN_REG, (uint32_t)(aadBits));
        whal_Reg_Write(base, CRYP_DIN_REG, (uint32_t)(dataBits >> 32));
        whal_Reg_Write(base, CRYP_DIN_REG, (uint32_t)(dataBits));

        err = WaitOutputReady(base, cfg->timeout);
        if (err)
            goto cleanup;

        uint8_t tagBuf[16];
        ReadBlock(base, tagBuf);

        size_t copyLen = (args->tagSz < 16) ? args->tagSz : 16;
        for (size_t i = 0; i < copyLen; i++)
            args->tag[i] = tagBuf[i];
    }

cleanup:
    DisableCryp(base);
    return err;
}

whal_Error whal_Stm32h7Aes_AesGmac(whal_Crypto *cryptoDev, void *opArgs)
{
    /* GMAC is GCM with no payload data */
    whal_Crypto_AesGmacArgs *args;

    if (!cryptoDev || !opArgs)
        return WHAL_EINVAL;

    args = (whal_Crypto_AesGmacArgs *)opArgs;

    whal_Crypto_AesGcmArgs gcmArgs = {
        .key = args->key,
        .keySz = args->keySz,
        .iv = args->iv,
        .aad = args->aad,
        .aadSz = args->aadSz,
        .in = NULL,
        .out = NULL,
        .sz = 0,
        .tag = args->tag,
        .tagSz = args->tagSz,
        .dir = WHAL_CRYPTO_ENCRYPT,
    };

    return whal_Stm32h7Aes_AesGcm(cryptoDev, &gcmArgs);
}

whal_Error whal_Stm32h7Aes_AesCcm(whal_Crypto *cryptoDev, void *opArgs)
{
    /* CCM stub - similar to GCM but with CCM-specific block formatting */
    (void)cryptoDev;
    (void)opArgs;
    return WHAL_EINVAL;
}

const whal_CryptoDriver whal_Stm32h7Aes_Driver = {
    .Init = whal_Stm32h7Aes_Init,
    .Deinit = whal_Stm32h7Aes_Deinit,
};
