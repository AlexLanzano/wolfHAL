WHAL_DIR = $(CURDIR)/..

BOARD ?= stm32wb55xx_nucleo
BOARD_DIR = boards/$(BOARD)
BUILD_DIR = build/$(BOARD)
INCLUDE = -I$(WHAL_DIR) -I$(BOARD_DIR) -I.

include $(BOARD_DIR)/Makefile.inc

TESTS ?= clock gpio flash timer

uc = $(shell echo $(1) | tr a-z A-Z)
CFLAGS += $(foreach t,$(TESTS),-DWHAL_TEST_ENABLE_$(call uc,$(t)))
CFLAGS += $(foreach t,$(TESTS),$(if $(wildcard $(t)/test_$(PLATFORM)_$(t).c),-DWHAL_TEST_ENABLE_$(call uc,$(t))_PLATFORM))

SOURCE = main.c
SOURCE += $(BOARD_SOURCE)
SOURCE += $(foreach t,$(TESTS),$(t)/test_$(t).c)
SOURCE += $(foreach t,$(TESTS),$(wildcard $(t)/test_$(PLATFORM)_$(t).c))

OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCE))
DEPENDS = $(OBJECTS:.o=.d)

LINKER_SCRIPT = $(BOARD_DIR)/linker.ld

all: $(BUILD_DIR)/test_hw.bin

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
