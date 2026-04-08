#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Irq_Api(void)
{
    WHAL_ASSERT_EQ(whal_Irq_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Irq_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Irq_Enable(NULL, 0, NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Irq_Disable(NULL, 0), WHAL_EINVAL);
}

void whal_Test_Irq(void)
{
    WHAL_TEST_SUITE_START("irq");
    WHAL_TEST(Test_Irq_Api);
    WHAL_TEST_SUITE_END();
}
