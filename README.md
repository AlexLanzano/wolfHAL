# wolfHAL

wolfHAL is a lightweight hardware abstraction layer for embedded targets written
in C. It provides a uniform driver model based on vtable dispatch, register
helpers for safe read-modify-write access, and reference drivers for multiple
platforms.

## Supported platforms

| Platform | Board | Drivers |
|----------|-------|---------|
| STM32WB55 | Nucleo | RCC (PLL + MSI), GPIO, UART, LPUART, SPI, Flash, RNG, SysTick |
| PIC32CZ | Curiosity Ultra | Clock (dual PLL), GPIO, UART, Flash, Supply (SUPC), SysTick |

## Architecture

Every peripheral is represented as a device struct containing a register map, a
driver vtable, and a platform-specific config:

```c
typedef struct {
    const whal_Regmap regmap;       /* base address + size */
    const whal_GpioDriver *driver;  /* function pointers  */
    void *cfg;                      /* platform config     */
} whal_Gpio;
```

Public API functions validate the device, driver, and vtable entry before
dispatching. When `WHAL_CFG_DIRECT_CALLBACKS` is defined, dispatch compiles down to
direct calls with zero overhead.

Board configs instantiate devices using platform macros:

```c
whal_Gpio g_whalGpio = {
    WHAL_STM32WB55_GPIO_DEVICE,
    .cfg = &gpioConfig,
};
```

## Modules

| Module | Header | Operations |
|--------|--------|------------|
| Clock  | `wolfHAL/clock/clock.h` | Init, Deinit, Enable, Disable, GetRate |
| GPIO   | `wolfHAL/gpio/gpio.h` | Init, Deinit, Get, Set |
| UART   | `wolfHAL/uart/uart.h` | Init, Deinit, Send, Recv |
| Flash  | `wolfHAL/flash/flash.h` | Init, Deinit, Lock, Unlock, Read, Write, Erase |
| SPI    | `wolfHAL/spi/spi.h` | Init, Deinit, SendRecv, Send, Recv |
| Timer  | `wolfHAL/timer/timer.h` | Init, Deinit, Start, Stop, Reset |
| RNG    | `wolfHAL/rng/rng.h` | Init, Deinit, Generate |
| Supply | `wolfHAL/supply/supply.h` | Init, Deinit, Enable, Disable |

Utilities: `wolfHAL/regmap.h` (masked register access), `wolfHAL/bitops.h`
(bit manipulation), `wolfHAL/error.h` (error codes).

## Repository layout

```
wolfHAL/                    Public headers (API surface)
  platform/arm/             Cortex-M SysTick definitions
  platform/st/              STM32WB55 device macros
  platform/microchip/       PIC32CZ device macros
src/                        Driver implementations (generic + platform)
boards/
  stm32wb55xx_nucleo/       STM32WB55 Nucleo board support
  pic32cz_curiosity_ultra/  PIC32CZ Curiosity Ultra board support
examples/
  blinky/                   LED blink + UART echo (multi-board)
tests/
  test.h                    Minimal test framework (no libc dependency)
  core/                     Host-compiled tests (bitops, dispatch validation)
  clock/ gpio/ flash/ ...   On-target per-module tests
```

## Getting started

Add the repository root to your include path (`-I/path/to/wolfHAL`) and compile
the sources you need:

```
# Generic dispatch (include all, or just the modules you use)
src/clock/clock.c  src/gpio/gpio.c  src/uart/uart.c
src/flash/flash.c  src/spi/spi.c    src/timer/timer.c
src/supply/supply.c  src/rng/rng.c

# Platform drivers (pick your target)
src/clock/stm32wb_rcc.c  src/gpio/stm32wb_gpio.c  ...
src/clock/pic32cz_clock.c  src/gpio/pic32cz_gpio.c  ...
src/timer/systick.c
```

Create a board config file that instantiates devices with your pin assignments,
clock settings, and peripheral configs. See `boards/stm32wb55xx_nucleo/board.c`
or `boards/pic32cz_curiosity_ultra/board.c` for reference.

To write a driver for a new platform, implement the functions in the relevant
`*Driver` vtable and provide device macros in a platform header.

## Building the examples

Examples use `arm-none-eabi-gcc` and produce a `.bin` suitable for flashing.
Select a board with `BOARD=`:

```sh
cd examples/blinky
make BOARD=stm32wb55xx_nucleo       # -> build/stm32wb55xx_nucleo/blinky.bin
make BOARD=pic32cz_curiosity_ultra  # -> build/pic32cz_curiosity_ultra/blinky.bin
```

## Tests

**Core tests** run on the host and validate the abstraction layer without any
hardware:

```sh
cd tests/core && make run
```

**Hardware tests** cross-compile for a target board. They boot the board, report
results over UART, and signal pass/fail via LED:

```sh
cd tests
make BOARD=stm32wb55xx_nucleo       # -> build/stm32wb55xx_nucleo/test_hw.bin
make BOARD=pic32cz_curiosity_ultra  # -> build/pic32cz_curiosity_ultra/test_hw.bin
```

Each board's `Makefile.inc` defines a default `TESTS` list (e.g. `clock gpio
flash timer rng`). Override it on the command line to run a subset.

## CI

GitHub Actions runs on every push and PR to `main`:
- **core-tests** -- builds and runs the host test suite
- **cross-compile** -- verifies all examples and hardware tests compile cleanly
  with `arm-none-eabi-gcc` for every supported board

## Documentation

Headers are annotated for Doxygen. Generate HTML docs with:

```sh
doxygen Doxyfile
```

## License

GPLv3 -- see [LICENSE](LICENSE) for details.
