_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = stm32h5
TESTS ?= clock gpio timer flash rng eth uart

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

CFLAGS += -Wall -Werror $(INCLUDE) -g3 \
          -ffreestanding -nostdlib -mcpu=cortex-m33 \
          -DPLATFORM_STM32H5 -MMD -MP \
          -DWHAL_CFG_GPIO_API_MAPPING_STM32H5 \
          -DWHAL_CFG_CLOCK_API_MAPPING_STM32H5_PLL \
          -DWHAL_CFG_UART_API_MAPPING_STM32H5 \
          -DWHAL_CFG_SPI_API_MAPPING_STM32H5 \
          -DWHAL_CFG_RNG_API_MAPPING_STM32H5 \
          -DWHAL_CFG_ETH_API_MAPPING_STM32H5 \
          -DWHAL_CFG_ETH_PHY_API_MAPPING_LAN8742A
LDFLAGS = --omagic -static

LINKER_SCRIPT ?= $(_BOARD_DIR)/linker.ld

INCLUDE += -I$(_BOARD_DIR) -I$(WHAL_DIR)/boards/peripheral

BOARD_SOURCE = $(_BOARD_DIR)/ivt.c
BOARD_SOURCE += $(_BOARD_DIR)/board.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/supply.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/crypto.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/block.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/stm32h5_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/lan8742a_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)

# Peripheral devices
include $(WHAL_DIR)/boards/peripheral/Makefile.inc
