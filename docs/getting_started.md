# Getting Started

This guide walks through integrating wolfHAL into a bare-metal embedded project.

## Project Layout

A typical project using wolfHAL looks like this:

```
my_project/
  wolfHAL/                          wolfHAL repository (submodule, copy, etc.)
  boards/
    <board_name>/
      board.h                       Per-peripheral DEV macros and config initializers
      board.c                       Pointer-based device globals and Board_Init
      ivt.c                         Interrupt vector table and Reset_Handler
      linker.ld                     Linker script for your MCU
      board.mk                      Toolchain, source list, and feature flags
  src/
    main.c                          Application entry point
    ...                             Additional application sources
  Makefile
```

Your project provides the board-level glue (device instances, pin assignments,
clock config, startup code) and wolfHAL provides the driver implementations
and API.

## Adding wolfHAL to Your Project

wolfHAL is a source-level library with no external dependencies beyond a C
compiler and standard headers (`stdint.h`, `stddef.h`). To use it:

1. Add the wolfHAL repository root to your include path (e.g., `-I/path/to/wolfHAL`).
2. Compile the generic dispatch sources for the device types you need
   (`src/gpio/gpio.c`, `src/uart/uart.c`, `src/flash/flash.c`, …), **except**
   for types where you've enabled direct API mapping — see below.
3. Compile the platform-specific driver sources for your target
   (`src/gpio/<platform>_gpio.c`, `src/uart/<platform>_uart.c`, …).

You only need the modules and drivers your project actually uses.

## The Device Model

Most HALs hand you a single fixed device struct per peripheral and a single
way to call into it. wolfHAL doesn't, because the deployments it targets are
too different from each other for one shape to fit. A test build that wants
the smallest possible binary on a chip with one of everything has different
needs than a board that hosts on-chip flash *and* an external SPI NOR flash
behind the same `whal_Flash_*` API. So instead of a single shape, a wolfHAL
device sits at a point in a small configuration space: how its state is
*owned* (single-instance device vs caller-passed pointer), how its API is
*dispatched* (direct symbol vs vtable indirection), and where its storage
*lives* (driver `.c` vs `board.c`). Each axis is independent. A board
picks a point per peripheral, and a `BOARD_<X>_DEV` macro hides the
choice from application code.

The rest of this section unpacks each axis, then shows how the four
useful combinations look on a real board.

### Driver Categories

- **Platform drivers** talk to SoC registers directly (`stm32wb_uart`,
  `stm32wb_gpio`, `stm32wb_aes`).
- **Peripheral drivers** talk to external chips over a bus
  (`spi_nor_w25q64`, `sdhc_spi_block`, `bmi270_sensor`, `lan8742a_eth_phy`).
- **Board-level drivers** only expose chip-specific helpers — no vtable,
  no generic `whal_<Type>_*` API, no device struct (clock, power). These
  drivers are only called within the board.c for platform init/deinit.

Platform and peripheral drivers share the same vtable interface — application
code calls `whal_Flash_Read()` whether the flash is on-chip or SPI NOR.
Board-level drivers are the exception: applications reach them through
`Board_<Operation>()` wrappers in `board.c`, not a generic API.

The three knobs below apply to platform and peripheral drivers. Board-level
drivers don't have them. For details on board-level drivers, see
`docs/writing_a_driver.md`.

### Knob 1: Single-instance vs Pointer-based

**Pointer-based (the default).** The driver reads its `.base` and `.cfg`
from the device handle the caller passes in:

```c
whal_Error whal_Stm32wb_Uart_Send(whal_Uart *dev, ...)
{
    size_t base = dev->base;
    whal_Stm32wb_Uart_Cfg *cfg = (whal_Stm32wb_Uart_Cfg *)dev->cfg;
    /* ... */
}
```

The board declares the device as a `g_whal<X>` global in `board.c`. The
caller passes its address through the API.

**Single-instance.** The driver reads `.base` and `.cfg` from a named
single-instance device it owns. The handle parameter still exists (the
function sits behind a generic vtable signature) but the body ignores it:

```c
extern const whal_Uart whal_Stm32wb_Uart_Dev;   /* in the driver header */

whal_Error whal_Stm32wb_Uart_Send(whal_Uart *dev, ...)
{
    size_t base = whal_Stm32wb_Uart_Dev.base;
    (void)dev;
    /* ... */
}
```

The single-instance device struct is *defined* in the driver `.c`,
initialized from a `WHAL_CFG_<PLAT>_<X>_DEV` macro the board supplies
in `board.h`. The driver `#include`s `board.h` to pull in the
initializer. Callers pass `WHAL_INTERNAL_DEV` (defined as `((void *)0)`)
at the call site to make the intent explicit.

Single-instance drivers come in two flavors:

- **Unconditional single-instance.** Used when every chip in this driver's
  family has exactly one instance — there's no plausible board where the
  pointer path would be exercised. The driver has no `#if` fences: it
  always reads from its single-instance device. Examples: GPIO (one
  register block per chip), on-chip flash, RNG, on-MCU AES, IWDG, WWDG,
  NVIC, SysTick.
- **Conditional single-instance.** Used when the chip generally has
  multiple instances but a given board only wires one. Gated by
  `WHAL_CFG_<PLAT>_<DRV>_SINGLE_INSTANCE`. The driver body is bifurcated
  with `#if defined(...SINGLE_INSTANCE...) / #else`: the single-instance
  branch reads the single-instance device, the `#else` branch is the
  original pointer-based path. Boards opt in per peripheral. Examples:
  UART, SPI, I2C, DMA.

Conditional single-instance is genuinely opt-in: a board with a single
UART can keep the pointer-based path if it prefers (the WB55 board does
this).

### Knob 2: Direct API Mapping vs Vtable Dispatch

**Vtable dispatch (the default).** The generic dispatch source
`src/<type>/<type>.c` provides the top-level `whal_<Type>_*` symbols and
forwards each call through a function pointer in the device's `.driver`
field. The board sets `.driver = WHAL_<PLAT>_<X>_DRIVER` in the device
initializer.

**Direct API mapping** (`WHAL_CFG_<DRIVER>_DIRECT_API_MAPPING`). The
chip-specific functions are renamed to the top-level API names at the
definition site, so `src/<type>/<plat>_<type>.c` itself provides the
`whal_<Type>_*` symbols. No vtable, no indirection. The board omits the
`.driver` field from the device initializer.

```c
/* board.c — vtable dispatch */
whal_Flash g_whalFlash = {
    .base   = WHAL_STM32WB55_FLASH_BASE,
    .driver = WHAL_STM32WB55_FLASH_DRIVER,
    .cfg    = &flashCfg,
};

/* board.c — direct API mapping */
whal_Uart g_whalUart = {
    .base = WHAL_STM32WB55_UART1_BASE,
    /* .driver: direct API mapping */
    .cfg  = &uartCfg,
};
```

Two constraints come with direct API mapping:

1. **Only one driver of that type per build.** Both the dispatch source
   and the mapped driver source provide the same top-level symbols, so
   the dispatch source must be excluded from `board.mk`. And a board
   that needs to host multiple drivers of the same type (e.g. on-chip
   flash *and* SPI NOR flash) can't use mapping for that type — it has
   to keep vtable dispatch so both drivers can link.
2. **Only one mapping flag per type per build.** Two
   `WHAL_CFG_<X>_<TYPE>_DIRECT_API_MAPPING` flags in the same build
   produce duplicate `whal_<Type>_*` definitions and won't link.

Mapping flags go in `board.mk` as `-D` flags, not in headers. See
`boards/stm32wb55xx_nucleo/board.mk` for a real list.

### Knob 3: Where the Device Storage Lives

This knob is tightly coupled to knob 1, but worth naming explicitly:

- **Pointer-based driver →** device is a `g_whal<X>` global in `board.c`.
  The board owns the storage; the driver `.c` never sees it.
- **Single-instance driver →** the *single-instance device struct*
  (storage) lives in the driver `.c`, defined from a
  `WHAL_CFG_<PLAT>_<X>_DEV` initializer macro in `board.h`. The board
  owns the macro's *contents*, the driver owns the device's *storage*.

For the single-instance case, the driver `.c` `#include`s `board.h` at
the top so the preprocessor can see the initializer when it defines the
single-instance device:

```c
/* src/rng/stm32wb_rng.c */
#include "board.h"
const whal_Rng whal_Stm32wb_Rng_Dev = WHAL_CFG_STM32WB_RNG_DEV;
```

If a single-instance driver needs mutable scratch state (e.g. an AES-GCM
state struct), that storage is also `static` in the driver `.c`; the
`WHAL_CFG_*_DEV` macro's `.state` field plumbs its address through. See
`g_stm32wbAesGcmDevState` in `src/crypto/stm32wb_aes.c`.

### `BOARD_<PERIPH>_DEV` Ties It Together

The three knobs are per-peripheral choices, so application code that
hard-codes `&g_whalUart` or `WHAL_INTERNAL_DEV` becomes non-portable as
soon as the board changes one. Each `board.h` defines a `BOARD_<X>_DEV`
macro per peripheral that resolves to whichever shape the board uses:

```c
/* boards/stm32wb55xx_nucleo/board.h */
#define BOARD_GPIO_DEV       WHAL_INTERNAL_DEV
#define BOARD_UART_DEV       (&g_whalUart)
#define BOARD_SPI_DEV        (&g_whalSpi)
#define BOARD_I2C_DEV        (&g_whalI2c)
#define BOARD_FLASH_DEV      ((whal_Flash *)&whal_Stm32wb_Flash_Dev)
#define BOARD_WATCHDOG_DEV   WHAL_INTERNAL_DEV
#define BOARD_RNG_DEV        WHAL_INTERNAL_DEV
#define BOARD_AES_GCM_DEV    WHAL_INTERNAL_DEV
/* ... */
```

Apps call `whal_Rng_Generate(BOARD_RNG_DEV, ...)` regardless of which
shape the board picked. Three shapes show up in practice:

- **`WHAL_INTERNAL_DEV`** — single-instance driver. The driver body
  ignores the pointer, so the sentinel just documents the intent.
- **`(&g_whal<X>)`** — pointer-based driver. The board defines the global
  in `board.c`.
- **`((whal_<Type> *)&whal_<Plat>_<X>_Dev)`** — single-instance driver
  *with* vtable dispatch (knobs 1 and 2 are independent). The
  single-instance device lives in the driver `.c`, but the API call
  still vtable-dispatches because another driver of the same type
  coexists. The cast strips the `const` since the generic API takes a
  non-const pointer.

### Worked Combinations

The four combinations of knobs 1 and 2 all appear on
`boards/stm32wb55xx_nucleo` (with knob 3 falling out of knob 1):

| Peripheral | Knob 1 | Knob 2 | `BOARD_*_DEV` resolves to |
|---|---|---|---|
| GPIO, RNG, IWDG, NVIC, SysTick, AES* | unconditional single-instance | direct mapping | `WHAL_INTERNAL_DEV` |
| On-chip Flash | unconditional single-instance | **vtable** (SPI NOR coexists) | `(whal_Flash *)&whal_Stm32wb_Flash_Dev` |
| UART, SPI, I2C | **pointer** (board didn't opt into conditional single-instance) | direct mapping | `&g_whal<X>` |
| SPI NOR W25Q64 | pointer | vtable (peripheral driver) | `&g_whalSpiNorW25q64` |

\* Including the per-algorithm AES devices (ECB, CBC, CTR, GCM, GMAC, CCM).

The UART row is the most instructive: STM32WB has conditional
single-instance available for UART, and the WB55 board only wires one
UART, but the board chose to keep it pointer-based anyway. That's a
legitimate choice — conditional single-instance is opt-in, not automatic.
The Flash row is the second-most: on-chip Flash is unconditional
single-instance, yet its `BOARD_FLASH_DEV` still uses vtable dispatch
because the peripheral board's SPI NOR driver shares the `whal_Flash_*`
API and both have to be linkable simultaneously.

## Configuring Devices

Each driver defines its own configuration struct. The knob-1 choice
determines where you put the device initializer.

### Single-instance: `WHAL_CFG_<PLAT>_<X>_DEV` in `board.h`

The driver `.c` defines the single-instance device struct from a
`WHAL_CFG_<PLAT>_<X>_DEV` initializer macro that the board supplies in
`board.h`. The board never defines the storage itself — it only writes
the macro. The driver header `extern`-declares the device so the board
(and any code that needs to take its address) can see it.

A typical single-instance initializer for a driver with both `.base` and
`.cfg`:

```c
/* boards/stm32wb55xx_nucleo/board.h */
#define WHAL_CFG_STM32WB_RNG_DEV { \
    .base = WHAL_STM32WB55_RNG_BASE, \
    /* .driver: direct API mapping */ \
    .cfg  = (void *)&(const whal_Stm32wb_Rng_Cfg){ \
        .timeout = &g_whalTimeout, \
    }, \
}
```

A few important shape details:

- The `.cfg` payload is a **compound literal cast to `const` and pointed
  at**. Because the macro expands inside the device's static
  initializer, the compound literal also gets static storage duration —
  it's not on a stack frame.
- The `.driver` field is omitted when direct API mapping is active for
  this type. Leave the `/* .driver: direct API mapping */` comment so
  readers know the omission is intentional, not an oversight.
- If the driver pairs with another driver of the same type (knob 2 says
  vtable), `.driver` *is* present — see the Flash example below.

When the on-chip Flash coexists with an external SPI NOR Flash on the
same board, the single-instance driver still has its device struct in
the driver `.c`, but the initializer keeps `.driver` so `whal_Flash_*`
calls can vtable-dispatch:

```c
/* boards/stm32wb55xx_nucleo/board.h */
#define WHAL_CFG_STM32WB_FLASH_DEV { \
    .driver = WHAL_STM32WB55_FLASH_DRIVER, \
    .base   = WHAL_STM32WB55_FLASH_BASE, \
    .cfg    = (void *)&(const whal_Stm32wb_Flash_Cfg){ \
        .timeout   = &g_whalTimeout, \
        .startAddr = 0x08000000, \
        .size      = 0x80000, \
    }, \
}
```

The GPIO case is the same shape but its `.cfg` is much larger — a pin
table. The board's `enum { LED_PIN, UART_TX_PIN, ... PIN_COUNT }` gives
each pin an index, and the initializer fills the table by index using a
`WHAL_<PLATFORM>_GPIO_PIN(...)` helper macro:

```c
/* boards/stm32wb55xx_nucleo/board.h */
#define WHAL_CFG_STM32WB_GPIO_DEV { \
    .base = WHAL_STM32WB55_GPIO_BASE, \
    .cfg  = (void *)&(const whal_Stm32wb_Gpio_Cfg){ \
        .pinCfg = (const whal_Stm32wb_Gpio_PinCfg[PIN_COUNT]){ \
            [LED_PIN] = WHAL_STM32WB_GPIO_PIN( \
                WHAL_STM32WB_GPIO_PORT_B, 5, WHAL_STM32WB_GPIO_MODE_OUT, \
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_LOW, \
                WHAL_STM32WB_GPIO_PULL_UP, 0), \
            [UART_TX_PIN] = WHAL_STM32WB_GPIO_PIN( \
                WHAL_STM32WB_GPIO_PORT_B, 6, WHAL_STM32WB_GPIO_MODE_ALTFN, \
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST, \
                WHAL_STM32WB_GPIO_PULL_UP, 7), \
            /* ... one entry per pin in the enum ... */ \
        }, \
        .pinCount = PIN_COUNT, \
    }, \
}
```

### Pointer-based: `g_whal<X>` global in `board.c`

For drivers that aren't single-instance (either because the chip
genuinely has many instances and the driver is conditional-single-instance
without the flag set, or because the board chose not to opt in), the
device lives in `board.c` as a regular global:

```c
/* boards/stm32wb55xx_nucleo/board.c — pointer-based UART, direct mapping */
whal_Uart g_whalUart = {
    .base = WHAL_STM32WB55_UART1_BASE,
    /* .driver: direct API mapping */
    .cfg  = &(whal_Stm32wb_Uart_Cfg) {
        .brr     = WHAL_STM32WB_UART_BRR(64000000, 115200),
        .timeout = &g_whalTimeout,
    },
};
```

And `board.h` declares it `extern` and points `BOARD_UART_DEV` at it:

```c
extern whal_Uart g_whalUart;
#define BOARD_UART_DEV (&g_whalUart)
```

For peripheral drivers (which talk over a bus), the `.cfg` carries the
bus device pointer plus chip-specific addressing. SPI NOR W25Q64 is a
good reference:

```c
/* boards/peripheral/flash/spi_nor_w25q64.c */
whal_Flash g_whalSpiNorW25q64 = {
    .driver = &whal_SpiNor_Driver,
    .cfg = &(whal_SpiNor_Cfg) {
        .spiDev    = BOARD_SPI_DEV,        /* uses the board's SPI device */
        .spiComCfg = &g_w25q64ComCfg,      /* freq, mode, wordSz, dataLines */
        .gpioDev   = BOARD_GPIO_DEV,       /* GPIO controller for chip select */
        .csPin     = SPI_CS_PIN,
        .timeout   = &g_whalTimeout,
        .pageSz    = W25Q64_PAGE_SZ,
        .capacity  = W25Q64_CAPACITY,
    },
};
```

The same generic API runs against either flash device — the vtable in
each `.driver` field dispatches to the right implementation. Application
code calls `whal_Flash_Read(BOARD_FLASH_DEV, ...)` for on-chip flash and
`whal_Flash_Read(BOARD_SPI_FLASH_DEV, ...)` for the W25Q64 with no other
difference.

See the platform-specific headers in `wolfHAL/<device_type>/` for each
driver's full config struct, and the example boards in `boards/` for
complete instantiations.

## Initialization

The board is responsible for initializing peripherals in dependency order.
Drivers do not enable their own clocks or power supplies — the board must
handle these prerequisites explicitly before calling each driver's `Init`.

A typical sequence:

1. Pre-clock setup (flash wait states, power supplies)
2. Bring up the clock tree (oscillators, optional PLL, sysclk source)
3. Enable peripheral clocks
4. Initialize peripheral drivers
5. Start timers

The chip's clock driver exposes `Enable*`/`Disable*`/`Set*` helpers that
the board calls in order. There is no generic `whal_Clock_Init` walker —
clock-tree shape varies too much across vendors to abstract.

```c
static const whal_Myplatform_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_MYPLATFORM_GPIOB_GATE},
    {WHAL_MYPLATFORM_UART1_GATE},
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Bring up clocks — chip-specific helpers, called in order. No device
     * pointer; each helper reads the chip's fixed base from its own header. */
    err = whal_Myplatform_Rcc_EnableOsc(
        &(whal_Myplatform_Rcc_OscCfg){WHAL_MYPLATFORM_OSC0_CFG});
    if (err) return err;
    err = whal_Myplatform_Rcc_SetSysClock(WHAL_MYPLATFORM_SYSCLK_SRC_OSC0);
    if (err) return err;

    /* Enable peripheral clocks. */
    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Myplatform_Rcc_EnablePeriphClk(&g_periphClks[i]);
        if (err) return err;
    }

    /* Initialize peripherals through BOARD_<X>_DEV. */
    err = whal_Gpio_Init(BOARD_GPIO_DEV);       if (err) return err;
    err = whal_Uart_Init(BOARD_UART_DEV);       if (err) return err;
    err = whal_Timer_Init(BOARD_TIMER_DEV);     if (err) return err;
    err = whal_Timer_Start(BOARD_TIMER_DEV);    if (err) return err;

    return WHAL_SUCCESS;
}
```

See the board examples in `boards/` for complete sequences.

## Using the API

After initialization, use the wolfHAL API to interact with peripherals.
Always reach each device through `BOARD_<X>_DEV` so application source
stays portable across boards.

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

All API functions return `whal_Error`. The codes are:

| Code | Meaning |
|------|---------|
| `WHAL_SUCCESS` | Operation completed successfully |
| `WHAL_EINVAL` | Invalid argument (null device pointer, null data pointer) |
| `WHAL_ENOTSUP` | Argument value not supported by this driver/hardware |
| `WHAL_ENOTREADY` | Resource is busy or not yet available |
| `WHAL_EHARDWARE` | Hardware error (e.g., RNG entropy failure) |
| `WHAL_ETIMEOUT` | Operation timed out waiting for hardware |

## Reducing Code Size Further

Knob 2 (direct API mapping) and knob 1 (single-instance) already remove
the dispatch and indirection overheads. A few additional knobs help when
you need to squeeze further.

### Custom vtables

For vtable-dispatched devices, the platform driver exports a full vtable
with every operation populated. If you only use a subset, define your
own vtable with just the entries you need:

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

With `-ffunction-sections -fdata-sections -Wl,--gc-sections`, the unused
driver functions are stripped from the final binary.

This knob is meaningless for direct-API-mapping devices — those have no
vtable, and `--gc-sections` already drops any unreferenced entry points.

### Calling driver functions directly

For maximum control on a vtable-dispatched, pointer-based driver, you
can skip the vtable and call the chip-specific function directly:

```c
#include <wolfHAL/uart/stm32wb_uart.h>

whal_Stm32wb_Uart_Init(&g_whalUart);
whal_Stm32wb_Uart_Send(&g_whalUart, "hi\r\n", 4);
```

This eliminates the vtable indirection and lets the compiler inline more
aggressively. For single-instance drivers the equivalent is to call the
chip-specific function with `WHAL_INTERNAL_DEV`:

```c
whal_Stm32wb_Gpio_Set(WHAL_INTERNAL_DEV, BOARD_LED_PIN, 1);
```

The `BOARD_<X>_DEV` macros normally hide this distinction — only reach
for direct chip-specific calls when portability across boards is not a
goal (e.g. board-internal code).

### Disabling timeouts

Define `WHAL_CFG_NO_TIMEOUT` globally to remove all timeout logic from
the binary. `WHAL_TIMEOUT_START` becomes a no-op and `WHAL_TIMEOUT_EXPIRED`
always evaluates to `0`, so polling loops run until the hardware
condition is met. Useful when you trust the hardware and care more about
code size than about bounding a stuck-bus hang.

## Next Steps

- See `boards/` for complete board configuration examples.
- See [Writing a Driver](writing_a_driver.md) for how to add support for
  a new platform.
- See [Adding a Board](adding_a_board.md) for how to wire up a new
  hardware target.
