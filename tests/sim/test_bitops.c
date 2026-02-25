#include <wolfHAL/bitops.h>
#include "../test.h"

static void test_bitmask_4(void)
{
    WHAL_ASSERT_EQ(WHAL_BITMASK(4), 0xFul);
}

static void test_bitmask_8(void)
{
    WHAL_ASSERT_EQ(WHAL_BITMASK(8), 0xFFul);
}

static void test_bitmask_1(void)
{
    WHAL_ASSERT_EQ(WHAL_BITMASK(1), 1ul);
}

static void test_setbits_low(void)
{
    size_t msk = WHAL_BITMASK(4);
    size_t pos = 0;
    WHAL_ASSERT_EQ(whal_SetBits(msk, pos, 0xA), 0xAul);
}

static void test_setbits_shifted(void)
{
    size_t msk = (WHAL_BITMASK(4) << 4);
    size_t pos = 4;
    WHAL_ASSERT_EQ(whal_SetBits(msk, pos, 0x5), 0x50ul);
}

static void test_getbits_low(void)
{
    size_t msk = WHAL_BITMASK(4);
    size_t pos = 0;
    WHAL_ASSERT_EQ(whal_GetBits(msk, pos, 0xABul), 0xBul);
}

static void test_getbits_shifted(void)
{
    size_t msk = (WHAL_BITMASK(4) << 4);
    size_t pos = 4;
    WHAL_ASSERT_EQ(whal_GetBits(msk, pos, 0xABul), 0xAul);
}

static void test_setbits_getbits_roundtrip(void)
{
    size_t msk = (WHAL_BITMASK(7) << 8);
    size_t pos = 8;
    size_t val = 42;
    size_t encoded = whal_SetBits(msk, pos, val);
    WHAL_ASSERT_EQ(whal_GetBits(msk, pos, encoded), val);
}

static void test_setbits_single_bit(void)
{
    size_t msk = (1UL << 24);
    size_t pos = 24;
    WHAL_ASSERT_EQ(whal_SetBits(msk, pos, 1), (1UL << 24));
    WHAL_ASSERT_EQ(whal_SetBits(msk, pos, 0), 0ul);
}

void test_bitops(void)
{
    WHAL_TEST_SUITE_START("bitops");
    WHAL_TEST(test_bitmask_4);
    WHAL_TEST(test_bitmask_8);
    WHAL_TEST(test_bitmask_1);
    WHAL_TEST(test_setbits_low);
    WHAL_TEST(test_setbits_shifted);
    WHAL_TEST(test_getbits_low);
    WHAL_TEST(test_getbits_shifted);
    WHAL_TEST(test_setbits_getbits_roundtrip);
    WHAL_TEST(test_setbits_single_bit);
    WHAL_TEST_SUITE_END();
}
