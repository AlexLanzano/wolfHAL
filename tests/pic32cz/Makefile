WHAL_DIR = $(PWD)/../../
EXAMPLE_DIR = $(WHAL_DIR)/examples/pic32cz

INCLUDE = -I$(WHAL_DIR) -I$(EXAMPLE_DIR)
CFLAGS = -Wall -Werror $(INCLUDE) -g3 \
         -ffreestanding -nostdlib -mcpu=cortex-m7 \
         -MMD -MF $(DEPDIR)/$*.d
LDFLAGS = -static

DEPDIR = .deps/

# Test sources
TEST_SRC = test_main.c test_clock.c test_gpio.c test_flash.c test_timer.c

# Board config and IVT from the example
BOARD_SRC = $(EXAMPLE_DIR)/pic32cz_curiosity_ultra.c \
            $(EXAMPLE_DIR)/ivt.c

# All wolfHAL sources (generic + platform drivers)
WHAL_SRC = $(wildcard $(WHAL_DIR)/src/*.c) \
           $(wildcard $(WHAL_DIR)/src/*/*.c)

SOURCE = $(TEST_SRC) $(BOARD_SRC) $(WHAL_SRC)
OBJECTS = $(patsubst %.c,%.o,$(SOURCE))
DEPENDS = $(patsubst %.c,$(DEPDIR)/%.d,$(SOURCE))

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

LINKER_SCRIPT = $(EXAMPLE_DIR)/linker.ld

all: test_hw.bin

%.d:
	@mkdir -p $(@D)

%.o: %.c Makefile
	$(GCC) $(CFLAGS) -c -o $@ $<

.SECONDARY:
%.elf: $(OBJECTS) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $(OBJECTS)

%.bin: %.elf
	$(OBJCOPY) $^ -O binary $@

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(OBJECTS) test_hw.bin test_hw.elf

-include $(DEPENDS)
