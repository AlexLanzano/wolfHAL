_PERIPHERAL_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

BOARD_SOURCE += $(_PERIPHERAL_DIR)/peripheral.c

ifdef PERIPHERAL_SDHC_SPI_SDCARD32GB
CFLAGS += -DPERIPHERAL_SDHC_SPI_SDCARD32GB
BOARD_SOURCE += $(_PERIPHERAL_DIR)/block/sdhc_spi_sdcard32gb.c
BOARD_SOURCE += $(WHAL_DIR)/src/block/sdhc_spi.c
endif

ifdef PERIPHERAL_SPI_NOR_W25Q64
CFLAGS += -DPERIPHERAL_SPI_NOR_W25Q64
BOARD_SOURCE += $(_PERIPHERAL_DIR)/flash/spi_nor_w25q64.c
BOARD_SOURCE += $(WHAL_DIR)/src/flash/spi_nor.c
endif
