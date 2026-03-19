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
      Makefile.inc                  Toolchain and source list
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
src/clock/clock.c
src/gpio/gpio.c
src/uart/uart.c
src/flash/flash.c
src/spi/spi.c
src/timer/timer.c
src/rng/rng.c
src/supply/supply.c
```

3. Compile the platform-specific driver sources for your target:

```
src/clock/<platform>_clock.c
src/gpio/<platform>_gpio.c
src/uart/<platform>_uart.c
...
```

You only need to include the modules and platform drivers your project actually
uses.

## The Device Model

Every peripheral in wolfHAL is represented by a device struct with three fields:

```c
struct whal_Gpio {
    const whal_Regmap regmap;        /* base address and size */
    const whal_GpioDriver *driver;   /* vtable of function pointers */
    const void *cfg;                 /* platform-specific configuration */
};
```

- **regmap** — identifies the peripheral's memory-mapped register block
- **driver** — points to the platform driver implementation (the vtable)
- **cfg** — points to a platform-specific configuration struct that the driver
  reads during Init

Platform headers provide device macros that fill in the `regmap` and `driver`
fields, so you only need to provide the `cfg`:

```c
#include <wolfHAL/platform/st/stm32wb55xx.h>

whal_Gpio g_whalGpio = {
    WHAL_STM32WB55_GPIO_DEVICE,
    .cfg = &gpioConfig,
};
```

## Configuring Peripherals

Each platform driver defines its own configuration struct with the parameters
it needs. For example, a GPIO driver might need a clock controller reference
and a pin configuration table:

```c
whal_Gpio g_whalGpio = {
    WHAL_STM32WB55_GPIO_DEVICE,

    .cfg = &(whal_Stm32wbGpio_Cfg) {
        .pinCfg = (whal_Stm32wbGpio_PinCfg[]) {
            { /* LED */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 5,
                .mode = WHAL_STM32WB_GPIO_MODE_OUT,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_LOW,
            },
        },
        .pinCount = 1,
    },
};
```

A UART driver might need a pre-computed baud rate register value and a timeout:

```c
whal_Uart g_whalUart = {
    WHAL_STM32WB55_UART1_DEVICE,

    .cfg = &(whal_Stm32wbUart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32WB_UART_BRR(64000000, 115200),
    },
};
```

See the platform-specific headers in `wolfHAL/<device_type>/` for the full set
of configuration options for each driver.

## Initialization

The board is responsible for initializing peripherals in dependency order.
Drivers do not enable their own clocks or power supplies — the board must
handle these prerequisites explicitly before calling a driver's Init.

A typical initialization sequence:

1. Do any pre-clock-controller initialization (e.g., flash wait states,
   power supplies)
2. Initialize the clock controller
3. Enable peripheral clocks
4. Initialize peripheral drivers
5. Start timers

```c
static const MyPlatformClk g_peripheralClocks[] = {
    {MY_PLATFORM_GPIOB_CLOCK},
    {MY_PLATFORM_UART1_CLOCK},
};
#define PERIPHERAL_CLOCK_COUNT \
    (sizeof(g_peripheralClocks) / sizeof(g_peripheralClocks[0]))

whal_Error Board_Init(void)
{
    whal_Error err;

    err = whal_Clock_Init(&g_whalClock);
    if (err)
        return err;

    /* Enable peripheral clocks */
    for (size_t i = 0; i < PERIPHERAL_CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_peripheralClocks[i]);
        if (err)
            return err;
    }

    /* Initialize peripherals */
    err = whal_Gpio_Init(&g_whalGpio);
    if (err)
        return err;

    err = whal_Uart_Init(&g_whalUart);
    if (err)
        return err;

    err = whal_Timer_Init(&g_whalTimer);
    if (err)
        return err;

    err = whal_Timer_Start(&g_whalTimer);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
```

See the board examples in `boards/` for complete initialization sequences
including platform-specific steps.

## Using the API

After initialization, use the wolfHAL API to interact with peripherals:

```c
#include <wolfHAL/wolfHAL.h>
#include "board.h"

void main(void)
{
    if (Board_Init() != WHAL_SUCCESS)
        while (1);

    while (1) {
        whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 1);
        whal_Uart_Send(&g_whalUart, "Hello!\r\n", 8);
        Board_WaitMs(1000);

        whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 0);
        Board_WaitMs(1000);
    }
}
```

All API functions return `whal_Error`. Check for `WHAL_SUCCESS` to confirm the
operation completed. The error codes are:

| Code | Meaning |
|------|---------|
| `WHAL_SUCCESS` | Operation completed successfully |
| `WHAL_EINVAL` | Invalid argument or unsupported operation |
| `WHAL_ENOTREADY` | Resource is busy or not yet available |
| `WHAL_EHARDWARE` | Hardware error (e.g., RNG entropy failure) |
| `WHAL_ETIMEOUT` | Operation timed out waiting for hardware |

## Optimizing for Size

wolfHAL gives you several ways to reduce code size depending on how much
control you want.

### Direct Callbacks

Define `WHAL_CFG_DIRECT_CALLBACKS` to bypass the generic dispatch layer. API
calls compile down to direct function pointer calls with no input validation,
eliminating the dispatch source files entirely.

### Custom Vtables

The platform drivers provide a pre-built vtable with all operations populated.
If you only use a subset of a driver's functionality, you can define your own
vtable that only includes the functions you need:

```c
static const whal_GpioDriver myGpioDriver = {
    .Init   = whal_Stm32wbGpio_Init,
    .Deinit = whal_Stm32wbGpio_Deinit,
    .Set    = whal_Stm32wbGpio_Set,
    /* Get left as NULL — not needed, saves pulling in that code */
};

whal_Gpio g_whalGpio = {
    .regmap = { .base = 0x48000000, .size = 0x2000 },
    .driver = &myGpioDriver,
    .cfg = &gpioConfig,
};
```

With link-time optimization (`-flto`) or garbage collection (`-ffunction-sections`
+ `-Wl,--gc-sections`), any driver functions not referenced through the vtable
will be stripped from the final binary.

### Calling Driver Functions Directly

For maximum control, you can skip the vtable entirely and call the underlying
platform driver functions directly:

```c
#include <wolfHAL/gpio/stm32wb_gpio.h>

whal_Stm32wbGpio_Init(&g_whalGpio);
whal_Stm32wbGpio_Set(&g_whalGpio, BOARD_LED_PIN, 1);
```

This eliminates the vtable indirection and lets the compiler inline or optimize
the calls more aggressively.

Register-level drivers do not call other drivers internally, so this works
without any caveats. Bus-device drivers (e.g., SPI flash) still call their
bus driver through the vtable.

## Next Steps

- See `boards/` for complete board configuration examples
- See [Writing a Driver](writing_a_driver.md) for how to add support for a new
  platform
- See [Adding a Board](adding_a_board.md) for how to create a board
  configuration for your hardware
