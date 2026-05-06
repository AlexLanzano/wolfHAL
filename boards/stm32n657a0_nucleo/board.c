/* Board configuration for the STM32N657A0 Nucleo-144 (NUCLEO-N657X0-Q) */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32n657a0.h>
#include <wolfHAL/eth_phy/lan8742a_eth_phy.h>
#include "peripheral.h"

/* SysTick timing */
volatile uint32_t g_tick = 0;

void SysTick_Handler(void)
{
    g_tick++;
}

uint32_t Board_GetTick(void)
{
    return g_tick;
}

whal_Timeout g_whalTimeout = {
    .timeoutTicks = 1000, /* 1s timeout */
    .GetTick = Board_GetTick,
};

/* IRQ */
whal_Irq g_whalIrq = {
    .base = WHAL_CORTEX_M55_NVIC_BASE,
    .driver = WHAL_CORTEX_M55_NVIC_DRIVER,
};

/* Clock: HSI at 64 MHz (default after reset).
 * The STM32N6 boots from ROM into HSI; PLL setup is complex (PLL1+IC dividers).
 * For initial bring-up, run at HSI 64 MHz. */
/* API is directly mapped */
whal_Clock g_whalClock = {
    .base = WHAL_STM32N657_RCC_BASE,
};

static const whal_Stm32n6_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32N657_GPIOA_CLOCK},
    {WHAL_STM32N657_GPIOB_CLOCK},
    {WHAL_STM32N657_GPIOE_CLOCK},
    {WHAL_STM32N657_GPIOF_CLOCK},
    {WHAL_STM32N657_GPIOG_CLOCK},
    {WHAL_STM32N657_USART1_CLOCK},
    {WHAL_STM32N657_SPI1_CLOCK},
    {WHAL_STM32N657_I2C1_CLOCK},
    {WHAL_STM32N657_RNG_CLOCK},
    {WHAL_STM32N657_CRYP_CLOCK},
    {WHAL_STM32N657_HASH_CLOCK},
#ifdef BOARD_WATCHDOG_WWDG
    {WHAL_STM32N657_WWDG_CLOCK},
#endif
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

static const whal_Stm32n6_Rcc_PeriphClk g_ethClocks[] = {
    {WHAL_STM32N657_ETH1MAC_CLOCK},
    {WHAL_STM32N657_ETH1TX_CLOCK},
    {WHAL_STM32N657_ETH1RX_CLOCK},
    {WHAL_STM32N657_ETH1_CLOCK},
};
#define ETH_PERIPH_CLK_COUNT (sizeof(g_ethClocks) / sizeof(g_ethClocks[0]))

/* GPIO */
/* API is directly mapped */
whal_Gpio g_whalGpio = {
    .base = WHAL_STM32N657_GPIO_BASE,

    .cfg = &(whal_Stm32n6_Gpio_Cfg) {
        .pinCfg = (whal_Stm32n6_Gpio_PinCfg[PIN_COUNT]) {
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

/* I2C */
/* API is directly mapped */
whal_I2c g_whalI2c = {
    .base = WHAL_STM32N657_I2C1_BASE,

    .cfg = &(whal_Stm32n6_I2c_Cfg) {
        .pclk = 64000000,
        .timeout = &g_whalTimeout,
    },
};

/* SPI */
/* API is directly mapped */
whal_Spi g_whalSpi = {
    .base = WHAL_STM32N657_SPI1_BASE,

    .cfg = &(whal_Stm32n6_Spi_Cfg) {
        .pclk = 64000000,
        .timeout = &g_whalTimeout,
    },
};

/* Timer (SysTick at 64 MHz HSI) */
whal_Timer g_whalTimer = {
    .base = WHAL_CORTEX_M55_SYSTICK_BASE,
    .driver = WHAL_CORTEX_M55_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 64000000 / 1000, /* 64 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* DMA */
#ifdef BOARD_DMA
/* API is directly mapped */
whal_Dma g_whalDma1 = {
    .base = WHAL_STM32N657_GPDMA1_BASE,
    .cfg = &(whal_Stm32n6_Gpdma_Cfg){
        .numChannels = 16,
        .timeout = &g_whalTimeout,
    },
};

static const whal_Stm32n6_Rcc_PeriphClk g_dmaClock = {WHAL_STM32N657_GPDMA1_CLOCK};
#endif

/* UART (USART1 via VCP at 115200 baud, 64 MHz HSI) */
/* API is directly mapped */
whal_Uart g_whalUart = {
    .base = WHAL_STM32N657_USART1_BASE,

    .cfg = &(whal_Stm32n6_Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32N6_UART_BRR(32000000, 115200),
    },
};

/* RNG */
/* API is directly mapped */
whal_Rng g_whalRng = {
    .base = WHAL_STM32N657_RNG_BASE,

    .cfg = &(whal_Stm32n6_Rng_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

/* Crypto (CRYP hardware accelerator) */
whal_Crypto g_whalCrypto = {
    .base = WHAL_STM32N657_CRYP_BASE,
    .driver = &whal_Stm32n6_Cryp_CryptoDriver,

    .cfg = &(whal_Stm32n6_Cryp_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

whal_AesEcb g_whalAesEcb = {
    .crypto = &g_whalCrypto,
    /* .driver: direct API mapping */
};

whal_AesCbc g_whalAesCbc = {
    .crypto = &g_whalCrypto,
    /* .driver: direct API mapping */
};

whal_AesCtr g_whalAesCtr = {
    .crypto = &g_whalCrypto,
    /* .driver: direct API mapping */
};

static whal_Stm32n6_AesGcm_State g_aesGcmState;

whal_AesGcm g_whalAesGcm = {
    .crypto = &g_whalCrypto,
    /* .driver: direct API mapping */
    .state = &g_aesGcmState,
};

whal_AesGmac g_whalAesGmac = {
    .crypto = &g_whalCrypto,
    /* .driver: direct API mapping */
};

static whal_Stm32n6_AesCcm_State g_aesCcmState;

whal_AesCcm g_whalAesCcm = {
    .crypto = &g_whalCrypto,
    /* .driver: direct API mapping */
    .state = &g_aesCcmState,
};

/* Hash (HASH hardware accelerator) */
whal_Crypto g_whalHash = {
    .base = WHAL_STM32N657_HASH_BASE,
    .driver = &whal_Stm32n6_Hash_CryptoDriver,

    .cfg = &(whal_Stm32n6_Hash_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

whal_Sha1 g_whalSha1 = {
    .crypto = &g_whalHash,
    /* .driver: direct API mapping */
};

whal_Sha224 g_whalSha224 = {
    .crypto = &g_whalHash,
    /* .driver: direct API mapping */
};

whal_Sha256 g_whalSha256 = {
    .crypto = &g_whalHash,
    /* .driver: direct API mapping */
};

static whal_Stm32n6_HmacSha1_State g_hmacSha1State;

whal_HmacSha1 g_whalHmacSha1 = {
    .crypto = &g_whalHash,
    /* .driver: direct API mapping */
    .state = &g_hmacSha1State,
};

static whal_Stm32n6_HmacSha224_State g_hmacSha224State;

whal_HmacSha224 g_whalHmacSha224 = {
    .crypto = &g_whalHash,
    /* .driver: direct API mapping */
    .state = &g_hmacSha224State,
};

static whal_Stm32n6_HmacSha256_State g_hmacSha256State;

whal_HmacSha256 g_whalHmacSha256 = {
    .crypto = &g_whalHash,
    /* .driver: direct API mapping */
    .state = &g_hmacSha256State,
};

#ifdef BOARD_WATCHDOG_IWDG
whal_Watchdog g_whalWatchdog = {
    .base = WHAL_STM32N657_IWDG_BASE,
    .driver = WHAL_STM32N657_IWDG_DRIVER,

    .cfg = &(whal_Stm32n6_Iwdg_Cfg) {
        .prescaler = WHAL_STM32N6_IWDG_PR_32,
        .reload = 100,
        .timeout = &g_whalTimeout,
    },
};
#elif defined(BOARD_WATCHDOG_WWDG)
whal_Watchdog g_whalWatchdog = {
    .base = WHAL_STM32N657_WWDG_BASE,
    .driver = WHAL_STM32N657_WWDG_DRIVER,

    .cfg = &(whal_Stm32n6_Wwdg_Cfg) {
        .prescaler = WHAL_STM32N6_WWDG_TB_128,
        .window = 0x7F,
        .counter = 0x7F,
    },
};
#endif

/* Ethernet */
#define ETH_TX_DESC_COUNT 4
#define ETH_RX_DESC_COUNT 4
#define ETH_TX_BUF_SIZE   1536
#define ETH_RX_BUF_SIZE   1536

/* ETH DMA descriptors and frame buffers must live in AXI-master-visible
 * RAM. The default RAM region (FLEXRAM at 0x34000000) is allocated as
 * Cortex-M55 TCM and is not reachable by the ETH AXI master, so place
 * these in AXISRAM1 via the .axisram1 section. */
static whal_Stm32n6_Eth_TxDesc ethTxDescs[ETH_TX_DESC_COUNT]
    __attribute__((aligned(16), section(".axisram1")));
static whal_Stm32n6_Eth_RxDesc ethRxDescs[ETH_RX_DESC_COUNT]
    __attribute__((aligned(16), section(".axisram1")));
static uint8_t ethTxBufs[ETH_TX_DESC_COUNT * ETH_TX_BUF_SIZE]
    __attribute__((aligned(8), section(".axisram1")));
static uint8_t ethRxBufs[ETH_RX_DESC_COUNT * ETH_RX_BUF_SIZE]
    __attribute__((aligned(8), section(".axisram1")));

/* API is directly mapped */
whal_Eth g_whalEth = {
    .base = WHAL_STM32N657_ETH_BASE,

    .macAddr = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x01},
    .cfg = &(whal_Stm32n6_Eth_Cfg) {
        .txDescs = ethTxDescs,
        .txBufs = ethTxBufs,
        .txDescCount = ETH_TX_DESC_COUNT,
        .txBufSize = ETH_TX_BUF_SIZE,
        .rxDescs = ethRxDescs,
        .rxBufs = ethRxBufs,
        .rxDescCount = ETH_RX_DESC_COUNT,
        .rxBufSize = ETH_RX_BUF_SIZE,
        .timeout = &g_whalTimeout,
    },
};

/* Ethernet PHY (LAN8742A) — API is directly mapped */
whal_EthPhy g_whalEthPhy = {
    .eth = &g_whalEth,
    .addr = BOARD_ETH_PHY_ADDR,

    .cfg = &(whal_Lan8742a_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

void Board_WaitMs(size_t ms)
{
    uint32_t startCount = g_tick;
    while ((g_tick - startCount) < ms)
        ;
}

/* Diagnostic: grant the ETH1 AXI master CID=1 / secure / privileged so
 * RISAF2 (default region) lets it read/write AXISRAM1 descriptors and
 * frame buffers. Refactor into a proper RIF helper if this fixes the
 * loopback test. */
static void Board_AllowEth1Master(void)
{
    /* RIFSC secure alias 0x54024000; ETH1 = RIMU index 6, RISUP index 60. */

    /* Mark ETH1 (slave index 60) as secure + privileged. Without this,
     * the "secure guard" forces RIMC_ATTR6.MSEC to 0 because the ETH1
     * config port is nonsecure-accessible by default. */
    *(volatile uint32_t *)(0x54024000U + 0x014U) |= (1U << 28); /* SECCFGR1 */
    *(volatile uint32_t *)(0x54024000U + 0x034U) |= (1U << 28); /* PRIVCFGR1 */

    /* RIMC_ATTR6: MPRIV=1, MSEC=1, MCID=1 — ETH1 master tags AXI
     * transactions to match RISAF2 default-region policy. */
    *(volatile uint32_t *)(0x54024000U + 0xC10U + 6U * 4U) =
        (1U << 9) | (1U << 8) | (1U << 4);
}

whal_Error Board_Init(void)
{
    whal_Error err;

    Board_AllowEth1Master();

    /* HSI 64 MHz: enable, then select as both system and CPU clocks. */
    err = whal_Stm32n6_Rcc_EnableOsc(&g_whalClock,
        &(whal_Stm32n6_Rcc_OscCfg){WHAL_STM32N6_RCC_HSI_CFG});
    if (err)
        return err;
    err = whal_Stm32n6_Rcc_SetCpuClock(&g_whalClock, WHAL_STM32N6_RCC_CPUCLK_SRC_HSI);
    if (err)
        return err;
    err = whal_Stm32n6_Rcc_SetSysClock(&g_whalClock, WHAL_STM32N6_RCC_SYSCLK_SRC_HSI);
    if (err)
        return err;

    /* Enable peripheral clocks */
    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32n6_Rcc_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    /* Select RMII for the ETH1 MAC-PHY interface. Must precede the ETH1
     * clock-enable loop per RM0486 §14.10.51. */
    err = whal_Stm32n6_Rcc_SetEth1If(&g_whalClock,
                                        WHAL_STM32N6_RCC_ETH1_IF_RMII);
    if (err)
        return err;

    /* Enable ETH clocks */
    for (size_t i = 0; i < ETH_PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32n6_Rcc_EnablePeriphClk(&g_whalClock, &g_ethClocks[i]);
        if (err)
            return err;
    }

    err = whal_Irq_Init(&g_whalIrq);
    if (err)
        return err;

#ifdef BOARD_DMA
    err = whal_Stm32n6_Rcc_EnablePeriphClk(&g_whalClock, &g_dmaClock);
    if (err)
        return err;
    err = whal_Dma_Init(&g_whalDma1);
    if (err)
        return err;
#endif

    err = whal_Gpio_Init(&g_whalGpio);
    if (err)
        return err;

    err = whal_Uart_Init(&g_whalUart);
    if (err)
        return err;

    err = whal_Spi_Init(&g_whalSpi);
    if (err)
        return err;

    err = whal_I2c_Init(&g_whalI2c);
    if (err)
        return err;

    err = whal_Rng_Init(&g_whalRng);
    if (err)
        return err;

    err = whal_Crypto_Init(&g_whalCrypto);
    if (err)
        return err;

    err = whal_Crypto_Init(&g_whalHash);
    if (err)
        return err;

    err = whal_Eth_Init(&g_whalEth);
    if (err)
        return err;

    err = whal_EthPhy_Init(&g_whalEthPhy);
    if (err)
        return err;

    err = whal_Timer_Init(&g_whalTimer);
    if (err)
        return err;

    err = whal_Timer_Start(&g_whalTimer);
    if (err)
        return err;

    err = Peripheral_Init();
    if (err)
        return err;

    return WHAL_SUCCESS;
}

whal_Error Board_Deinit(void)
{
    whal_Error err;

    err = Peripheral_Deinit();
    if (err)
        return err;

    err = whal_Timer_Stop(&g_whalTimer);
    if (err)
        return err;

    err = whal_Timer_Deinit(&g_whalTimer);
    if (err)
        return err;

    err = whal_EthPhy_Deinit(&g_whalEthPhy);
    if (err)
        return err;

    err = whal_Eth_Deinit(&g_whalEth);
    if (err)
        return err;

    err = whal_Crypto_Deinit(&g_whalHash);
    if (err)
        return err;

    err = whal_Crypto_Deinit(&g_whalCrypto);
    if (err)
        return err;

    err = whal_Rng_Deinit(&g_whalRng);
    if (err)
        return err;

    err = whal_I2c_Deinit(&g_whalI2c);
    if (err)
        return err;

    err = whal_Spi_Deinit(&g_whalSpi);
    if (err)
        return err;

    err = whal_Uart_Deinit(&g_whalUart);
    if (err)
        return err;

    err = whal_Gpio_Deinit(&g_whalGpio);
    if (err)
        return err;

#ifdef BOARD_DMA
    err = whal_Dma_Deinit(&g_whalDma1);
    if (err)
        return err;
    err = whal_Stm32n6_Rcc_DisablePeriphClk(&g_whalClock, &g_dmaClock);
    if (err)
        return err;
#endif

    err = whal_Irq_Deinit(&g_whalIrq);
    if (err)
        return err;

    /* Disable ETH clocks */
    for (size_t i = 0; i < ETH_PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32n6_Rcc_DisablePeriphClk(&g_whalClock, &g_ethClocks[i]);
        if (err)
            return err;
    }

    /* Disable peripheral clocks */
    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32n6_Rcc_DisablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    /* HSI is the post-Deinit fallback; nothing else to tear down. */

    return WHAL_SUCCESS;
}
