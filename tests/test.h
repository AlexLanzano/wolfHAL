#ifndef WHAL_TEST_H
#define WHAL_TEST_H

#include <stdarg.h>
#include <stddef.h>

/*
 * Minimal test framework for wolfHAL.
 *
 * Provides test runner macros, assertions, and pass/fail tracking.
 * The print backend (whalTest_Puts) is implemented per-harness:
 *   - sim: libc puts
 *   - hw:  UART send
 */

/* Provided by each harness */
void whalTest_Puts(const char *s);

/* Convert an integer to decimal in a static buffer (no libc dependency) */
static inline const char *whalTest_Itoa(int val)
{
    static char buf[16];
    char *p = buf + sizeof(buf) - 1;
    int neg = 0;

    *p = '\0';
    if (val < 0) {
        neg = 1;
        val = -val;
    }

    do {
        *--p = '0' + (val % 10);
        val /= 10;
    } while (val);

    if (neg)
        *--p = '-';

    return p;
}

/* Minimal printf: supports %d, %s, and %%. Calls whalTest_Puts once. */
static inline void whalTest_Printf(const char *fmt, ...)
{
    static char buf[128];
    char *out = buf;
    char *end = buf + sizeof(buf) - 1;
    va_list ap;

    va_start(ap, fmt);
    while (*fmt && out < end) {
        if (*fmt != '%') {
            *out++ = *fmt++;
            continue;
        }
        fmt++;
        if (*fmt == 'd') {
            const char *s = whalTest_Itoa(va_arg(ap, int));
            while (*s && out < end)
                *out++ = *s++;
        }
        else if (*fmt == 's') {
            const char *s = va_arg(ap, const char *);
            while (*s && out < end)
                *out++ = *s++;
        }
        else if (*fmt == '%') {
            *out++ = '%';
        }
        fmt++;
    }
    va_end(ap);

    *out = '\0';
    whalTest_Puts(buf);
}

extern int g_whalTestPassed;
extern int g_whalTestFailed;
extern int g_whalTestCurFailed;

#define WHAL_TEST_SUITE_START(name)                  \
    do {                                        \
        whalTest_Printf("\n=== " name " ===\n"); \
    } while (0)

#define WHAL_TEST_SUITE_END() \
    do { } while (0)

#define WHAL_TEST(fn)                            \
    do {                                    \
        g_whalTestCurFailed = 0;               \
        fn();                               \
        if (g_whalTestCurFailed) {              \
            whalTest_Printf(#fn ": FAIL\n");    \
            g_whalTestFailed++;                 \
        }                                   \
        else {                              \
            whalTest_Printf(#fn ": PASS\n");    \
            g_whalTestPassed++;                 \
        }                                   \
    } while (0)

#define WHAL_ASSERT_EQ(a, b)                                         \
    do {                                                        \
        if ((a) != (b)) {                                       \
            whalTest_Printf("  ASSERT_EQ failed at line %d\n", __LINE__); \
            whalTest_Printf("  got: %d, expected: %d\n",              \
                            (int)(a), (int)(b));                \
            g_whalTestCurFailed = 1;                                \
            return;                                             \
        }                                                       \
    } while (0)

#define WHAL_ASSERT_NEQ(a, b)                                        \
    do {                                                        \
        if ((a) == (b)) {                                       \
            whalTest_Printf("  ASSERT_NEQ failed at line %d\n", __LINE__);\
            g_whalTestCurFailed = 1;                                \
            return;                                             \
        }                                                       \
    } while (0)

#define WHAL_ASSERT_MEM_EQ(a, b, len)                                \
    do {                                                        \
        const unsigned char *_a = (const unsigned char *)(a);   \
        const unsigned char *_b = (const unsigned char *)(b);   \
        for (size_t _i = 0; _i < (len); _i++) {                \
            if (_a[_i] != _b[_i]) {                             \
                whalTest_Printf("  ASSERT_MEM_EQ failed at line %d, " \
                                "byte offset: %d\n", __LINE__, (int)_i); \
                g_whalTestCurFailed = 1;                            \
                return;                                         \
            }                                                   \
        }                                                       \
    } while (0)

#define WHAL_TEST_SUMMARY()                                  \
    do {                                                \
        whalTest_Printf("\n");                                  \
        whalTest_Printf("--- Results ---\n");                   \
        whalTest_Printf("Passed: %d\n", g_whalTestPassed);     \
        whalTest_Printf("Failed: %d\n", g_whalTestFailed);     \
    } while (0)

#endif /* WHAL_TEST_H */
