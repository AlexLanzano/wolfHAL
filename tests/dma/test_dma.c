#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Dma_Api(void)
{
    uint8_t chCfg[1];

    WHAL_ASSERT_EQ(whal_Dma_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Dma_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Dma_Configure(NULL, 0, chCfg), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Dma_Start(NULL, 0), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Dma_Stop(NULL, 0), WHAL_EINVAL);
}

void whal_Test_Dma(void)
{
    WHAL_TEST_SUITE_START("dma");
    WHAL_TEST(Test_Dma_Api);
    WHAL_TEST_SUITE_END();
}
