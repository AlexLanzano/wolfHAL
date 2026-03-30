_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = stm32f4
TESTS ?= clock gpio timer flash

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

CFLAGS += -Wall -Werror $(INCLUDE) -g3 \
          -ffreestanding -nostdlib -mcpu=cortex-m4 -mfloat-abi=hard \
          -mfpu=fpv4-sp-d16 -mthumb \
          -DPLATFORM_STM32F4 -MMD -MP
LDFLAGS = --omagic -static

LINKER_SCRIPT ?= $(_BOARD_DIR)/linker.ld

INCLUDE += -I$(_BOARD_DIR) -I$(WHAL_DIR)/boards/peripheral

BOARD_SOURCE = $(_BOARD_DIR)/ivt.c
BOARD_SOURCE += $(_BOARD_DIR)/board.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/gpio.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/clock.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/uart.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/supply.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/spi.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/rng.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/crypto.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/block.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/stm32f4_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)

# Peripheral devices
include $(WHAL_DIR)/boards/peripheral/Makefile.inc
