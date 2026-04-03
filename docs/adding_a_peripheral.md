# Adding a Peripheral

This guide covers how to add an external peripheral device to the wolfHAL
peripheral system. Peripherals are bus-attached devices (e.g., SPI-NOR flash,
SD cards) that live in `boards/peripheral/` and are opt-in at build time.

## Overview

A peripheral consists of three parts:

1. A **device configuration file** that instantiates the device with
   board-specific parameters (SPI bus, CS pin, clock speed, etc.)
2. An **entry in the peripheral registry** (`peripheral.c`) so that board init
   and tests can discover the device
3. A **Makefile.inc entry** to conditionally compile the peripheral and its
   driver source

## File Layout

Peripherals are organized by device type under `boards/peripheral/`:

```
boards/peripheral/
  peripheral.h          # Registry structs and extern arrays
  peripheral.c          # Registry arrays (g_peripheralBlock[], g_peripheralFlash[], g_peripheralSensor[])
  Makefile.inc          # Conditional build rules
  block/
    sdhc_spi_sdcard32gb.h
    sdhc_spi_sdcard32gb.c
  flash/
    spi_nor_w25q64.h
    spi_nor_w25q64.c
  sensor/imu/
    bmi270.h
    bmi270.c
```

## Step 1: Create the Device Configuration

Create a header and source file for your device under the appropriate type
directory.

### Header

Declare the global device instance:

```c
#ifndef BOARD_SPI_NOR_W25Q64_H
#define BOARD_SPI_NOR_W25Q64_H

#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/flash/spi_nor.h>

extern whal_Flash g_whalSpiNorW25q64;

#endif
```

### Source

Define the device instance with its configuration. The device references board
globals (`g_whalSpi`, `g_whalGpio`, `SPI_CS_PIN`, `g_whalTimeout`) from
`board.h`:

```c
#include "spi_nor_w25q64.h"
#include <wolfHAL/flash/spi_nor.h>
#include "board.h"

#define W25Q64_PAGE_SZ  256
#define W25Q64_CAPACITY (8 * 1024 * 1024)

static whal_Spi_ComCfg g_w25q64ComCfg = {
    .freq = 25000000,
    .mode = WHAL_SPI_MODE_0,
    .wordSz = 8,
    .dataLines = 1,
};

whal_Flash g_whalSpiNorW25q64 = {
    .driver = &whal_SpiNor_Driver,
    .cfg = &(whal_SpiNor_Cfg) {
        .spiDev = &g_whalSpi,
        .spiComCfg = &g_w25q64ComCfg,
        .gpioDev = &g_whalGpio,
        .csPin = SPI_CS_PIN,
        .timeout = &g_whalTimeout,
        .pageSz = W25Q64_PAGE_SZ,
        .capacity = W25Q64_CAPACITY,
    },
};
```

## Step 2: Register in peripheral.c

Add a conditional include and an entry in the appropriate registry array.

In `peripheral.c`:

```c
#ifdef PERIPHERAL_SPI_NOR_W25Q64
#include "flash/spi_nor_w25q64.h"
#endif
```

And add an entry to the matching array (before the sentinel):

```c
whal_PeripheralFlash_Cfg g_peripheralFlash[] = {
#ifdef PERIPHERAL_SPI_NOR_W25Q64
    {
        .name = "spi_nor_w25q64",
        .dev = &g_whalSpiNorW25q64,
        .sectorSz = 4096,
    },
#endif
    {0}, /* sentinel */
};
```

The registry structs are defined in `peripheral.h`:

- `whal_PeripheralBlock_Cfg` for block devices (`g_peripheralBlock[]`)
- `whal_PeripheralFlash_Cfg` for flash devices (`g_peripheralFlash[]`)
- `whal_PeripheralSensor_Cfg` for sensor devices (`g_peripheralSensor[]`)

Each array is terminated by a zero sentinel so that board init and test code
can iterate without knowing the count.

## Step 3: Add Build Rules

In `boards/peripheral/Makefile.inc`, add a conditional block for your
peripheral. The block checks whether the peripheral name appears in the
`PERIPHERALS` variable and adds the define, config source, and driver source:

```makefile
ifneq ($(filter mydevice,$(PERIPHERALS)),)
CFLAGS += -DPERIPHERAL_MYDEVICE
BOARD_SOURCE += $(_PERIPHERAL_DIR)/type/mydevice.c
BOARD_SOURCE += $(WHAL_DIR)/src/type/mydevice_driver.c
endif
```

This compiles both the peripheral configuration and the underlying driver
source when the peripheral is enabled.

## Building

Enable peripherals using the `PERIPHERALS` variable:

```
make BOARD=stm32wb55xx_nucleo PERIPHERALS="spi_nor_w25q64"
```

Multiple peripherals can be enabled simultaneously:

```
make BOARD=stm32wb55xx_nucleo PERIPHERALS="spi_nor_w25q64 bmi270"
```

## Testing

Peripheral devices are automatically picked up by their matching test suite.
Flash peripherals are tested by the `flash` test, and block peripherals by the
`block` test. See [Adding a Test](adding_a_test.md) for details on the test
framework.

## Naming Convention

- Flag: `PERIPHERAL_<NAME>` (e.g., `PERIPHERAL_BMI270`)
- PERIPHERALS variable: lowercase name (e.g., `bmi270`, `spi_nor_w25q64`)
- Directory: `boards/peripheral/<type>/` (e.g., `flash/`, `block/`, `sensor/imu/`)
- Files: `<name>.h` and `<name>.c`
- Global instance: `g_whal<Name>` (e.g., `g_whalBmi270`, `g_whalSpiNorW25q64`)
