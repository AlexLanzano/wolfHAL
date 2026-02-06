#include <wolfHAL/bitops.h>
#include "../test.h"

static void test_mask_bit0(void)
{
    WHAL_ASSERT_EQ(WHAL_MASK(0), 1ul);
}

static void test_mask_bit15(void)
{
    WHAL_ASSERT_EQ(WHAL_MASK(15), (1ul << 15));
}

static void test_mask_bit31(void)
{
    WHAL_ASSERT_EQ(WHAL_MASK(31), (1ul << 31));
}

static void test_mask_range_single_bit(void)
{
    WHAL_ASSERT_EQ(WHAL_MASK_RANGE(0, 0), 1ul);
    WHAL_ASSERT_EQ(WHAL_MASK_RANGE(7, 7), (1ul << 7));
}

static void test_mask_range_multi_bit(void)
{
    WHAL_ASSERT_EQ(WHAL_MASK_RANGE(3, 0), 0xFul);
    WHAL_ASSERT_EQ(WHAL_MASK_RANGE(7, 4), 0xF0ul);
    WHAL_ASSERT_EQ(WHAL_MASK_RANGE(15, 8), 0xFF00ul);
}

static void test_mask_range_full_byte(void)
{
    WHAL_ASSERT_EQ(WHAL_MASK_RANGE(7, 0), 0xFFul);
}

static void test_setbits_low(void)
{
    size_t mask = WHAL_MASK_RANGE(3, 0);
    WHAL_ASSERT_EQ(whal_SetBits(mask, 0xA), 0xAul);
}

static void test_setbits_shifted(void)
{
    size_t mask = WHAL_MASK_RANGE(7, 4);
    WHAL_ASSERT_EQ(whal_SetBits(mask, 0x5), 0x50ul);
}

static void test_getbits_low(void)
{
    size_t mask = WHAL_MASK_RANGE(3, 0);
    WHAL_ASSERT_EQ(whal_GetBits(mask, 0xABul), 0xBul);
}

static void test_getbits_shifted(void)
{
    size_t mask = WHAL_MASK_RANGE(7, 4);
    WHAL_ASSERT_EQ(whal_GetBits(mask, 0xABul), 0xAul);
}

static void test_setbits_getbits_roundtrip(void)
{
    size_t mask = WHAL_MASK_RANGE(14, 8);
    size_t val = 42;
    size_t encoded = whal_SetBits(mask, val);
    WHAL_ASSERT_EQ(whal_GetBits(mask, encoded), val);
}

static void test_setbits_single_bit(void)
{
    size_t mask = WHAL_MASK(24);
    WHAL_ASSERT_EQ(whal_SetBits(mask, 1), (1ul << 24));
    WHAL_ASSERT_EQ(whal_SetBits(mask, 0), 0ul);
}

void test_bitops(void)
{
    WHAL_TEST_SUITE_START("bitops");
    WHAL_TEST(test_mask_bit0);
    WHAL_TEST(test_mask_bit15);
    WHAL_TEST(test_mask_bit31);
    WHAL_TEST(test_mask_range_single_bit);
    WHAL_TEST(test_mask_range_multi_bit);
    WHAL_TEST(test_mask_range_full_byte);
    WHAL_TEST(test_setbits_low);
    WHAL_TEST(test_setbits_shifted);
    WHAL_TEST(test_getbits_low);
    WHAL_TEST(test_getbits_shifted);
    WHAL_TEST(test_setbits_getbits_roundtrip);
    WHAL_TEST(test_setbits_single_bit);
    WHAL_TEST_SUITE_END();
}
