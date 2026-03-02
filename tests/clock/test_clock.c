#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Clock_GetRate(void)
{
    size_t rate = 0;
    WHAL_ASSERT_EQ(whal_Clock_GetRate(&g_whalClock, &rate), WHAL_SUCCESS);
    WHAL_ASSERT_NEQ(rate, 0);
}

void whal_Test_Clock(void)
{
    WHAL_TEST_SUITE_START("clock");
    WHAL_TEST(Test_Clock_GetRate);
    WHAL_TEST_SUITE_END();
}
