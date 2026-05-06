#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Crypto_Api(void)
{
    WHAL_ASSERT_EQ(whal_Crypto_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Crypto_Init(NULL), WHAL_EINVAL);
}

void whal_Test_Crypto(void)
{
    WHAL_TEST_SUITE_START("crypto");
    WHAL_TEST(Test_Crypto_Api);
    WHAL_TEST_SUITE_END();
}
