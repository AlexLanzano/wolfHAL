# wolfHAL Example Board Definitions

The board definitions in this directory are **examples** for use with
wolfHAL's tests and sample applications. They are configured for specific
development boards and are not intended for production use. Users should
create their own board support packages tailored to their hardware.

Each subdirectory contains a board support package (BSP) for a specific
development board. A BSP provides everything needed to build wolfHAL for a
given target: startup code, peripheral initialization, linker script, and
build configuration.

## Supported Boards

| Board | Platform | CPU | Directory |
|-------|----------|-----|-----------|
| Microchip PIC32CZ CA Curiosity Ultra | PIC32CZ | Cortex-M7 | `pic32cz_curiosity_ultra/` |
| ST NUCLEO-C031C6 | STM32C0 | Cortex-M0+ | `stm32c031_nucleo/` |
| WeAct BlackPill STM32F411 | STM32F4 | Cortex-M4 | `stm32f411_blackpill/` |
| ST NUCLEO-H563ZI | STM32H5 | Cortex-M33 | `stm32h563zi_nucleo/` |
| ST NUCLEO-WB55RG | STM32WB | Cortex-M4 | `stm32wb55xx_nucleo/` |

## Board Directory Contents

Each board directory contains:

- **`Makefile.inc`** - Build configuration: toolchain, CPU flags, platform
  drivers, and linker script. Included by application Makefiles via
  `include $(BOARD_DIR)/Makefile.inc`.
- **`board.h`** - Board-level declarations: global peripheral instances,
  pin definitions, and `Board_Init()`/`Board_Deinit()` prototypes.
- **`board.c`** - Peripheral instantiation and `Board_Init()` implementation
  (supply, clock, GPIO, UART, flash, timer).
- **`linker.ld`** - Linker script defining memory regions (flash, RAM).
- Any additional board-specific source files (e.g. interrupt vector table,
  architecture-specific startup code).

## Makefile.inc Convention

Board `Makefile.inc` files use a self-referencing pattern so that they can be
included from any directory:

```makefile
_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
```

`_BOARD_DIR` points to the board's own directory, while the application
Makefile sets `BOARD_DIR` which may point elsewhere (e.g. a private board
overlay). This enables private repositories to extend a board by including
the base `Makefile.inc` and adding additional sources.

### What `BOARD_SOURCE` includes

Board `Makefile.inc` populates `BOARD_SOURCE` with all of the sources
required to build the wolfHAL tests and sample applications for that board:

- Board files: `board.c` and any additional board-specific source files
- Platform / SoC drivers: e.g. `pic32cz_*.c`, `stm32wb_*.c`
- Architecture support: `systick.c` and any related startup / vector code
- Core wolfHAL modules and common sources: generic drivers such as
  `gpio.c`, `clock.c`, `uart.c`, and other files under `src/*.c`

In your own projects you may either reuse these defaults by including the
board `Makefile.inc` as-is, or define your own `BOARD_SOURCE` in your
application Makefile to select a different set of modules.

## Adding a New Board

1. Create a new directory: `boards/<vendor>_<board>/`
2. Add `Makefile.inc` following the `_BOARD_DIR` pattern above
3. Implement `board.h`, `board.c`, and `linker.ld`
4. Set `PLATFORM`, `TESTS`, toolchain variables, `CFLAGS`, and `BOARD_SOURCE`
5. Build with `make BOARD=<your_board>`
