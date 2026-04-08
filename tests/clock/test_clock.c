#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Clock_Api(void)
{
    size_t rate;

    WHAL_ASSERT_EQ(whal_Clock_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Enable(NULL, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Disable(NULL, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_GetRate(NULL, &rate), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_GetRate(&g_whalClock, NULL), WHAL_EINVAL);
}

static void Test_Clock_GetRate(void)
{
    size_t rate = 0;
    WHAL_ASSERT_EQ(whal_Clock_GetRate(&g_whalClock, &rate), WHAL_SUCCESS);
    WHAL_ASSERT_NEQ(rate, 0);
}

void whal_Test_Clock(void)
{
    WHAL_TEST_SUITE_START("clock");
    WHAL_TEST(Test_Clock_Api);
    WHAL_TEST(Test_Clock_GetRate);
    WHAL_TEST_SUITE_END();
}
