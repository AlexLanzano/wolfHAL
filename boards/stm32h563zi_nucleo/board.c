/* Board configuration for the STM32H563ZI Nucleo-144 dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32h563xx.h>
#include <wolfHAL/eth_phy/lan8742a_eth_phy.h>
#include "peripheral.h"

/* SysTick timing */
volatile uint32_t g_tick = 0;

void SysTick_Handler()
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

/* Clock */
whal_Clock g_whalClock = {
    .regmap = { WHAL_STM32H563_RCC_REGMAP },
};

static const whal_Stm32h5_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32H563_GPIOA_CLOCK},
    {WHAL_STM32H563_GPIOB_CLOCK},
    {WHAL_STM32H563_GPIOD_CLOCK},
    {WHAL_STM32H563_GPIOC_CLOCK},
    {WHAL_STM32H563_GPIOG_CLOCK},
    {WHAL_STM32H563_USART3_CLOCK},
    {WHAL_STM32H563_SPI1_CLOCK},
    {WHAL_STM32H563_RNG_CLOCK},
    {WHAL_STM32H563_SBS_CLOCK},
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

static const whal_Stm32h5_Rcc_PeriphClk g_ethClocks[] = {
    {WHAL_STM32H563_ETH_CLOCK},
    {WHAL_STM32H563_ETHTX_CLOCK},
    {WHAL_STM32H563_ETHRX_CLOCK},
};
#define ETH_CLOCK_COUNT (sizeof(g_ethClocks) / sizeof(g_ethClocks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    .regmap = { WHAL_STM32H563_GPIO_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32h5_Gpio_Cfg) {
        .pinCfg = (whal_Stm32h5_Gpio_PinCfg[PIN_COUNT]) {
            /* LD1 Green LED on PB0 */
            [LED_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_B, 0, WHAL_STM32H5_GPIO_MODE_OUT,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_LOW,
                WHAL_STM32H5_GPIO_PULL_NONE, 0),
            /* USART3 TX on PD8, AF7 (ST-Link VCP) */
            [UART_TX_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_D, 8, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_FAST,
                WHAL_STM32H5_GPIO_PULL_UP, 7),
            /* USART3 RX on PD9, AF7 (ST-Link VCP) */
            [UART_RX_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_D, 9, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_FAST,
                WHAL_STM32H5_GPIO_PULL_UP, 7),
            /* SPI1 SCK on PA5 */
            [SPI_SCK_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_A, 5, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_FAST,
                WHAL_STM32H5_GPIO_PULL_NONE, 5),
            /* SPI1 MISO on PG9 */
            [SPI_MISO_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_G, 9, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_FAST,
                WHAL_STM32H5_GPIO_PULL_NONE, 5),
            /* SPI1 MOSI on PB5 */
            [SPI_MOSI_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_B, 5, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_FAST,
                WHAL_STM32H5_GPIO_PULL_NONE, 5),
            /* SPI CS on PD14 */
            [SPI_CS_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_D, 14, WHAL_STM32H5_GPIO_MODE_OUT,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_FAST,
                WHAL_STM32H5_GPIO_PULL_UP, 0),
            /* RMII REF_CLK on PA1 */
            [ETH_RMII_REF_CLK_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_A, 1, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
            /* RMII MDIO on PA2 */
            [ETH_RMII_MDIO_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_A, 2, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
            /* RMII MDC on PC1 */
            [ETH_RMII_MDC_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_C, 1, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
            /* RMII CRS_DV on PA7 */
            [ETH_RMII_CRS_DV_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_A, 7, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
            /* RMII RXD0 on PC4 */
            [ETH_RMII_RXD0_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_C, 4, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
            /* RMII RXD1 on PC5 */
            [ETH_RMII_RXD1_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_C, 5, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
            /* RMII TX_EN on PG11 */
            [ETH_RMII_TX_EN_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_G, 11, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
            /* RMII TXD0 on PG13 */
            [ETH_RMII_TXD0_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_G, 13, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
            /* RMII TXD1 on PB15 */
            [ETH_RMII_TXD1_PIN] = WHAL_STM32H5_GPIO_PIN(
                WHAL_STM32H5_GPIO_PORT_B, 15, WHAL_STM32H5_GPIO_MODE_ALTFN,
                WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32H5_GPIO_SPEED_HIGH,
                WHAL_STM32H5_GPIO_PULL_NONE, 11),
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    .regmap = { WHAL_CORTEX_M33_SYSTICK_REGMAP },
    .driver = WHAL_CORTEX_M33_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 168000000 / 1000, /* 168 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART */
whal_Uart g_whalUart = {
    .regmap = { WHAL_STM32H563_USART3_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32h5_Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32H5_UART_BRR(168000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    .regmap = { WHAL_STM32H563_SPI1_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32h5_Spi_Cfg) {
        .pclk = 168000000,
        .timeout = &g_whalTimeout,
    },
};

/* RNG */
whal_Rng g_whalRng = {
    .regmap = { WHAL_STM32H563_RNG_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32h5_Rng_Cfg) {
        .timeout = &g_whalTimeout,
    },
};


/* Flash */
whal_Flash g_whalFlash = {
    .regmap = { WHAL_STM32H563_FLASH_REGMAP },
    .driver = WHAL_STM32H563_FLASH_DRIVER,

    .cfg = &(whal_Stm32h5_Flash_Cfg) {
        .startAddr = 0x08000000,
        .size = 0x200000, /* 2 MB */
        .timeout = &g_whalTimeout,
    },
};

/* Ethernet */
#define ETH_TX_DESC_COUNT 4
#define ETH_RX_DESC_COUNT 4
#define ETH_TX_BUF_SIZE   1536
#define ETH_RX_BUF_SIZE   1536

static whal_Stm32h5_Eth_TxDesc ethTxDescs[ETH_TX_DESC_COUNT]
    __attribute__((aligned(16)));
static whal_Stm32h5_Eth_RxDesc ethRxDescs[ETH_RX_DESC_COUNT]
    __attribute__((aligned(16)));
static uint8_t ethTxBufs[ETH_TX_DESC_COUNT * ETH_TX_BUF_SIZE]
    __attribute__((aligned(4)));
static uint8_t ethRxBufs[ETH_RX_DESC_COUNT * ETH_RX_BUF_SIZE]
    __attribute__((aligned(4)));

whal_Eth g_whalEth = {
    .regmap = { WHAL_STM32H563_ETH_REGMAP },
    /* .driver: direct API mapping */

    .macAddr = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x01},
    .cfg = &(whal_Stm32h5_Eth_Cfg) {
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

/* Ethernet PHY (LAN8742A) */
whal_EthPhy g_whalEthPhy = {
    .eth = &g_whalEth,
    .addr = BOARD_ETH_PHY_ADDR,
    /* .driver: direct API mapping */

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

/*
 * FLASH_ACR: 0x40022000
 * LATENCY[3:0] = 5 wait states for 168 MHz
 * WRHIGHFREQ[5:4] = 2
 */
#define FLASH_ACR_ADDR 0x40022000
#define FLASH_ACR_LATENCY_168MHZ ((2 << 4) | 5)

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Set flash latency before increasing clock speed */
    *(volatile uint32_t *)FLASH_ACR_ADDR = FLASH_ACR_LATENCY_168MHZ;

    /* HSI 64 MHz -> PLL1 (HSI/8 * 63 / 3 = 168 MHz) -> SYSCLK = PLL1 */

    /* RCC_CR.HSIDIV resets to /4 (16 MHz) on H5, not /1. Force it back to
     * /1 so the PLL sees 64 MHz; otherwise the divider chain below silently
     * lands on 42 MHz instead of 168 MHz. */
    err = whal_Stm32h5_Rcc_SetHsiDiv(&g_whalClock, 0);
    if (err)
        return err;

    err = whal_Stm32h5_Rcc_EnableOsc(&g_whalClock,
        &(whal_Stm32h5_Rcc_OscCfg){WHAL_STM32H5_RCC_HSI_CFG});
    if (err)
        return err;

    err = whal_Stm32h5_Rcc_EnablePll1(&g_whalClock, &(whal_Stm32h5_Rcc_PllCfg){
        .clkSrc = WHAL_STM32H5_RCC_PLLCLK_SRC_HSI,
        .m = 8, .n = 62, .p = 2, .q = 2, .r = 2,
    });
    if (err)
        return err;

    err = whal_Stm32h5_Rcc_SetSysClock(&g_whalClock, WHAL_STM32H5_RCC_SYSCLK_SRC_PLL1);
    if (err)
        return err;

    /* Enable clocks (excludes ETH — needs SBS RMII config first) */
    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32h5_Rcc_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    /* Select RMII mode in SBS_PMCR (SBS base 0x44000400 + 0x100) bits [23:21] = 0b100 */
    *(volatile uint32_t *)0x44000500 =
        (*(volatile uint32_t *)0x44000500 & ~(7UL << 21)) | (4UL << 21);

    /* Enable ETH clocks after RMII mode is selected */
    for (size_t i = 0; i < ETH_CLOCK_COUNT; i++) {
        err = whal_Stm32h5_Rcc_EnablePeriphClk(&g_whalClock, &g_ethClocks[i]);
        if (err)
            return err;
    }

    /* Enable HSI48 for RNG kernel clock */
    err = whal_Stm32h5_Rcc_EnableOsc(&g_whalClock,
        &(whal_Stm32h5_Rcc_OscCfg){WHAL_STM32H5_RCC_HSI48_CFG});
    if (err)
        return err;

    err = whal_Gpio_Init(&g_whalGpio);
    if (err)
        return err;

    err = whal_Uart_Init(&g_whalUart);
    if (err)
        return err;

    err = whal_Spi_Init(&g_whalSpi);
    if (err)
        return err;

    err = whal_Rng_Init(&g_whalRng);
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

    err = whal_Rng_Deinit(&g_whalRng);
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

    /* Disable ETH clocks */
    for (size_t i = ETH_CLOCK_COUNT; i-- > 0; ) {
        err = whal_Stm32h5_Rcc_DisablePeriphClk(&g_whalClock, &g_ethClocks[i]);
        if (err)
            return err;
    }

    /* Disable clocks */
    for (size_t i = PERIPH_CLK_COUNT; i-- > 0; ) {
        err = whal_Stm32h5_Rcc_DisablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    err = whal_Stm32h5_Rcc_SetSysClock(&g_whalClock, WHAL_STM32H5_RCC_SYSCLK_SRC_HSI);
    if (err)
        return err;
    err = whal_Stm32h5_Rcc_DisablePll1(&g_whalClock);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
