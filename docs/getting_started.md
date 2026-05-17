# Getting Started

This guide walks through integrating wolfHAL into a bare-metal embedded project.

## Project Layout

A typical project using wolfHAL looks like this:

```
my_project/
  wolfHAL/                          wolfHAL repository (submodule, copy, etc.)
  boards/
    <board_name>/
      board.h                       Peripheral externs and board constants
      board.c                       Device instances, configuration, and Board_Init
      ivt.c                         Interrupt vector table and Reset_Handler
      linker.ld                     Linker script for your MCU
      board.mk                      Toolchain and source list
  src/
    main.c                          Application entry point
    ...                             Additional application sources
  Makefile
```

The key idea is that your project provides the board-level glue (device
instances, pin assignments, clock config, startup code) and wolfHAL provides
the driver implementations and API.

## Adding wolfHAL to Your Project

wolfHAL is a source-level library with no external dependencies beyond a C
compiler and standard headers (`stdint.h`, `stddef.h`). To use it:

1. Add the wolfHAL repository root to your include path (e.g., `-I/path/to/wolfHAL`)
2. Compile the generic dispatch sources for the modules you need:

```
src/gpio/gpio.c
src/uart/uart.c
src/flash/flash.c
...
```

3. Compile the platform-specific driver sources for your target:

```
src/gpio/<platform>_gpio.c
src/uart/<platform>_uart.c
src/flash/<platform>_flash.c
...
```

You only need to include the modules and platform drivers your project actually
uses.

Clock drivers are header-only — every `whal_<Chip>_<Subsys>_*` helper in
`wolfHAL/clock/<platform>_*.h` is a `static inline` function, so there is no
matching `.c` file to compile.

## The Device Model

wolfHAL has three driver categories — platform, peripheral, and board-level
(see `docs/writing_a_driver.md`). Their device structs have slightly
different shapes.

### Platform drivers

Platform drivers operate directly on SoC registers. The device struct has
three fields:

```c
struct whal_Gpio {
    const size_t base;               /* peripheral base address */
    const whal_GpioDriver *driver;   /* vtable of function pointers */
    const void *cfg;                 /* platform-specific configuration */
};
```

- **base** — the peripheral's memory-mapped register base address
- **driver** — points to the driver implementation (the vtable)
- **cfg** — points to a driver-specific configuration struct that the
  driver reads during Init

Platform headers provide `_BASE` and `_DRIVER` macros for each peripheral,
so you only need to fill in the `cfg`:

```c
#include <wolfHAL/platform/st/stm32wb55xx.h>

/* In board.c (vtable-dispatched driver): */
whal_Gpio g_whalGpio = {
    .base = WHAL_STM32WB55_GPIO_BASE,
    .driver = WHAL_STM32WB55_GPIO_DRIVER,
    .cfg = &gpioConfig,
};
```

When direct API mapping is active for a device type, the `.driver` field is
omitted since calls go directly to the driver implementation. When the driver
is wired as single-instance, the device is defined in the driver's `.c` file
from an initializer macro the board supplies — see the next section.

#### Single-instance platform drivers

Some platform drivers are wired as **single-instance**: rather than reading
`.base` and `.cfg` from a handle the caller passes in, the driver reads
directly from a named `whal_<Plat>_<X>_Dev` singleton. The device handle
parameter becomes a formal-only argument — the board passes
`WHAL_INTERNAL_DEV` (defined as `((void *)0)`) to make that intent explicit.

There are two flavors:

- **Unconditional** — the driver always reads from its singleton. Used
  for driver types where every supported chip exposes a single instance,
  so the pointer-based path would never get exercised.
- **Conditional** — gated on `WHAL_CFG_<PLAT>_<DRV>_SINGLE_INSTANCE`.
  Used for driver types where chips commonly have more than one instance
  (UART, SPI, I2C, DMA, etc.). The default build keeps the pointer-based
  path; boards opt in per driver via the macro when they only use one
  instance.

The driver header `extern`-declares the singleton; the driver `.c` defines
it from a `WHAL_CFG_<PLAT>_<X>_DEV` initializer macro the board provides in
`board.h`:

```c
/* wolfHAL/rng/stm32wb_rng.h */
extern const whal_Rng whal_Stm32wb_Rng_Dev;

/* src/rng/stm32wb_rng.c */
#include "board.h"
const whal_Rng whal_Stm32wb_Rng_Dev = WHAL_CFG_STM32WB_RNG_DEV;

/* boards/<board>/board.h */
#define WHAL_CFG_STM32WB_RNG_DEV { \
    .base = WHAL_STM32WB55_RNG_BASE, \
    .cfg  = (void *)&(const whal_Stm32wb_Rng_Cfg){ .timeout = &g_whalTimeout }, \
}
```

Aliased drivers (where one chip's driver re-exports another's) reuse the
upstream singleton's extern declaration through a `#define` in the alias
header — see `wolfHAL/watchdog/stm32n6_iwdg.h` for an example.

### Peripheral drivers

Peripheral drivers talk to external chips over a bus (SPI, I2C, MDIO). The
device struct shape mirrors platform drivers — same vtable + cfg pattern —
but instead of a register block the device carries the bus handle and any
chip-specific addressing inside its cfg:

```c
whal_Flash g_whalSpiFlash = {
    .driver = WHAL_SPI_NOR_W25Q64_FLASH_DRIVER,
    .cfg = &(whal_SpiNor_W25q64_Cfg) {
        .spi = BOARD_SPI_DEV,        /* underlying bus driver, per board.h */
        .csPin = SPI_FLASH_CS_PIN,
        /* ...chip-specific fields... */
    },
};
```

The application calls the same generic API (`whal_Flash_Read(&g_whalSpiFlash, ...)`)
whether the flash is on-chip (platform driver) or external SPI NOR (peripheral
driver) — the device pointer determines which implementation runs.

### Board-level drivers

Board-level drivers (clock, power) only expose chip-specific helpers — no
vtable, no generic `whal_<Type>_*` API, no device struct. The driver header
owns the chip's `_BASE` macro at the top and the helpers take no device
pointer:

```c
/* wolfHAL/clock/stm32wb_rcc.h (excerpt) */
#define WHAL_STM32WB_RCC_BASE 0x58000000

static inline whal_Error whal_Stm32wb_Rcc_EnableMsi(
    whal_Stm32wb_Rcc_MsiRange range);
```

Boards bring up the clock tree imperatively in `Board_Init` by calling these
helpers in the required order — `whal_Stm32wb_Rcc_EnableOsc()`,
`EnablePll()`, `SetSysClock()`, etc. Applications that need to trigger
board-level behavior at runtime (e.g., enter low-power mode) call a
board-provided wrapper such as `Board_Sleep()` rather than a generic
`whal_X` function.

## Configuring Devices

Each platform driver defines its own configuration struct with the parameters
it needs. For example, a GPIO driver takes a pin configuration table; the
platform usually provides a `WHAL_<PLATFORM>_GPIO_PIN(...)` macro to populate
each entry compactly:

For GPIO (a single-instance driver — see above), the singleton is defined
in the driver `.c` from a `WHAL_CFG_<PLAT>_<X>_DEV` initializer in `board.h`:

```c
/* board.h */
#define WHAL_CFG_STM32WB_GPIO_DEV { \
    .base = WHAL_STM32WB55_GPIO_BASE, \
    /* .driver: direct API mapping */ \
    .cfg = (void *)&(const whal_Stm32wb_Gpio_Cfg){ \
        .pinCfg = (const whal_Stm32wb_Gpio_PinCfg[PIN_COUNT]){ \
            [LED_PIN] = WHAL_STM32WB_GPIO_PIN( \
                WHAL_STM32WB_GPIO_PORT_B, 5, WHAL_STM32WB_GPIO_MODE_OUT, \
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_LOW, \
                WHAL_STM32WB_GPIO_PULL_UP, 0), \
        }, \
        .pinCount = PIN_COUNT, \
    }, \
}
```

A UART driver typically takes a pre-computed baud rate register value and a
timeout. For boards that opt the UART into single-instance, the device
goes alongside GPIO in `board.h`; otherwise it stays in `board.c` as a
regular `g_whal<X>` global:

```c
whal_Uart g_whalUart = {
    .base = WHAL_STM32WB55_UART1_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32wb_Uart_Cfg) {
        .brr = WHAL_STM32WB_UART_BRR(64000000, 115200),
        .timeout = &g_whalTimeout,
    },
};
```

The `.driver: direct API mapping` comment indicates this peripheral is using
the optimization path described above — calls to `whal_Uart_*` link directly
to the chip-specific implementation, no vtable dispatch.

See the platform-specific headers in `wolfHAL/<device_type>/` for the full set
of configuration options for each driver, and the example boards in `boards/`
for full instantiations.

## Initialization

The board is responsible for initializing peripherals in dependency order.
Drivers do not enable their own clocks or power supplies — the board must
handle these prerequisites explicitly before calling a driver's Init.

A typical initialization sequence:

1. Do any pre-clock-controller initialization (e.g., flash wait states,
   power supplies)
2. Bring up the clock tree (oscillators, optional PLL, sysclk source)
3. Enable peripheral clocks
4. Initialize peripheral drivers
5. Start timers

The chip's clock driver exposes `Enable*`/`Disable*`/`Set*` helpers that
the board calls in order. There is no generic `whal_Clock_Init` walker —
clock-tree shape varies too much across vendors to abstract.

```c
static const whal_Myplatform_Clock_PeriphClk g_periphClks[] = {
    {MY_PLATFORM_GPIOB_CLOCK},
    {MY_PLATFORM_UART1_CLOCK},
};
#define PERIPH_CLK_COUNT \
    (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Bring up clocks (chip-specific helpers, called in order — no device
     * pointer; each helper reads the chip's fixed clock-controller base
     * address from its own header). */
    err = whal_Myplatform_Clock_EnableOsc(
        &(whal_Myplatform_Clock_OscCfg){WHAL_MYPLATFORM_CLOCK_OSC0_CFG});
    if (err)
        return err;
    err = whal_Myplatform_Clock_SetSysClock(
        WHAL_MYPLATFORM_CLOCK_SYSCLK_SRC_OSC0);
    if (err)
        return err;

    /* Enable peripheral clocks */
    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Myplatform_Clock_EnablePeriphClk(&g_periphClks[i]);
        if (err)
            return err;
    }

    /* Initialize peripherals through the board's BOARD_<X>_DEV macros */
    err = whal_Gpio_Init(BOARD_GPIO_DEV);
    if (err)
        return err;

    err = whal_Uart_Init(BOARD_UART_DEV);
    if (err)
        return err;

    err = whal_Timer_Init(BOARD_TIMER_DEV);
    if (err)
        return err;

    err = whal_Timer_Start(BOARD_TIMER_DEV);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
```

See the board examples in `boards/` for complete initialization sequences
including platform-specific steps.

## Using the API

After initialization, use the wolfHAL API to interact with peripherals.
Reach each device through the `BOARD_<PERIPH>_DEV` macro that `board.h`
provides — the board picks whether that resolves to `WHAL_INTERNAL_DEV` (for
single-instance drivers) or `&g_whal<X>` (for vtable-dispatched drivers),
so the application source stays portable.

```c
#include <wolfHAL/wolfHAL.h>
#include "board.h"

void main(void)
{
    if (Board_Init() != WHAL_SUCCESS)
        while (1);

    while (1) {
        whal_Gpio_Set(BOARD_GPIO_DEV, BOARD_LED_PIN, 1);
        whal_Uart_Send(BOARD_UART_DEV, "Hello!\r\n", 8);
        Board_WaitMs(1000);

        whal_Gpio_Set(BOARD_GPIO_DEV, BOARD_LED_PIN, 0);
        Board_WaitMs(1000);
    }
}
```

All API functions return `whal_Error`. Check for `WHAL_SUCCESS` to confirm the
operation completed. The error codes are:

| Code | Meaning |
|------|---------|
| `WHAL_SUCCESS` | Operation completed successfully |
| `WHAL_EINVAL` | Invalid argument (null device pointer, null data pointer) |
| `WHAL_ENOTREADY` | Resource is busy or not yet available |
| `WHAL_EHARDWARE` | Hardware error (e.g., RNG entropy failure) |
| `WHAL_ETIMEOUT` | Operation timed out waiting for hardware |
| `WHAL_ENOTSUP` | Operation or parameter not supported by this driver/hardware |

## Optimizing for Size

wolfHAL gives you several ways to reduce code size depending on how much
control you want.

### Direct API Mapping

Each driver source (platform or peripheral) provides an `#ifdef` block that
renames its driver functions to the top-level API names. When the
corresponding `WHAL_CFG_<TYPE>_API_MAPPING_<VARIANT>` flag is defined, the
driver file itself provides the definition of the top-level API — no
wrapper, no vtable indirection, no runtime null-check.

For example, `-DWHAL_CFG_UART_API_MAPPING_STM32WB` causes
`src/uart/stm32wb_uart.c` to emit external symbols named `whal_Uart_Init`,
`whal_Uart_Deinit`, `whal_Uart_Send`, and `whal_Uart_Recv`, each bound to
the polled STM32WB UART driver body. Application code calling
`whal_Uart_Send(&dev, buf, sz)` links directly to the driver.

The same pattern works for peripheral drivers: enabling
`WHAL_CFG_SDHC_SPI_BLOCK_DIRECT_API_MAPPING`, for instance, makes the
external SD-card-over-SPI driver provide the top-level `whal_Block_*` API
symbols directly.

Direct API mapping is only safe when a single driver of that type is
present in the build. Boards that combine multiple drivers of the same type
(e.g., on-chip flash + external SPI NOR flash) cannot enable mapping for
that type — they keep the vtable dispatch so both drivers can be linked
simultaneously.

**The dispatch source `src/<type>/<type>.c` must not be compiled when the
corresponding mapping flag is active.** Both the dispatch source and the
mapped driver source provide the same top-level symbols, which would cause
a multiple-definition link error. Exclude the dispatch source from the
board's source list.

Only one mapping flag may be active per device type per build.

### Custom Vtables

The platform drivers provide a pre-built vtable with all operations populated.
For vtable-dispatched devices, if you only use a subset of a driver's
functionality, you can define your own vtable that only includes the functions
you need:

```c
static const whal_SpiDriver mySpiDriver = {
    .Init     = whal_Stm32wb_Spi_Init,
    .Deinit   = whal_Stm32wb_Spi_Deinit,
    .StartCom = whal_Stm32wb_Spi_StartCom,
    .EndCom   = whal_Stm32wb_Spi_EndCom,
    .SendRecv = whal_Stm32wb_Spi_SendRecv,
};

whal_Spi g_whalSpi = {
    .base = WHAL_STM32WB55_SPI1_BASE,
    .driver = &mySpiDriver,
    .cfg = &spiConfig,
};
```

With link-time optimization (`-flto`) or garbage collection (`-ffunction-sections`
+ `-Wl,--gc-sections`), any driver functions not referenced through the vtable
will be stripped from the final binary.

Single-instance drivers (see above) are not vtable-dispatched in their
single-instance form — the driver body reads its `.base` and `.cfg` straight
out of the named singleton its `.c` defines from `WHAL_CFG_<PLAT>_<X>_DEV`
in `board.h`. The "custom vtable" knob does not apply; any unreferenced
entry point is dropped by `--gc-sections`.

### Calling Driver Functions Directly

For maximum control on a vtable-dispatched driver, you can skip the vtable
and call the underlying platform driver functions directly:

```c
#include <wolfHAL/uart/stm32wb_uart.h>

whal_Stm32wb_Uart_Init(&g_whalUart);
whal_Stm32wb_Uart_Send(&g_whalUart, "hi\r\n", 4);
```

This eliminates the vtable indirection and lets the compiler inline or
optimize the calls more aggressively.

Single-instance drivers fall into two cases depending on whether direct API
mapping is also active for them:

- **Single-instance + direct API mapping** (the common case for RNG,
  watchdog, NVIC, etc.). The chip-specific functions are renamed to the
  generic API names, and the driver body reads from the singleton (which
  the driver `.c` defines from the `WHAL_CFG_<PLAT>_<X>_DEV` initializer
  in `board.h`). Call sites pass `WHAL_INTERNAL_DEV`:

  ```c
  whal_Stm32wb_Gpio_Set(WHAL_INTERNAL_DEV, BOARD_LED_PIN, 1);
  ```

- **Single-instance without direct API mapping** (used when several
  drivers of the same type must coexist on one board — for example
  on-chip flash plus an external SPI-NOR flash). The chip's `_Dev`
  singleton (defined in the driver `.c` from the `WHAL_CFG_<PLAT>_<X>_DEV`
  initializer) carries `.driver`, `.base`, and `.cfg`, so generic API
  calls vtable-dispatch through it. Call sites pass the singleton's
  address (cast away the const because the generic API takes a non-const
  pointer):

  ```c
  whal_Stm32wb_Flash_Read((whal_Flash *)&whal_Stm32wb_Flash_Dev,
                          addr, buf, sz);
  ```

The `BOARD_<PERIPH>_DEV` macros described above abstract this distinction
away from application code — boards point each macro at whichever shape
they wired.

Register-level drivers do not call other drivers internally, so this works
without any caveats. Peripheral drivers (e.g., SPI flash) still call their
bus driver through the vtable.

## Next Steps

- See `boards/` for complete board configuration examples
- See [Writing a Driver](writing_a_driver.md) for how to add support for a new
  platform
- See [Adding a Board](adding_a_board.md) for how to create a board
  configuration for your hardware
