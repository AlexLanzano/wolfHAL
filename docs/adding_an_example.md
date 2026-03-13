# Adding a New Example

This guide covers adding a new example application to wolfHAL.

## Overview

Each example is a standalone application with its own `main.c` and `Makefile`.
Board support is pulled in from the top-level `boards/` directory.

Examples may be platform-specific if they exercise platform-specific features,
but you should aim to make them as general as possible so they can run on
multiple boards.

## Step 1: Create the Directory

Create `examples/<example_name>/` with two files:

### main.c

```c
#include <wolfHAL/wolfHAL.h>
#include "board.h"

void main(void)
{
    if (Board_Init() != WHAL_SUCCESS)
        goto loop;

    /* Application code using g_whal* peripherals from board.h */

loop:
    while (1);
}
```

Key points:
- Include `wolfHAL/wolfHAL.h` for the wolfHAL API
- Include `board.h` for peripheral instances and board constants
- Call `Board_Init()` before using any peripherals
- Use the global peripheral instances (e.g., `g_whalGpio`, `g_whalUart`)
- Use board constants (e.g., `BOARD_LED_PIN`) for portability across boards

### Makefile

```makefile
WHAL_DIR = $(CURDIR)/../..

BOARD ?= stm32wb55xx_nucleo
BOARD_DIR = $(WHAL_DIR)/boards/$(BOARD)
BUILD_DIR = build/$(BOARD)

INCLUDE = -I$(WHAL_DIR)

include $(BOARD_DIR)/Makefile.inc

SOURCE = main.c
SOURCE += $(BOARD_SOURCE)

OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCE))
DEPENDS = $(OBJECTS:.o=.d)

all: $(BUILD_DIR)/$(notdir $(CURDIR)).bin

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(GCC) $(CFLAGS) -c -o $@ $<

.SECONDARY:
$(BUILD_DIR)/%.elf: $(OBJECTS) $(LINKER_SCRIPT)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) $^ -O binary $@

.PHONY: clean
clean:
	rm -rf build

-include $(DEPENDS)
```

This Makefile is the same for all examples. The board's `Makefile.inc` provides
the toolchain, flags, and platform sources.

## Step 2: Build

```sh
cd examples/<example_name>
make BOARD=<board>
```

The output binary is placed in `build/<board>/<example_name>.bin`.

## Adding Extra Source Files

If your example needs additional source files, add them to `SOURCE` in the
Makefile:

```makefile
SOURCE = main.c
SOURCE += helper.c
SOURCE += $(BOARD_SOURCE)
```
