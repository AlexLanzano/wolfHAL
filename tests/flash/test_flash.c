#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Flash_EraseBlank(void)
{
    uint8_t readback[8] = {0};
    uint8_t erased[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    WHAL_ASSERT_EQ(whal_Flash_Unlock(&g_whalFlash, BOARD_FLASH_TEST_ADDR, BOARD_FLASH_SECTOR_SZ), WHAL_SUCCESS);

    WHAL_ASSERT_EQ(whal_Flash_Erase(&g_whalFlash, BOARD_FLASH_TEST_ADDR,
                                     BOARD_FLASH_SECTOR_SZ),
                   WHAL_SUCCESS);


    WHAL_ASSERT_EQ(whal_Flash_Read(&g_whalFlash, BOARD_FLASH_TEST_ADDR,
                                    readback, sizeof(readback)),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(readback, erased, sizeof(erased));

    WHAL_ASSERT_EQ(whal_Flash_Lock(&g_whalFlash, BOARD_FLASH_TEST_ADDR, BOARD_FLASH_SECTOR_SZ), WHAL_SUCCESS);
}

static void Test_Flash_WriteRead(void)
{
    uint8_t pattern[] = "wolfHAL";
    uint8_t readback[sizeof(pattern)] = {0};

    WHAL_ASSERT_EQ(whal_Flash_Unlock(&g_whalFlash, BOARD_FLASH_TEST_ADDR, BOARD_FLASH_SECTOR_SZ), WHAL_SUCCESS);

    WHAL_ASSERT_EQ(whal_Flash_Erase(&g_whalFlash, BOARD_FLASH_TEST_ADDR,
                                     BOARD_FLASH_SECTOR_SZ),
                   WHAL_SUCCESS);

    whal_Error err;
    do {
        err = whal_Flash_Write(&g_whalFlash, BOARD_FLASH_TEST_ADDR, pattern,
                               sizeof(pattern));
    } while (err == WHAL_ENOTREADY);
    WHAL_ASSERT_EQ(err, WHAL_SUCCESS);


    WHAL_ASSERT_EQ(whal_Flash_Read(&g_whalFlash, BOARD_FLASH_TEST_ADDR,
                                    readback, sizeof(readback)),
                   WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pattern, readback, sizeof(pattern));

    WHAL_ASSERT_EQ(whal_Flash_Lock(&g_whalFlash, BOARD_FLASH_TEST_ADDR, BOARD_FLASH_SECTOR_SZ), WHAL_SUCCESS);
}

void whal_Test_Flash(void)
{
    WHAL_TEST_SUITE_START("flash");
    WHAL_TEST(Test_Flash_EraseBlank);
    WHAL_TEST(Test_Flash_WriteRead);
    WHAL_TEST_SUITE_END();
}
