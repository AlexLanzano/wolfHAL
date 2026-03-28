/* Board configuration for the STM32H563ZI Nucleo-144 dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32h563xx.h>
#include "peripheral.h"

/* SysTick timing */
volatile uint32_t g_tick = 0;
volatile uint8_t g_waiting = 0;
volatile uint8_t g_tickOverflow = 0;

void SysTick_Handler()
{
    uint32_t tickBefore = g_tick++;
    if (g_waiting) {
        if (tickBefore > g_tick)
            g_tickOverflow = 1;
    }
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
    WHAL_STM32H563_RCC_PLL_DEVICE,

    .cfg = &(whal_Stm32h5Rcc_Cfg) {
        .sysClkSrc = WHAL_STM32H5_RCC_SYSCLK_SRC_PLL1,
        .sysClkCfg = &(whal_Stm32h5Rcc_PllClkCfg) {
            .clkSrc = WHAL_STM32H5_RCC_PLLCLK_SRC_HSI,
            /* HSI = 64 MHz (div1), PLL1: (64 / 8) * (62+1) / (2+1) = 504 / 3 = 168 MHz */
            .m = 8,
            .n = 62,
            .p = 2,
            .q = 2,
            .r = 2,
        },
    },
};

static const whal_Stm32h5Rcc_Clk g_clocks[] = {
    {WHAL_STM32H563_GPIOA_CLOCK},
    {WHAL_STM32H563_GPIOB_CLOCK},
    {WHAL_STM32H563_GPIOD_CLOCK},
    {WHAL_STM32H563_GPIOG_CLOCK},
    {WHAL_STM32H563_USART2_CLOCK},
    {WHAL_STM32H563_SPI1_CLOCK},
    {WHAL_STM32H563_RNG_CLOCK},
};
#define CLOCK_COUNT (sizeof(g_clocks) / sizeof(g_clocks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    WHAL_STM32H563_GPIO_DEVICE,

    .cfg = &(whal_Stm32h5Gpio_Cfg) {
        .pinCfg = (whal_Stm32h5Gpio_PinCfg[PIN_COUNT]) {
            [LED_PIN] = { /* LD1 Green LED on PB0 */
                .port = WHAL_STM32H5_GPIO_PORT_B,
                .pin = 0,
                .mode = WHAL_STM32H5_GPIO_MODE_OUT,
                .outType = WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H5_GPIO_SPEED_LOW,
                .pull = WHAL_STM32H5_GPIO_PULL_NONE,
                .altFn = 0,
            },
            [UART_TX_PIN] = { /* USART3 TX on PD8 */
                .port = WHAL_STM32H5_GPIO_PORT_D,
                .pin = 5,
                .mode = WHAL_STM32H5_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H5_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H5_GPIO_PULL_UP,
                .altFn = 7,
            },
            [UART_RX_PIN] = { /* USART2 RX on PD6 */
                .port = WHAL_STM32H5_GPIO_PORT_D,
                .pin = 6,
                .mode = WHAL_STM32H5_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H5_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H5_GPIO_PULL_UP,
                .altFn = 7,
            },
            [SPI_SCK_PIN] = { /* SPI1 SCK on PA5 */
                .port = WHAL_STM32H5_GPIO_PORT_A,
                .pin = 5,
                .mode = WHAL_STM32H5_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H5_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H5_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MISO_PIN] = { /* SPI1 MISO on PG9 */
                .port = WHAL_STM32H5_GPIO_PORT_G,
                .pin = 9,
                .mode = WHAL_STM32H5_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H5_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H5_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MOSI_PIN] = { /* SPI1 MOSI on PB5 */
                .port = WHAL_STM32H5_GPIO_PORT_B,
                .pin = 5,
                .mode = WHAL_STM32H5_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H5_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H5_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_CS_PIN] = { /* SPI CS on PD14 */
                .port = WHAL_STM32H5_GPIO_PORT_D,
                .pin = 14,
                .mode = WHAL_STM32H5_GPIO_MODE_OUT,
                .outType = WHAL_STM32H5_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32H5_GPIO_SPEED_FAST,
                .pull = WHAL_STM32H5_GPIO_PULL_UP,
            },
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    WHAL_CORTEX_M4_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 168000000 / 1000, /* 250 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART */
whal_Uart g_whalUart = {
    WHAL_STM32H563_USART2_DEVICE,

    .cfg = &(whal_Stm32h5Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32H5_UART_BRR(168000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    WHAL_STM32H563_SPI1_DEVICE,

    .cfg = &(whal_Stm32h5Spi_Cfg) {
        .pclk = 168000000,
        .timeout = &g_whalTimeout,
    },
};

/* RNG */
whal_Rng g_whalRng = {
    WHAL_STM32H563_RNG_DEVICE,

    .cfg = &(whal_Stm32h5Rng_Cfg) {
        .timeout = &g_whalTimeout,
    },
};


/* Flash */
whal_Flash g_whalFlash = {
    WHAL_STM32H563_FLASH_DEVICE,

    .cfg = &(whal_Stm32h5Flash_Cfg) {
        .startAddr = 0x08000000,
        .size = 0x200000, /* 2 MB */
        .timeout = &g_whalTimeout,
    },
};

void Board_WaitMs(size_t ms)
{
    uint32_t startCount = g_tick;
    g_waiting = 1;
    while (1) {
        uint32_t currentCount = g_tick;
        if (g_tickOverflow) {
            if ((UINT32_MAX - startCount) + currentCount > ms) {
                break;
            }
        } else if (currentCount - startCount > ms) {
            break;
        }
    }

    g_waiting = 0;
    g_tickOverflow = 0;
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

    err = whal_Clock_Init(&g_whalClock);
    if (err)
        return err;

    /* Enable clocks */
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_clocks[i]);
        if (err)
            return err;
    }

    /* Enable HSI48 for RNG kernel clock */
    err = whal_Stm32h5Rcc_Ext_EnableHsi48(&g_whalClock, 1);
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

    /* Disable clocks */
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
