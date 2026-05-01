_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = stm32wba
TESTS ?= clock gpio flash timer rng crypto uart spi i2c irq

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

CFLAGS += -Wall -Werror $(INCLUDE) -g3 \
          -ffreestanding -nostdlib -mcpu=cortex-m33 -mthumb \
          -DPLATFORM_STM32WBA -MMD -MP \
          -DWHAL_CFG_STM32WBA_RCC_PLL_DRIVER \
          $(if $(DMA),-DBOARD_DMA) \
          $(if $(filter iwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_IWDG) \
          $(if $(filter wwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_WWDG) \
          -DWHAL_CFG_CRYPTO_AES_ECB  -DWHAL_CFG_CRYPTO_AES_CBC  \
          -DWHAL_CFG_CRYPTO_AES_CTR  -DWHAL_CFG_CRYPTO_AES_GCM  \
          -DWHAL_CFG_CRYPTO_AES_GMAC -DWHAL_CFG_CRYPTO_AES_CCM  \
          -DWHAL_CFG_CRYPTO_SHA1     -DWHAL_CFG_CRYPTO_SHA224    \
          -DWHAL_CFG_CRYPTO_SHA256   -DWHAL_CFG_CRYPTO_HMAC_SHA1 \
          -DWHAL_CFG_CRYPTO_HMAC_SHA224 -DWHAL_CFG_CRYPTO_HMAC_SHA256
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
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/spi.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/i2c.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/sensor.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/watchdog.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/rng.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/crypto.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/block.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/dma.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/irq.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/irq/cortex_m4_nvic.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/stm32wba_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)

# Peripheral devices
include $(WHAL_DIR)/boards/peripheral/board.mk

# Flash via openocd: make flash BOARD=<board> IMAGE=<path/to/image>
OPENOCD ?= /opt/openocd/bin/openocd
OPENOCD_INTERFACE ?= interface/stlink.cfg
OPENOCD_TARGET ?= target/stm32wba5x.cfg

.PHONY: flash
flash:
	@test -n "$(IMAGE)" || { echo "IMAGE=<path/to/image> required" >&2; exit 1; }
	$(OPENOCD) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) \
	    -c "program $(IMAGE) verify reset exit"
