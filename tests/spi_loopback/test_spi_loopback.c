#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

/*
 * Generic SPI loopback test.
 *
 * Requires MOSI wired to MISO so transmitted data is received back.
 * The board must provide g_whalSpi.
 */

static whal_Spi_ComCfg loopbackComCfg = {
    .freq = 1000000,
    .mode = WHAL_SPI_MODE_0,
    .wordSz = 8,
    .dataLines = 1,
};

static void Test_SpiLoopback_SendRecv(void)
{
    uint8_t tx[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t rx[4] = {0};

    WHAL_ASSERT_EQ(whal_Spi_StartCom(&g_whalSpi, &loopbackComCfg),
                   WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Spi_SendRecv(&g_whalSpi,
                                      tx, sizeof(tx), rx, sizeof(rx)),
                   WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Spi_EndCom(&g_whalSpi), WHAL_SUCCESS);
    WHAL_ASSERT_MEM_EQ(rx, tx, sizeof(tx));
}

static void Test_SpiLoopback_NullBufWithLen(void)
{
    uint8_t buf[1] = {0};

    WHAL_ASSERT_EQ(whal_Spi_StartCom(&g_whalSpi, &loopbackComCfg),
                   WHAL_SUCCESS);

    /* NULL tx with nonzero txLen */
    WHAL_ASSERT_EQ(whal_Spi_SendRecv(&g_whalSpi, NULL, 1, buf, 1),
                   WHAL_EINVAL);

    /* NULL rx with nonzero rxLen */
    WHAL_ASSERT_EQ(whal_Spi_SendRecv(&g_whalSpi, buf, 1, NULL, 1),
                   WHAL_EINVAL);

    WHAL_ASSERT_EQ(whal_Spi_EndCom(&g_whalSpi), WHAL_SUCCESS);
}

static void Test_SpiLoopback_SendRecvDrain(void)
{
    uint8_t tx[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t rx[4] = {0};
    uint8_t expected[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    WHAL_ASSERT_EQ(whal_Spi_StartCom(&g_whalSpi, &loopbackComCfg),
                   WHAL_SUCCESS);

    /* Send-only: driver must drain RX FIFO internally */
    WHAL_ASSERT_EQ(whal_Spi_SendRecv(&g_whalSpi, tx, sizeof(tx), NULL, 0),
                   WHAL_SUCCESS);

    /* Receive-only: loopback returns 0xFF (the dummy TX byte) */
    WHAL_ASSERT_EQ(whal_Spi_SendRecv(&g_whalSpi, NULL, 0, rx, sizeof(rx)),
                   WHAL_SUCCESS);

    WHAL_ASSERT_EQ(whal_Spi_EndCom(&g_whalSpi), WHAL_SUCCESS);

    /* If RX wasn't drained, stale 0xDE/0xAD/0xBE/0xEF leaks here */
    WHAL_ASSERT_MEM_EQ(rx, expected, sizeof(expected));
}

void whal_Test_Spi_Loopback(void)
{
    WHAL_TEST_SUITE_START("spi_loopback");
    WHAL_TEST(Test_SpiLoopback_SendRecv);
    WHAL_TEST(Test_SpiLoopback_NullBufWithLen);
    WHAL_TEST(Test_SpiLoopback_SendRecvDrain);
    WHAL_TEST_SUITE_END();
}
