/* stm32f4_hash.h
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

#ifndef WHAL_STM32F4_HASH_H
#define WHAL_STM32F4_HASH_H

#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/timeout.h>

/**
 * @file stm32f4_hash.h
 * @brief STM32F4 HASH hardware accelerator driver.
 *
 * The STM32F42xxx / F43xxx HASH peripheral supports SHA-1, SHA-224, SHA-256,
 * and MD5 in both plain HASH and HMAC modes. The CR register layout is very
 * close to the STM32WBA HASH, but ALGO is split across CR[18] and CR[7]
 * rather than the contiguous CR[18:17] used on WBA — so this is a from-
 * scratch driver rather than an alias.
 *
 * RM0090 §25.4 (HASH register map, STM32F43xxx variant) is the authority for
 * bit field positions; cross-checked against the textual descriptions in
 * §25.4.2 since the register-map figure and prose disagree on some bit
 * placements (the prose is authoritative).
 */

/**
 * @brief HASH device configuration.
 */
typedef struct {
    whal_Timeout *timeout;
} whal_Stm32f4_Hash_Cfg;

/* ---- Streaming state ---- */

/**
 * @brief HMAC streaming state (key pointer retained for outer-key phase).
 */
typedef struct {
    const void *key;
    size_t keySz;
} whal_Stm32f4_HmacSha1_State;

typedef whal_Stm32f4_HmacSha1_State whal_Stm32f4_HmacSha224_State;
typedef whal_Stm32f4_HmacSha1_State whal_Stm32f4_HmacSha256_State;

/* ---- Direct API mapping ---- */

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

/**
 * @brief Initialize the STM32F4 HASH peripheral.
 */
whal_Error whal_Stm32f4_Hash_Init(whal_Crypto *dev);

/**
 * @brief Deinitialize the STM32F4 HASH peripheral.
 */
whal_Error whal_Stm32f4_Hash_Deinit(whal_Crypto *dev);

/* ---- SHA-1 ---- */

whal_Error whal_Stm32f4_Sha1_Oneshot(whal_Sha1 *dev,
                                     const void *in, size_t inSz,
                                     void *digest, size_t digestSz);
whal_Error whal_Stm32f4_Sha1_Start(whal_Sha1 *dev);
whal_Error whal_Stm32f4_Sha1_Process(whal_Sha1 *dev,
                                     const void *in, size_t inSz);
whal_Error whal_Stm32f4_Sha1_Finalize(whal_Sha1 *dev,
                                      void *digest, size_t digestSz);

/* ---- SHA-224 ---- */

whal_Error whal_Stm32f4_Sha224_Oneshot(whal_Sha224 *dev,
                                       const void *in, size_t inSz,
                                       void *digest, size_t digestSz);
whal_Error whal_Stm32f4_Sha224_Start(whal_Sha224 *dev);
whal_Error whal_Stm32f4_Sha224_Process(whal_Sha224 *dev,
                                       const void *in, size_t inSz);
whal_Error whal_Stm32f4_Sha224_Finalize(whal_Sha224 *dev,
                                        void *digest, size_t digestSz);

/* ---- SHA-256 ---- */

whal_Error whal_Stm32f4_Sha256_Oneshot(whal_Sha256 *dev,
                                       const void *in, size_t inSz,
                                       void *digest, size_t digestSz);
whal_Error whal_Stm32f4_Sha256_Start(whal_Sha256 *dev);
whal_Error whal_Stm32f4_Sha256_Process(whal_Sha256 *dev,
                                       const void *in, size_t inSz);
whal_Error whal_Stm32f4_Sha256_Finalize(whal_Sha256 *dev,
                                        void *digest, size_t digestSz);

/* ---- HMAC-SHA-1 ---- */

whal_Error whal_Stm32f4_HmacSha1_Oneshot(whal_HmacSha1 *dev,
                                         const void *key, size_t keySz,
                                         const void *in, size_t inSz,
                                         void *digest, size_t digestSz);
whal_Error whal_Stm32f4_HmacSha1_Start(whal_HmacSha1 *dev,
                                       const void *key, size_t keySz);
whal_Error whal_Stm32f4_HmacSha1_Process(whal_HmacSha1 *dev,
                                         const void *in, size_t inSz);
whal_Error whal_Stm32f4_HmacSha1_Finalize(whal_HmacSha1 *dev,
                                          void *digest, size_t digestSz);

/* ---- HMAC-SHA-224 ---- */

whal_Error whal_Stm32f4_HmacSha224_Oneshot(whal_HmacSha224 *dev,
                                           const void *key, size_t keySz,
                                           const void *in, size_t inSz,
                                           void *digest, size_t digestSz);
whal_Error whal_Stm32f4_HmacSha224_Start(whal_HmacSha224 *dev,
                                         const void *key, size_t keySz);
whal_Error whal_Stm32f4_HmacSha224_Process(whal_HmacSha224 *dev,
                                           const void *in, size_t inSz);
whal_Error whal_Stm32f4_HmacSha224_Finalize(whal_HmacSha224 *dev,
                                            void *digest, size_t digestSz);

/* ---- HMAC-SHA-256 ---- */

whal_Error whal_Stm32f4_HmacSha256_Oneshot(whal_HmacSha256 *dev,
                                           const void *key, size_t keySz,
                                           const void *in, size_t inSz,
                                           void *digest, size_t digestSz);
whal_Error whal_Stm32f4_HmacSha256_Start(whal_HmacSha256 *dev,
                                         const void *key, size_t keySz);
whal_Error whal_Stm32f4_HmacSha256_Process(whal_HmacSha256 *dev,
                                           const void *in, size_t inSz);
whal_Error whal_Stm32f4_HmacSha256_Finalize(whal_HmacSha256 *dev,
                                            void *digest, size_t digestSz);

/* ---- Singleton externs ---- */

/**
 * @brief Platform-owned HASH + algorithm singletons. Defined in the driver
 *        TU from the WHAL_CFG_STM32F4_HASH*_DEV initializers in board.h.
 */
extern const whal_Crypto     whal_Stm32f4_Hash_Dev;
extern const whal_Sha1       whal_Stm32f4_Sha1_Dev;
extern const whal_Sha224     whal_Stm32f4_Sha224_Dev;
extern const whal_Sha256     whal_Stm32f4_Sha256_Dev;
extern const whal_HmacSha1   whal_Stm32f4_HmacSha1_Dev;
extern const whal_HmacSha224 whal_Stm32f4_HmacSha224_Dev;
extern const whal_HmacSha256 whal_Stm32f4_HmacSha256_Dev;

/* ---- Vtable externs ---- */

#ifndef WHAL_CFG_STM32F4_HASH_INIT_DIRECT_API_MAPPING
extern const whal_CryptoDriver whal_Stm32f4_Hash_CryptoDriver;
#endif
#ifndef WHAL_CFG_STM32F4_HASH_SHA1_DIRECT_API_MAPPING
extern const whal_Sha1Driver whal_Stm32f4_Hash_Sha1Driver;
#endif
#ifndef WHAL_CFG_STM32F4_HASH_SHA224_DIRECT_API_MAPPING
extern const whal_Sha224Driver whal_Stm32f4_Hash_Sha224Driver;
#endif
#ifndef WHAL_CFG_STM32F4_HASH_SHA256_DIRECT_API_MAPPING
extern const whal_Sha256Driver whal_Stm32f4_Hash_Sha256Driver;
#endif
#ifndef WHAL_CFG_STM32F4_HASH_HMAC_SHA1_DIRECT_API_MAPPING
extern const whal_HmacSha1Driver whal_Stm32f4_Hash_HmacSha1Driver;
#endif
#ifndef WHAL_CFG_STM32F4_HASH_HMAC_SHA224_DIRECT_API_MAPPING
extern const whal_HmacSha224Driver whal_Stm32f4_Hash_HmacSha224Driver;
#endif
#ifndef WHAL_CFG_STM32F4_HASH_HMAC_SHA256_DIRECT_API_MAPPING
extern const whal_HmacSha256Driver whal_Stm32f4_Hash_HmacSha256Driver;
#endif

#endif /* WHAL_STM32F4_HASH_H */
