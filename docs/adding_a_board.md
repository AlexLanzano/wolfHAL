# Adding a New Board

This guide covers adding a new board configuration to wolfHAL.

## Overview

A board ties a platform to concrete hardware by defining peripheral instances,
pin assignments, clock settings, and startup code. Each board lives in its own
directory under `boards/` named `<platform>_<board_name>/`.

## Required Files

### board.h

`board.h` is where the board describes each peripheral to the rest of the
project. It contains four kinds of declaration:

1. `extern` globals for vtable-dispatched drivers (`g_whalUart`,
   `g_whalSpi`, `g_whalI2c`, ...) defined over in `board.c`.
2. `static const` singletons (`whal_<Plat>_<Drv>_Dev`) for
   single-instance drivers — these are read directly by the driver
   bodies (`#include "board.h"` from the driver source). Each
   translation unit including `board.h` gets its own const copy in
   `.rodata`; `--gc-sections` folds the unused ones.
3. `BOARD_<PERIPH>_DEV` macros that the application and tests use to
   reach each peripheral — these resolve to either `WHAL_SINGLETON` or
   `&g_whal<X>`, depending on how this board has wired the driver.
4. Board constants (pin indices, flash test addresses, etc.) and the
   `Board_Init` / `Board_Deinit` / `Board_WaitMs` prototypes.

```c
#pragma once

#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/vendor/device.h>

extern whal_Clock   g_whalClock;
extern whal_Uart    g_whalUart;
extern whal_Timeout g_whalTimeout;

#define BOARD_LED_PIN 0

/* BOARD_<PERIPH>_DEV: how this board reaches each peripheral.
 * WHAL_SINGLETON for single-instance drivers whose body reads from a
 * named singleton below; &g_whal<X> for drivers that still use vtable
 * dispatch (or for the coexistence-stub variant). */
#define BOARD_GPIO_DEV     WHAL_SINGLETON
#define BOARD_UART_DEV     (&g_whalUart)
#define BOARD_CLOCK_DEV    (&g_whalClock)
#define BOARD_WATCHDOG_DEV WHAL_SINGLETON
#define BOARD_RNG_DEV      WHAL_SINGLETON

/* Singleton consumed by the GPIO driver body directly. */
static const whal_Gpio whal_Myplatform_Gpio_Dev = {
    .base = WHAL_MYPLATFORM_GPIO_BASE,
    .cfg  = (void *)&(const whal_Myplatform_Gpio_Cfg){
        /* ...pin table... */
    },
};

/* Singleton consumed by the RNG driver body directly. */
static const whal_Rng whal_Myplatform_Rng_Dev = {
    .base = WHAL_MYPLATFORM_RNG_BASE,
    .cfg  = (void *)&(const whal_Myplatform_Rng_Cfg){
        .timeout = &g_whalTimeout,
    },
};

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);
```

When a chip's driver is implemented as an alias of another chip's driver
(e.g. STM32N6 IWDG reusing the STM32WB body), the singleton must be
declared under this board's own platform name — the alias header bridges
the two names with a `#define` (see `wolfHAL/watchdog/stm32n6_iwdg.h`).
A small number of singletons are inherently cross-platform and keep
neutral names: `whal_Nvic_Dev` (Cortex-M NVIC), `whal_SysTick_Dev`
(Cortex-M SysTick), `whal_Lan8742a_Dev` (generic Ethernet PHY).

Single-instance drivers that need per-instance mutable state (e.g.
AES-GCM / AES-CCM accumulators) declare the state as a `static` variable
in `board.h` immediately above the singleton that points at it. Each
including translation unit gets its own copy, but only the driver TU
writes to it, and `--gc-sections` strips the rest.

### board.c

Defines the `extern` device instances declared in `board.h` (the
vtable-dispatched ones) and implements `Board_Init()` / `Board_Deinit()`.
Per-instance const cfg for single-instance drivers lives in `board.h`
alongside the singleton; `board.c` does not need to repeat it.

`Board_Init()` is responsible for initializing all peripherals in
dependency order. For example, the clock controller must be initialized
before peripherals that depend on it, and a power supply controller (if
present) may need to come before the clock. It should return
`WHAL_SUCCESS` on success or an error code on failure.

`Board_Deinit()` tears down peripherals in reverse order.

```c
#include "board.h"
#include <wolfHAL/platform/vendor/device.h>
#include "peripheral.h"

whal_Clock g_whalClock = {
    .base = WHAL_MYPLATFORM_CLOCK_BASE,
};

whal_Uart g_whalUart = {
    .base = WHAL_MYPLATFORM_UART1_BASE,
    /* .driver: direct API mapping */
    .cfg = &(whal_Myplatform_Uart_Cfg) {
        .brr = WHAL_MYPLATFORM_UART_BRR(SYSCLK_HZ, 115200),
        .timeout = &g_whalTimeout,
    },
};

static const whal_Myplatform_Clock_PeriphClk g_periphClks[] = {
    {WHAL_MYPLATFORM_GPIOA_GATE},
    {WHAL_MYPLATFORM_UART1_GATE},
};
#define PERIPH_CLK_COUNT \
    (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

whal_Error Board_Init(void)
{
    whal_Error err;
    size_t i;

    /* Bring up the clock tree imperatively. The chip's clock driver
     * exposes Enable*/Disable*/Set* helpers; boards call them in order.
     * The exact sequence is chip-specific — see the chip's clock header. */
    err = whal_Myplatform_Clock_EnableOsc(&g_whalClock,
        &(whal_Myplatform_Clock_OscCfg){WHAL_MYPLATFORM_CLOCK_OSC0_CFG});
    if (err)
        return err;
    err = whal_Myplatform_Clock_SetSysClock(&g_whalClock,
        WHAL_MYPLATFORM_CLOCK_SYSCLK_SRC_OSC0);
    if (err)
        return err;

    for (i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Myplatform_Clock_EnablePeriphClk(&g_whalClock,
                                                    &g_periphClks[i]);
        if (err)
            return err;
    }

    /* Initialize peripherals. Pass the corresponding BOARD_<PERIPH>_DEV
     * macro — it will be WHAL_SINGLETON for single-instance drivers or
     * &g_whal<X> for vtable-dispatched drivers, whichever this board
     * declared in board.h. */
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

    err = Peripheral_Init();
    if (err)
        return err;

    return WHAL_SUCCESS;
}
```

#### Coexisting drivers of the same type

When two drivers of the same generic type share a board (the typical case
is on-chip flash plus an external SPI-NOR flash), the const cfg singleton
in `board.h` is not enough on its own — the generic dispatcher
(`whal_Flash_Read`, etc.) must still vtable-dispatch to pick which
driver runs. The pattern is a "stub" in `board.c` carrying only the
`.driver` field, paired with the const cfg singleton in `board.h`:

```c
/* board.h */
static const whal_Flash whal_Myplatform_Flash_Dev = {
    .base = WHAL_MYPLATFORM_FLASH_BASE,
    .cfg  = (void *)&(const whal_Myplatform_Flash_Cfg){ /* ... */ },
};

#define BOARD_FLASH_DEV (&g_whalFlash)

/* board.c — dispatcher stub. The driver body still reads .base and .cfg
 * from whal_Myplatform_Flash_Dev; this struct only exists so that
 * whal_Flash_* can vtable-dispatch to it. */
whal_Flash g_whalFlash = {
    .driver = WHAL_MYPLATFORM_FLASH_DRIVER,
};
```

The external SPI-NOR side does not need this dance — it is already a
peripheral driver with its own `g_whal<X>` and vtable, and lives in
`boards/peripheral/`.

The watchdog is intentionally excluded from `Board_Init()` and `Board_Deinit()`.
Once started, most watchdog peripherals cannot be stopped — if `Board_Init()`
starts the watchdog, any delay before the application begins refreshing it will
cause an unexpected reset. The application or test should call
`whal_Watchdog_Init(BOARD_WATCHDOG_DEV)` directly when it is ready to begin
refreshing. The board still declares the watchdog device (whether as a
singleton in `board.h` or as `g_whalWatchdog` in `board.c`, depending on the
driver), points `BOARD_WATCHDOG_DEV` at it, and enables any required clocks
(e.g., WWDG APB clock, IWDG LSI oscillator) in `Board_Init()` so the
watchdog is ready to be started.

```c
whal_Error Board_Deinit(void)
{
    whal_Error err;

    err = Peripheral_Deinit();
    if (err)
        return err;

    whal_Timer_Stop(BOARD_TIMER_DEV);
    whal_Timer_Deinit(BOARD_TIMER_DEV);
    whal_Uart_Deinit(BOARD_UART_DEV);
    whal_Gpio_Deinit(BOARD_GPIO_DEV);

    for (size_t i = PERIPH_CLK_COUNT; i-- > 0; ) {
        err = whal_Myplatform_Clock_DisablePeriphClk(&g_whalClock,
                                                     &g_periphClks[i]);
        if (err)
            return err;
    }

    return WHAL_SUCCESS;
}
```

### board.mk

Defines the toolchain, compiler flags, and source file list:

```makefile
_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = myplatform
TESTS = gpio clock uart flash timer

GCC     = arm-none-eabi-gcc
LD      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -mcpu=cortex-m4 -mthumb -Os -Wall -MMD $(INCLUDE) -I$(_BOARD_DIR) \
          -DWHAL_CFG_MYPLATFORM_GPIO_DIRECT_API_MAPPING \
          -DWHAL_CFG_MYPLATFORM_UART_DIRECT_API_MAPPING \
          -DWHAL_CFG_MYPLATFORM_UART_SINGLE_INSTANCE
LDFLAGS = -mcpu=cortex-m4 -mthumb -nostdlib -lgcc

LINKER_SCRIPT = $(_BOARD_DIR)/linker.ld

INCLUDE += -I$(_BOARD_DIR) -I$(WHAL_DIR)/boards/peripheral

BOARD_SOURCE  = $(_BOARD_DIR)/board.c
BOARD_SOURCE += $(_BOARD_DIR)/ivt.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*.c)
# Dispatch sources for mapped types are excluded; keep dispatch sources
# for types that may have peripheral drivers (flash, block, sensor).
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/block.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/myplatform_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)

# Peripheral devices
include $(WHAL_DIR)/boards/peripheral/board.mk
```

Two families of build-time knobs commonly appear in `board.mk`:

- `WHAL_CFG_<DRIVER>_DIRECT_API_MAPPING` renames the chip-specific
  driver functions to the generic API names, eliminating the vtable
  dispatch wrapper. Safe whenever a generic-API type has at most one
  driver linked into the build.
- `WHAL_CFG_<PLAT>_<DRV>_SINGLE_INSTANCE` makes a conditionally
  single-instance driver read its `.base` and `.cfg` from the
  corresponding `whal_<Plat>_<Drv>_Dev` singleton in `board.h` instead
  of dereferencing the handle argument. The flag is per-driver
  per-platform; default builds keep the pointer-based path. Other
  drivers are unconditionally single-instance and need no flag — their
  driver bodies always read from the singleton.

## Peripheral Devices

Boards support optional external peripheral devices (e.g., SPI-NOR flash, SD
cards) through the peripheral system in `boards/peripheral/`. To enable this:

1. Include `peripheral.h` in `board.c` and add the peripheral include path
   (`-I$(WHAL_DIR)/boards/peripheral`) in `board.mk`.

2. Include `boards/peripheral/board.mk` at the end of the board's
   `board.mk`. This conditionally compiles peripheral drivers based on
   build-time flags (e.g., `PERIPHERAL_SPI_NOR_W25Q64=1`).

3. Call `Peripheral_Init()` at the end of `Board_Init()` and
   `Peripheral_Deinit()` at the top of `Board_Deinit()`. These functions
   iterate the peripheral registry arrays and initialize/deinitialize all
   enabled peripheral devices.

`Peripheral_Init()` and `Peripheral_Deinit()` are safe to call even when no
peripherals are enabled — the registry arrays will be empty and the functions
return immediately.

See [Adding a Peripheral](adding_a_peripheral.md) for details on how to add
new peripheral devices to the registry.

### linker.ld

Linker script defining the memory layout for your board's MCU. Must define
FLASH and RAM regions, place `.isr_vector` at the start of FLASH, and set up
`.text`, `.data`, and `.bss` sections.

### ivt.c (ARM targets)

Interrupt vector table and `Reset_Handler`. The reset handler copies `.data`
from FLASH to RAM, zeroes `.bss`, and calls `main()`. This file is specific to
ARM Cortex-M targets. Other architectures will need their own startup code.
