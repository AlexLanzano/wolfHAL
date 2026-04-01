#ifndef WHAL_STM32H7_AES_H
#define WHAL_STM32H7_AES_H

/*
 * @file stm32h7_aes.h
 * @brief STM32H7 CRYP hardware accelerator driver.
 *
 * The STM32H753 has a CRYP peripheral (not the simpler AES peripheral found
 * on STM32WB). The CRYP uses FIFO-based data input/output with CRYP_DIN and
 * CRYP_DOUT registers, and has a different CR layout with ALGOMODE, ALGODIR,
 * CRYPEN, FFLUSH, KEYSIZE, DATATYPE, and GCM_CCMPH fields.
 *
 * Register offsets (RM0433 Section 35.7):
 *   CRYP_CR:    0x00
 *   CRYP_SR:    0x04
 *   CRYP_DIN:   0x08
 *   CRYP_DOUT:  0x0C
 *   CRYP_K0LR:  0x20  (key bits 255:224)
 *   CRYP_K0RR:  0x24  (key bits 223:192)
 *   CRYP_K1LR:  0x28  (key bits 191:160)
 *   CRYP_K1RR:  0x2C  (key bits 159:128)
 *   CRYP_K2LR:  0x30  (key bits 127:96)
 *   CRYP_K2RR:  0x34  (key bits 95:64)
 *   CRYP_K3LR:  0x38  (key bits 63:32)
 *   CRYP_K3RR:  0x3C  (key bits 31:0)
 *   CRYP_IV0LR: 0x40  (IV bits 127:96)
 *   CRYP_IV0RR: 0x44  (IV bits 95:64)
 *   CRYP_IV1LR: 0x48  (IV bits 63:32)
 *   CRYP_IV1RR: 0x4C  (IV bits 31:0)
 */

#include <stdint.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/timeout.h>

/*
 * @brief AES device configuration.
 */
typedef struct {
    whal_Timeout *timeout;
} whal_Stm32h7Aes_Cfg;

/*
 * @brief Driver instance for STM32H7 CRYP peripheral.
 */
extern const whal_CryptoDriver whal_Stm32h7Aes_Driver;

whal_Error whal_Stm32h7Aes_Init(whal_Crypto *cryptoDev);
whal_Error whal_Stm32h7Aes_Deinit(whal_Crypto *cryptoDev);
whal_Error whal_Stm32h7Aes_AesEcb(whal_Crypto *cryptoDev, void *opArgs);
whal_Error whal_Stm32h7Aes_AesCbc(whal_Crypto *cryptoDev, void *opArgs);
whal_Error whal_Stm32h7Aes_AesCtr(whal_Crypto *cryptoDev, void *opArgs);
whal_Error whal_Stm32h7Aes_AesGcm(whal_Crypto *cryptoDev, void *opArgs);
whal_Error whal_Stm32h7Aes_AesGmac(whal_Crypto *cryptoDev, void *opArgs);
whal_Error whal_Stm32h7Aes_AesCcm(whal_Crypto *cryptoDev, void *opArgs);

#endif /* WHAL_STM32H7_AES_H */
