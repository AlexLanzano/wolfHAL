# Adding a New Board

This guide covers adding a new board configuration to wolfHAL.

## Overview

A board ties a platform to concrete hardware by defining peripheral instances,
pin assignments, clock settings, and startup code. Each board lives in its own
directory under `boards/` named `<platform>_<board_name>/`.

## Required Files

### board.h

Exports global peripheral instances and board-specific constants:

```c
#pragma once

#include <wolfHAL/wolfHAL.h>

extern whal_Clock   g_whalClock;
extern whal_Gpio    g_whalGpio;
extern whal_Uart    g_whalUart;
extern whal_Timer   g_whalTimer;
extern whal_Flash   g_whalFlash;
extern whal_Timeout g_whalTimeout;

#define BOARD_LED_PIN 0

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);
```

### board.c

Defines global device instances with their configurations and implements
`Board_Init()` and `Board_Deinit()`.

`Board_Init()` is responsible for initializing all peripherals in dependency
order. For example, the clock controller must be initialized before peripherals
that depend on it, and a power supply controller (if present) may need to come
before the clock. It should return `WHAL_SUCCESS` on success or an error code
on failure.

`Board_Deinit()` tears down peripherals in reverse order.

```c
#include "board.h"
#include <wolfHAL/platform/vendor/device.h>
#include "peripheral.h"

static whal_MyplatformGpio_PinCfg pinCfg[] = { /* ... */ };

static whal_MyplatformGpio_Cfg gpioConfig = {
    .pinCfg  = pinCfg,
    .pinCount = sizeof(pinCfg) / sizeof(pinCfg[0]),
};

whal_Gpio g_whalGpio = {
    WHAL_MYPLATFORM_GPIO_DEVICE,
    .cfg = &gpioConfig,
};

static const MyPlatformClk g_peripheralClocks[] = {
    {MY_PLATFORM_GPIO_CLOCK},
    {MY_PLATFORM_UART_CLOCK},
};
#define PERIPHERAL_CLOCK_COUNT \
    (sizeof(g_peripheralClocks) / sizeof(g_peripheralClocks[0]))

whal_Error Board_Init(void)
{
    whal_Error err;

    err = whal_Clock_Init(&g_whalClock);
    if (err)
        return err;

    for (size_t i = 0; i < PERIPHERAL_CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_peripheralClocks[i]);
        if (err)
            return err;
    }

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

    err = Peripheral_Init();
    if (err)
        return err;

    return WHAL_SUCCESS;
}

whal_Error Board_Deinit(void)
{
    whal_Error err;

    err = Peripheral_Deinit();
    if (err)
        return err;

    whal_Timer_Stop(&g_whalTimer);
    whal_Timer_Deinit(&g_whalTimer);
    whal_Uart_Deinit(&g_whalUart);
    whal_Gpio_Deinit(&g_whalGpio);

    for (size_t i = 0; i < PERIPHERAL_CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_peripheralClocks[i]);
        if (err)
            return err;
    }

    whal_Clock_Deinit(&g_whalClock);
    return WHAL_SUCCESS;
}
```

### Makefile.inc

Defines the toolchain, compiler flags, and source file list:

```makefile
_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = myplatform
TESTS = gpio clock uart flash timer

GCC     = arm-none-eabi-gcc
LD      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -mcpu=cortex-m4 -mthumb -Os -Wall -MMD $(INCLUDE) -I$(_BOARD_DIR)
LDFLAGS = -mcpu=cortex-m4 -mthumb -nostdlib -lgcc

LINKER_SCRIPT = $(_BOARD_DIR)/linker.ld

INCLUDE += -I$(_BOARD_DIR) -I$(WHAL_DIR)/boards/peripheral

BOARD_SOURCE  = $(_BOARD_DIR)/board.c
BOARD_SOURCE += $(_BOARD_DIR)/ivt.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/myplatform_*.c)
BOARD_SOURCE += $(WHAL_DIR)/src/timer/systick.c

# Peripheral devices
include $(WHAL_DIR)/boards/peripheral/Makefile.inc
```

## Peripheral Devices

Boards support optional external peripheral devices (e.g., SPI-NOR flash, SD
cards) through the peripheral system in `boards/peripheral/`. To enable this:

1. Include `peripheral.h` in `board.c` and add the peripheral include path
   (`-I$(WHAL_DIR)/boards/peripheral`) in `Makefile.inc`.

2. Include `boards/peripheral/Makefile.inc` at the end of the board's
   `Makefile.inc`. This conditionally compiles peripheral drivers based on
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
