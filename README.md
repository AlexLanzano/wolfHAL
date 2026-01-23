# wolfHAL

wolfHAL is a small C hardware abstraction layer that presents a uniform driver
model for embedded targets. It ships with simple register helpers, lightweight
device descriptors, and reference drivers for STM32WB55 peripherals (RCC, GPIO,
UART, SPI, SysTick, and Flash).

## Features
- Minimal driver vtables for clocks, GPIO, timers, UART, SPI, and flash
- Register helpers for masked read/modify/write operations
- Platform shims for STM32WB55 (Nucleo example included)
- Doxygen-ready headers and override-friendly logging macros

## Repository layout
- `wolfHAL/` – public headers (API surface)
- `src/` – driver implementations (generic and STM32WB55-specific)
- `examples/stm32wb/` – reference board bring-up and echo loop
- `wolfHAL/platform/` – platform convenience initializers

## Getting started
1. Add the public headers to your include path: `-IwolfHAL`.
2. Compile the core sources (`src/reg.c`, `src/clock/*.c`, `src/gpio/*.c`,
   `src/uart/*.c`, `src/spi/*.c`, `src/timer/*.c`, `src/flash/*.c`) into your
   firmware build.
3. Provide platform-specific driver configs (see the STM32WB example) or author
   your own drivers by filling the function pointers in each `*Driver` vtable.
4. Override the logging macros in `wolfHAL/print.h` to route logs as needed.

### STM32WB55 Nucleo example
The example under `examples/stm32wb/` configures:
- RCC to run from PLL fed by MSI (64 MHz)
- GPIO for LED and LPUART1 pins
- SysTick at 1 kHz
- LPUART1 at 115200 baud

`main.c` then initializes the devices, starts SysTick, echoes UART input, and
blinks the LED once per second. Use it as a template for board bring-up.

## Documentation
Headers are annotated for Doxygen. Generate HTML docs by running `doxygen` with
a Doxyfile that includes `wolfHAL/` as an input path.

## License
GPLv3 – see `LICENSE` for details.
