_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = stm32f0
TESTS ?= clock gpio timer flash uart spi

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-gcc
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

CFLAGS += -Wall -Werror $(INCLUDE) -g3 \
          -ffreestanding -nostdlib -mcpu=cortex-m0 -mthumb \
          -DPLATFORM_STM32F0 -MMD -MP \
          -DWHAL_CFG_STM32F0_GPIO_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32F0_RCC_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32F0_UART_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32F0_SPI_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32F0_I2C_DIRECT_API_MAPPING \
          $(if $(filter iwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_IWDG)
LDFLAGS = -mcpu=cortex-m0 -mthumb -ffreestanding -nostartfiles \
          -Wl,--omagic -static

LINKER_SCRIPT ?= $(_BOARD_DIR)/linker.ld

INCLUDE += -I$(_BOARD_DIR) -I$(WHAL_DIR)/boards/peripheral

BOARD_SOURCE = $(_BOARD_DIR)/ivt.c
BOARD_SOURCE += $(_BOARD_DIR)/board.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/rng.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/crypto.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/sensor.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/block.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/watchdog.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/stm32f0_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)

# Peripheral devices
include $(WHAL_DIR)/boards/peripheral/board.mk

# Flash via openocd: make flash BOARD=<board> IMAGE=<path/to/image>
OPENOCD ?= /opt/openocd/bin/openocd
OPENOCD_INTERFACE ?= interface/stlink.cfg
OPENOCD_TARGET ?= target/stm32f0x.cfg

.PHONY: flash
flash:
	@test -n "$(IMAGE)" || { echo "IMAGE=<path/to/image> required" >&2; exit 1; }
	$(OPENOCD) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) \
	    -c "program $(IMAGE) verify reset exit"
