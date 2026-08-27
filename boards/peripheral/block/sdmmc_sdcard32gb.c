/* sdmmc_sdcard32gb.c
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

#include "sdmmc_sdcard32gb.h"
#include <wolfHAL/block/sdmmc_block.h>
#include "board.h"

whal_Block g_whalSdmmcSdcard32gb = {
    .driver = &whal_Sdmmc_Driver,
    .cfg = &(whal_Sdmmc_Cfg) {
        .sdhc    = BOARD_SDHC_DEV,
        .timeout = &g_whalTimeoutSdio,
    },
};
