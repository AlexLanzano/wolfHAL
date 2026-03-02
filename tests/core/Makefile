WHAL_DIR = $(CURDIR)/../..

INCLUDE = -I$(WHAL_DIR)
CFLAGS = -Wall -Werror $(INCLUDE) -g3

TEST_SRC = main.c test_bitops.c test_dispatch.c

WHAL_SRC = $(WHAL_DIR)/src/clock/clock.c \
           $(WHAL_DIR)/src/gpio/gpio.c \
           $(WHAL_DIR)/src/uart/uart.c \
           $(WHAL_DIR)/src/flash/flash.c \
           $(WHAL_DIR)/src/timer/timer.c \
           $(WHAL_DIR)/src/spi/spi.c \
           $(WHAL_DIR)/src/rng/rng.c \
           $(WHAL_DIR)/src/supply/supply.c

SOURCE = $(TEST_SRC) $(WHAL_SRC)
OBJECTS = $(patsubst %.c,%.o,$(SOURCE))

TARGET = test_core

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

.PHONY: run clean

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
