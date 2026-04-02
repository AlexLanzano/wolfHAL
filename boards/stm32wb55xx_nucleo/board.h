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
extern whal_Crypto g_whalCrypto;
extern whal_Irq g_whalIrq;

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

#define BOARD_LED_PIN             0
#define BOARD_LED_PORT_OFFSET     0x400 /* GPIOB */
#define BOARD_LED_PIN_NUM         5
#define BOARD_FLASH_START_ADDR    0x08000000
#define BOARD_FLASH_SIZE          0x80000  /* 512 KB (upper half reserved for BLE stack) */
#define BOARD_FLASH_TEST_ADDR     0x0807F000
#define BOARD_FLASH_SECTOR_SZ     0x1000

enum {
    BOARD_CRYPTO_AES_ECB,
    BOARD_CRYPTO_AES_CBC,
    BOARD_CRYPTO_AES_CTR,
    BOARD_CRYPTO_AES_GCM,
    BOARD_CRYPTO_AES_GMAC,
    BOARD_CRYPTO_AES_CCM,
    BOARD_CRYPTO_OP_COUNT,
};

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
