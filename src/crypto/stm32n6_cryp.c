#include <stdint.h>
#include <wolfHAL/crypto/stm32n6_cryp.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/endian.h>

/* Control Register (CRYP_CR) */
#define CRYP_CR_REG            0x00
#define CRYP_CR_ALGODIR_Pos    2
#define CRYP_CR_ALGODIR_Msk    (1UL << CRYP_CR_ALGODIR_Pos)
#define CRYP_CR_ALGOMODE_LO_Pos 3
#define CRYP_CR_ALGOMODE_LO_Msk (7UL << CRYP_CR_ALGOMODE_LO_Pos)
#define CRYP_CR_DATATYPE_Pos   6
#define CRYP_CR_DATATYPE_Msk   (3UL << CRYP_CR_DATATYPE_Pos)
#define CRYP_CR_KEYSIZE_Pos    8
#define CRYP_CR_KEYSIZE_Msk    (3UL << CRYP_CR_KEYSIZE_Pos)
#define CRYP_CR_FFLUSH_Pos     14
#define CRYP_CR_FFLUSH_Msk     (1UL << CRYP_CR_FFLUSH_Pos)
#define CRYP_CR_CRYPEN_Pos     15
#define CRYP_CR_CRYPEN_Msk     (1UL << CRYP_CR_CRYPEN_Pos)
#define CRYP_CR_GCM_CCMPH_Pos  16
#define CRYP_CR_GCM_CCMPH_Msk  (3UL << CRYP_CR_GCM_CCMPH_Pos)
#define CRYP_CR_ALGOMODE_HI_Pos 19
#define CRYP_CR_ALGOMODE_HI_Msk (1UL << CRYP_CR_ALGOMODE_HI_Pos)
#define CRYP_CR_NPBLB_Pos      20
#define CRYP_CR_NPBLB_Msk      (0xFUL << CRYP_CR_NPBLB_Pos)
#define CRYP_CR_KMOD_Pos       24
#define CRYP_CR_KMOD_Msk       (3UL << CRYP_CR_KMOD_Pos)
#define CRYP_CR_IPRST_Pos      31
#define CRYP_CR_IPRST_Msk      (1UL << CRYP_CR_IPRST_Pos)

#define CRYP_CR_ALGOMODE_Msk \
    (CRYP_CR_ALGOMODE_LO_Msk | CRYP_CR_ALGOMODE_HI_Msk)

/* Pack a 4-bit ALGOMODE into the split CR bitfields (bits 5:3 + bit 19). */
#define CRYP_CR_ALGOMODE(v) \
    ((((uint32_t)(v) & 0x7U) << CRYP_CR_ALGOMODE_LO_Pos) | \
     ((((uint32_t)(v) >> 3) & 0x1U) << CRYP_CR_ALGOMODE_HI_Pos))

/* ALGOMODE values */
#define CRYP_ALGOMODE_AES_ECB     0x4
#define CRYP_ALGOMODE_AES_CBC     0x5
#define CRYP_ALGOMODE_AES_CTR     0x6
#define CRYP_ALGOMODE_AES_KEYPREP 0x7
#define CRYP_ALGOMODE_AES_GCM     0x8
#define CRYP_ALGOMODE_AES_CCM     0x9

#define CRYP_ALGODIR_ENCRYPT 0
#define CRYP_ALGODIR_DECRYPT 1

#define CRYP_KEYSIZE_128 0
#define CRYP_KEYSIZE_192 1
#define CRYP_KEYSIZE_256 2

#define CRYP_GCM_CCMPH_INIT    0
#define CRYP_GCM_CCMPH_HEADER  1
#define CRYP_GCM_CCMPH_PAYLOAD 2
#define CRYP_GCM_CCMPH_FINAL   3

/* Status Register (CRYP_SR) */
#define CRYP_SR_REG            0x04
#define CRYP_SR_IFEM_Msk       (1UL << 0)
#define CRYP_SR_IFNF_Msk       (1UL << 1)
#define CRYP_SR_OFNE_Msk       (1UL << 2)
#define CRYP_SR_OFFU_Msk       (1UL << 3)
#define CRYP_SR_BUSY_Msk       (1UL << 4)
#define CRYP_SR_KERF_Msk       (1UL << 6)
#define CRYP_SR_KEYVALID_Msk   (1UL << 7)

/* Data Registers */
#define CRYP_DINR_REG          0x08
#define CRYP_DOUTR_REG         0x0C

/* Key Registers (write-only): K0LR..K3RR at 0x20..0x3C */
#define CRYP_K0LR_REG          0x20
#define CRYP_K0RR_REG          0x24
#define CRYP_K1LR_REG          0x28
#define CRYP_K1RR_REG          0x2C
#define CRYP_K2LR_REG          0x30
#define CRYP_K2RR_REG          0x34
#define CRYP_K3LR_REG          0x38
#define CRYP_K3RR_REG          0x3C

/* Initialization Vector Registers: IV0LR..IV1RR at 0x40..0x4C */
#define CRYP_IV0LR_REG         0x40
#define CRYP_IV0RR_REG         0x44
#define CRYP_IV1LR_REG         0x48
#define CRYP_IV1RR_REG         0x4C

static whal_Error WaitKeyValid(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, CRYP_SR_REG, CRYP_SR_KEYVALID_Msk,
                             CRYP_SR_KEYVALID_Msk, timeout);
}

static whal_Error WaitBusyClear(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, CRYP_SR_REG, CRYP_SR_BUSY_Msk, 0, timeout);
}

static whal_Error WaitCrypEnClear(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, CRYP_CR_REG, CRYP_CR_CRYPEN_Msk, 0, timeout);
}

static whal_Error WaitOutputReady(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, CRYP_SR_REG, CRYP_SR_OFNE_Msk,
                             CRYP_SR_OFNE_Msk, timeout);
}

static void DisableAndFlush(size_t base)
{
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_CRYPEN_Msk, 0);
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_FFLUSH_Msk, CRYP_CR_FFLUSH_Msk);
}

static void Enable(size_t base)
{
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_CRYPEN_Msk, CRYP_CR_CRYPEN_Msk);
}

static void Disable(size_t base)
{
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_CRYPEN_Msk, 0);
}

/*
 * Write the AES key into CRYP_KxL/R registers in the order required by the
 * hardware key scheduler: high-order 32-bit words first, descending into the
 * low 32 bits (K3RR last). The key is read big-endian from the input buffer.
 */
static void WriteKey(size_t base, const uint8_t *key, size_t keySz)
{
    const uint8_t *k = key;

    if (keySz == 32) {
        whal_Reg_Write(base, CRYP_K0LR_REG, whal_LoadBe32(k));
        whal_Reg_Write(base, CRYP_K0RR_REG, whal_LoadBe32(k + 4));
        whal_Reg_Write(base, CRYP_K1LR_REG, whal_LoadBe32(k + 8));
        whal_Reg_Write(base, CRYP_K1RR_REG, whal_LoadBe32(k + 12));
        k += 16;
    } else if (keySz == 24) {
        whal_Reg_Write(base, CRYP_K1LR_REG, whal_LoadBe32(k));
        whal_Reg_Write(base, CRYP_K1RR_REG, whal_LoadBe32(k + 4));
        k += 8;
    }
    whal_Reg_Write(base, CRYP_K2LR_REG, whal_LoadBe32(k));
    whal_Reg_Write(base, CRYP_K2RR_REG, whal_LoadBe32(k + 4));
    whal_Reg_Write(base, CRYP_K3LR_REG, whal_LoadBe32(k + 8));
    whal_Reg_Write(base, CRYP_K3RR_REG, whal_LoadBe32(k + 12));
}

static void WriteIv16(size_t base, const uint8_t *iv)
{
    whal_Reg_Write(base, CRYP_IV0LR_REG, whal_LoadBe32(iv));
    whal_Reg_Write(base, CRYP_IV0RR_REG, whal_LoadBe32(iv + 4));
    whal_Reg_Write(base, CRYP_IV1LR_REG, whal_LoadBe32(iv + 8));
    whal_Reg_Write(base, CRYP_IV1RR_REG, whal_LoadBe32(iv + 12));
}

static void WriteBlock(size_t base, const uint8_t *in)
{
    whal_Reg_Write(base, CRYP_DINR_REG, whal_LoadBe32(in));
    whal_Reg_Write(base, CRYP_DINR_REG, whal_LoadBe32(in + 4));
    whal_Reg_Write(base, CRYP_DINR_REG, whal_LoadBe32(in + 8));
    whal_Reg_Write(base, CRYP_DINR_REG, whal_LoadBe32(in + 12));
}

static void ReadBlock(size_t base, uint8_t *out)
{
    whal_StoreBe32(out,      whal_Reg_Read(base, CRYP_DOUTR_REG));
    whal_StoreBe32(out + 4,  whal_Reg_Read(base, CRYP_DOUTR_REG));
    whal_StoreBe32(out + 8,  whal_Reg_Read(base, CRYP_DOUTR_REG));
    whal_StoreBe32(out + 12, whal_Reg_Read(base, CRYP_DOUTR_REG));
}

/*
 * Configure CR for an AES operation: clears every mode-related field and sets
 * the requested ALGOMODE/ALGODIR/KEYSIZE/KMOD/PHASE/NPBLB. CRYPEN/FFLUSH/IPRST
 * are left untouched.
 */
static void ConfigureMode(size_t base, uint32_t algoMode, uint32_t algoDir,
                          uint32_t keySize, uint32_t phase, uint32_t npblb,
                          uint32_t kmod)
{
    uint32_t mask = CRYP_CR_ALGOMODE_Msk | CRYP_CR_ALGODIR_Msk |
                    CRYP_CR_KEYSIZE_Msk | CRYP_CR_DATATYPE_Msk |
                    CRYP_CR_GCM_CCMPH_Msk | CRYP_CR_NPBLB_Msk |
                    CRYP_CR_KMOD_Msk;
    uint32_t value = CRYP_CR_ALGOMODE(algoMode) |
                     whal_SetBits(CRYP_CR_ALGODIR_Msk, CRYP_CR_ALGODIR_Pos,
                                  algoDir) |
                     whal_SetBits(CRYP_CR_KEYSIZE_Msk, CRYP_CR_KEYSIZE_Pos,
                                  keySize) |
                     whal_SetBits(CRYP_CR_GCM_CCMPH_Msk, CRYP_CR_GCM_CCMPH_Pos,
                                  phase) |
                     whal_SetBits(CRYP_CR_NPBLB_Msk, CRYP_CR_NPBLB_Pos, npblb) |
                     whal_SetBits(CRYP_CR_KMOD_Msk, CRYP_CR_KMOD_Pos, kmod);

    whal_Reg_Update(base, CRYP_CR_REG, mask, value);
}

static whal_Error KeySizeBits(size_t keySz, uint32_t *out)
{
    if (keySz == 16) {
        *out = CRYP_KEYSIZE_128;
    } else if (keySz == 24) {
        *out = CRYP_KEYSIZE_192;
    } else if (keySz == 32) {
        *out = CRYP_KEYSIZE_256;
    } else {
        return WHAL_ENOTSUP;
    }
    return WHAL_SUCCESS;
}

#ifdef WHAL_CFG_STM32N6_CRYP_DIRECT_API_MAPPING
#define whal_Stm32n6_Cryp_Init    whal_Crypto_Init
#define whal_Stm32n6_Cryp_Deinit  whal_Crypto_Deinit
#define whal_Stm32n6_Cryp_StartOp whal_Crypto_StartOp
#define whal_Stm32n6_Cryp_Process whal_Crypto_Process
#define whal_Stm32n6_Cryp_EndOp   whal_Crypto_EndOp
#endif /* WHAL_CFG_STM32N6_CRYP_DIRECT_API_MAPPING */

whal_Error whal_Stm32n6_Cryp_Init(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg) {
        return WHAL_EINVAL;
    }
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6_Cryp_Deinit(whal_Crypto *cryptoDev)
{
    if (!cryptoDev || !cryptoDev->cfg) {
        return WHAL_EINVAL;
    }

    Disable(cryptoDev->regmap.base);
    return WHAL_SUCCESS;
}

#if defined(WHAL_CFG_CRYPTO_AES_ECB) || defined(WHAL_CFG_CRYPTO_AES_CBC) || \
    defined(WHAL_CFG_CRYPTO_AES_CTR) || defined(WHAL_CFG_CRYPTO_AES_GCM) || \
    defined(WHAL_CFG_CRYPTO_AES_GMAC) || defined(WHAL_CFG_CRYPTO_AES_CCM)
/*
 * Run the ECB/CBC decryption key-preparation pass. Required before performing
 * an ECB or CBC decryption: the hardware computes the last round key, then
 * auto-clears CRYPEN. Caller must hold the timeout and have keys staged.
 */
static whal_Error PrepareDecryptionKey(size_t base, const uint8_t *key,
                                       size_t keySz, uint32_t keySizeBits,
                                       whal_Timeout *timeout)
{
    whal_Error err;

    DisableAndFlush(base);
    ConfigureMode(base, CRYP_ALGOMODE_AES_KEYPREP, CRYP_ALGODIR_ENCRYPT,
                  keySizeBits, 0, 0, 0);
    WriteKey(base, key, keySz);
    err = WaitKeyValid(base, timeout);
    if (err)
        return err;
    Enable(base);
    return WaitCrypEnClear(base, timeout);
}
#endif

#if defined(WHAL_CFG_CRYPTO_AES_ECB) || defined(WHAL_CFG_CRYPTO_AES_CBC) || \
    defined(WHAL_CFG_CRYPTO_AES_CTR)
static whal_Error Process_BlockCipher(whal_Crypto *cryptoDev,
                                      const uint8_t *in, uint8_t *out,
                                      size_t sz)
{
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    whal_Error err;
    size_t i;

    if (sz == 0)
        return WHAL_SUCCESS;

    if (!in || !out || (sz & 0xF) != 0)
        return WHAL_EINVAL;

    for (i = 0; i < sz; i += 16) {
        WriteBlock(base, in + i);
        err = WaitOutputReady(base, cfg->timeout);
        if (err) {
            Disable(base);
            return err;
        }
        ReadBlock(base, out + i);
    }

    return WHAL_SUCCESS;
}

static whal_Error EndOp_BlockCipher(whal_Crypto *cryptoDev)
{
    Disable(cryptoDev->regmap.base);
    return WHAL_SUCCESS;
}
#endif /* AES_ECB || AES_CBC || AES_CTR */

#ifdef WHAL_CFG_CRYPTO_AES_ECB
static whal_Error StartOp_AesEcb(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesEcbArgs *args = (whal_Crypto_AesEcbArgs *)opArgs;
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint32_t keySizeBits;
    whal_Error err;

    if (!args->key)
        return WHAL_EINVAL;

    err = KeySizeBits(args->keySz, &keySizeBits);
    if (err)
        return err;

    if (args->dir == WHAL_CRYPTO_DECRYPT) {
        err = PrepareDecryptionKey(base, args->key, args->keySz, keySizeBits,
                                   cfg->timeout);
        if (err)
            return err;
        ConfigureMode(base, CRYP_ALGOMODE_AES_ECB, CRYP_ALGODIR_DECRYPT,
                      keySizeBits, 0, 0, 0);
        Enable(base);
        return WHAL_SUCCESS;
    }

    DisableAndFlush(base);
    ConfigureMode(base, CRYP_ALGOMODE_AES_ECB, CRYP_ALGODIR_ENCRYPT,
                  keySizeBits, 0, 0, 0);
    WriteKey(base, args->key, args->keySz);
    err = WaitKeyValid(base, cfg->timeout);
    if (err)
        return err;
    Enable(base);
    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_ECB */

#ifdef WHAL_CFG_CRYPTO_AES_CBC
static whal_Error StartOp_AesCbc(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCbcArgs *args = (whal_Crypto_AesCbcArgs *)opArgs;
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint32_t keySizeBits;
    whal_Error err;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;

    err = KeySizeBits(args->keySz, &keySizeBits);
    if (err)
        return err;

    if (args->dir == WHAL_CRYPTO_DECRYPT) {
        err = PrepareDecryptionKey(base, args->key, args->keySz, keySizeBits,
                                   cfg->timeout);
        if (err)
            return err;
        ConfigureMode(base, CRYP_ALGOMODE_AES_CBC, CRYP_ALGODIR_DECRYPT,
                      keySizeBits, 0, 0, 0);
        WriteIv16(base, (const uint8_t *)args->iv);
        Enable(base);
        return WHAL_SUCCESS;
    }

    DisableAndFlush(base);
    ConfigureMode(base, CRYP_ALGOMODE_AES_CBC, CRYP_ALGODIR_ENCRYPT,
                  keySizeBits, 0, 0, 0);
    WriteIv16(base, (const uint8_t *)args->iv);
    WriteKey(base, args->key, args->keySz);
    err = WaitKeyValid(base, cfg->timeout);
    if (err)
        return err;
    Enable(base);
    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_CBC */

#ifdef WHAL_CFG_CRYPTO_AES_CTR
static whal_Error StartOp_AesCtr(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCtrArgs *args = (whal_Crypto_AesCtrArgs *)opArgs;
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint32_t keySizeBits;
    uint32_t algoDir;
    whal_Error err;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;

    err = KeySizeBits(args->keySz, &keySizeBits);
    if (err)
        return err;

    algoDir = (args->dir == WHAL_CRYPTO_ENCRYPT) ? CRYP_ALGODIR_ENCRYPT
                                                 : CRYP_ALGODIR_DECRYPT;

    DisableAndFlush(base);
    ConfigureMode(base, CRYP_ALGOMODE_AES_CTR, algoDir, keySizeBits, 0, 0, 0);
    WriteIv16(base, (const uint8_t *)args->iv);
    WriteKey(base, args->key, args->keySz);
    err = WaitKeyValid(base, cfg->timeout);
    if (err)
        return err;
    Enable(base);
    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_CTR */

#if defined(WHAL_CFG_CRYPTO_AES_GCM) || defined(WHAL_CFG_CRYPTO_AES_GMAC)
/*
 * Run GCM init phase: configure CR for GCM, load IV (12 bytes + counter=2)
 * and key, then enable CRYP and wait for the hash subkey computation to
 * complete (CRYPEN auto-clears).
 */
static whal_Error GcmInit(whal_Crypto *cryptoDev, const uint8_t *key,
                          size_t keySz, uint32_t keySizeBits,
                          uint32_t algoDir, const uint8_t *iv12)
{
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    whal_Error err;

    DisableAndFlush(base);
    ConfigureMode(base, CRYP_ALGOMODE_AES_GCM, algoDir, keySizeBits,
                  CRYP_GCM_CCMPH_INIT, 0, 0);
    whal_Reg_Write(base, CRYP_IV0LR_REG, whal_LoadBe32(iv12));
    whal_Reg_Write(base, CRYP_IV0RR_REG, whal_LoadBe32(iv12 + 4));
    whal_Reg_Write(base, CRYP_IV1LR_REG, whal_LoadBe32(iv12 + 8));
    whal_Reg_Write(base, CRYP_IV1RR_REG, 0x00000002UL);
    WriteKey(base, key, keySz);
    err = WaitKeyValid(base, cfg->timeout);
    if (err)
        return err;
    Enable(base);
    return WaitCrypEnClear(base, cfg->timeout);
}

/*
 * Feed AAD blocks during the header phase. The peripheral consumes header
 * data without producing output; the last partial block must be zero-padded.
 */
static whal_Error GcmHeaderPhase(whal_Crypto *cryptoDev, const uint8_t *aad,
                                 size_t aadSz)
{
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    whal_Error err;
    size_t i;

    if (aadSz == 0)
        return WHAL_SUCCESS;

    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_GCM_CCMPH_Msk,
                    whal_SetBits(CRYP_CR_GCM_CCMPH_Msk,
                                 CRYP_CR_GCM_CCMPH_Pos,
                                 CRYP_GCM_CCMPH_HEADER));
    Enable(base);

    for (i = 0; i < aadSz; i += 16) {
        size_t remain = aadSz - i;
        if (remain >= 16) {
            WriteBlock(base, aad + i);
        } else {
            uint8_t pad[16] = {0};
            size_t j;
            for (j = 0; j < remain; j++)
                pad[j] = aad[i + j];
            WriteBlock(base, pad);
        }
    }

    err = WaitBusyClear(base, cfg->timeout);
    if (err)
        return err;
    return WHAL_SUCCESS;
}
#endif /* AES_GCM || AES_GMAC */

#ifdef WHAL_CFG_CRYPTO_AES_GCM
static whal_Error StartOp_AesGcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesGcmArgs *args = (whal_Crypto_AesGcmArgs *)opArgs;
    uint32_t keySizeBits;
    uint32_t algoDir;
    whal_Error err;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;
    if (args->ivSz != 12)
        return WHAL_ENOTSUP;
    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;
    if (args->sz > 0 && (!args->in || !args->out))
        return WHAL_EINVAL;

    err = KeySizeBits(args->keySz, &keySizeBits);
    if (err)
        return err;

    algoDir = (args->dir == WHAL_CRYPTO_ENCRYPT) ? CRYP_ALGODIR_ENCRYPT
                                                 : CRYP_ALGODIR_DECRYPT;

    err = GcmInit(cryptoDev, (const uint8_t *)args->key, args->keySz,
                  keySizeBits, algoDir, (const uint8_t *)args->iv);
    if (err)
        return err;

    return GcmHeaderPhase(cryptoDev, (const uint8_t *)args->aad, args->aadSz);
}

static whal_Error Process_AesGcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesGcmArgs *args = (whal_Crypto_AesGcmArgs *)opArgs;
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    whal_Error err;
    size_t i;

    if (args->sz == 0)
        return WHAL_SUCCESS;

    if (!args->in || !args->out)
        return WHAL_EINVAL;

    Disable(base);
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_GCM_CCMPH_Msk,
                    whal_SetBits(CRYP_CR_GCM_CCMPH_Msk,
                                 CRYP_CR_GCM_CCMPH_Pos,
                                 CRYP_GCM_CCMPH_PAYLOAD));
    Enable(base);

    for (i = 0; i < args->sz; i += 16) {
        const uint8_t *in = (const uint8_t *)args->in + i;
        uint8_t *out = (uint8_t *)args->out + i;
        size_t remain = args->sz - i;
        uint8_t blockIn[16] = {0};
        uint8_t blockOut[16];
        size_t j;

        if (remain >= 16) {
            WriteBlock(base, in);
        } else {
            if (args->dir == WHAL_CRYPTO_ENCRYPT) {
                whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_NPBLB_Msk,
                                whal_SetBits(CRYP_CR_NPBLB_Msk,
                                             CRYP_CR_NPBLB_Pos,
                                             16 - remain));
            }
            for (j = 0; j < remain; j++)
                blockIn[j] = in[j];
            WriteBlock(base, blockIn);
        }

        err = WaitOutputReady(base, cfg->timeout);
        if (err) {
            Disable(base);
            return err;
        }

        if (remain >= 16) {
            ReadBlock(base, out);
        } else {
            ReadBlock(base, blockOut);
            for (j = 0; j < remain; j++)
                out[j] = blockOut[j];
        }
    }

    return WHAL_SUCCESS;
}

static whal_Error EndOp_AesGcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesGcmArgs *args = (whal_Crypto_AesGcmArgs *)opArgs;
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint8_t tagBuf[16];
    uint64_t aadBits;
    uint64_t payloadBits;
    whal_Error err;
    size_t i;

    if (!args->tag || args->tagSz == 0 || args->tagSz > 16)
        return WHAL_EINVAL;

    Disable(base);
    whal_Reg_Update(base, CRYP_CR_REG,
                    CRYP_CR_GCM_CCMPH_Msk | CRYP_CR_ALGODIR_Msk,
                    whal_SetBits(CRYP_CR_GCM_CCMPH_Msk,
                                 CRYP_CR_GCM_CCMPH_Pos,
                                 CRYP_GCM_CCMPH_FINAL));
    Enable(base);

    aadBits = (uint64_t)args->aadSz * 8;
    payloadBits = (uint64_t)args->sz * 8;
    whal_Reg_Write(base, CRYP_DINR_REG, (uint32_t)(aadBits >> 32));
    whal_Reg_Write(base, CRYP_DINR_REG, (uint32_t)aadBits);
    whal_Reg_Write(base, CRYP_DINR_REG, (uint32_t)(payloadBits >> 32));
    whal_Reg_Write(base, CRYP_DINR_REG, (uint32_t)payloadBits);

    err = WaitOutputReady(base, cfg->timeout);
    if (err) {
        Disable(base);
        return err;
    }

    ReadBlock(base, tagBuf);
    for (i = 0; i < args->tagSz; i++)
        ((uint8_t *)args->tag)[i] = tagBuf[i];

    Disable(base);
    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_GCM */

#ifdef WHAL_CFG_CRYPTO_AES_GMAC
static whal_Error StartOp_AesGmac(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesGmacArgs *args = (whal_Crypto_AesGmacArgs *)opArgs;
    uint32_t keySizeBits;
    whal_Error err;

    if (!args->key || !args->iv)
        return WHAL_EINVAL;
    if (args->ivSz != 12)
        return WHAL_ENOTSUP;
    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;

    err = KeySizeBits(args->keySz, &keySizeBits);
    if (err)
        return err;

    err = GcmInit(cryptoDev, (const uint8_t *)args->key, args->keySz,
                  keySizeBits, CRYP_ALGODIR_ENCRYPT,
                  (const uint8_t *)args->iv);
    if (err)
        return err;

    return GcmHeaderPhase(cryptoDev, (const uint8_t *)args->aad, args->aadSz);
}

static whal_Error EndOp_AesGmac(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesGmacArgs *args = (whal_Crypto_AesGmacArgs *)opArgs;
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint8_t tagBuf[16];
    uint64_t aadBits;
    whal_Error err;
    size_t i;

    if (!args->tag || args->tagSz == 0 || args->tagSz > 16)
        return WHAL_EINVAL;

    Disable(base);
    whal_Reg_Update(base, CRYP_CR_REG,
                    CRYP_CR_GCM_CCMPH_Msk | CRYP_CR_ALGODIR_Msk,
                    whal_SetBits(CRYP_CR_GCM_CCMPH_Msk,
                                 CRYP_CR_GCM_CCMPH_Pos,
                                 CRYP_GCM_CCMPH_FINAL));
    Enable(base);

    aadBits = (uint64_t)args->aadSz * 8;
    whal_Reg_Write(base, CRYP_DINR_REG, (uint32_t)(aadBits >> 32));
    whal_Reg_Write(base, CRYP_DINR_REG, (uint32_t)aadBits);
    whal_Reg_Write(base, CRYP_DINR_REG, 0);
    whal_Reg_Write(base, CRYP_DINR_REG, 0);

    err = WaitOutputReady(base, cfg->timeout);
    if (err) {
        Disable(base);
        return err;
    }

    ReadBlock(base, tagBuf);
    for (i = 0; i < args->tagSz; i++)
        ((uint8_t *)args->tag)[i] = tagBuf[i];

    Disable(base);
    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_GMAC */

#ifdef WHAL_CFG_CRYPTO_AES_CCM
/*
 * Build the 16-byte CCM B0 first authentication block from the user nonce,
 * tag length, and message length per NIST SP 800-38C Appendix A.
 */
static void CcmBuildB0(const uint8_t *nonce, size_t nonceSz, size_t tagSz,
                       size_t msgSz, int hasAad, uint8_t *b0)
{
    size_t q = 15 - nonceSz;
    size_t i;
    size_t msg = msgSz;

    b0[0] = (uint8_t)((hasAad ? 0x40 : 0) |
                      (((tagSz - 2) / 2) << 3) |
                      (q - 1));
    for (i = 0; i < nonceSz; i++)
        b0[1 + i] = nonce[i];
    for (i = 0; i < q; i++) {
        b0[15 - i] = (uint8_t)(msg & 0xFF);
        msg >>= 8;
    }
}

static whal_Error StartOp_AesCcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCcmArgs *args = (whal_Crypto_AesCcmArgs *)opArgs;
    whal_Stm32n6_Cryp_Cfg *cfg = (whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint32_t keySizeBits;
    uint32_t algoDir;
    uint8_t b0[16];
    uint8_t ctr1[16];
    size_t q;
    size_t i;
    whal_Error err;

    if (!args->key || !args->nonce)
        return WHAL_EINVAL;
    if (args->nonceSz < 7 || args->nonceSz > 13)
        return WHAL_EINVAL;
    if (args->tagSz < 4 || args->tagSz > 16 || (args->tagSz & 1) != 0)
        return WHAL_EINVAL;
    if (args->aadSz > 0 && !args->aad)
        return WHAL_EINVAL;
    if (args->sz > 0 && (!args->in || !args->out))
        return WHAL_EINVAL;

    err = KeySizeBits(args->keySz, &keySizeBits);
    if (err)
        return err;

    algoDir = (args->dir == WHAL_CRYPTO_ENCRYPT) ? CRYP_ALGODIR_ENCRYPT
                                                 : CRYP_ALGODIR_DECRYPT;

    q = 15 - args->nonceSz;
    CcmBuildB0((const uint8_t *)args->nonce, args->nonceSz, args->tagSz,
               args->sz, args->aadSz > 0, b0);

    /* CTR0 = B0 with the top-5 flag bits cleared and the message-length
     * bytes (last q bytes) zeroed. Saved on cfg for use in EndOp to encrypt
     * the tag (the streaming Finalize wrapper drops the nonce). */
    for (i = 0; i < 16; i++)
        cfg->ccmCtr0[i] = b0[i];
    cfg->ccmCtr0[0] &= 0x07;
    for (i = 16 - q; i < 16; i++)
        cfg->ccmCtr0[i] = 0;

    /* CTR1 = CTR0 with bit 0 set (counter = 1). Per RM0486 Table 421 this
     * is what the IV registers receive at init time. */
    for (i = 0; i < 16; i++)
        ctr1[i] = cfg->ccmCtr0[i];
    ctr1[15] |= 0x01;

    DisableAndFlush(base);
    ConfigureMode(base, CRYP_ALGOMODE_AES_CCM, algoDir, keySizeBits,
                  CRYP_GCM_CCMPH_INIT, 0, 0);
    WriteIv16(base, ctr1);
    WriteKey(base, args->key, args->keySz);
    err = WaitKeyValid(base, cfg->timeout);
    if (err)
        return err;
    Enable(base);

    /* Feed B0 to start the CBC-MAC; CRYPEN auto-clears when init completes. */
    WriteBlock(base, b0);
    err = WaitCrypEnClear(base, cfg->timeout);
    if (err)
        return err;

    if (args->aadSz > 0) {
        const uint8_t *aad = (const uint8_t *)args->aad;
        uint8_t hdr[16] = {0};
        size_t hdrOff;
        size_t aadOff = 0;

        whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_GCM_CCMPH_Msk,
                        whal_SetBits(CRYP_CR_GCM_CCMPH_Msk,
                                     CRYP_CR_GCM_CCMPH_Pos,
                                     CRYP_GCM_CCMPH_HEADER));
        Enable(base);

        hdr[0] = (uint8_t)(args->aadSz >> 8);
        hdr[1] = (uint8_t)args->aadSz;
        hdrOff = 2;
        while (hdrOff < 16 && aadOff < args->aadSz)
            hdr[hdrOff++] = aad[aadOff++];
        WriteBlock(base, hdr);

        while (aadOff < args->aadSz) {
            uint8_t blk[16] = {0};
            size_t j;
            for (j = 0; j < 16 && aadOff < args->aadSz; j++)
                blk[j] = aad[aadOff++];
            WriteBlock(base, blk);
        }

        err = WaitBusyClear(base, cfg->timeout);
        if (err)
            return err;
    }

    return WHAL_SUCCESS;
}

static whal_Error Process_AesCcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCcmArgs *args = (whal_Crypto_AesCcmArgs *)opArgs;
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    whal_Error err;
    size_t i;

    if (args->sz == 0)
        return WHAL_SUCCESS;

    if (!args->in || !args->out)
        return WHAL_EINVAL;

    Disable(base);
    whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_GCM_CCMPH_Msk,
                    whal_SetBits(CRYP_CR_GCM_CCMPH_Msk,
                                 CRYP_CR_GCM_CCMPH_Pos,
                                 CRYP_GCM_CCMPH_PAYLOAD));
    Enable(base);

    for (i = 0; i < args->sz; i += 16) {
        const uint8_t *in = (const uint8_t *)args->in + i;
        uint8_t *out = (uint8_t *)args->out + i;
        size_t remain = args->sz - i;
        uint8_t blockIn[16] = {0};
        uint8_t blockOut[16];
        size_t j;

        if (remain >= 16) {
            WriteBlock(base, in);
        } else {
            if (args->dir == WHAL_CRYPTO_DECRYPT) {
                whal_Reg_Update(base, CRYP_CR_REG, CRYP_CR_NPBLB_Msk,
                                whal_SetBits(CRYP_CR_NPBLB_Msk,
                                             CRYP_CR_NPBLB_Pos,
                                             16 - remain));
            }
            for (j = 0; j < remain; j++)
                blockIn[j] = in[j];
            WriteBlock(base, blockIn);
        }

        err = WaitOutputReady(base, cfg->timeout);
        if (err) {
            Disable(base);
            return err;
        }

        if (remain >= 16) {
            ReadBlock(base, out);
        } else {
            ReadBlock(base, blockOut);
            for (j = 0; j < remain; j++)
                out[j] = blockOut[j];
        }
    }

    return WHAL_SUCCESS;
}

static whal_Error EndOp_AesCcm(whal_Crypto *cryptoDev, void *opArgs)
{
    whal_Crypto_AesCcmArgs *args = (whal_Crypto_AesCcmArgs *)opArgs;
    const whal_Stm32n6_Cryp_Cfg *cfg =
        (const whal_Stm32n6_Cryp_Cfg *)cryptoDev->cfg;
    size_t base = cryptoDev->regmap.base;
    uint8_t tagBuf[16];
    whal_Error err;
    size_t i;

    if (!args->tag || args->tagSz < 4 || args->tagSz > 16 ||
        (args->tagSz & 1) != 0)
        return WHAL_EINVAL;

    Disable(base);
    whal_Reg_Update(base, CRYP_CR_REG,
                    CRYP_CR_GCM_CCMPH_Msk | CRYP_CR_ALGODIR_Msk,
                    whal_SetBits(CRYP_CR_GCM_CCMPH_Msk,
                                 CRYP_CR_GCM_CCMPH_Pos,
                                 CRYP_GCM_CCMPH_FINAL));
    Enable(base);

    WriteBlock(base, cfg->ccmCtr0);

    err = WaitOutputReady(base, cfg->timeout);
    if (err) {
        Disable(base);
        return err;
    }

    ReadBlock(base, tagBuf);
    for (i = 0; i < args->tagSz; i++)
        ((uint8_t *)args->tag)[i] = tagBuf[i];

    Disable(base);
    return WHAL_SUCCESS;
}
#endif /* WHAL_CFG_CRYPTO_AES_CCM */

whal_Error whal_Stm32n6_Cryp_StartOp(whal_Crypto *cryptoDev, size_t opId,
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

whal_Error whal_Stm32n6_Cryp_Process(whal_Crypto *cryptoDev, size_t opId,
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

whal_Error whal_Stm32n6_Cryp_EndOp(whal_Crypto *cryptoDev, size_t opId,
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

#ifndef WHAL_CFG_STM32N6_CRYP_DIRECT_API_MAPPING
const whal_CryptoDriver whal_Stm32n6_Cryp_Driver = {
    .Init    = whal_Stm32n6_Cryp_Init,
    .Deinit  = whal_Stm32n6_Cryp_Deinit,
    .StartOp = whal_Stm32n6_Cryp_StartOp,
    .Process = whal_Stm32n6_Cryp_Process,
    .EndOp   = whal_Stm32n6_Cryp_EndOp,
};
#endif /* !WHAL_CFG_STM32N6_CRYP_DIRECT_API_MAPPING */
