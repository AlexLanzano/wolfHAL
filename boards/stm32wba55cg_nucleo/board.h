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
extern whal_Flash g_whalFlash;
extern whal_Rng g_whalRng;
extern whal_I2c g_whalI2c;
extern whal_Crypto g_whalCrypto;
extern whal_Crypto g_whalHash;
extern whal_Irq g_whalIrq;
extern whal_Watchdog g_whalWatchdog;
#ifdef BOARD_DMA
extern whal_Dma g_whalDma1;
#endif

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
    I2C_SCL_PIN,
    I2C_SDA_PIN,
    PIN_COUNT,
};

/* LD2 (Green) on PA9 -- avoids conflict with SPI1_SCK on PB4 (LD1) */
#define BOARD_LED_PIN             0
#define BOARD_LED_PORT_OFFSET     0x000 /* GPIOA */
#define BOARD_LED_PIN_NUM         9
#define BOARD_FLASH_START_ADDR    0x08000000
#define BOARD_FLASH_SIZE          0x100000  /* 1 MB */
#define BOARD_FLASH_TEST_ADDR     0x080FE000
#define BOARD_FLASH_SECTOR_SZ     0x2000    /* 8 KB */

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
