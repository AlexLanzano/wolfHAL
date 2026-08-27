/* lpc55s6x_sdhc.h
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

#ifndef WHAL_LPC55S6X_SDHC_H
#define WHAL_LPC55S6X_SDHC_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/sdhc/sdhc.h>
#include <wolfHAL/timeout.h>

/*
 * @file lpc55s6x_sdhc.h
 * @brief LPC55S6x SDMMC (SD host controller) driver configuration.
 */

/*
 * @brief LPC55S6x SDHC device configuration.
 */
typedef struct whal_Lpc55s6x_Sdhc_Cfg {
    uint32_t fclkHz;
    whal_Timeout *timeout;
    uint8_t cardNum;       /* card slot: 0 = SD0, 1 = SD1 */
} whal_Lpc55s6x_Sdhc_Cfg;

/*
 * @brief Fixed device instance. Defined in the driver TU
 * from the WHAL_CFG_LPC55S6X_SDHC_DEV initializer in board.h.
 */
#if defined(WHAL_CFG_LPC55S6X_SDHC_SINGLE_INSTANCE)
extern const whal_Sdhc whal_Lpc55s6x_Sdhc_Dev;
#endif

#ifndef WHAL_CFG_LPC55S6X_SDHC_DIRECT_API_MAPPING
/*
 * @brief Driver instance for LPC55S6x SDHC peripheral.
 */
extern const whal_SdhcDriver whal_Lpc55s6x_Sdhc_Driver;

/*
 * @brief Initialize the LPC55S6x SDMMC peripheral.
 *
 * @param dev SDHC device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Sdhc_Init(whal_Sdhc *dev);

/*
 * @brief Deinitialize the LPC55S6x SDMMC peripheral.
 *
 * @param dev SDHC device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinitialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Sdhc_Deinit(whal_Sdhc *dev);

/*
 * @brief Apply bus configuration to the LPC55S6x SDMMC peripheral.
 *
 * @param dev    SDHC device instance.
 * @param comCfg Bus configuration to apply.
 *
 * @retval WHAL_SUCCESS Configuration applied.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Lpc55s6x_Sdhc_ConfigureCom(whal_Sdhc *dev,
                                           const whal_Sdhc_ComCfg *comCfg);

/*
 * @brief Issue an SD command on the LPC55S6x SDMMC peripheral.
 *
 * @param dev  SDHC device instance.
 * @param cmd  Command to issue; the response is written back into it.
 * @param data Optional data phase, or NULL for a command-only request.
 *
 * @retval WHAL_SUCCESS   Command completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ETIMEOUT  Response or data transfer timed out.
 * @retval WHAL_EHARDWARE Response or data CRC / bus error.
 */
whal_Error whal_Lpc55s6x_Sdhc_Request(whal_Sdhc *dev, whal_Sdhc_Command *cmd,
                                      whal_Sdhc_Data *data);
#endif /* !WHAL_CFG_LPC55S6X_SDHC_DIRECT_API_MAPPING */

#endif /* WHAL_LPC55S6X_SDHC_H */
