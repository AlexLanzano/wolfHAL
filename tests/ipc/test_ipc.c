#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Ipc_InitDeinit(void)
{
    WHAL_ASSERT_EQ(whal_Ipc_Init(&g_whalIpc), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Ipc_Deinit(&g_whalIpc), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Ipc_Init(&g_whalIpc), WHAL_SUCCESS);
}

static void Test_Ipc_InitNull(void)
{
    WHAL_ASSERT_EQ(whal_Ipc_Init(NULL), WHAL_EINVAL);
}

static void Test_Ipc_SendNull(void)
{
    WHAL_ASSERT_EQ(whal_Ipc_Send(&g_whalIpc, NULL, 0), WHAL_EINVAL);
}

static void Test_Ipc_RecvNull(void)
{
    WHAL_ASSERT_EQ(whal_Ipc_Recv(&g_whalIpc, NULL, 0), WHAL_EINVAL);
}

void whal_Test_Ipc(void)
{
    WHAL_TEST_SUITE_START("ipc");
    WHAL_TEST(Test_Ipc_InitDeinit);
    WHAL_TEST(Test_Ipc_InitNull);
    WHAL_TEST(Test_Ipc_SendNull);
    WHAL_TEST(Test_Ipc_RecvNull);
    WHAL_TEST_SUITE_END();
}
