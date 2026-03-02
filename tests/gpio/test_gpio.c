#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Gpio_SetGetHighLow(void)
{
    size_t val = 0;

    /* Set high and verify */
    WHAL_ASSERT_EQ(whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 1), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Gpio_Get(&g_whalGpio, BOARD_LED_PIN, &val), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(val, 1);

    /* Set low and verify */
    WHAL_ASSERT_EQ(whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 0), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Gpio_Get(&g_whalGpio, BOARD_LED_PIN, &val), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(val, 0);
}

void whal_Test_Gpio(void)
{
    WHAL_TEST_SUITE_START("gpio");
    WHAL_TEST(Test_Gpio_SetGetHighLow);
    WHAL_TEST_SUITE_END();
}
