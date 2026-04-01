/* Board configuration for the STM32H753 Nucleo-144 dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32h753xx.h>
#include <wolfHAL/eth_phy/lan8742a.h>
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

/* Clock: PLL1 driven from HSI
 * HSI = 64 MHz (div1)
 * PLL1: (64 / 4) * (99+1) / (1+1) = 16 * 100 / 2 = 800 MHz VCO / 2 = 400 MHz sys
 * Actual: configure for a safe 400 MHz for the Cortex-M7
 */
whal_Clock g_whalClock = {
    WHAL_STM32H753_RCC_PLL_DEVICE,

    .cfg = &(whal_Stm32h7Rcc_Cfg) {
        .sysClkSrc = WHAL_STM32H7_RCC_SYSCLK_SRC_PLL1,
        .sysClkCfg = &(whal_Stm32h7Rcc_PllClkCfg) {
            .clkSrc = WHAL_STM32H7_RCC_PLLCLK_SRC_HSI,
            /* HSI=64MHz, M=4 -> 16MHz ref, N=99 -> VCO=1600MHz, P=1 -> /2=400MHz */
            /* Actually: N=99 means DIVN1=99, multiply by 100, so 16*100=1600 is too high.
             * Use N=49 -> multiply by 50 -> 16*50=800 MHz VCO, P=1 -> /2 = 400 MHz */
            .m = 4,
            .n = 49,
            .p = 1,
            .q = 1,
            .r = 1,
        },
    },
};

static const whal_Stm32h7Rcc_Clk g_clocks[] = {
    {WHAL_STM32H753_GPIOA_CLOCK},
    {WHAL_STM32H753_GPIOB_CLOCK},
    {WHAL_STM32H753_GPIOC_CLOCK},
    {WHAL_STM32H753_GPIOD_CLOCK},
    {WHAL_STM32H753_GPIOG_CLOCK},
    {WHAL_STM32H753_USART3_CLOCK},
    {WHAL_STM32H753_SPI1_CLOCK},
    {WHAL_STM32H753_RNG_CLOCK},
    {WHAL_STM32H753_SYSCFG_CLOCK},
};
#define CLOCK_COUNT (sizeof(g_clocks) / sizeof(g_clocks[0]))

static const whal_Stm32h7Rcc_Clk g_ethClocks[] = {
    {WHAL_STM32H753_ETH_CLOCK},
    {WHAL_STM32H753_ETHTX_CLOCK},
    {WHAL_STM32H753_ETHRX_CLOCK},
};
#define ETH_CLOCK_COUNT (sizeof(g_ethClocks) / sizeof(g_ethClocks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    WHAL_STM32H753_GPIO_DEVICE,

    .cfg = &(whal_Stm32h7Gpio_Cfg) {
        .pinCfg = (whal_Stm32h7Gpio_PinCfg[PIN_COUNT]) {
            [LED_PIN] = { /* LD1 Green LED on PB0 */
                .port = WHAL_STM32H7_GPIO_PORT_B,
                .pin = 0,
                .mode = WHAL_STM32H7_GPIO_MODE_OUT,
                .outType = WHAL_STM32H7_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H7_GPIO_SPEED_LOW,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 0,
            },
            [UART_TX_PIN] = { /* USART3 TX on PD8 */
                .port = WHAL_STM32H7_GPIO_PORT_D,
                .pin = 8,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H7_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H7_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H7_GPIO_PULL_UP,
                .altFn = 7,
            },
            [UART_RX_PIN] = { /* USART3 RX on PD9 */
                .port = WHAL_STM32H7_GPIO_PORT_D,
                .pin = 9,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H7_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H7_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H7_GPIO_PULL_UP,
                .altFn = 7,
            },
            [SPI_SCK_PIN] = { /* SPI1 SCK on PA5 */
                .port = WHAL_STM32H7_GPIO_PORT_A,
                .pin = 5,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H7_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H7_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MISO_PIN] = { /* SPI1 MISO on PA6 */
                .port = WHAL_STM32H7_GPIO_PORT_A,
                .pin = 6,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H7_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H7_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MOSI_PIN] = { /* SPI1 MOSI on PD7 */
                .port = WHAL_STM32H7_GPIO_PORT_D,
                .pin = 7,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H7_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H7_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_CS_PIN] = { /* SPI CS on PD14 */
                .port = WHAL_STM32H7_GPIO_PORT_D,
                .pin = 14,
                .mode = WHAL_STM32H7_GPIO_MODE_OUT,
                .outType = WHAL_STM32H7_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H7_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H7_GPIO_PULL_UP,
            },
            [ETH_RMII_REF_CLK_PIN] = { /* RMII REF_CLK on PA1 */
                .port = WHAL_STM32H7_GPIO_PORT_A,
                .pin = 1,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
            [ETH_RMII_MDIO_PIN] = { /* RMII MDIO on PA2 */
                .port = WHAL_STM32H7_GPIO_PORT_A,
                .pin = 2,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
            [ETH_RMII_MDC_PIN] = { /* RMII MDC on PC1 */
                .port = WHAL_STM32H7_GPIO_PORT_C,
                .pin = 1,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
            [ETH_RMII_CRS_DV_PIN] = { /* RMII CRS_DV on PA7 */
                .port = WHAL_STM32H7_GPIO_PORT_A,
                .pin = 7,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
            [ETH_RMII_RXD0_PIN] = { /* RMII RXD0 on PC4 */
                .port = WHAL_STM32H7_GPIO_PORT_C,
                .pin = 4,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
            [ETH_RMII_RXD1_PIN] = { /* RMII RXD1 on PC5 */
                .port = WHAL_STM32H7_GPIO_PORT_C,
                .pin = 5,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
            [ETH_RMII_TX_EN_PIN] = { /* RMII TX_EN on PG11 */
                .port = WHAL_STM32H7_GPIO_PORT_G,
                .pin = 11,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
            [ETH_RMII_TXD0_PIN] = { /* RMII TXD0 on PG13 */
                .port = WHAL_STM32H7_GPIO_PORT_G,
                .pin = 13,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
            [ETH_RMII_TXD1_PIN] = { /* RMII TXD1 on PB13 */
                .port = WHAL_STM32H7_GPIO_PORT_B,
                .pin = 13,
                .mode = WHAL_STM32H7_GPIO_MODE_ALTFN,
                .speed = WHAL_STM32H7_GPIO_SPEED_HIGH,
                .pull = WHAL_STM32H7_GPIO_PULL_NONE,
                .altFn = 11,
            },
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    WHAL_CORTEX_M7_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 400000000 / 1000, /* 400 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART: USART3 on the Nucleo-144 (ST-LINK VCP) */
whal_Uart g_whalUart = {
    WHAL_STM32H753_USART3_DEVICE,

    .cfg = &(whal_Stm32h7Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32H7_UART_BRR(100000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    WHAL_STM32H753_SPI1_DEVICE,

    .cfg = &(whal_Stm32h7Spi_Cfg) {
        .pclk = 200000000,
        .timeout = &g_whalTimeout,
    },
};

/* RNG */
whal_Rng g_whalRng = {
    WHAL_STM32H753_RNG_DEVICE,

    .cfg = &(whal_Stm32h7Rng_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    WHAL_STM32H753_FLASH_DEVICE,

    .cfg = &(whal_Stm32h7Flash_Cfg) {
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

static whal_Stm32h7Eth_TxDesc ethTxDescs[ETH_TX_DESC_COUNT]
    __attribute__((aligned(16)));
static whal_Stm32h7Eth_RxDesc ethRxDescs[ETH_RX_DESC_COUNT]
    __attribute__((aligned(16)));
static uint8_t ethTxBufs[ETH_TX_DESC_COUNT * ETH_TX_BUF_SIZE]
    __attribute__((aligned(4)));
static uint8_t ethRxBufs[ETH_RX_DESC_COUNT * ETH_RX_BUF_SIZE]
    __attribute__((aligned(4)));

whal_Eth g_whalEth = {
    WHAL_STM32H753_ETH_DEVICE,

    .cfg = &(whal_Stm32h7Eth_Cfg) {
        .macAddr = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x01},
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
    .driver = &whal_Lan8742a_Driver,

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
 * FLASH_ACR: 0x52002000 (flash interface base)
 * LATENCY[3:0] = 4 wait states for 400 MHz at VOS1
 * WRHIGHFREQ[5:4] = 2
 */
#define FLASH_ACR_ADDR 0x52002000
#define FLASH_ACR_LATENCY_400MHZ ((2 << 4) | 4)

/*
 * SYSCFG_PMCR: 0x58000404
 * ETH_SEL_PHY[23:21] = 0b100 selects RMII
 */
#define SYSCFG_PMCR_ADDR 0x58000404

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Set flash latency before increasing clock speed */
    *(volatile uint32_t *)FLASH_ACR_ADDR = FLASH_ACR_LATENCY_400MHZ;

    err = whal_Clock_Init(&g_whalClock);
    if (err)
        return err;

    /* Enable clocks (excludes ETH) */
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_clocks[i]);
        if (err)
            return err;
    }

    /* Select RMII mode in SYSCFG_PMCR bits [23:21] = 0b100 */
    *(volatile uint32_t *)SYSCFG_PMCR_ADDR =
        (*(volatile uint32_t *)SYSCFG_PMCR_ADDR & ~(7UL << 21)) | (4UL << 21);

    /* Enable ETH clocks after RMII mode is selected */
    for (size_t i = 0; i < ETH_CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_ethClocks[i]);
        if (err)
            return err;
    }

    /* Enable HSI48 for RNG kernel clock */
    err = whal_Stm32h7Rcc_Ext_EnableHsi48(&g_whalClock, 1);
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

    for (size_t i = 0; i < ETH_CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_ethClocks[i]);
        if (err)
            return err;
    }

    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_clocks[i]);
        if (err)
            return err;
    }

    err = whal_Clock_Deinit(&g_whalClock);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
