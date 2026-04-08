#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Sensor_Api(void)
{
    uint8_t data[32];

    WHAL_ASSERT_EQ(whal_Sensor_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Sensor_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Sensor_Read(NULL, data), WHAL_EINVAL);
}

void whal_Test_Sensor(void)
{
    WHAL_TEST_SUITE_START("sensor");
    WHAL_TEST(Test_Sensor_Api);
    WHAL_TEST_SUITE_END();
}
