#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/spi/stm32wb_spi.h>

extern whal_Clock g_whalClock;
extern whal_Gpio g_whalGpio;
extern whal_Timer g_whalTimer;
extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
extern whal_Stm32wbSpi_ComCfg g_whalSpiComCfg;
extern whal_Flash g_whalFlash;
extern whal_Rng g_whalRng;
extern whal_Crypto g_whalCrypto;

extern volatile uint32_t g_tick;

#define BOARD_LED_PIN         0
#define BOARD_FLASH_TEST_ADDR 0x08080000
#define BOARD_FLASH_SECTOR_SZ 0x1000

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
