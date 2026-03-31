/* Board configuration for the WeAct BlackPill STM32F411CEU6 */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32f411xx.h>
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

/* STM32F411CE sector layout (512 KB) */
static const whal_Stm32f4Flash_Sector g_flashSectors[] = {
    { .addr = 0x08000000, .size = 0x04000 },  /* Sector 0: 16 KB */
    { .addr = 0x08004000, .size = 0x04000 },  /* Sector 1: 16 KB */
    { .addr = 0x08008000, .size = 0x04000 },  /* Sector 2: 16 KB */
    { .addr = 0x0800C000, .size = 0x04000 },  /* Sector 3: 16 KB */
    { .addr = 0x08010000, .size = 0x10000 },  /* Sector 4: 64 KB */
    { .addr = 0x08020000, .size = 0x20000 },  /* Sector 5: 128 KB */
    { .addr = 0x08040000, .size = 0x20000 },  /* Sector 6: 128 KB */
    { .addr = 0x08060000, .size = 0x20000 },  /* Sector 7: 128 KB */
};
#define FLASH_SECTOR_COUNT (sizeof(g_flashSectors) / sizeof(g_flashSectors[0]))

/* Clock */
whal_Clock g_whalClock = {
    WHAL_STM32F411_RCC_PLL_DEVICE,

    .cfg = &(whal_Stm32f4Rcc_Cfg) {
        .sysClkSrc = WHAL_STM32F4_RCC_SYSCLK_SRC_PLL,
        .sysClkCfg = &(whal_Stm32f4Rcc_PllClkCfg) {
            .clkSrc = WHAL_STM32F4_RCC_PLLCLK_SRC_HSE,
            /* HSE = 25 MHz, PLL: (25 / 25) * 200 / 2 = 100 MHz */
            .m = 25,
            .n = 200,
            .p = 0,  /* 0 = div by 2 */
            .q = 4,  /* 200 / 4 = 50 MHz (not valid for USB, which needs 48 MHz) */
        },
        .ppre1 = 4, /* APB1 = SYSCLK / 2 = 50 MHz (APB1 max is 50 MHz) */
        .ppre2 = 0, /* APB2 = SYSCLK / 1 = 100 MHz */
    },
};

static const whal_Stm32f4Rcc_Clk g_clocks[] = {
    {WHAL_STM32F411_GPIOA_CLOCK},
    {WHAL_STM32F411_GPIOC_CLOCK},
    {WHAL_STM32F411_USART2_CLOCK},
    {WHAL_STM32F411_SPI1_CLOCK},
};
#define CLOCK_COUNT (sizeof(g_clocks) / sizeof(g_clocks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    WHAL_STM32F411_GPIO_DEVICE,

    .cfg = &(whal_Stm32f4Gpio_Cfg) {
        .pinCfg = (whal_Stm32f4Gpio_PinCfg[PIN_COUNT]) {
            [LED_PIN] = { /* LED on PC13 (active low) */
                .port = WHAL_STM32F4_GPIO_PORT_C,
                .pin = 13,
                .mode = WHAL_STM32F4_GPIO_MODE_OUT,
                .outType = WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32F4_GPIO_SPEED_LOW,
                .pull = WHAL_STM32F4_GPIO_PULL_NONE,
                .altFn = 0,
            },
            [UART_TX_PIN] = { /* USART2 TX on PA2 (AF7) */
                .port = WHAL_STM32F4_GPIO_PORT_A,
                .pin = 2,
                .mode = WHAL_STM32F4_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32F4_GPIO_SPEED_FAST,
                .pull = WHAL_STM32F4_GPIO_PULL_UP,
                .altFn = 7,
            },
            [UART_RX_PIN] = { /* USART2 RX on PA3 (AF7) */
                .port = WHAL_STM32F4_GPIO_PORT_A,
                .pin = 3,
                .mode = WHAL_STM32F4_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32F4_GPIO_SPEED_FAST,
                .pull = WHAL_STM32F4_GPIO_PULL_UP,
                .altFn = 7,
            },
            [SPI_SCK_PIN] = { /* SPI1 SCK on PA5 (AF5) */
                .port = WHAL_STM32F4_GPIO_PORT_A,
                .pin = 5,
                .mode = WHAL_STM32F4_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32F4_GPIO_SPEED_FAST,
                .pull = WHAL_STM32F4_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MISO_PIN] = { /* SPI1 MISO on PA6 (AF5) */
                .port = WHAL_STM32F4_GPIO_PORT_A,
                .pin = 6,
                .mode = WHAL_STM32F4_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32F4_GPIO_SPEED_FAST,
                .pull = WHAL_STM32F4_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MOSI_PIN] = { /* SPI1 MOSI on PA7 (AF5) */
                .port = WHAL_STM32F4_GPIO_PORT_A,
                .pin = 7,
                .mode = WHAL_STM32F4_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32F4_GPIO_SPEED_FAST,
                .pull = WHAL_STM32F4_GPIO_PULL_NONE,
                .altFn = 5,
            },
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    WHAL_CORTEX_M4_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 100000000 / 1000, /* 100 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART */
whal_Uart g_whalUart = {
    WHAL_STM32F411_USART2_DEVICE,

    .cfg = &(whal_Stm32f4Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32F4_UART_BRR(50000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    WHAL_STM32F411_SPI1_DEVICE,

    .cfg = &(whal_Stm32f4Spi_Cfg) {
        .pclk = 100000000,
        .timeout = &g_whalTimeout,
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    WHAL_STM32F411_FLASH_DEVICE,

    .cfg = &(whal_Stm32f4Flash_Cfg) {
        .startAddr = 0x08000000,
        .size = 0x80000, /* 512 KB */
        .sectors = g_flashSectors,
        .sectorCount = FLASH_SECTOR_COUNT,
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
 * Flash latency for 100 MHz at 2.7-3.6V: 3 wait states (Table 5 in RM0383)
 *
 * RCC_CFGR APB1 prescaler (PPRE1[2:0], bits 12:10):
 *   100 = AHB clock divided by 2 => APB1 = 50 MHz
 * APB2 prescaler (PPRE2[2:0], bits 15:13):
 *   0xx = AHB clock not divided => APB2 = 100 MHz
 */

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Set flash latency before increasing clock speed */
    err = whal_Stm32f4Flash_Ext_SetLatency(&g_whalFlash,
                                            WHAL_STM32F4_FLASH_LATENCY_3);
    if (err)
        return err;

    err = whal_Clock_Init(&g_whalClock);

    /* Enable clocks */
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_clocks[i]);
        if (err)
            return err;
    }

    err = whal_Gpio_Init(&g_whalGpio);
    if (err)
        return err;

    err = whal_Uart_Init(&g_whalUart);
    if (err)
        return err;

    err = whal_Spi_Init(&g_whalSpi);
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
