/* sdhc.h
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

#ifndef WHAL_SDHC_H
#define WHAL_SDHC_H

#include <wolfHAL/error.h>
#include <stdint.h>
#include <stddef.h>

/*
 * @file sdhc.h
 * @brief Generic SD host controller (SDHC) transport abstraction and driver
 * interface.
 *
 * Models the SD/MMC/SDIO host controller as a command/data transport: the
 * driver issues SD commands and moves data blocks, and knows nothing about
 * card types. Card enumeration and the SD, eMMC, and SDIO protocols are built
 * on top of this interface. It is deliberately not a block device -- a data
 * phase is a property of a command (Request), so the same transport carries
 * memory reads/writes, register reads (CID/CSD/SCR/SD status), and SDIO IO.
 */

/*
 * Command response type (whal_Sdhc_Command.responseType). Encodes the expected
 * response length and check semantics; the driver maps each to the
 * controller's response-type field.
 */
#define WHAL_SDHC_RSP_NONE 0 /* no response */
#define WHAL_SDHC_RSP_R1   1 /* 48-bit, normal */
#define WHAL_SDHC_RSP_R1B  2 /* 48-bit, with DAT0 busy signalling */
#define WHAL_SDHC_RSP_R2   3 /* 136-bit (CID/CSD) */
#define WHAL_SDHC_RSP_R3   4 /* 48-bit, OCR, no CRC */
#define WHAL_SDHC_RSP_R4   5 /* 48-bit, SDIO OCR */
#define WHAL_SDHC_RSP_R5   6 /* 48-bit, SDIO CMD52/CMD53 */
#define WHAL_SDHC_RSP_R6   7 /* 48-bit, published RCA */
#define WHAL_SDHC_RSP_R7   8 /* 48-bit, card interface condition */

/* Data phase direction (whal_Sdhc_Data.dir). */
#define WHAL_SDHC_DATA_READ  0
#define WHAL_SDHC_DATA_WRITE 1

/* Bus width (whal_Sdhc_ComCfg.busWidth). */
#define WHAL_SDHC_BUS_WIDTH_1 0
#define WHAL_SDHC_BUS_WIDTH_4 1
#define WHAL_SDHC_BUS_WIDTH_8 2

/* Bus timing / speed mode (whal_Sdhc_ComCfg.timing). */
#define WHAL_SDHC_TIMING_LEGACY 0 /* default speed */
#define WHAL_SDHC_TIMING_HS     1 /* high speed */
#define WHAL_SDHC_TIMING_SDR12  2
#define WHAL_SDHC_TIMING_SDR25  3
#define WHAL_SDHC_TIMING_SDR50  4
#define WHAL_SDHC_TIMING_SDR104 5
#define WHAL_SDHC_TIMING_DDR50  6
#define WHAL_SDHC_TIMING_HS200  7 /* eMMC */
#define WHAL_SDHC_TIMING_HS400  8 /* eMMC */

/* Signal voltage (whal_Sdhc_ComCfg.signalVoltage). */
#define WHAL_SDHC_VOLTAGE_330 0 /* 3.3 V */
#define WHAL_SDHC_VOLTAGE_180 1 /* 1.8 V */
#define WHAL_SDHC_VOLTAGE_120 2 /* 1.2 V */

/* Card power (whal_Sdhc_ComCfg.powerMode). */
#define WHAL_SDHC_POWER_OFF 0
#define WHAL_SDHC_POWER_ON  1

/* Bus drive mode (whal_Sdhc_ComCfg.busMode): open-drain during the CMD0/CMD1
 * identification phase, push-pull thereafter. */
#define WHAL_SDHC_BUSMODE_OPENDRAIN 0
#define WHAL_SDHC_BUSMODE_PUSHPULL  1

/*
 * @brief A single SD command, optionally carrying a data phase.
 */
typedef struct {
    uint32_t opcode;       /* Command index (CMD0..CMD63) */
    uint32_t arg;          /* 32-bit command argument */
    uint32_t response[4];  /* Response: R1/R3/R6/R7 in [0]; R2 fills [0..3] */
    uint8_t  responseType; /* WHAL_SDHC_RSP_x */
} whal_Sdhc_Command;

/*
 * @brief Optional data phase attached to a Request.
 */
typedef struct {
    void    *data;        /* Read: destination buffer; write: source buffer */
    uint32_t blockSize;   /* Bytes per block (typically 512) */
    uint32_t blockCount;  /* Number of blocks */
    uint8_t  dir;         /* WHAL_SDHC_DATA_READ or _WRITE */
    uint32_t bytesXfered; /* Output: bytes actually transferred */
} whal_Sdhc_Data;

/*
 * @brief Bus configuration applied by ConfigureCom.
 */
typedef struct {
    uint32_t clockHz;       /* Bus clock in Hz; 0 turns the clock off */
    uint8_t  busWidth;      /* WHAL_SDHC_BUS_WIDTH_x */
    uint8_t  timing;        /* WHAL_SDHC_TIMING_x */
    uint8_t  signalVoltage; /* WHAL_SDHC_VOLTAGE_x */
    uint8_t  powerMode;     /* WHAL_SDHC_POWER_x */
    uint8_t  busMode;       /* WHAL_SDHC_BUSMODE_x */
} whal_Sdhc_ComCfg;

typedef struct whal_Sdhc whal_Sdhc;

/*
 * @brief Driver vtable for SD host controllers.
 */
typedef struct {
    /* Initialize the host controller hardware. */
    whal_Error (*Init)(whal_Sdhc *dev);
    /* Deinitialize the host controller hardware. */
    whal_Error (*Deinit)(whal_Sdhc *dev);
    /* Apply bus configuration (clock, width, voltage, timing, power). */
    whal_Error (*ConfigureCom)(whal_Sdhc *dev, const whal_Sdhc_ComCfg *comCfg);
    /* Issue a command with an optional data phase (data may be NULL). */
    whal_Error (*Request)(whal_Sdhc *dev, whal_Sdhc_Command *cmd,
                          whal_Sdhc_Data *data);
} whal_SdhcDriver;

/*
 * @brief SD host controller device instance with base address, driver vtable,
 * and driver-specific cfg.
 */
struct whal_Sdhc {
    const size_t base;
    const whal_SdhcDriver *driver;
    void *cfg;
};

/*
 * @brief Initialize an SD host controller and its driver.
 *
 * @param dev Pointer to the SDHC instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer.
 * @retval WHAL_ENOTSUP No driver bound or operation unsupported.
 */
whal_Error whal_Sdhc_Init(whal_Sdhc *dev);

/*
 * @brief Deinitialize an SD host controller and release resources.
 *
 * @param dev Pointer to the SDHC instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer.
 * @retval WHAL_ENOTSUP No driver bound or operation unsupported.
 */
whal_Error whal_Sdhc_Deinit(whal_Sdhc *dev);

/*
 * @brief Apply bus configuration to the host controller.
 *
 * Sets the bus clock, width, signal voltage, timing mode, drive mode, and card
 * power. Called repeatedly as the card negotiation walks up from
 * identification (400 kHz, 1-bit, open-drain) toward the operational mode.
 *
 * @param dev    Pointer to the SDHC instance.
 * @param comCfg Bus configuration to apply.
 *
 * @retval WHAL_SUCCESS Configuration applied.
 * @retval WHAL_EINVAL  Null pointer.
 * @retval WHAL_ENOTSUP No driver bound, or an unsupported setting.
 */
whal_Error whal_Sdhc_ConfigureCom(whal_Sdhc *dev,
                                  const whal_Sdhc_ComCfg *comCfg);

/*
 * @brief Issue an SD command, optionally with a data phase.
 *
 * Sends @p cmd and waits for its response. When @p data is non-NULL the command
 * carries a data phase in the direction given by @p data->dir, and the call
 * blocks until both the response and the data transfer complete. Pass @p data
 * as NULL for command-only requests.
 *
 * @param dev  Pointer to the SDHC instance.
 * @param cmd  Command to issue; the response is written back into it.
 * @param data Optional data phase, or NULL for a command-only request.
 *
 * @retval WHAL_SUCCESS  Command (and data phase, if any) completed.
 * @retval WHAL_EINVAL   Null dev or cmd.
 * @retval WHAL_ENOTSUP  No driver bound or operation unsupported.
 * @retval WHAL_ETIMEOUT Response or data transfer did not complete in time.
 */
whal_Error whal_Sdhc_Request(whal_Sdhc *dev, whal_Sdhc_Command *cmd,
                             whal_Sdhc_Data *data);

#endif /* WHAL_SDHC_H */
