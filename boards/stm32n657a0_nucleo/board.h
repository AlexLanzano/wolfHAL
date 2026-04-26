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
extern whal_I2c g_whalI2c;
extern whal_Crypto g_whalCrypto;
extern whal_Crypto g_whalHash;
extern whal_Irq g_whalIrq;
extern whal_Watchdog g_whalWatchdog;
extern whal_Eth g_whalEth;
extern whal_EthPhy g_whalEthPhy;
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
    ETH_RMII_REF_CLK_PIN,
    ETH_RMII_MDIO_PIN,
    ETH_RMII_MDC_PIN,
    ETH_RMII_CRS_DV_PIN,
    ETH_RMII_RXD0_PIN,
    ETH_RMII_RXD1_PIN,
    ETH_RMII_TX_EN_PIN,
    ETH_RMII_TXD0_PIN,
    ETH_RMII_TXD1_PIN,
    PIN_COUNT,
};

#define BOARD_LED_PIN 0

/* Ethernet PHY: LAN8742A on MDIO address 0 */
#define BOARD_ETH_PHY_ADDR 0
#define BOARD_ETH_PHY_ID1  0x0007
#define BOARD_ETH_PHY_ID2  0xC131

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
