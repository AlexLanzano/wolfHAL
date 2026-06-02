/* stm32f4_hash.c
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include <stdint.h>
#include "board.h"  /* provides WHAL_CFG_STM32F4_HASH*_DEV initializers */
#include <wolfHAL/crypto/stm32f4_hash.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/endian.h>

const whal_Crypto     whal_Stm32f4_Hash_Dev       = WHAL_CFG_STM32F4_HASH_DEV;
const whal_Sha1       whal_Stm32f4_Sha1_Dev       = WHAL_CFG_STM32F4_HASH_SHA1_DEV;
const whal_Sha224     whal_Stm32f4_Sha224_Dev     = WHAL_CFG_STM32F4_HASH_SHA224_DEV;
const whal_Sha256     whal_Stm32f4_Sha256_Dev     = WHAL_CFG_STM32F4_HASH_SHA256_DEV;
const whal_HmacSha1   whal_Stm32f4_HmacSha1_Dev   = WHAL_CFG_STM32F4_HASH_HMAC_SHA1_DEV;
const whal_HmacSha224 whal_Stm32f4_HmacSha224_Dev = WHAL_CFG_STM32F4_HASH_HMAC_SHA224_DEV;
const whal_HmacSha256 whal_Stm32f4_HmacSha256_Dev = WHAL_CFG_STM32F4_HASH_HMAC_SHA256_DEV;

/*
 * STM32F4 (F42xxx/F43xxx) HASH register map (RM0090 §25.4.2 for the
 * F43xxx CR variant, §25.4.5 for HR0..7 at 0x310..0x32C).
 *
 *   CR    @ 0x00
 *   DIN   @ 0x04
 *   STR   @ 0x08
 *   SR    @ 0x24
 *   HR0..4 (legacy aliases) @ 0x0C..0x1C
 *   HR0..7 (canonical)       @ 0x310..0x32C
 *
 * CR bit layout differs from STM32WBA HASH only in ALGO: F4 splits it
 * across bit 18 (MSB) and bit 7 (LSB) rather than the contiguous bits
 * 18:17 used by WBA. The encoding values are unchanged:
 *   00 = SHA-1, 01 = MD5, 10 = SHA-224, 11 = SHA-256.
 *
 * DATATYPE on F4 lives at bits 5:4 (RM0090 §25.4.2) — the same position as
 * STM32WBA HASH after cross-checking the textual prose against the figure.
 */

/* Control Register */
#define HASH_CR_REG           0x00

#define HASH_CR_INIT_Pos      2
#define HASH_CR_INIT_Msk      (1UL << HASH_CR_INIT_Pos)

#define HASH_CR_DMAE_Pos      3
#define HASH_CR_DMAE_Msk      (1UL << HASH_CR_DMAE_Pos)

#define HASH_CR_DATATYPE_Pos  4
#define HASH_CR_DATATYPE_Msk  (3UL << HASH_CR_DATATYPE_Pos)

#define HASH_CR_MODE_Pos      6
#define HASH_CR_MODE_Msk      (1UL << HASH_CR_MODE_Pos)

/* ALGO is split: ALGO[0] at bit 7, ALGO[1] at bit 18 (RM0090 §25.4.2). */
#define HASH_CR_ALGO0_Pos     7
#define HASH_CR_ALGO0_Msk     (1UL << HASH_CR_ALGO0_Pos)
#define HASH_CR_ALGO1_Pos     18
#define HASH_CR_ALGO1_Msk     (1UL << HASH_CR_ALGO1_Pos)
#define HASH_CR_ALGO_Msk      (HASH_CR_ALGO0_Msk | HASH_CR_ALGO1_Msk)

#define HASH_CR_LKEY_Pos      16
#define HASH_CR_LKEY_Msk      (1UL << HASH_CR_LKEY_Pos)

/* Data Input Register */
#define HASH_DIN_REG          0x04

/* Start Register */
#define HASH_STR_REG          0x08

#define HASH_STR_NBLW_Pos     0
#define HASH_STR_NBLW_Msk     (0x1FUL << HASH_STR_NBLW_Pos)

#define HASH_STR_DCAL_Pos     8
#define HASH_STR_DCAL_Msk     (1UL << HASH_STR_DCAL_Pos)

/* Status Register */
#define HASH_SR_REG           0x24

#define HASH_SR_BUSY_Pos      3
#define HASH_SR_BUSY_Msk      (1UL << HASH_SR_BUSY_Pos)

/* Digest Result Registers (canonical F43xxx mapping at 0x310..0x32C). */
#define HASH_HR_BASE_REG      0x310

/* Algorithm encoding (CR ALGO field value, before splitting). */
#define HASH_ALGO_SHA1        0
#define HASH_ALGO_MD5         1
#define HASH_ALGO_SHA224      2
#define HASH_ALGO_SHA256      3

#define HASH_MODE_HASH        0
#define HASH_MODE_HMAC        1

/* Per-driver HMAC state surviving Start→Finalize. Singleton drivers so static. */
static whal_Stm32f4_HmacSha1_State   g_hmacSha1State;
static whal_Stm32f4_HmacSha224_State g_hmacSha224State;
static whal_Stm32f4_HmacSha256_State g_hmacSha256State;

/* Single-instance HASH peripheral — shared 0-3 byte staging buffer is enough
 * since only one streaming session can be live at a time. Process tops it up
 * to a full word and pushes; Finalize drains it as the last partial word. */
static uint8_t g_streamBuf[4];
static size_t  g_streamBufBytes;

/* Encode a 2-bit ALGO value into its split CR positions. */
static uint32_t AlgoBits(size_t algo)
{
    return whal_SetBits(HASH_CR_ALGO0_Msk, HASH_CR_ALGO0_Pos, algo & 1) |
           whal_SetBits(HASH_CR_ALGO1_Msk, HASH_CR_ALGO1_Pos, (algo >> 1) & 1);
}

static whal_Error WaitForReady(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, HASH_SR_REG, HASH_SR_BUSY_Msk, 0, timeout);
}

/* Reset the stream staging buffer; called from each Start. */
static void StreamReset(void)
{
    g_streamBufBytes = 0;
}

/* Accumulate Process input — same algorithm as the STM32WBA HASH driver. */
static void StreamWrite(size_t base, const uint8_t *in, size_t inSz)
{
    if (g_streamBufBytes > 0) {
        while (g_streamBufBytes < 4 && inSz > 0) {
            g_streamBuf[g_streamBufBytes++] = *in++;
            inSz--;
        }
        if (g_streamBufBytes == 4) {
            whal_Reg_Write(base, HASH_DIN_REG, whal_LoadBe32(g_streamBuf));
            g_streamBufBytes = 0;
        }
    }

    while (inSz >= 4) {
        whal_Reg_Write(base, HASH_DIN_REG, whal_LoadBe32(in));
        in += 4;
        inSz -= 4;
    }

    while (inSz > 0) {
        g_streamBuf[g_streamBufBytes++] = *in++;
        inSz--;
    }
}

/* Push inSz bytes immediately before issuing DCAL: full words first, then
 * if any trailing bytes remain, set NBLW and push one partial word. */
static void WriteTail(size_t base, const uint8_t *in, size_t inSz)
{
    while (inSz >= 4) {
        whal_Reg_Write(base, HASH_DIN_REG, whal_LoadBe32(in));
        in += 4;
        inSz -= 4;
    }

    if (inSz > 0) {
        whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk,
                        whal_SetBits(HASH_STR_NBLW_Msk, HASH_STR_NBLW_Pos,
                                     inSz * 8));
        whal_Reg_Write(base, HASH_DIN_REG, whal_LoadBe32Partial(in, inSz));
    }
}

/* Drain any buffered 0-3 trailing bytes as the final partial word right
 * before DCAL; sets NBLW correctly via WriteTail. */
static void StreamDrain(size_t base)
{
    WriteTail(base, g_streamBuf, g_streamBufBytes);
    g_streamBufBytes = 0;
}

static void ReadDigest(size_t base, uint8_t *digest, size_t digestSz)
{
    size_t words = digestSz / 4;
    size_t i;

    for (i = 0; i < words; i++)
        whal_StoreBe32(digest + i * 4,
                       whal_Reg_Read(base, HASH_HR_BASE_REG + i * 4));
}


/* ---- Direct API mapping (in-TU shadow defines) ---- */

#ifdef WHAL_CFG_STM32F4_HASH_INIT_DIRECT_API_MAPPING
#define whal_Stm32f4_Hash_Init           whal_Crypto_Init
#define whal_Stm32f4_Hash_Deinit         whal_Crypto_Deinit
#endif

#ifdef WHAL_CFG_STM32F4_HASH_SHA1_DIRECT_API_MAPPING
#define whal_Stm32f4_Sha1_Oneshot        whal_Sha1_Oneshot
#define whal_Stm32f4_Sha1_Start          whal_Sha1_Start
#define whal_Stm32f4_Sha1_Process        whal_Sha1_Process
#define whal_Stm32f4_Sha1_Finalize       whal_Sha1_Finalize
#endif

#ifdef WHAL_CFG_STM32F4_HASH_SHA224_DIRECT_API_MAPPING
#define whal_Stm32f4_Sha224_Oneshot      whal_Sha224_Oneshot
#define whal_Stm32f4_Sha224_Start        whal_Sha224_Start
#define whal_Stm32f4_Sha224_Process      whal_Sha224_Process
#define whal_Stm32f4_Sha224_Finalize     whal_Sha224_Finalize
#endif

#ifdef WHAL_CFG_STM32F4_HASH_SHA256_DIRECT_API_MAPPING
#define whal_Stm32f4_Sha256_Oneshot      whal_Sha256_Oneshot
#define whal_Stm32f4_Sha256_Start        whal_Sha256_Start
#define whal_Stm32f4_Sha256_Process      whal_Sha256_Process
#define whal_Stm32f4_Sha256_Finalize     whal_Sha256_Finalize
#endif

#ifdef WHAL_CFG_STM32F4_HASH_HMAC_SHA1_DIRECT_API_MAPPING
#define whal_Stm32f4_HmacSha1_Oneshot    whal_HmacSha1_Oneshot
#define whal_Stm32f4_HmacSha1_Start      whal_HmacSha1_Start
#define whal_Stm32f4_HmacSha1_Process    whal_HmacSha1_Process
#define whal_Stm32f4_HmacSha1_Finalize   whal_HmacSha1_Finalize
#endif

#ifdef WHAL_CFG_STM32F4_HASH_HMAC_SHA224_DIRECT_API_MAPPING
#define whal_Stm32f4_HmacSha224_Oneshot  whal_HmacSha224_Oneshot
#define whal_Stm32f4_HmacSha224_Start    whal_HmacSha224_Start
#define whal_Stm32f4_HmacSha224_Process  whal_HmacSha224_Process
#define whal_Stm32f4_HmacSha224_Finalize whal_HmacSha224_Finalize
#endif

#ifdef WHAL_CFG_STM32F4_HASH_HMAC_SHA256_DIRECT_API_MAPPING
#define whal_Stm32f4_HmacSha256_Oneshot  whal_HmacSha256_Oneshot
#define whal_Stm32f4_HmacSha256_Start    whal_HmacSha256_Start
#define whal_Stm32f4_HmacSha256_Process  whal_HmacSha256_Process
#define whal_Stm32f4_HmacSha256_Finalize whal_HmacSha256_Finalize
#endif


/* ---- Init / Deinit ---- */

whal_Error whal_Stm32f4_Hash_Init(whal_Crypto *dev)
{
    (void)dev;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Hash_Deinit(whal_Crypto *dev)
{
    (void)dev;
    return WHAL_SUCCESS;
}

const whal_CryptoDriver whal_Stm32f4_Hash_CryptoDriver = {
    .Init = whal_Stm32f4_Hash_Init,
    .Deinit = whal_Stm32f4_Hash_Deinit,
};


/* ---- SHA-1 ---- */

whal_Error whal_Stm32f4_Sha1_Oneshot(whal_Sha1 *dev,
                                     const void *in, size_t inSz,
                                     void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 20)
        return WHAL_EINVAL;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA1) | HASH_CR_INIT_Msk);

    if (inSz > 0) {
        if (!in)
            return WHAL_EINVAL;
        WriteTail(base, (const uint8_t *)in, inSz);
    }

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha1_Start(whal_Sha1 *dev)
{
    size_t base = whal_Stm32f4_Hash_Dev.base;
    (void)dev;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA1) | HASH_CR_INIT_Msk);

    StreamReset();
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha1_Process(whal_Sha1 *dev,
                                     const void *in, size_t inSz)
{
    (void)dev;
    if (inSz == 0)
        return WHAL_SUCCESS;
    if (!in)
        return WHAL_EINVAL;
    StreamWrite(whal_Stm32f4_Hash_Dev.base, (const uint8_t *)in, inSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha1_Finalize(whal_Sha1 *dev,
                                      void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 20)
        return WHAL_EINVAL;

    StreamDrain(base);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

const whal_Sha1Driver whal_Stm32f4_Hash_Sha1Driver = {
    .Oneshot = whal_Stm32f4_Sha1_Oneshot,
    .Start = whal_Stm32f4_Sha1_Start,
    .Process = whal_Stm32f4_Sha1_Process,
    .Finalize = whal_Stm32f4_Sha1_Finalize,
};


/* ---- SHA-224 ---- */

whal_Error whal_Stm32f4_Sha224_Oneshot(whal_Sha224 *dev,
                                       const void *in, size_t inSz,
                                       void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 28)
        return WHAL_EINVAL;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA224) | HASH_CR_INIT_Msk);

    if (inSz > 0) {
        if (!in)
            return WHAL_EINVAL;
        WriteTail(base, (const uint8_t *)in, inSz);
    }

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha224_Start(whal_Sha224 *dev)
{
    size_t base = whal_Stm32f4_Hash_Dev.base;
    (void)dev;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA224) | HASH_CR_INIT_Msk);

    StreamReset();
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha224_Process(whal_Sha224 *dev,
                                       const void *in, size_t inSz)
{
    (void)dev;
    if (inSz == 0)
        return WHAL_SUCCESS;
    if (!in)
        return WHAL_EINVAL;
    StreamWrite(whal_Stm32f4_Hash_Dev.base, (const uint8_t *)in, inSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha224_Finalize(whal_Sha224 *dev,
                                        void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 28)
        return WHAL_EINVAL;

    StreamDrain(base);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

const whal_Sha224Driver whal_Stm32f4_Hash_Sha224Driver = {
    .Oneshot = whal_Stm32f4_Sha224_Oneshot,
    .Start = whal_Stm32f4_Sha224_Start,
    .Process = whal_Stm32f4_Sha224_Process,
    .Finalize = whal_Stm32f4_Sha224_Finalize,
};


/* ---- SHA-256 ---- */

whal_Error whal_Stm32f4_Sha256_Oneshot(whal_Sha256 *dev,
                                       const void *in, size_t inSz,
                                       void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 32)
        return WHAL_EINVAL;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA256) | HASH_CR_INIT_Msk);

    if (inSz > 0) {
        if (!in)
            return WHAL_EINVAL;
        WriteTail(base, (const uint8_t *)in, inSz);
    }

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha256_Start(whal_Sha256 *dev)
{
    size_t base = whal_Stm32f4_Hash_Dev.base;
    (void)dev;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA256) | HASH_CR_INIT_Msk);

    StreamReset();
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha256_Process(whal_Sha256 *dev,
                                       const void *in, size_t inSz)
{
    (void)dev;
    if (inSz == 0)
        return WHAL_SUCCESS;
    if (!in)
        return WHAL_EINVAL;
    StreamWrite(whal_Stm32f4_Hash_Dev.base, (const uint8_t *)in, inSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_Sha256_Finalize(whal_Sha256 *dev,
                                        void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 32)
        return WHAL_EINVAL;

    StreamDrain(base);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

const whal_Sha256Driver whal_Stm32f4_Hash_Sha256Driver = {
    .Oneshot = whal_Stm32f4_Sha256_Oneshot,
    .Start = whal_Stm32f4_Sha256_Start,
    .Process = whal_Stm32f4_Sha256_Process,
    .Finalize = whal_Stm32f4_Sha256_Finalize,
};


/* ---- HMAC-SHA-1 ---- */

whal_Error whal_Stm32f4_HmacSha1_Oneshot(whal_HmacSha1 *dev,
                                         const void *key, size_t keySz,
                                         const void *in, size_t inSz,
                                         void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    size_t lkey;
    whal_Error err;
    (void)dev;

    if (!key || !digest || digestSz != 20)
        return WHAL_EINVAL;

    lkey = (keySz > 64) ? 1 : 0;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA1) |
                    whal_SetBits(HASH_CR_MODE_Msk, HASH_CR_MODE_Pos,
                                 HASH_MODE_HMAC) |
                    whal_SetBits(HASH_CR_LKEY_Msk, HASH_CR_LKEY_Pos, lkey) |
                    HASH_CR_INIT_Msk);

    /* Inner key */
    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    /* Message */
    if (inSz > 0) {
        if (!in)
            return WHAL_EINVAL;
        WriteTail(base, (const uint8_t *)in, inSz);
    }

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    /* Outer key */
    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);
    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha1_Start(whal_HmacSha1 *dev,
                                       const void *key, size_t keySz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    size_t lkey;
    whal_Error err;
    (void)dev;

    if (!key)
        return WHAL_EINVAL;

    lkey = (keySz > 64) ? 1 : 0;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA1) |
                    whal_SetBits(HASH_CR_MODE_Msk, HASH_CR_MODE_Pos,
                                 HASH_MODE_HMAC) |
                    whal_SetBits(HASH_CR_LKEY_Msk, HASH_CR_LKEY_Pos, lkey) |
                    HASH_CR_INIT_Msk);

    /* Inner key */
    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    g_hmacSha1State.key = key;
    g_hmacSha1State.keySz = keySz;

    StreamReset();
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha1_Process(whal_HmacSha1 *dev,
                                         const void *in, size_t inSz)
{
    (void)dev;
    if (inSz == 0)
        return WHAL_SUCCESS;
    if (!in)
        return WHAL_EINVAL;
    StreamWrite(whal_Stm32f4_Hash_Dev.base, (const uint8_t *)in, inSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha1_Finalize(whal_HmacSha1 *dev,
                                          void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 20)
        return WHAL_EINVAL;

    StreamDrain(base);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    /* Outer key */
    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);
    WriteTail(base, (const uint8_t *)g_hmacSha1State.key, g_hmacSha1State.keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

const whal_HmacSha1Driver whal_Stm32f4_Hash_HmacSha1Driver = {
    .Oneshot = whal_Stm32f4_HmacSha1_Oneshot,
    .Start = whal_Stm32f4_HmacSha1_Start,
    .Process = whal_Stm32f4_HmacSha1_Process,
    .Finalize = whal_Stm32f4_HmacSha1_Finalize,
};


/* ---- HMAC-SHA-224 ---- */

whal_Error whal_Stm32f4_HmacSha224_Oneshot(whal_HmacSha224 *dev,
                                           const void *key, size_t keySz,
                                           const void *in, size_t inSz,
                                           void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    size_t lkey;
    whal_Error err;
    (void)dev;

    if (!key || !digest || digestSz != 28)
        return WHAL_EINVAL;

    lkey = (keySz > 64) ? 1 : 0;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA224) |
                    whal_SetBits(HASH_CR_MODE_Msk, HASH_CR_MODE_Pos,
                                 HASH_MODE_HMAC) |
                    whal_SetBits(HASH_CR_LKEY_Msk, HASH_CR_LKEY_Pos, lkey) |
                    HASH_CR_INIT_Msk);

    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    if (inSz > 0) {
        if (!in)
            return WHAL_EINVAL;
        WriteTail(base, (const uint8_t *)in, inSz);
    }

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);
    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha224_Start(whal_HmacSha224 *dev,
                                         const void *key, size_t keySz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    size_t lkey;
    whal_Error err;
    (void)dev;

    if (!key)
        return WHAL_EINVAL;

    lkey = (keySz > 64) ? 1 : 0;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA224) |
                    whal_SetBits(HASH_CR_MODE_Msk, HASH_CR_MODE_Pos,
                                 HASH_MODE_HMAC) |
                    whal_SetBits(HASH_CR_LKEY_Msk, HASH_CR_LKEY_Pos, lkey) |
                    HASH_CR_INIT_Msk);

    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    g_hmacSha224State.key = key;
    g_hmacSha224State.keySz = keySz;

    StreamReset();
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha224_Process(whal_HmacSha224 *dev,
                                           const void *in, size_t inSz)
{
    (void)dev;
    if (inSz == 0)
        return WHAL_SUCCESS;
    if (!in)
        return WHAL_EINVAL;
    StreamWrite(whal_Stm32f4_Hash_Dev.base, (const uint8_t *)in, inSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha224_Finalize(whal_HmacSha224 *dev,
                                            void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 28)
        return WHAL_EINVAL;

    StreamDrain(base);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);
    WriteTail(base, (const uint8_t *)g_hmacSha224State.key, g_hmacSha224State.keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

const whal_HmacSha224Driver whal_Stm32f4_Hash_HmacSha224Driver = {
    .Oneshot = whal_Stm32f4_HmacSha224_Oneshot,
    .Start = whal_Stm32f4_HmacSha224_Start,
    .Process = whal_Stm32f4_HmacSha224_Process,
    .Finalize = whal_Stm32f4_HmacSha224_Finalize,
};


/* ---- HMAC-SHA-256 ---- */

whal_Error whal_Stm32f4_HmacSha256_Oneshot(whal_HmacSha256 *dev,
                                           const void *key, size_t keySz,
                                           const void *in, size_t inSz,
                                           void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    size_t lkey;
    whal_Error err;
    (void)dev;

    if (!key || !digest || digestSz != 32)
        return WHAL_EINVAL;

    lkey = (keySz > 64) ? 1 : 0;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA256) |
                    whal_SetBits(HASH_CR_MODE_Msk, HASH_CR_MODE_Pos,
                                 HASH_MODE_HMAC) |
                    whal_SetBits(HASH_CR_LKEY_Msk, HASH_CR_LKEY_Pos, lkey) |
                    HASH_CR_INIT_Msk);

    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    if (inSz > 0) {
        if (!in)
            return WHAL_EINVAL;
        WriteTail(base, (const uint8_t *)in, inSz);
    }

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);
    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha256_Start(whal_HmacSha256 *dev,
                                         const void *key, size_t keySz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    size_t lkey;
    whal_Error err;
    (void)dev;

    if (!key)
        return WHAL_EINVAL;

    lkey = (keySz > 64) ? 1 : 0;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);

    whal_Reg_Update(base, HASH_CR_REG,
                    HASH_CR_ALGO_Msk | HASH_CR_DATATYPE_Msk |
                    HASH_CR_MODE_Msk | HASH_CR_LKEY_Msk | HASH_CR_INIT_Msk,
                    AlgoBits(HASH_ALGO_SHA256) |
                    whal_SetBits(HASH_CR_MODE_Msk, HASH_CR_MODE_Pos,
                                 HASH_MODE_HMAC) |
                    whal_SetBits(HASH_CR_LKEY_Msk, HASH_CR_LKEY_Pos, lkey) |
                    HASH_CR_INIT_Msk);

    WriteTail(base, (const uint8_t *)key, keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    g_hmacSha256State.key = key;
    g_hmacSha256State.keySz = keySz;

    StreamReset();
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha256_Process(whal_HmacSha256 *dev,
                                           const void *in, size_t inSz)
{
    (void)dev;
    if (inSz == 0)
        return WHAL_SUCCESS;
    if (!in)
        return WHAL_EINVAL;
    StreamWrite(whal_Stm32f4_Hash_Dev.base, (const uint8_t *)in, inSz);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_HmacSha256_Finalize(whal_HmacSha256 *dev,
                                            void *digest, size_t digestSz)
{
    const whal_Stm32f4_Hash_Cfg *cfg =
        (const whal_Stm32f4_Hash_Cfg *)whal_Stm32f4_Hash_Dev.cfg;
    size_t base = whal_Stm32f4_Hash_Dev.base;
    whal_Error err;
    (void)dev;

    if (!digest || digestSz != 32)
        return WHAL_EINVAL;

    StreamDrain(base);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_NBLW_Msk, 0);
    WriteTail(base, (const uint8_t *)g_hmacSha256State.key, g_hmacSha256State.keySz);

    whal_Reg_Update(base, HASH_STR_REG, HASH_STR_DCAL_Msk,
                    HASH_STR_DCAL_Msk);

    err = WaitForReady(base, cfg->timeout);
    if (err)
        return err;

    ReadDigest(base, (uint8_t *)digest, digestSz);
    return WHAL_SUCCESS;
}

const whal_HmacSha256Driver whal_Stm32f4_Hash_HmacSha256Driver = {
    .Oneshot = whal_Stm32f4_HmacSha256_Oneshot,
    .Start = whal_Stm32f4_HmacSha256_Start,
    .Process = whal_Stm32f4_HmacSha256_Process,
    .Finalize = whal_Stm32f4_HmacSha256_Finalize,
};
