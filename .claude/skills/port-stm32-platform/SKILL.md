---
name: port-stm32-platform
description: Port a new STM32 chip family to wolfHAL. Creates the platform header, scaffolds new drivers, adds alias header + stub .c for reused drivers, and optionally wires up a first board.
argument-hint: <platform-short-name> [chip-name] [board-name]
---

# Port a new STM32 chip family to wolfHAL

You are helping the user port a new STM32 chip family (e.g., `stm32g4`, `stm32u5`, `stm32l5`) to wolfHAL. A "platform" in wolfHAL is a family of register-compatible chips that share drivers (e.g., `stm32wba` covers wba52/wba54/wba55). A specific chip (e.g., `stm32wba55cg`) gets its own platform header with concrete base addresses and clock macros. A board (e.g., `stm32wba55cg_nucleo`) ties a chip to a physical dev board.

Arguments passed after the skill name identify the scope:
- `$1`: platform short name (required, e.g., `stm32g4`)
- `$2`: specific chip (optional, e.g., `stm32g431cb`)
- `$3`: board name (optional, e.g., `stm32g431_nucleo`)

If any are missing, ask the user before starting the relevant phase.

## Prerequisites — gather before starting

Ask the user to provide (or confirm paths to):

1. **Reference Manual (TRM) PDF** for the chip family — e.g., `stm32g4_trm.pdf`. Place at repo root.
2. **Datasheet PDF** for the specific chip — e.g., `stm32g431cb.pdf`. Place at repo root.
3. **Board user guide PDF** if adding a board — e.g., `nucleo_g431.pdf`. Place at repo root.

Do NOT proceed with register layout work without the TRM — guessing from similar chips is the #1 source of silent bugs. STM32 family differences (bit positions, register offsets, new CONDRST/FIFO/voltage-scaling fields) are subtle and costly to debug later.

## Phase 1 — Discovery (read-only, no code changes)

Goal: figure out what is register-compatible with an existing wolfHAL driver vs what needs a new driver.

For each device type below, use `pdftotext -layout -f <start> -l <end> <trm.pdf>` on the TRM to extract the register map page, then compare against existing wolfHAL drivers in `src/<type>/`:

| Device type | Existing drivers to diff against | What to check in TRM |
|---|---|---|
| Clock/RCC   | `stm32wb_rcc.c`, `stm32wba_rcc.c`, `stm32h5_rcc.c`, `stm32f4_rcc.c` | PLL config register (PLLCFGR split vs unified), voltage scaling (PWR_VOSR), AHB/APB prescaler fields, HSI/HSE/LSI/LSE, peripheral clock enable register offsets (AHBxENR, APBxENR) |
| GPIO        | `stm32wb_gpio.c`, `stm32f4_gpio.c` | MODER/OTYPER/OSPEEDR/PUPDR/AFRL/AFRH — usually compatible across STM32 |
| UART        | `stm32wb_uart.c`, `stm32wba_uart.c`, `stm32wba_uart_dma.c`, `stm32h5_uart.c`, `stm32c0_uart.c`, `stm32f4_uart.c` | BRR calc (16x vs 8x oversample), FIFO enable (CR1.FIFOEN bit 29), ISR/ICR flags, TDR/RDR offsets |
| Flash       | `stm32wb_flash.c`, `stm32wba_flash.c`, `stm32h5_flash.c`, `stm32c0_flash.c`, `stm32f4_flash.c` | NSKEYR/KEYR unlock sequence, SR bit positions (BSY, EOP, errors), CR bit positions (PG, PER, STRT, LOCK, PNB), page size |
| RNG         | `stm32wb_rng.c`, `stm32wba_rng.c` | CR bits (RNGEN, CED, CONDRST), SR bits (DRDY, errors), clock source selection register (CCIPRx RNGSEL) |
| I2C         | `stm32wb_i2c.c` | CR1/CR2/TIMINGR — usually compatible on modern STM32 (V2 I2C) |
| SPI         | `stm32wb_spi.c`, `stm32h5_spi.c`, `stm32f4_spi.c` | CR1/CR2/SR layout (V1 SPI differs from V2 SPI used on newer chips) |
| AES         | `stm32wb_aes.c`, `stm32wba_aes.c` | CR (KEYSIZE, MODE, CHMOD, DATATYPE, NPBLB, GCMPH), KEYR, IVR, DIN/DOUT |
| HASH        | `stm32wba_hash.c` | CR (ALGO, MODE, DATATYPE, LKEY, INIT), STR (NBLW, DCAL), SR (BUSY), DIN, digest registers (HRx). Check ALGO bit positions — some chips split ALGO across non-contiguous bits |
| DMA         | `stm32wb_dma.c` (classic DMA+DMAMUX), `stm32wba_gpdma.c` (GPDMA) | Is it DMA+DMAMUX (older) or GPDMA (newer)? Not compatible. GPDMA appears on U5/WBA/H5/C0/G0 variants. |
| Timer       | `systick.c` (shared) | SysTick is architectural (Cortex-M), works unchanged |
| IRQ         | `cortex_m4_nvic.c`, `cortex_m33_nvic.c` | NVIC is architectural per core (M4 vs M33) |
| Watchdog    | `stm32wb_iwdg.c`, `stm32wb_wwdg.c`, `stm32wba_iwdg.c`, `stm32wba_wwdg.c` | IWDG/WWDG are very stable across STM32 |

For each device type, decide **reuse existing driver (with alias)** OR **write a new driver**. Write the decision to a checklist and show the user before starting Phase 2.

The clock driver is almost always new per family. GPIO, I2C, and IWDG/WWDG are almost always reusable. UART, flash, RNG, DMA require careful diffing.

## Phase 2 — Platform header

Create `wolfHAL/platform/st/<chip>.h` (e.g., `stm32g431cb.h`). Reference: `stm32wba55cg.h` (GPDMA chip) or `stm32wb55xx.h` (DMA+DMAMUX chip).

Conventions:
1. Every driver include uses the **new platform's prefix** — for reused drivers that means the alias header created in Phase 3, not the original family's header.
2. Every device macro uses `.driver = &whal_<NewPlatform><Type>_Driver` — the alias header `#define`s this to the original symbol at preprocess time, so no renaming of actual code is needed but callers see the new-prefix name.
3. Defines clock enable macros and, when applicable, DMA request mapping macros.

Skeleton:

```c
#include <wolfHAL/platform/arm/cortex_m4.h>  /* or cortex_m33.h / cortex_m7.h */

/* Every include uses the new platform's prefix — aliased or newly written */
#include <wolfHAL/clock/<platform>_rcc.h>
#include <wolfHAL/gpio/<platform>_gpio.h>
#include <wolfHAL/uart/<platform>_uart.h>
/* ...one include per device type... */

#define WHAL_<PLATFORM>_USART1_DEVICE   \
    .regmap = { .base = 0x<addr>, .size = 0x400 }, \
    .driver = &whal_<Platform>Uart_Driver

#define WHAL_<PLATFORM>_USART1_CLOCK \
    .regOffset = 0x<offset>,         \
    .enableMask = (1UL << <bit>),    \
    .enablePos  = <bit>
```

## Phase 3 — Drivers

### For each device type marked "write new"

1. Read `docs/writing_a_driver.md` for the driver pattern.
2. Copy the closest existing driver as a starting point.
3. Update register offsets, bit positions, and sequences per the TRM. **Cross-check register-map diagrams against the textual bit descriptions** — they sometimes disagree, and the textual description is authoritative.
4. Match the existing naming: `whal_<Platform><Type>_<Func>` for functions, `whal_<Platform><Type>_Driver` for the vtable, `whal_<Platform><Type>_Cfg` for the config struct.
5. Place files at `wolfHAL/<type>/<platform>_<type>.h` and `src/<type>/<platform>_<type>.c`.
6. Do not add cross-driver calls from inside a driver — clock enables, power sequencing, flash wait states, pin muxing are the board's responsibility.

### For each device type marked "reuse existing driver" — create an alias header + stub .c

Reference pattern in-tree: `wolfHAL/gpio/stm32f4_gpio.h` (alias header) and `src/gpio/stm32f4_gpio.c` (stub). Every symbol the caller sees must carry the new platform's prefix; aliasing is done at preprocess time via `typedef` and `#define`, so no code is duplicated and no GCC alias attribute is needed.

**Alias header** — `wolfHAL/<type>/<newplatform>_<type>.h`:

```c
#ifndef WHAL_<NEWPLATFORM>_<TYPE>_H
#define WHAL_<NEWPLATFORM>_<TYPE>_H

/*
 * @file <newplatform>_<type>.h
 * @brief <NewPlatform> <Type> driver (alias for <OrigPlatform> <Type>).
 *
 * The <NewPlatform> <Type> peripheral is register-compatible with the
 * <OrigPlatform> <Type>. This header re-exports the <OrigPlatform> driver
 * types and symbols under <NewPlatform>-specific names. The underlying
 * implementation is shared.
 */

#include <wolfHAL/<type>/<origplatform>_<type>.h>

/* Type aliases — one typedef per exposed type from the original header */
typedef whal_<OrigPlatform><Type>_Cfg    whal_<NewPlatform><Type>_Cfg;
typedef whal_<OrigPlatform><Type>_PinCfg whal_<NewPlatform><Type>_PinCfg;
/* ...repeat for every exposed type... */

/* Driver and function aliases — one #define per exposed function/driver */
#define whal_<NewPlatform><Type>_Driver whal_<OrigPlatform><Type>_Driver
#define whal_<NewPlatform><Type>_Init   whal_<OrigPlatform><Type>_Init
#define whal_<NewPlatform><Type>_Deinit whal_<OrigPlatform><Type>_Deinit
/* ...repeat for every exposed function... */

/* Macro / enum-value aliases — one #define per user-facing macro */
#define WHAL_<NEWPLATFORM>_<TYPE>_<CONST1> WHAL_<ORIGPLATFORM>_<TYPE>_<CONST1>
/* ...repeat for every mode selector, port letter, pin-packing macro, etc... */

#endif /* WHAL_<NEWPLATFORM>_<TYPE>_H */
```

**Stub .c** — `src/<type>/<newplatform>_<type>.c`:

```c
#include "<origplatform>_<type>.c"
```

That is the entire file. Examples in-tree: `src/gpio/stm32f4_gpio.c`, `src/gpio/stm32wba_gpio.c`, `src/i2c/stm32wba_i2c.c`, `src/uart/stm32wba_uart.c`, `src/watchdog/stm32wba_iwdg.c` — each is one line. The stub exists so the board's Makefile wildcard (`src/*/<newplatform>_*.c`) compiles the original implementation under the new-prefix filename. The original `.c` is NOT added to the Makefile separately; the `#include` pulls it into this translation unit exactly once.

**Test alias** — when a driver is reused via alias AND the original driver already has a platform-specific test file (`tests/<type>/test_<origplatform>_<type>.c`), create a matching test alias at `tests/<type>/test_<newplatform>_<type>.c`:

```c
#include "test_<origplatform>_<type>.c"
```

This is the same one-line pattern as the driver stub. The build system auto-discovers `test_$(PLATFORM)_$(t).c` files and defines `WHAL_TEST_ENABLE_<TYPE>_PLATFORM`, which gates the `whal_Test_<Type>_Platform()` call in `tests/main.c`. Examples in-tree: `tests/gpio/test_stm32c0_gpio.c`, `tests/gpio/test_stm32f0_gpio.c`.

### Common driver pitfalls (from prior ports)
- **Flash**: check if already unlocked before writing keys — double-unlock hard-faults. Bit positions (LOCK, STRT, PNB) differ between families; do not copy-paste.
- **RNG**: CONDRST + per-config register sequence goes in Init, not per Generate call. Select RNG clock source via `RCC_CCIPR`/`CCIPR2` before Init — default is often LSE, which requires LSE running.
- **Clock**: on chips with voltage scaling, transition to VOS Range 1 BEFORE raising SYSCLK above ~16 MHz; skipping this hard-faults at the first PLL switch.
- **DMA + USART**: enable `CR1.FIFOEN` when available — DMA→USART without FIFO has produced silent byte drops.
- **Polling loops**: every hardware-flag wait must go through `whal_Timeout` — bare `while` loops violate a repo-wide preference.
- **One-time hardware setup** (clock sources, trimming, feature selects): belongs in Init, not in per-operation functions.

## Phase 4 — Board (optional, only if $3 provided)

Create `boards/<board_name>/` with:

### `board.h`
Exports `extern whal_<Type>` instances and declares `Board_Init`/`Board_Deinit`/`Board_WaitMs`. Follow `docs/adding_a_board.md`.

### `board.c`
Define each `whal_<Type>` global with its platform macro + board-specific config (pins, baud rates, timeout, DMA channel assignments). Implement `Board_Init` in dependency order: PWR → Clock → peripheral clock enables → GPIO → UART → Timer → the rest. Keep the watchdog out of `Board_Init` (the app starts it when ready to refresh) per `docs/adding_a_board.md`. Guard DMA-specific setup under `#ifdef BOARD_DMA`, matching `boards/stm32wba55cg_nucleo/board.c`.

### GPIO pin conflict check
After writing the `pinCfg` array in `board.c`, scan every entry pair and verify no two entries share the same physical port+pin. This is a common mistake when a pin serves double duty (e.g., PA5 used as both an LED and SPI1_SCK). If a conflict is found, consult the chip's alternate-function table in the datasheet and remap the conflicting peripheral to an alternate pin on a different port.

### `Makefile.inc`
Model on `boards/stm32wba55cg_nucleo/Makefile.inc`:
- `PLATFORM = <platform>` — matches the prefix used in `src/*/<platform>_*.c`
- `TESTS ?= clock gpio flash timer rng crypto uart spi i2c irq` — trim to what the board supports
- `CFLAGS` — `-mcpu=cortex-m4`/`cortex-m33` to match the core, `-DPLATFORM_<UPPER>` for platform ifdefs
- `BOARD_SOURCE` wildcards over `$(WHAL_DIR)/src/*/<platform>_*.c` — picks up both native drivers and the alias stub `.c` files created in Phase 3. **Do NOT also wildcard over the original family's prefix**, or the original `.c` will compile twice (once directly, once through the stub's include) and you'll get duplicate-symbol errors at link time.

### `linker.ld`
Copy from a similar board and update the `MEMORY` block's FLASH/RAM origins and lengths from the chip's datasheet.

### `ivt.c`
Copy from a similar board with the same core (M4/M33). Update the vector table — vectors from position 16 onward are device-specific and listed in the TRM's interrupt mapping table. At minimum: SysTick plus any peripheral IRQs the tests exercise (USART1_IRQHandler, GPDMAx_ChannelY_IRQHandler, etc.).

### boards/README.md

Add a row to the **Supported Boards** table in `boards/README.md` with the board name, platform, CPU core, and directory. Keep the table sorted alphabetically by platform name.

### GitHub CI

Add the new board to `.github/workflows/boards.yml` by appending it to the `board` matrix list. This ensures the board builds are verified on every PR and push to main. If the board supports peripheral devices (BMI270, SPI-NOR, etc.), also add entries to `.github/workflows/peripheral-tests.yml`. If it supports watchdog, add entries to `.github/workflows/watchdog-tests.yml`.

## Phase 5 — Build and validate

1. `make BOARD=<board_name>` from the repo root. Fix errors in order. Typical failures:
   - Missing symbol for an aliased driver → the alias header is missing a `#define` for that function/type, or the stub `.c` isn't present.
   - Duplicate symbol → the Makefile is picking up both `<newplatform>_*.c` and the original `<origplatform>_*.c`; restrict the wildcard to the new prefix only.
   - Implicit declaration warnings for `whal_Reg_*` / `whal_SetBits` in a stub include → you probably wrote `#include <wolfHAL/...>` with angle brackets in the stub instead of `#include "<origplatform>_<type>.c"` with quotes. The stub must use quoted include so the preprocessor finds the sibling `.c` in the same directory.
2. Flash the binary (user runs this) and run the test suite. Each suite prints `PASS`, `FAIL`, or `SKIP`.
3. If UART output is garbled or missing bytes and the chip uses DMA, enable `CR1.FIFOEN` in the UART init before chasing further — DMA→USART without FIFO has caused byte drops on WBA.

## Output to the user

At each phase transition, summarize in under 10 lines:
- What was done
- Which existing drivers were reused (and note the alias header + stub `.c` pair created for each) vs newly written
- What the user should verify or flash

Wait for the user to confirm before moving to the next phase, unless they asked for an all-in-one run upfront.
