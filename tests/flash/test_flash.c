#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"
#include "peripheral.h"

static whal_Flash *g_testFlashDev;
static size_t g_testFlashAddr;
static size_t g_testFlashSectorSz;

static void Test_Flash_EraseBlank(void)
{
    uint8_t readback[8] = {0};
    uint8_t erased[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    WHAL_ASSERT_EQ(whal_Flash_Unlock(g_testFlashDev, g_testFlashAddr,
                                      g_testFlashSectorSz), WHAL_SUCCESS);

    WHAL_ASSERT_EQ(whal_Flash_Erase(g_testFlashDev, g_testFlashAddr,
                                     g_testFlashSectorSz), WHAL_SUCCESS);

    WHAL_ASSERT_EQ(whal_Flash_Read(g_testFlashDev, g_testFlashAddr,
                                    readback, sizeof(readback)), WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(readback, erased, sizeof(erased));

    WHAL_ASSERT_EQ(whal_Flash_Lock(g_testFlashDev, g_testFlashAddr,
                                    g_testFlashSectorSz), WHAL_SUCCESS);
}

static void Test_Flash_WriteRead(void)
{
    uint8_t pattern[] = "wolfHAL";
    uint8_t readback[sizeof(pattern)] = {0};

    WHAL_ASSERT_EQ(whal_Flash_Unlock(g_testFlashDev, g_testFlashAddr,
                                      g_testFlashSectorSz), WHAL_SUCCESS);

    WHAL_ASSERT_EQ(whal_Flash_Erase(g_testFlashDev, g_testFlashAddr,
                                     g_testFlashSectorSz), WHAL_SUCCESS);

    whal_Error err;
    do {
        err = whal_Flash_Write(g_testFlashDev, g_testFlashAddr, pattern,
                               sizeof(pattern));
    } while (err == WHAL_ENOTREADY);
    WHAL_ASSERT_EQ(err, WHAL_SUCCESS);

    WHAL_ASSERT_EQ(whal_Flash_Read(g_testFlashDev, g_testFlashAddr,
                                    readback, sizeof(readback)), WHAL_SUCCESS);

    WHAL_ASSERT_MEM_EQ(pattern, readback, sizeof(pattern));

    WHAL_ASSERT_EQ(whal_Flash_Lock(g_testFlashDev, g_testFlashAddr,
                                    g_testFlashSectorSz), WHAL_SUCCESS);
}

static void run_flash_tests(const char *name)
{
    WHAL_TEST_SUITE_START("flash");
    if (name)
        whal_Test_Printf("  device: %s\n", name);
    WHAL_TEST(Test_Flash_EraseBlank);
    WHAL_TEST(Test_Flash_WriteRead);
    WHAL_TEST_SUITE_END();
}

void whal_Test_Flash(void)
{
    /* Test on-chip flash */
    g_testFlashDev = &g_whalFlash;
    g_testFlashAddr = BOARD_FLASH_TEST_ADDR;
    g_testFlashSectorSz = BOARD_FLASH_SECTOR_SZ;
    run_flash_tests("on-chip");

    /* Test peripheral flash devices */
    for (size_t i = 0; g_peripheralFlash[i].dev; i++) {
        g_testFlashDev = g_peripheralFlash[i].dev;
        g_testFlashAddr = 0;
        g_testFlashSectorSz = g_peripheralFlash[i].sectorSz;
        run_flash_tests(g_peripheralFlash[i].name);
    }
}
