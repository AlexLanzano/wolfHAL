# wolfHAL Hardware Test Suite

## Building

```
make BOARD=<board>
```

Each board defines its supported tests in `boards/<board>/Makefile.inc`.

## Selecting Tests

Override the `TESTS` variable to build a subset of tests:

```
make BOARD=stm32wb55xx_nucleo TESTS=gpio
make BOARD=stm32wb55xx_nucleo TESTS="gpio clock"
```

## Test Structure

Tests are organized by device type:

```
tests/
  main.c          # Test runner entry point
  test.h          # Test macros (WHAL_ASSERT_EQ, WHAL_TEST, etc.)
  Makefile
  boards/
    <board>/
      board.c     # Device instances and Board_Init
      board.h     # Externs and board constants
      Makefile.inc
      ivt.c
      linker.ld
  gpio/
    test_gpio.c          # Generic API tests
    test_stm32wb_gpio.c  # STM32WB platform-specific tests
    test_pic32cz_gpio.c  # PIC32CZ platform-specific tests
  ...
```

Each device directory contains:

- `test_<device>.c` -- Generic tests that exercise the wolfHAL API.
- `test_<platform>_<device>.c` -- Platform-specific tests. These are automatically detected and compiled when building for the matching board.

## Adding a New Board

1. Create `boards/<board_name>/` with `board.c`, `board.h`, `Makefile.inc`, `linker.ld`, and any additional board-specific source files.
2. Set `PLATFORM` and `TESTS` in `Makefile.inc`.
3. Optionally add platform-specific test files as `<device>/test_<platform>_<device>.c`.

## Core Tests

Host-side unit tests (bitops, dispatch) live in `core/` and build with the native compiler:

```
cd core && make && make run
```
