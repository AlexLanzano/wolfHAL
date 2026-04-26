_BOARD_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

PLATFORM = stm32n6
TESTS ?= clock gpio timer rng eth uart spi i2c irq crypto

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

CFLAGS += -Wall -Werror $(INCLUDE) -g3 \
          -ffreestanding -nostdlib -mcpu=cortex-m55 -mthumb \
          -DPLATFORM_STM32N6 -MMD -MP \
          $(if $(DMA),-DBOARD_DMA) \
          $(if $(filter iwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_IWDG) \
          $(if $(filter wwdg,$(WATCHDOG)),-DBOARD_WATCHDOG_WWDG) \
          -DWHAL_CFG_GPIO_API_MAPPING_STM32N6 \
          -DWHAL_CFG_CLOCK_API_MAPPING_STM32N6_HSI \
          -DWHAL_CFG_UART_API_MAPPING_STM32N6 \
          -DWHAL_CFG_SPI_API_MAPPING_STM32N6 \
          -DWHAL_CFG_I2C_API_MAPPING_STM32N6 \
          -DWHAL_CFG_RNG_API_MAPPING_STM32N6 \
          -DWHAL_CFG_DMA_API_MAPPING_STM32N6 \
          -DWHAL_CFG_ETH_API_MAPPING_STM32N6 \
          -DWHAL_CFG_ETH_PHY_API_MAPPING_LAN8742A \
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
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/supply.c)
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
include $(WHAL_DIR)/boards/peripheral/Makefile.inc
