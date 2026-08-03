/* test_timeout.c
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include <wolfHAL/timeout.h>
#include "../test.h"

#ifndef WHAL_CFG_NO_TIMEOUT

#ifdef WHAL_CFG_64BIT_TICK
static uint64_t g_fakeTick;

static uint64_t FakeTick(void)
{
    return g_fakeTick;
}
#else
static uint32_t g_fakeTick;

static uint32_t FakeTick(void)
{
    return g_fakeTick;
}
#endif

static whal_Timeout g_timeout = {
    .timeoutTicks = 10,
    .GetTick = FakeTick,
};

/* Helper: get a pointer to g_timeout (avoids -Waddress on &global) */
static whal_Timeout *timeout(void)
{
    return &g_timeout;
}

/* WHAL_TIMEOUT_EXPIRED returns 0 when NULL */
static void Test_Timeout_NullNotExpired(void)
{
    whal_Timeout *t = NULL;
    int expired = WHAL_TIMEOUT_EXPIRED(t);
    WHAL_ASSERT_EQ(expired, 0);
}

/* START snapshots current tick */
static void Test_Timeout_StartSnapshotsTick(void)
{
    g_fakeTick = 42;
    WHAL_TIMEOUT_START(timeout());
    WHAL_ASSERT_EQ(g_timeout.startTick, 42);
}

/* Not expired immediately after start */
static void Test_Timeout_NotExpiredImmediately(void)
{
    g_fakeTick = 0;
    WHAL_TIMEOUT_START(timeout());
    int expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_EQ(expired, 0);
}

/* Not expired one tick before deadline */
static void Test_Timeout_NotExpiredBeforeDeadline(void)
{
    g_fakeTick = 0;
    WHAL_TIMEOUT_START(timeout());
    g_fakeTick = 9;
    int expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_EQ(expired, 0);
}

/* Expired exactly at deadline */
static void Test_Timeout_ExpiredAtDeadline(void)
{
    g_fakeTick = 0;
    WHAL_TIMEOUT_START(timeout());
    g_fakeTick = 10;
    int expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_NEQ(expired, 0);
}

/* Expired well past deadline */
static void Test_Timeout_ExpiredPastDeadline(void)
{
    g_fakeTick = 0;
    WHAL_TIMEOUT_START(timeout());
    g_fakeTick = 100;
    int expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_NEQ(expired, 0);
}

/* START resets the window */
static void Test_Timeout_StartResetsWindow(void)
{
    int expired;

    g_fakeTick = 0;
    WHAL_TIMEOUT_START(timeout());
    g_fakeTick = 10;
    expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_NEQ(expired, 0);

    /* Restart — should no longer be expired */
    WHAL_TIMEOUT_START(timeout());
    expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_EQ(expired, 0);
}

/* Non-zero start tick works correctly */
static void Test_Timeout_NonZeroStart(void)
{
    int expired;

    g_fakeTick = 1000;
    WHAL_TIMEOUT_START(timeout());
    g_fakeTick = 1005;
    expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_EQ(expired, 0);
    g_fakeTick = 1010;
    expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_NEQ(expired, 0);
}

/* Tick counter wrapping (unsigned subtraction handles this) */
static void Test_Timeout_TickWrap(void)
{
    int expired;

    g_fakeTick = WHAL_TICK_MAX - 3;
    WHAL_TIMEOUT_START(timeout());
    g_fakeTick = WHAL_TICK_MAX;
    expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_EQ(expired, 0);
    g_fakeTick = WHAL_TICK_MAX - 3 + 10;
    expired = WHAL_TIMEOUT_EXPIRED(timeout());
    WHAL_ASSERT_NEQ(expired, 0);
}

void whal_Test_Timeout(void)
{
    WHAL_TEST_SUITE_START("timeout");
    WHAL_TEST(Test_Timeout_NullNotExpired);
    WHAL_TEST(Test_Timeout_StartSnapshotsTick);
    WHAL_TEST(Test_Timeout_NotExpiredImmediately);
    WHAL_TEST(Test_Timeout_NotExpiredBeforeDeadline);
    WHAL_TEST(Test_Timeout_ExpiredAtDeadline);
    WHAL_TEST(Test_Timeout_ExpiredPastDeadline);
    WHAL_TEST(Test_Timeout_StartResetsWindow);
    WHAL_TEST(Test_Timeout_NonZeroStart);
    WHAL_TEST(Test_Timeout_TickWrap);
    WHAL_TEST_SUITE_END();
}

#endif /* !WHAL_CFG_NO_TIMEOUT */
