# Adding a Test

This guide covers how to add hardware tests to the wolfHAL test suite.

## Test Framework

Tests use the macros defined in `tests/test.h`:

- `WHAL_TEST_SUITE_START(name)` — print a suite header
- `WHAL_TEST_SUITE_END()` — end a suite
- `WHAL_TEST(fn)` — run a test function and report PASS/FAIL
- `WHAL_ASSERT_EQ(a, b)` — assert equal, prints line number and values on
  failure, then returns from the test function
- `WHAL_ASSERT_NEQ(a, b)` — assert not equal
- `WHAL_ASSERT_MEM_EQ(a, b, len)` — assert memory regions are equal, reports
  the byte offset of the first mismatch

Test functions take no arguments and return void. On assertion failure the
function returns early and the test is marked as failed.

## Generic Tests

Generic tests exercise the wolfHAL API using only the public interface. They
rely on global peripheral instances and board constants from `board.h` (e.g.,
`g_whalGpio`, `BOARD_LED_PIN`), so they run on any board that supports the
device type without modification.

Generic tests should verify the API contract — that functions return the
expected error codes, that Set followed by Get produces the correct value, that
data written can be read back, etc. They should not include platform-specific
headers or make assumptions about register layouts.

Create `tests/<device>/test_<device>.c`:

```c
#include <wolfHAL/wolfHAL.h>
#include "board.h"
#include "test.h"

static void Test_Foo_BasicOperation(void)
{
    WHAL_ASSERT_EQ(whal_Foo_Init(&g_whalFoo), WHAL_SUCCESS);
}

void whal_Test_Foo(void)
{
    WHAL_TEST_SUITE_START("foo");
    WHAL_TEST(Test_Foo_BasicOperation);
    WHAL_TEST_SUITE_END();
}
```

The entry point function must be named `whal_Test_<Device>` (capitalized device
name). Individual test functions are static and named `Test_<Device>_<Name>`.

## Platform-Specific Tests

Platform-specific tests verify that the driver correctly manipulates hardware
registers. They include platform-specific headers and read registers directly
using `whal_Reg_Get()` to confirm that Init configured the right fields, that
Set wrote to the correct output register, etc.

These tests are automatically detected and compiled when building for the
matching platform. The Makefile looks for files named
`test_<platform>_<device>.c` and compiles them when the board's `PLATFORM`
variable matches the `<platform>` portion of the filename.

Create `tests/<device>/test_<platform>_<device>.c`:

```c
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/<device>/<platform>_<device>.h>
#include <wolfHAL/bitops.h>
#include "board.h"
#include "test.h"

static void Test_Foo_SomeRegister(void)
{
    size_t val = 0;
    whal_Reg_Get(g_whalFoo.regmap.base, REG_OFFSET, MASK, POS, &val);
    WHAL_ASSERT_EQ(val, EXPECTED);
}

void whal_Test_Foo_Platform(void)
{
    WHAL_TEST(Test_Foo_SomeRegister);
}
```

The entry point must be named `whal_Test_<Device>_Platform`. No
`WHAL_TEST_SUITE_START` is needed — platform tests run as a continuation of
the generic suite.

## Hooking into main.c

Add your test to `tests/main.c`:

1. Add a conditional declaration block:

```c
#ifdef WHAL_TEST_ENABLE_FOO
void whal_Test_Foo(void);
#ifdef WHAL_TEST_ENABLE_FOO_PLATFORM
void whal_Test_Foo_Platform(void);
#endif
#endif
```

2. Add the corresponding call block in `main()`:

```c
#ifdef WHAL_TEST_ENABLE_FOO
    whal_Test_Foo();
#ifdef WHAL_TEST_ENABLE_FOO_PLATFORM
    whal_Test_Foo_Platform();
#endif
#endif
```

## Enabling on Boards

Add `foo` to the `TESTS` list in each board's `Makefile.inc` that supports this
device type. The Makefile automatically:

- Defines `-DWHAL_TEST_ENABLE_FOO` so the test compiles in
- Compiles `foo/test_foo.c`
- Detects and compiles `foo/test_<platform>_foo.c` if it exists (and defines
  `-DWHAL_TEST_ENABLE_FOO_PLATFORM`)
