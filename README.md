# wolfHAL
f
wolfHAL is a lightweight hardware abstraction layer for embedded targets written
in C. It provides a uniform driver model based on vtable dispatch, register
helpers for safe read-modify-write access, and reference drivers for multiple
platforms.

## Supported platforms

| Platform | Board | Drivers |
|----------|-------|---------|
| STM32WB55 | Nucleo | RCC (PLL + MSI), GPIO, UART, LPUART, SPI, Flash, SysTick |
| PIC32CZ | Curiosity Ultra | Clock (dual PLL), GPIO, UART, Supply (SUPC), SysTick |

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
dispatching. When `WHAL_CFG_NO_CALLBACKS` is defined, dispatch compiles down to
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
examples/
  stm32wb/                  STM32WB55 Nucleo board bring-up and UART echo
  pic32cz/                  PIC32CZ Curiosity Ultra board bring-up
tests/
  test.h                    Minimal test framework (no libc dependency)
  sim/                      Host-compiled tests (bitops, dispatch validation)
  hw/stm32wb/               On-target tests (clock, GPIO, flash, timer)
```

## Getting started

Add the repository root to your include path (`-I/path/to/wolfHAL`) and compile
the sources you need:

```
# Always needed
src/reg.c

# Generic dispatch (include all, or just the modules you use)
src/clock/clock.c  src/gpio/gpio.c  src/uart/uart.c
src/flash/flash.c  src/spi/spi.c    src/timer/timer.c
src/supply/supply.c

# Platform drivers (pick your target)
src/clock/stm32wb_rcc.c  src/gpio/stm32wb_gpio.c  ...
src/clock/pic32cz_clock.c  src/gpio/pic32cz_gpio.c  ...
src/timer/systick.c
```

Create a board config file that instantiates devices with your pin assignments,
clock settings, and peripheral configs. See `examples/stm32wb/stm32wb55xx_nucleo.c`
or `examples/pic32cz/pic32cz_curiosity_ultra.c` for reference.

To write a driver for a new platform, implement the functions in the relevant
`*Driver` vtable and provide device macros in a platform header.

## Building the examples

Both examples use `arm-none-eabi-gcc` and produce a `.bin` suitable for flashing:

```sh
cd examples/stm32wb && make    # -> boot.bin (Cortex-M4)
cd examples/pic32cz && make    # -> boot.bin (Cortex-M33)
```

## Tests

**Simulation tests** run on the host and validate the abstraction layer without
any hardware:

```sh
cd tests/sim && make run
```

**Hardware tests** cross-compile and run on an STM32WB55 Nucleo. They boot the
board, report results over UART, and signal pass/fail via LED:

```sh
cd tests/stm32wb && make    # -> test_hw.bin
```

## CI

GitHub Actions runs on every push and PR to `main`:
- **sim-tests** -- builds and runs the host test suite
- **cross-compile** -- verifies all examples and hardware tests compile cleanly
  with `arm-none-eabi-gcc`

## Documentation

Headers are annotated for Doxygen. Generate HTML docs with:

```sh
doxygen Doxyfile
```

## License

GPLv3 -- see [LICENSE](LICENSE) for details.
