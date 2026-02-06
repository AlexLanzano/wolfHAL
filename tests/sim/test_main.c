#include <stdio.h>
#include "../test.h"

int g_whalTestPassed;
int g_whalTestFailed;
int g_whalTestCurFailed;

void whalTest_Puts(const char *s)
{
    fputs(s, stdout);
}

void test_bitops(void);
void test_dispatch(void);

int main(void)
{
    g_whalTestPassed = 0;
    g_whalTestFailed = 0;

    test_bitops();
    test_dispatch();

    WHAL_TEST_SUMMARY();

    return g_whalTestFailed ? 1 : 0;
}
