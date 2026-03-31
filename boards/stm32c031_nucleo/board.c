/* Board configuration for the NUCLEO-C031C6 dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32c031xx.h>
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
    WHAL_STM32C031_RCC_DEVICE,

    .cfg = &(whal_Stm32c0Rcc_Cfg) {
        .hsidiv = WHAL_STM32C0_RCC_HSIDIV_1, /* HSI48 / 1 = 48 MHz */
    },
};

static const whal_Stm32c0Rcc_Clk g_clocks[] = {
    {WHAL_STM32C031_GPIOA_CLOCK},
    {WHAL_STM32C031_GPIOB_CLOCK},
    {WHAL_STM32C031_GPIOC_CLOCK},
    {WHAL_STM32C031_USART1_CLOCK},
    {WHAL_STM32C031_SPI1_CLOCK},
};
#define CLOCK_COUNT (sizeof(g_clocks) / sizeof(g_clocks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    WHAL_STM32C031_GPIO_DEVICE,

    .cfg = &(whal_Stm32c0Gpio_Cfg) {
        .pinCfg = (whal_Stm32c0Gpio_PinCfg[PIN_COUNT]) {
            [LED_PIN] = { /* LD4 Green LED on PA5 */
                .port = WHAL_STM32C0_GPIO_PORT_A,
                .pin = 5,
                .mode = WHAL_STM32C0_GPIO_MODE_OUT,
                .outType = WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32C0_GPIO_SPEED_LOW,
                .pull = WHAL_STM32C0_GPIO_PULL_NONE,
                .altFn = 0,
            },
            [UART_TX_PIN] = { /* USART1 TX on PB6, AF1 */
                .port = WHAL_STM32C0_GPIO_PORT_B,
                .pin = 6,
                .mode = WHAL_STM32C0_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32C0_GPIO_SPEED_FAST,
                .pull = WHAL_STM32C0_GPIO_PULL_UP,
                .altFn = 0,
            },
            [UART_RX_PIN] = { /* USART1 RX on PB7, AF1 */
                .port = WHAL_STM32C0_GPIO_PORT_B,
                .pin = 7,
                .mode = WHAL_STM32C0_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32C0_GPIO_SPEED_FAST,
                .pull = WHAL_STM32C0_GPIO_PULL_UP,
                .altFn = 0,
            },
            [SPI_SCK_PIN] = { /* SPI1 SCK on PA1, AF0 */
                .port = WHAL_STM32C0_GPIO_PORT_A,
                .pin = 1,
                .mode = WHAL_STM32C0_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32C0_GPIO_SPEED_FAST,
                .pull = WHAL_STM32C0_GPIO_PULL_NONE,
                .altFn = 0,
            },
            [SPI_MISO_PIN] = { /* SPI1 MISO on PA6, AF0 */
                .port = WHAL_STM32C0_GPIO_PORT_A,
                .pin = 6,
                .mode = WHAL_STM32C0_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32C0_GPIO_SPEED_FAST,
                .pull = WHAL_STM32C0_GPIO_PULL_NONE,
                .altFn = 0,
            },
            [SPI_MOSI_PIN] = { /* SPI1 MOSI on PA7, AF0 */
                .port = WHAL_STM32C0_GPIO_PORT_A,
                .pin = 7,
                .mode = WHAL_STM32C0_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32C0_GPIO_SPEED_FAST,
                .pull = WHAL_STM32C0_GPIO_PULL_NONE,
                .altFn = 0,
            },
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    WHAL_CORTEX_M0PLUS_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 48000000 / 1000, /* 48 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART */
whal_Uart g_whalUart = {
    WHAL_STM32C031_USART1_DEVICE,

    .cfg = &(whal_Stm32c0Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32C0_UART_BRR(48000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    WHAL_STM32C031_SPI1_DEVICE,

    .cfg = &(whal_Stm32c0Spi_Cfg) {
        .pclk = 48000000,
        .timeout = &g_whalTimeout,
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    WHAL_STM32C031_FLASH_DEVICE,

    .cfg = &(whal_Stm32c0Flash_Cfg) {
        .startAddr = 0x08000000,
        .size = 0x8000, /* 32 KB */
        .timeout = &g_whalTimeout,
    },
};

void Board_WaitMs(size_t ms)
{
    uint32_t startCount = g_tick;
    while ((g_tick - startCount) < ms)
        ;
}

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Set flash latency before increasing clock speed */
    err = whal_Stm32c0Flash_Ext_SetLatency(&g_whalFlash, WHAL_STM32C0_FLASH_LATENCY_1);
    if (err)
        return err;

    err = whal_Clock_Init(&g_whalClock);
    if (err)
        return err;

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
