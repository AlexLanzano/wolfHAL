#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

/*
 * Generic SPI loopback test.
 *
 * Requires MOSI wired to MISO so transmitted data is received back.
 * The board must provide g_whalSpi and g_whalSpiComCfg.
 */

static void Test_Spi_Loopback(void)
{
    uint8_t tx[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t rx[4] = {0};

    WHAL_ASSERT_EQ(whal_Spi_SendRecv(&g_whalSpi, &g_whalSpiComCfg,
                                      tx, sizeof(tx), rx, sizeof(rx)),
                   WHAL_SUCCESS);
    WHAL_ASSERT_MEM_EQ(rx, tx, sizeof(tx));
}

void whal_Test_Spi_Loopback(void)
{
    WHAL_TEST_SUITE_START("spi_loopback");
    WHAL_TEST(Test_Spi_Loopback);
    WHAL_TEST_SUITE_END();
}
