#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32n657a0.h>
#include <wolfHAL/eth_phy/lan8742a_eth_phy.h>
#include <wolfHAL/crypto/stm32n6_cryp.h>
#include <wolfHAL/crypto/stm32n6_hash.h>

extern whal_Clock g_whalClock;
extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
extern whal_I2c g_whalI2c;
extern whal_Crypto g_whalCrypto;
extern whal_AesEcb g_whalAesEcb;
extern whal_AesCbc g_whalAesCbc;
extern whal_AesCtr g_whalAesCtr;
extern whal_AesGcm g_whalAesGcm;
extern whal_AesGmac g_whalAesGmac;
extern whal_AesCcm g_whalAesCcm;
extern whal_Crypto g_whalHash;
extern whal_Sha1 g_whalSha1;
extern whal_Sha224 g_whalSha224;
extern whal_Sha256 g_whalSha256;
extern whal_HmacSha1 g_whalHmacSha1;
extern whal_HmacSha224 g_whalHmacSha224;
extern whal_HmacSha256 g_whalHmacSha256;
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

static const whal_Gpio whal_Stm32n6_Gpio_Dev = {
    .base = WHAL_STM32N657_GPIO_BASE,

    .cfg = (void *)&(const whal_Stm32n6_Gpio_Cfg){
        .pinCfg = (const whal_Stm32n6_Gpio_PinCfg[PIN_COUNT]){
            /* LD1 Green LED on PB0 */
            [LED_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_B, 0, WHAL_STM32N6_GPIO_MODE_OUT,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_LOW,
                WHAL_STM32N6_GPIO_PULL_NONE, 0),
            /* USART1 TX on PE5, AF7 */
            [UART_TX_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_E, 5, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_FAST,
                WHAL_STM32N6_GPIO_PULL_UP, 7),
            /* USART1 RX on PE6, AF7 */
            [UART_RX_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_E, 6, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_FAST,
                WHAL_STM32N6_GPIO_PULL_UP, 7),
            /* SPI1 SCK on PA5, AF5 */
            [SPI_SCK_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_A, 5, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_FAST,
                WHAL_STM32N6_GPIO_PULL_NONE, 5),
            /* SPI1 MISO on PA6, AF5 */
            [SPI_MISO_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_A, 6, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_FAST,
                WHAL_STM32N6_GPIO_PULL_NONE, 5),
            /* SPI1 MOSI on PA7, AF5 */
            [SPI_MOSI_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_A, 7, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_FAST,
                WHAL_STM32N6_GPIO_PULL_NONE, 5),
            /* SPI CS on PA4, output */
            [SPI_CS_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_A, 4, WHAL_STM32N6_GPIO_MODE_OUT,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_FAST,
                WHAL_STM32N6_GPIO_PULL_UP, 0),
            /* I2C1 SCL on PB6, AF4, open-drain */
            [I2C_SCL_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_B, 6, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32N6_GPIO_SPEED_FAST,
                WHAL_STM32N6_GPIO_PULL_UP, 4),
            /* I2C1 SDA on PB7, AF4, open-drain */
            [I2C_SDA_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_B, 7, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32N6_GPIO_SPEED_FAST,
                WHAL_STM32N6_GPIO_PULL_UP, 4),
            /* RMII REF_CLK on PF7, AF11 */
            [ETH_RMII_REF_CLK_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_F, 7, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
            /* RMII MDIO on PF4, AF11 */
            [ETH_RMII_MDIO_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_F, 4, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
            /* RMII MDC on PG11, AF11 */
            [ETH_RMII_MDC_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_G, 11, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
            /* RMII CRS_DV on PF10, AF11 */
            [ETH_RMII_CRS_DV_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_F, 10, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
            /* RMII RXD0 on PF14, AF11 */
            [ETH_RMII_RXD0_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_F, 14, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
            /* RMII RXD1 on PF15, AF11 */
            [ETH_RMII_RXD1_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_F, 15, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
            /* RMII TX_EN on PF11, AF11 */
            [ETH_RMII_TX_EN_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_F, 11, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
            /* RMII TXD0 on PF12, AF11 */
            [ETH_RMII_TXD0_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_F, 12, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
            /* RMII TXD1 on PF13, AF11 */
            [ETH_RMII_TXD1_PIN] = WHAL_STM32N6_GPIO_PIN(
                WHAL_STM32N6_GPIO_PORT_F, 13, WHAL_STM32N6_GPIO_MODE_ALTFN,
                WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32N6_GPIO_SPEED_HIGH,
                WHAL_STM32N6_GPIO_PULL_NONE, 11),
        },
        .pinCount = PIN_COUNT,
    },
};

/* RNG singleton — referenced by stm32n6_rng.c directly. */
static const whal_Rng whal_Stm32h5_Rng_Dev = {
    .base = WHAL_STM32N657_RNG_BASE,
    .cfg  = (void *)&(const whal_Stm32n6_Rng_Cfg){
        .timeout = &g_whalTimeout,
    },
};

/* ETH descriptor rings + buffer pool. Live in board.c with the .axisram1
 * section attribute; declared here so the ETH singleton cfg below can
 * take their addresses at compile time. */
#define BOARD_ETH_TX_DESC_COUNT 4
#define BOARD_ETH_RX_DESC_COUNT 4
#define BOARD_ETH_TX_BUF_SIZE   1536
#define BOARD_ETH_RX_BUF_SIZE   1536

extern whal_Stm32n6_Eth_TxDesc ethTxDescs[BOARD_ETH_TX_DESC_COUNT];
extern whal_Stm32n6_Eth_RxDesc ethRxDescs[BOARD_ETH_RX_DESC_COUNT];
extern uint8_t ethTxBufs[BOARD_ETH_TX_DESC_COUNT * BOARD_ETH_TX_BUF_SIZE];
extern uint8_t ethRxBufs[BOARD_ETH_RX_DESC_COUNT * BOARD_ETH_RX_BUF_SIZE];

/* ETH singleton — referenced by stm32n6_eth.c directly. */
static const whal_Eth whal_Stm32h5_Eth_Dev = {
    .base    = WHAL_STM32N657_ETH_BASE,
    .macAddr = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x01},
    .cfg     = (void *)&(const whal_Stm32n6_Eth_Cfg){
        .txDescs     = ethTxDescs,
        .txBufs      = ethTxBufs,
        .txDescCount = BOARD_ETH_TX_DESC_COUNT,
        .txBufSize   = BOARD_ETH_TX_BUF_SIZE,
        .rxDescs     = ethRxDescs,
        .rxBufs      = ethRxBufs,
        .rxDescCount = BOARD_ETH_RX_DESC_COUNT,
        .rxBufSize   = BOARD_ETH_RX_BUF_SIZE,
        .timeout     = &g_whalTimeout,
    },
};

/* LAN8742A PHY singleton — referenced by lan8742a_eth_phy.c directly. */
static const whal_EthPhy whal_Lan8742a_Dev = {
    .eth  = NULL,
    .addr = BOARD_ETH_PHY_ADDR,
    .cfg  = (void *)&(const whal_Lan8742a_Cfg){
        .timeout = &g_whalTimeout,
    },
};

/* NVIC singleton — referenced by cortex_m4_nvic.c directly. */
static const whal_Irq whal_Nvic_Dev = {
    .base = WHAL_CORTEX_M55_NVIC_BASE,
    /* .driver: direct API mapping */
};

/* SysTick singleton — referenced by systick.c directly. */
static const whal_Timer whal_SysTick_Dev = {
    .base = WHAL_CORTEX_M55_SYSTICK_BASE,
    /* .driver: direct API mapping */

    .cfg = (void *)&(const whal_SysTick_Cfg){
        .cyclesPerTick = 64000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
