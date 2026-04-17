_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = stm32wb
TESTS ?= clock gpio flash timer rng crypto block uart i2c dma irq

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

CFLAGS += -Wall -Werror $(INCLUDE) -g3 \
          -ffreestanding -nostdlib -mcpu=cortex-m4 \
          -DPLATFORM_STM32WB -MMD -MP \
          $(if $(DMA),-DBOARD_DMA) \
          $(if $(filter iwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_IWDG) \
          $(if $(filter wwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_WWDG) \
          $(if $(DMA),-DWHAL_CFG_UART_API_MAPPING_STM32WB_DMA,-DWHAL_CFG_UART_API_MAPPING_STM32WB) \
          -DWHAL_CFG_GPIO_API_MAPPING_STM32WB \
          -DWHAL_CFG_SPI_API_MAPPING_STM32WB \
          -DWHAL_CFG_I2C_API_MAPPING_STM32WB \
          -DWHAL_CFG_CLOCK_API_MAPPING_STM32WB_PLL \
          -DWHAL_CFG_RNG_API_MAPPING_STM32WB \
          -DWHAL_CFG_DMA_API_MAPPING_STM32WB \
          -DWHAL_CFG_CRYPTO_API_MAPPING_STM32WB_AES
LDFLAGS = --omagic -static

LINKER_SCRIPT ?= $(_BOARD_DIR)/linker.ld

INCLUDE += -I$(_BOARD_DIR) -I$(WHAL_DIR)/boards/peripheral

BOARD_SOURCE = $(_BOARD_DIR)/ivt.c
BOARD_SOURCE += $(_BOARD_DIR)/board.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/crypto.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/supply.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/sensor.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/watchdog.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/block.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/irq.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/irq/cortex_m4_nvic.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/stm32wb_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)

# Peripheral devices
include $(WHAL_DIR)/boards/peripheral/Makefile.inc
