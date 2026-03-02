#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/clock/pic32cz_clock.h>
#include <wolfHAL/bitops.h>
#include "board.h"
#include "test.h"

#define GCLK_PCHCTRL_OFFSET(ch) (0x10000 + 0x80 + ((ch) * 4))
#define GCLK_PCHCTRL_CHEN_Pos   6
#define GCLK_PCHCTRL_CHEN_Msk   (1UL << GCLK_PCHCTRL_CHEN_Pos)

#define MCLK_CLKMSK_OFFSET(inst) (0x12000 + 0x3C + ((inst) * 4))

static void Test_Clock_EnableDisable(void)
{
    /* Use a test clock descriptor for SERCOM4 (same as UART clock) */
    whal_Pic32czClock_Clk testClk = {
        .gclkPeriphChannel = 25,
        .gclkPeriphSrc = 0,
        .mclkEnableInst = 1,
        .mclkEnableMask = (1UL << 3),
        .mclkEnablePos = 3,
    };

    /* Save original state */
    size_t origChen = 0;
    whal_Reg_Get(g_whalClock.regmap.base, GCLK_PCHCTRL_OFFSET(25),
                 GCLK_PCHCTRL_CHEN_Msk, GCLK_PCHCTRL_CHEN_Pos, &origChen);

    size_t origMclk = 0;
    whal_Reg_Get(g_whalClock.regmap.base, MCLK_CLKMSK_OFFSET(1),
                 (1UL << 3), 3, &origMclk);

    size_t val = 0;

    /* Enable and verify */
    WHAL_ASSERT_EQ(whal_Clock_Enable(&g_whalClock, &testClk), WHAL_SUCCESS);

    whal_Reg_Get(g_whalClock.regmap.base, GCLK_PCHCTRL_OFFSET(25),
                 GCLK_PCHCTRL_CHEN_Msk, GCLK_PCHCTRL_CHEN_Pos, &val);
    WHAL_ASSERT_EQ(val, 1);

    whal_Reg_Get(g_whalClock.regmap.base, MCLK_CLKMSK_OFFSET(1),
                 (1UL << 3), 3, &val);
    WHAL_ASSERT_EQ(val, 1);

    /* Disable and verify */
    WHAL_ASSERT_EQ(whal_Clock_Disable(&g_whalClock, &testClk), WHAL_SUCCESS);

    whal_Reg_Get(g_whalClock.regmap.base, GCLK_PCHCTRL_OFFSET(25),
                 GCLK_PCHCTRL_CHEN_Msk, GCLK_PCHCTRL_CHEN_Pos, &val);
    WHAL_ASSERT_EQ(val, 0);

    /* Restore original state */
    if (origChen)
        whal_Clock_Enable(&g_whalClock, &testClk);
}

void whal_Test_Clock_Platform(void)
{
    WHAL_TEST(Test_Clock_EnableDisable);
}
