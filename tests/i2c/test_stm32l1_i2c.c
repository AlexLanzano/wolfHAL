/* test_stm32l1_i2c.c
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

#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/i2c/stm32l1_i2c.h>
#include "wolfHAL_board.h"
#include "test.h"

/* The L1 I2C peripheral supports Standard and Fast mode only */
static void Test_Stm32l1_I2c_FreqLimit(void)
{
    whal_I2c_ComCfg com = {
        .freq = 400001,
        .addr = 0x50,
        .addrSz = 7,
    };

    WHAL_ASSERT_EQ(whal_I2c_StartCom(BOARD_I2C_DEV, &com), WHAL_EINVAL);

    com.freq = 1000000;
    WHAL_ASSERT_EQ(whal_I2c_StartCom(BOARD_I2C_DEV, &com), WHAL_EINVAL);

    com.freq = 400000;
    WHAL_ASSERT_EQ(whal_I2c_StartCom(BOARD_I2C_DEV, &com), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_I2c_EndCom(BOARD_I2C_DEV), WHAL_SUCCESS);
}

void whal_Test_I2c_Platform(void)
{
    WHAL_TEST_SUITE_START("i2c_platform");
    WHAL_TEST(Test_Stm32l1_I2c_FreqLimit);
    WHAL_TEST_SUITE_END();
}
