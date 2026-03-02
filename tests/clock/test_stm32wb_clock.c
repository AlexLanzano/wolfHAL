#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32wb55xx.h>
#include <wolfHAL/clock/stm32wb_rcc.h>
#include <wolfHAL/bitops.h>
#include "board.h"
#include "test.h"

static void Test_Clock_EnableDisable(void)
{
    whal_Stm32wbRcc_Clk testClk = { WHAL_STM32WB55_GPIOA_CLOCK };

    /* Save original state */
    size_t origVal = 0;
    whal_Reg_Get(g_whalClock.regmap.base, 0x4C, (1 << 0), 0, &origVal);

    /* Enable and verify */
    WHAL_ASSERT_EQ(whal_Clock_Enable(&g_whalClock, &testClk), WHAL_SUCCESS);

    size_t val = 0;
    whal_Reg_Get(g_whalClock.regmap.base, 0x4C, (1 << 0), 0, &val);
    WHAL_ASSERT_EQ(val, 1);

    /* Disable and verify */
    WHAL_ASSERT_EQ(whal_Clock_Disable(&g_whalClock, &testClk), WHAL_SUCCESS);

    whal_Reg_Get(g_whalClock.regmap.base, 0x4C, (1 << 0), 0, &val);
    WHAL_ASSERT_EQ(val, 0);

    /* Restore original state */
    if (origVal)
        whal_Clock_Enable(&g_whalClock, &testClk);
}

void whal_Test_Clock_Platform(void)
{
    WHAL_TEST(Test_Clock_EnableDisable);
}
