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
