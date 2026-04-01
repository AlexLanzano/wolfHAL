#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>

extern whal_Clock g_whalClock;
extern whal_Gpio g_whalGpio;
extern whal_Timer g_whalTimer;
extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
extern whal_Rng g_whalRng;

extern whal_Timeout g_whalTimeout;
extern volatile uint32_t g_tick;

enum {
    LED_PIN,
    UART_TX_PIN,
    UART_RX_PIN,
    SPI_SCK_PIN,
    SPI_MISO_PIN,
    SPI_MOSI_PIN,
    SPI_CS_PIN,
    PIN_COUNT,
};

#define BOARD_LED_PIN 0

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
