#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Uart_Api(void)
{
    uint8_t buf[8];

    WHAL_ASSERT_EQ(whal_Uart_Init(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Uart_Deinit(NULL), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Uart_Send(NULL, buf, sizeof(buf)), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Uart_Recv(NULL, buf, sizeof(buf)), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Uart_SendAsync(NULL, buf, sizeof(buf)), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Uart_RecvAsync(NULL, buf, sizeof(buf)), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Uart_Send(BOARD_UART_DEV, NULL, 8), WHAL_EINVAL);
    WHAL_ASSERT_EQ(whal_Uart_Recv(BOARD_UART_DEV, NULL, 8), WHAL_EINVAL);
}

void whal_Test_Uart(void)
{
    WHAL_TEST_SUITE_START("uart");
    WHAL_TEST(Test_Uart_Api);
    WHAL_TEST_SUITE_END();
}
