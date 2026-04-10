#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Supply_Api(void)
{
    WHAL_ASSERT_EQ(whal_Supply_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Supply_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Supply_Enable(NULL, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Supply_Disable(NULL, NULL), WHAL_EINVAL);
}

void whal_Test_Supply(void)
{
    WHAL_TEST_SUITE_START("supply");
    WHAL_TEST(Test_Supply_Api);
    WHAL_TEST_SUITE_END();
}
