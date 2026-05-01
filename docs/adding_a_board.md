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

static whal_Myplatform_Gpio_PinCfg pinCfg[] = { /* ... */ };

static whal_Myplatform_Gpio_Cfg gpioConfig = {
    .pinCfg  = pinCfg,
    .pinCount = sizeof(pinCfg) / sizeof(pinCfg[0]),
};

whal_Gpio g_whalGpio = {
    .regmap = { WHAL_MYPLATFORM_GPIO_REGMAP },
    .cfg = &gpioConfig,
};

static const whal_Myplatform_Clock_PeriphClk g_periphClks[] = {
    {MY_PLATFORM_GPIO_CLOCK},
    {MY_PLATFORM_UART_CLOCK},
};
#define PERIPH_CLK_COUNT \
    (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

whal_Error Board_Init(void)
{
    whal_Error err;

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

    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Myplatform_Clock_EnablePeriphClk(&g_whalClock,
                                                   &g_periphClks[i]);
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

    return WHAL_SUCCESS;
}
```

```c
whal_Error Board_Deinit(void)
{
    whal_Error err;

    whal_Timer_Stop(&g_whalTimer);
    whal_Timer_Deinit(&g_whalTimer);
    whal_Uart_Deinit(&g_whalUart);
    whal_Gpio_Deinit(&g_whalGpio);

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
          -DWHAL_CFG_GPIO_API_MAPPING_MYPLATFORM \
          -DWHAL_CFG_CLOCK_API_MAPPING_MYPLATFORM \
          -DWHAL_CFG_UART_API_MAPPING_MYPLATFORM
LDFLAGS = -mcpu=cortex-m4 -mthumb -nostdlib -lgcc

LINKER_SCRIPT = $(_BOARD_DIR)/linker.ld

INCLUDE += -I$(_BOARD_DIR)

BOARD_SOURCE  = $(_BOARD_DIR)/board.c
BOARD_SOURCE += $(_BOARD_DIR)/ivt.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*.c)
# Dispatch sources for mapped types are excluded; keep dispatch sources
# for types whose drivers don't use direct API mapping (e.g., flash on
# boards that mix on-chip flash with external SPI NOR).
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/myplatform_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)
```

### linker.ld

Linker script defining the memory layout for your board's MCU. Must define
FLASH and RAM regions, place `.isr_vector` at the start of FLASH, and set up
`.text`, `.data`, and `.bss` sections.

### ivt.c (ARM targets)

Interrupt vector table and `Reset_Handler`. The reset handler copies `.data`
from FLASH to RAM, zeroes `.bss`, and calls `main()`. This file is specific to
ARM Cortex-M targets. Other architectures will need their own startup code.
