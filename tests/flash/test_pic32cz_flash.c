/* test_pic32cz_flash.c
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
#include <wolfHAL/wolfHAL.h>
#include "wolfHAL_board.h"
#include "test.h"

/* Each case gets its own 128-byte slot in the erased test sector */
#define SLOT_SZ     128
#define ERASED_BYTE 0xFF

static void Test_Flash_DoublewordWrites(void)
{
    static const size_t offsets[] = { 0, 8, 16, 24 };
    static const size_t lens[] = { 8, 16, 24, 32, 40, 64 };
    uint8_t pattern[64];
    uint8_t readback[SLOT_SZ];
    size_t slot = 0;
    size_t i;
    size_t j;
    size_t k;

    for (i = 0; i < sizeof(pattern); i++)
        pattern[i] = (uint8_t)(i * 7 + 3);

    WHAL_ASSERT_EQ(whal_Flash_Unlock(BOARD_FLASH_DEV, BOARD_FLASH_TEST_ADDR,
                                     BOARD_FLASH_SECTOR_SZ),
                   WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Flash_Erase(BOARD_FLASH_DEV, BOARD_FLASH_TEST_ADDR,
                                    BOARD_FLASH_SECTOR_SZ),
                   WHAL_SUCCESS);

    for (i = 0; i < sizeof(offsets) / sizeof(offsets[0]); i++) {
        for (j = 0; j < sizeof(lens) / sizeof(lens[0]); j++, slot++) {
            size_t slotAddr = BOARD_FLASH_TEST_ADDR + slot * SLOT_SZ;

            WHAL_ASSERT_EQ(whal_Flash_Write(BOARD_FLASH_DEV,
                                            slotAddr + offsets[i],
                                            pattern, lens[j]),
                           WHAL_SUCCESS);
            WHAL_ASSERT_EQ(whal_Flash_Read(BOARD_FLASH_DEV, slotAddr,
                                           readback, sizeof(readback)),
                           WHAL_SUCCESS);

            for (k = 0; k < sizeof(readback); k++) {
                if (k < offsets[i] || k >= offsets[i] + lens[j])
                    WHAL_ASSERT_EQ(readback[k], ERASED_BYTE);
            }
            WHAL_ASSERT_MEM_EQ(readback + offsets[i], pattern, lens[j]);
        }
    }

    WHAL_ASSERT_EQ(whal_Flash_Erase(BOARD_FLASH_DEV, BOARD_FLASH_TEST_ADDR,
                                    BOARD_FLASH_SECTOR_SZ),
                   WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Flash_Lock(BOARD_FLASH_DEV, BOARD_FLASH_TEST_ADDR,
                                   BOARD_FLASH_SECTOR_SZ),
                   WHAL_SUCCESS);
}

void whal_Test_Flash_Platform(void)
{
    WHAL_TEST_SUITE_START("flash_platform");
    WHAL_TEST(Test_Flash_DoublewordWrites);
    WHAL_TEST_SUITE_END();
}
