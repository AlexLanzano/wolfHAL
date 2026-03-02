PLATFORM = pic32cz
TESTS = clock gpio flash timer

GCC = $(GCC_PATH)arm-none-eabi-gcc
LD = $(GCC_PATH)arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)arm-none-eabi-objcopy

CFLAGS += -Wall -Werror $(INCLUDE) -g3 \
          -ffreestanding -nostdlib -mcpu=cortex-m7 \
          -DPLATFORM_PIC32CZ -MMD -MP
LDFLAGS = --omagic -static

BOARD_SOURCE = $(BOARD_DIR)/ivt.c
BOARD_SOURCE += $(BOARD_DIR)/board.c
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/gpio.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/clock.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/uart.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/timer.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/supply.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/flash.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/pic32cz_*.c)
BOARD_SOURCE += $(wildcard $(WHAL_DIR)/src/*/systick.c)
