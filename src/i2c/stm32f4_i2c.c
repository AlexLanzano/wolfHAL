/* stm32f4_i2c.c
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
#ifdef WHAL_CFG_STM32F4_I2C_SINGLE_INSTANCE
#include "board.h"  /* provides whal_Stm32f4_I2c_Dev singleton */
#endif
#include <wolfHAL/i2c/stm32f4_i2c.h>
#include <wolfHAL/i2c/i2c.h>
#include <wolfHAL/error.h>
#include <wolfHAL/reg.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/*
 * STM32F4 I2C V1 register layout. Source: RM0090 section 27.6.
 */

#define I2C_CR1_REG  0x00
#define I2C_CR1_PE_Pos    0  /**< Peripheral enable. */
#define I2C_CR1_PE_Msk    (1UL << I2C_CR1_PE_Pos)
#define I2C_CR1_START_Pos 8  /**< Generate START. */
#define I2C_CR1_START_Msk (1UL << I2C_CR1_START_Pos)
#define I2C_CR1_STOP_Pos  9  /**< Generate STOP. */
#define I2C_CR1_STOP_Msk  (1UL << I2C_CR1_STOP_Pos)
#define I2C_CR1_ACK_Pos   10 /**< Acknowledge enable. */
#define I2C_CR1_ACK_Msk   (1UL << I2C_CR1_ACK_Pos)
#define I2C_CR1_POS_Pos   11 /**< ACK/POS position (for 2-byte read). */
#define I2C_CR1_POS_Msk   (1UL << I2C_CR1_POS_Pos)
#define I2C_CR1_SWRST_Pos 15 /**< Software reset. */
#define I2C_CR1_SWRST_Msk (1UL << I2C_CR1_SWRST_Pos)

#define I2C_CR2_REG  0x04
#define I2C_CR2_FREQ_Pos 0   /**< APB1 clock frequency in MHz. */
#define I2C_CR2_FREQ_Msk (0x3FUL << I2C_CR2_FREQ_Pos)

#define I2C_DR_REG   0x10

#define I2C_SR1_REG  0x14
#define I2C_SR1_SB_Pos     0
#define I2C_SR1_SB_Msk     (1UL << I2C_SR1_SB_Pos)
#define I2C_SR1_ADDR_Pos   1
#define I2C_SR1_ADDR_Msk   (1UL << I2C_SR1_ADDR_Pos)
#define I2C_SR1_BTF_Pos    2
#define I2C_SR1_BTF_Msk    (1UL << I2C_SR1_BTF_Pos)
#define I2C_SR1_STOPF_Pos  4
#define I2C_SR1_STOPF_Msk  (1UL << I2C_SR1_STOPF_Pos)
#define I2C_SR1_RXNE_Pos   6
#define I2C_SR1_RXNE_Msk   (1UL << I2C_SR1_RXNE_Pos)
#define I2C_SR1_TXE_Pos    7
#define I2C_SR1_TXE_Msk    (1UL << I2C_SR1_TXE_Pos)
#define I2C_SR1_BERR_Pos   8
#define I2C_SR1_BERR_Msk   (1UL << I2C_SR1_BERR_Pos)
#define I2C_SR1_ARLO_Pos   9
#define I2C_SR1_ARLO_Msk   (1UL << I2C_SR1_ARLO_Pos)
#define I2C_SR1_AF_Pos     10
#define I2C_SR1_AF_Msk     (1UL << I2C_SR1_AF_Pos)
#define I2C_SR1_ERR_Msk    (I2C_SR1_BERR_Msk | I2C_SR1_ARLO_Msk | I2C_SR1_AF_Msk)

#define I2C_SR2_REG  0x18
#define I2C_SR2_BUSY_Pos   1
#define I2C_SR2_BUSY_Msk   (1UL << I2C_SR2_BUSY_Pos)

#define I2C_CCR_REG  0x1C
#define I2C_CCR_CCR_Pos    0
#define I2C_CCR_CCR_Msk    (0xFFFUL << I2C_CCR_CCR_Pos)
#define I2C_CCR_DUTY_Pos   14
#define I2C_CCR_DUTY_Msk   (1UL << I2C_CCR_DUTY_Pos)
#define I2C_CCR_FS_Pos     15
#define I2C_CCR_FS_Msk     (1UL << I2C_CCR_FS_Pos)

#define I2C_TRISE_REG     0x20
#define I2C_TRISE_Pos     0
#define I2C_TRISE_Msk     (0x3FUL << I2C_TRISE_Pos)

#define STM32F4_I2C_SM_MAX_HZ 100000UL  /**< Sm mode upper bound. */

#ifdef WHAL_CFG_STM32F4_I2C_DIRECT_API_MAPPING
#define whal_Stm32f4_I2c_Init     whal_I2c_Init
#define whal_Stm32f4_I2c_Deinit   whal_I2c_Deinit
#define whal_Stm32f4_I2c_StartCom whal_I2c_StartCom
#define whal_Stm32f4_I2c_EndCom   whal_I2c_EndCom
#define whal_Stm32f4_I2c_Transfer whal_I2c_Transfer
#endif

#ifdef WHAL_CFG_STM32F4_I2C_SINGLE_INSTANCE
const whal_I2c whal_Stm32f4_I2c_Dev = WHAL_CFG_STM32F4_I2C_DEV;
#endif

/* I2Cv1 has no register that holds the target address across START — the
 * address byte is written to DR after each START. Transfer caches the
 * StartCom value here. Tests run one bus session at a time, so a single
 * file-scoped slot is sufficient for now. */
static uint16_t s_lastAddr;

/**
 * @brief Resolve the I2C base + cfg given the API arg.
 *
 * In single-instance builds we read from the driver-owned singleton; in
 * pointer-based builds we read from the device passed in.
 */
static inline void stm32f4_i2c_dev(whal_I2c *i2cDev, size_t *base,
                                   const whal_Stm32f4_I2c_Cfg **cfg)
{
#ifdef WHAL_CFG_STM32F4_I2C_SINGLE_INSTANCE
    (void)i2cDev;
    *base = whal_Stm32f4_I2c_Dev.base;
    *cfg  = (const whal_Stm32f4_I2c_Cfg *)whal_Stm32f4_I2c_Dev.cfg;
#else
    *base = i2cDev->base;
    *cfg  = (const whal_Stm32f4_I2c_Cfg *)i2cDev->cfg;
#endif
}

/**
 * @brief Poll a status-register bit-mask with a hardware-error escape.
 *
 * Returns WHAL_SUCCESS once any bit in @p mask is set in SR1.
 * Returns WHAL_EHARDWARE if BERR/ARLO/AF is observed (errors are
 * cleared before returning so the caller can recover).
 */
static whal_Error stm32f4_i2c_wait_sr1(size_t base, uint32_t mask,
                                       whal_Timeout *to)
{
    uint32_t sr1;
    WHAL_TIMEOUT_START(to);
    for (;;) {
        sr1 = whal_Reg_Read(base, I2C_SR1_REG);
        if (sr1 & mask)
            return WHAL_SUCCESS;
        if (sr1 & I2C_SR1_ERR_Msk) {
            whal_Reg_Update(base, I2C_SR1_REG, I2C_SR1_ERR_Msk, 0);
            return WHAL_EHARDWARE;
        }
        if (WHAL_TIMEOUT_EXPIRED(to))
            return WHAL_ETIMEOUT;
    }
}

/**
 * @brief Send START + 7-bit address with the R/W bit set per @p read.
 *
 * Caller must have ACK already configured correctly for the byte count
 * (see read paths in stm32f4_i2c_read for the 1-byte ACK-off rule).
 */
static whal_Error stm32f4_i2c_start_addr(size_t base, uint16_t addr,
                                         int read, whal_Timeout *to)
{
    whal_Error err;
    uint8_t addrByte = (uint8_t)((addr << 1) | (read ? 1 : 0));

    whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_START_Msk, I2C_CR1_START_Msk);
    err = stm32f4_i2c_wait_sr1(base, I2C_SR1_SB_Msk, to);
    if (err)
        return err;

    whal_Reg_Write(base, I2C_DR_REG, addrByte);
    return WHAL_SUCCESS;
}

/**
 * @brief Wait for ADDR=1, then clear it via the SR1→SR2 read sequence.
 */
static whal_Error stm32f4_i2c_clear_addr(size_t base, whal_Timeout *to)
{
    whal_Error err = stm32f4_i2c_wait_sr1(base, I2C_SR1_ADDR_Msk, to);
    if (err)
        return err;
    (void)whal_Reg_Read(base, I2C_SR1_REG);
    (void)whal_Reg_Read(base, I2C_SR2_REG);
    return WHAL_SUCCESS;
}

static whal_Error stm32f4_i2c_write(size_t base, const uint8_t *data,
                                    size_t dataSz, whal_Timeout *to)
{
    whal_Error err;
    for (size_t i = 0; i < dataSz; ++i) {
        err = stm32f4_i2c_wait_sr1(base, I2C_SR1_TXE_Msk, to);
        if (err)
            return err;
        whal_Reg_Write(base, I2C_DR_REG, data[i]);
    }
    /* Drain TX: wait BTF so the shift register has flushed before STOP. */
    return stm32f4_i2c_wait_sr1(base, I2C_SR1_BTF_Msk, to);
}

/**
 * @brief Controller receive — handles the N=1, N=2, N>2 protocol cases
 * required by I2Cv1 to issue NACK before the final byte and STOP at
 * the right cycle (see RM0090 §27.3.3 "Closing the communication").
 */
static whal_Error stm32f4_i2c_read(size_t base, uint8_t *data,
                                   size_t dataSz, int stop,
                                   whal_Timeout *to)
{
    whal_Error err;

    if (dataSz == 0)
        return WHAL_SUCCESS;

    if (dataSz == 1) {
        /* N=1: ACK off before clearing ADDR; STOP after ADDR cleared. */
        whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_ACK_Msk, 0);
        err = stm32f4_i2c_clear_addr(base, to);
        if (err)
            return err;
        if (stop)
            whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_STOP_Msk,
                            I2C_CR1_STOP_Msk);
        err = stm32f4_i2c_wait_sr1(base, I2C_SR1_RXNE_Msk, to);
        if (err)
            return err;
        data[0] = (uint8_t)whal_Reg_Read(base, I2C_DR_REG);
        return WHAL_SUCCESS;
    }

    if (dataSz == 2) {
        /* N=2: POS=1, ACK=0, clear ADDR, wait BTF, STOP, read both. */
        whal_Reg_Update(base, I2C_CR1_REG,
                        I2C_CR1_ACK_Msk | I2C_CR1_POS_Msk, I2C_CR1_POS_Msk);
        err = stm32f4_i2c_clear_addr(base, to);
        if (err)
            return err;
        err = stm32f4_i2c_wait_sr1(base, I2C_SR1_BTF_Msk, to);
        if (err)
            return err;
        if (stop)
            whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_STOP_Msk,
                            I2C_CR1_STOP_Msk);
        data[0] = (uint8_t)whal_Reg_Read(base, I2C_DR_REG);
        data[1] = (uint8_t)whal_Reg_Read(base, I2C_DR_REG);
        /* Restore POS for subsequent transactions. */
        whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_POS_Msk, 0);
        return WHAL_SUCCESS;
    }

    /* N > 2 */
    whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_ACK_Msk, I2C_CR1_ACK_Msk);
    err = stm32f4_i2c_clear_addr(base, to);
    if (err)
        return err;

    /* Read bytes 0..N-3 with ACK enabled. */
    for (size_t i = 0; i + 3 < dataSz; ++i) {
        err = stm32f4_i2c_wait_sr1(base, I2C_SR1_RXNE_Msk, to);
        if (err)
            return err;
        data[i] = (uint8_t)whal_Reg_Read(base, I2C_DR_REG);
    }

    /* At N-3 in DR, N-2 in shift: clear ACK, read N-3, wait BTF (N-2 in
     * DR, N-1 in shift), STOP, then read both. */
    err = stm32f4_i2c_wait_sr1(base, I2C_SR1_BTF_Msk, to);
    if (err)
        return err;
    whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_ACK_Msk, 0);
    data[dataSz - 3] = (uint8_t)whal_Reg_Read(base, I2C_DR_REG);
    err = stm32f4_i2c_wait_sr1(base, I2C_SR1_BTF_Msk, to);
    if (err)
        return err;
    if (stop)
        whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_STOP_Msk,
                        I2C_CR1_STOP_Msk);
    data[dataSz - 2] = (uint8_t)whal_Reg_Read(base, I2C_DR_REG);
    data[dataSz - 1] = (uint8_t)whal_Reg_Read(base, I2C_DR_REG);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_I2c_Init(whal_I2c *i2cDev)
{
    size_t base;
    const whal_Stm32f4_I2c_Cfg *cfg;
    uint32_t freqMhz;

    stm32f4_i2c_dev(i2cDev, &base, &cfg);
    if (!cfg)
        return WHAL_EINVAL;

    /* Software reset to known state. */
    whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_SWRST_Msk, I2C_CR1_SWRST_Msk);
    whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_SWRST_Msk, 0);

    /* Program peripheral clock frequency in MHz (2-50 valid). */
    freqMhz = cfg->pclk1 / 1000000UL;
    if (freqMhz < 2 || freqMhz > 50)
        return WHAL_EINVAL;
    whal_Reg_Update(base, I2C_CR2_REG, I2C_CR2_FREQ_Msk,
                    whal_SetBits(I2C_CR2_FREQ_Msk, I2C_CR2_FREQ_Pos, freqMhz));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_I2c_Deinit(whal_I2c *i2cDev)
{
    size_t base;
    const whal_Stm32f4_I2c_Cfg *cfg;

    stm32f4_i2c_dev(i2cDev, &base, &cfg);
    whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_PE_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_I2c_StartCom(whal_I2c *i2cDev, whal_I2c_ComCfg *comCfg)
{
    size_t base;
    const whal_Stm32f4_I2c_Cfg *cfg;
    uint32_t ccr;
    uint32_t trise;
    uint32_t pclk1;
    int fastMode;

    if (!comCfg || comCfg->addrSz != 7)
        return WHAL_EINVAL;

    stm32f4_i2c_dev(i2cDev, &base, &cfg);
    pclk1 = cfg->pclk1;
    fastMode = comCfg->freq > STM32F4_I2C_SM_MAX_HZ;

    /* Peripheral must be disabled to program CCR/TRISE. */
    whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_PE_Msk, 0);

    /* CCR: see RM0090 §27.6.8.
     *   Sm:    CCR = f_PCLK1 / (2 * f_SCL)         (min 0x04)
     *   Fm 2:1 CCR = f_PCLK1 / (3 * f_SCL)
     */
    if (!fastMode) {
        ccr = pclk1 / (2 * comCfg->freq);
        if (ccr < 0x04)
            ccr = 0x04;
        /* TRISE = f_PCLK1_MHz + 1 for Sm (1000 ns max rise time). */
        trise = (pclk1 / 1000000UL) + 1;
        whal_Reg_Update(base, I2C_CCR_REG,
                        I2C_CCR_FS_Msk | I2C_CCR_DUTY_Msk | I2C_CCR_CCR_Msk,
                        whal_SetBits(I2C_CCR_CCR_Msk, I2C_CCR_CCR_Pos, ccr));
    } else {
        ccr = pclk1 / (3 * comCfg->freq);
        if (ccr < 0x01)
            ccr = 0x01;
        /* TRISE = (f_PCLK1_MHz * 300 / 1000) + 1 for Fm (300 ns max). */
        trise = ((pclk1 / 1000000UL) * 300UL) / 1000UL + 1UL;
        whal_Reg_Update(base, I2C_CCR_REG,
                        I2C_CCR_FS_Msk | I2C_CCR_DUTY_Msk | I2C_CCR_CCR_Msk,
                        I2C_CCR_FS_Msk |
                        whal_SetBits(I2C_CCR_CCR_Msk, I2C_CCR_CCR_Pos, ccr));
    }
    whal_Reg_Update(base, I2C_TRISE_REG, I2C_TRISE_Msk,
                    whal_SetBits(I2C_TRISE_Msk, I2C_TRISE_Pos, trise));

    /* Enable and arm ACK for receives. */
    whal_Reg_Update(base, I2C_CR1_REG,
                    I2C_CR1_PE_Msk | I2C_CR1_ACK_Msk,
                    I2C_CR1_PE_Msk | I2C_CR1_ACK_Msk);

    s_lastAddr = comCfg->addr;
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_I2c_EndCom(whal_I2c *i2cDev)
{
    size_t base;
    const whal_Stm32f4_I2c_Cfg *cfg;

    stm32f4_i2c_dev(i2cDev, &base, &cfg);
    whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_PE_Msk, 0);
    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4_I2c_Transfer(whal_I2c *i2cDev, whal_I2c_Msg *msgs,
                                     size_t numMsgs)
{
    size_t base;
    const whal_Stm32f4_I2c_Cfg *cfg;
    whal_Error err;

    if (!msgs || !numMsgs)
        return WHAL_EINVAL;
    stm32f4_i2c_dev(i2cDev, &base, &cfg);

    /* Each message represents one I2C segment. The address used is
     * supplied via StartCom; we re-issue START on any msg flagged
     * MSG_START (the I2C interface treats a START while MSL=1 as a
     * restart). */
    for (size_t m = 0; m < numMsgs; ++m) {
        const whal_I2c_Msg *msg = &msgs[m];
        int isRead = (msg->flags & WHAL_I2C_MSG_READ) ? 1 : 0;
        int doStart = (msg->flags & WHAL_I2C_MSG_START) ? 1 : 0;
        int doStop  = (msg->flags & WHAL_I2C_MSG_STOP)  ? 1 : 0;

        if (doStart) {
            err = stm32f4_i2c_start_addr(base, s_lastAddr, isRead,
                                         cfg->timeout);
            if (err)
                return err;
        }

        if (isRead) {
            err = stm32f4_i2c_read(base, (uint8_t *)msg->data, msg->dataSz,
                                   doStop, cfg->timeout);
            if (err)
                return err;
        } else {
            if (doStart) {
                /* For writes, the address phase needs ADDR cleared before
                 * data. Reads handle this inside stm32f4_i2c_read. */
                err = stm32f4_i2c_clear_addr(base, cfg->timeout);
                if (err)
                    return err;
            }
            err = stm32f4_i2c_write(base, (const uint8_t *)msg->data,
                                    msg->dataSz, cfg->timeout);
            if (err)
                return err;
            if (doStop)
                whal_Reg_Update(base, I2C_CR1_REG, I2C_CR1_STOP_Msk,
                                I2C_CR1_STOP_Msk);
        }
    }
    return WHAL_SUCCESS;
}

#ifndef WHAL_CFG_STM32F4_I2C_DIRECT_API_MAPPING
const whal_I2cDriver whal_Stm32f4_I2c_Driver = {
    .Init = whal_Stm32f4_I2c_Init,
    .Deinit = whal_Stm32f4_I2c_Deinit,
    .StartCom = whal_Stm32f4_I2c_StartCom,
    .EndCom = whal_Stm32f4_I2c_EndCom,
    .Transfer = whal_Stm32f4_I2c_Transfer,
};
#endif /* !WHAL_CFG_STM32F4_I2C_DIRECT_API_MAPPING */
