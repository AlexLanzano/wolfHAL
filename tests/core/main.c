#include <stdio.h>
#include "../test.h"

int g_whalTestPassed;
int g_whalTestFailed;
int g_whalTestCurFailed;

void whal_Test_Puts(const char *s)
{
    fputs(s, stdout);
}

void whal_Test_Bitops(void);
void whal_Test_Dispatch(void);

int main(void)
{
    g_whalTestPassed = 0;
    g_whalTestFailed = 0;

    whal_Test_Bitops();
    whal_Test_Dispatch();

    WHAL_TEST_SUMMARY();

    return g_whalTestFailed ? 1 : 0;
}
