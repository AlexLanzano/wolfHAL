_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = stm32n6
TESTS ?= clock gpio timer rng eth uart spi i2c irq crypto

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

CFLAGS += -Wall -Werror $(INCLUDE) -g3 -Os -ffunction-sections -fdata-sections \
          -ffreestanding -nostdlib -mcpu=cortex-m55 -mthumb \
          -DPLATFORM_STM32N6 -MMD -MP \
          $(if $(DMA),-DBOARD_DMA) \
          $(if $(filter iwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_IWDG) \
          $(if $(filter wwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_WWDG) \
          -DWHAL_CFG_STM32N6_GPIO_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32N6_RCC_HSI_DRIVER \
          -DWHAL_CFG_STM32N6_RCC_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32N6_UART_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32N6_SPI_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32N6_I2C_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32N6_RNG_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32N6_GPDMA_DIRECT_API_MAPPING \
          -DWHAL_CFG_STM32N6_ETH_DIRECT_API_MAPPING \
          -DWHAL_CFG_LAN8742A_ETH_PHY_DIRECT_API_MAPPING \
          -DWHAL_CFG_CRYPTO_AES_ECB  -DWHAL_CFG_CRYPTO_AES_CBC  \
          -DWHAL_CFG_CRYPTO_AES_CTR  -DWHAL_CFG_CRYPTO_AES_GCM  \
          -DWHAL_CFG_CRYPTO_AES_GMAC -DWHAL_CFG_CRYPTO_AES_CCM  \
          -DWHAL_CFG_CRYPTO_SHA1     -DWHAL_CFG_CRYPTO_SHA224    \
          -DWHAL_CFG_CRYPTO_SHA256   -DWHAL_CFG_CRYPTO_HMAC_SHA1 \
          -DWHAL_CFG_CRYPTO_HMAC_SHA224 -DWHAL_CFG_CRYPTO_HMAC_SHA256
LDFLAGS = --omagic -static --gc-sections

LINKER_SCRIPT ?= $(_BOARD_DIR)/linker.ld

INCLUDE += -I$(_BOARD_DIR) -I$(WHAL_DIR)/boards/peripheral

BOARD_SOURCE = $(_BOARD_DIR)/ivt.c
BOARD_SOURCE += $(_BOARD_DIR)/board.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/sensor.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/watchdog.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/crypto.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/block.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/irq.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/irq/cortex_m4_nvic.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/stm32n6_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/lan8742a_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)

# Peripheral devices
include $(WHAL_DIR)/boards/peripheral/board.mk

# Flash via openocd: make flash BOARD=<board> IMAGE=<path/to/image>
# N6 has no internal flash — load the ELF into SRAM, fetch SP/PC from the
# IVT at 0x34000000, and resume. Requires the board's BOOT pins set to
# development boot mode.
OPENOCD ?= /opt/openocd/bin/openocd
OPENOCD_INTERFACE ?= interface/stlink.cfg
OPENOCD_TARGET ?= target/stm32n6x.cfg

.PHONY: flash
flash:
	@test -n "$(IMAGE)" || { echo "IMAGE=<path/to/image> required" >&2; exit 1; }
	$(OPENOCD) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) \
	    -c "init; reset halt; load_image $(IMAGE); resume"
