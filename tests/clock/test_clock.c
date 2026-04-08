#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Clock_Api(void)
{
    WHAL_ASSERT_EQ(whal_Clock_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Enable(NULL, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Clock_Disable(NULL, NULL), WHAL_EINVAL);
}

void whal_Test_Clock(void)
{
    WHAL_TEST_SUITE_START("clock");
    WHAL_TEST(Test_Clock_Api);
    WHAL_TEST_SUITE_END();
}
