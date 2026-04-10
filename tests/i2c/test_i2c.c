#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_I2c_Api(void)
{
    whal_I2c_ComCfg comCfg = { .freq = 100000, .addr = 0x50, .addrSz = 7 };
    whal_I2c_Msg msg = { .data = (uint8_t[]){0}, .dataSz = 1, .flags = 0 };

    WHAL_ASSERT_EQ(whal_I2c_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_I2c_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_I2c_StartCom(NULL, &comCfg), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_I2c_EndCom(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_I2c_Transfer(NULL, &msg, 1), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_I2c_StartCom(&g_whalI2c, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_I2c_Transfer(&g_whalI2c, NULL, 1), WHAL_EINVAL);
}

void whal_Test_I2c(void)
{
    WHAL_TEST_SUITE_START("i2c");
    WHAL_TEST(Test_I2c_Api);
    WHAL_TEST_SUITE_END();
}
