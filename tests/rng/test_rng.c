#include <stdint.h>
#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Rng_GenerateNonzero(void)
{
    uint8_t buf[32] = {0};
    int allZero = 1;

    WHAL_ASSERT_EQ(whal_Rng_Generate(&g_whalRng, buf, sizeof(buf)), WHAL_SUCCESS);

    for (size_t i = 0; i < sizeof(buf); i++) {
        if (buf[i] != 0) {
            allZero = 0;
            break;
        }
    }

    WHAL_ASSERT_EQ(allZero, 0);
}

static void Test_Rng_GenerateUnique(void)
{
    uint8_t buf1[16] = {0};
    uint8_t buf2[16] = {0};
    int same = 1;

    WHAL_ASSERT_EQ(whal_Rng_Generate(&g_whalRng, buf1, sizeof(buf1)), WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Rng_Generate(&g_whalRng, buf2, sizeof(buf2)), WHAL_SUCCESS);

    for (size_t i = 0; i < sizeof(buf1); i++) {
        if (buf1[i] != buf2[i]) {
            same = 0;
            break;
        }
    }

    /* Two consecutive 16-byte outputs should differ */
    WHAL_ASSERT_EQ(same, 0);
}

void whal_Test_Rng(void)
{
    WHAL_TEST_SUITE_START("rng");
    WHAL_TEST(Test_Rng_GenerateNonzero);
    WHAL_TEST(Test_Rng_GenerateUnique);
    WHAL_TEST_SUITE_END();
}
