#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/timer/timer.h>
#include "pic32cz_curiosity_ultra.h"
#include "../test.h"

/* Tick counter incremented by SysTick_Handler in test_main.c */
extern volatile size_t g_tick;

static void test_timer_ticks_advance(void)
{
    size_t before = g_tick;

    /* Spin for ~100ms worth of ticks. At 1ms/tick this should yield ~100. */
    volatile size_t spin = 0;
    while (g_tick - before < 100) {
        spin++;
    }

    size_t elapsed = g_tick - before;

    /* At least 100 ticks should have passed */
    WHAL_ASSERT_NEQ(elapsed, 0);
    WHAL_ASSERT_EQ(elapsed >= 100, 1);
}

void test_timer(void)
{
    WHAL_TEST_SUITE_START("timer");
    WHAL_TEST(test_timer_ticks_advance);
    WHAL_TEST_SUITE_END();
}
