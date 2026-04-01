/* Board configuration for the STM32N657X0-Q Nucleo dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32n657xx.h>
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

/* Clock: HSI at 64 MHz (default, no PLL) */
whal_Clock g_whalClock = {
    WHAL_STM32N657_RCC_HSI_DEVICE,

    .cfg = &(whal_Stm32n6Rcc_Cfg) {
        .sysClkCfg = &(whal_Stm32n6Rcc_HsiClkCfg) {
            .div = 0, /* div1 = 64 MHz */
        },
    },
};

static const whal_Stm32n6Rcc_Clk g_busClocks[] = {
    {WHAL_STM32N657_AHB3_BUS_CLOCK},
    {WHAL_STM32N657_AHB4_BUS_CLOCK},
    {WHAL_STM32N657_APB1_BUS_CLOCK},
    {WHAL_STM32N657_APB2_BUS_CLOCK},
};
#define BUS_CLOCK_COUNT (sizeof(g_busClocks) / sizeof(g_busClocks[0]))

static const whal_Stm32n6Rcc_Clk g_clocks[] = {
    {WHAL_STM32N657_GPIOA_CLOCK},
    {WHAL_STM32N657_GPIOB_CLOCK},
    {WHAL_STM32N657_GPIOC_CLOCK},
    {WHAL_STM32N657_GPIOD_CLOCK},
    {WHAL_STM32N657_GPIOP_CLOCK},
    {WHAL_STM32N657_USART1_CLOCK},
    {WHAL_STM32N657_SPI1_CLOCK},
    {WHAL_STM32N657_RNG_CLOCK},
};
#define CLOCK_COUNT (sizeof(g_clocks) / sizeof(g_clocks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    WHAL_STM32N657_GPIO_DEVICE,

    .cfg = &(whal_Stm32n6Gpio_Cfg) {
        .pinCfg = (whal_Stm32n6Gpio_PinCfg[PIN_COUNT]) {
            [LED_PIN] = { /* LD1 Green LED on PP3 (port P = index 15, pin 3) */
                .port = WHAL_STM32N6_GPIO_PORT_D,
                .pin = 2,
                .mode = WHAL_STM32N6_GPIO_MODE_OUT,
                .outType = WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32N6_GPIO_SPEED_LOW,
                .pull = WHAL_STM32N6_GPIO_PULL_NONE,
                .altFn = 0,
            },
            [UART_TX_PIN] = { /* USART1 TX on PA9, AF7 */
                .port = WHAL_STM32N6_GPIO_PORT_A,
                .pin = 9,
                .mode = WHAL_STM32N6_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32N6_GPIO_SPEED_FAST,
                .pull = WHAL_STM32N6_GPIO_PULL_UP,
                .altFn = 7,
            },
            [UART_RX_PIN] = { /* USART1 RX on PA10, AF7 */
                .port = WHAL_STM32N6_GPIO_PORT_A,
                .pin = 10,
                .mode = WHAL_STM32N6_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32N6_GPIO_SPEED_FAST,
                .pull = WHAL_STM32N6_GPIO_PULL_UP,
                .altFn = 7,
            },
            [SPI_SCK_PIN] = { /* SPI1 SCK on PA5, AF5 */
                .port = WHAL_STM32N6_GPIO_PORT_A,
                .pin = 5,
                .mode = WHAL_STM32N6_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32N6_GPIO_SPEED_FAST,
                .pull = WHAL_STM32N6_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MISO_PIN] = { /* SPI1 MISO on PA6, AF5 */
                .port = WHAL_STM32N6_GPIO_PORT_A,
                .pin = 6,
                .mode = WHAL_STM32N6_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32N6_GPIO_SPEED_FAST,
                .pull = WHAL_STM32N6_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MOSI_PIN] = { /* SPI1 MOSI on PB5, AF5 */
                .port = WHAL_STM32N6_GPIO_PORT_B,
                .pin = 5,
                .mode = WHAL_STM32N6_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32N6_GPIO_SPEED_FAST,
                .pull = WHAL_STM32N6_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_CS_PIN] = { /* SPI CS on PD14 */
                .port = WHAL_STM32N6_GPIO_PORT_D,
                .pin = 14,
                .mode = WHAL_STM32N6_GPIO_MODE_OUT,
                .outType = WHAL_STM32N6_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32N6_GPIO_SPEED_FAST,
                .pull = WHAL_STM32N6_GPIO_PULL_UP,
            },
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    WHAL_CORTEX_M55_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 64000000 / 1000, /* 64 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART */
whal_Uart g_whalUart = {
    WHAL_STM32N657_USART1_DEVICE,

    .cfg = &(whal_Stm32n6Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32N6_UART_BRR(64000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    WHAL_STM32N657_SPI1_DEVICE,

    .cfg = &(whal_Stm32n6Spi_Cfg) {
        .pclk = 64000000,
        .timeout = &g_whalTimeout,
    },
};

/* RNG */
whal_Rng g_whalRng = {
    WHAL_STM32N657_RNG_DEVICE,

    .cfg = &(whal_Stm32n6Rng_Cfg) {
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

    err = whal_Clock_Init(&g_whalClock);
    if (err)
        return err;

    /* Enable bus clocks first */
    for (size_t i = 0; i < BUS_CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_busClocks[i]);
        if (err)
            return err;
    }

    /* Enable peripheral clocks */
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

    /* Disable peripheral clocks */
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_clocks[i]);
        if (err)
            return err;
    }

    /* Disable bus clocks */
    for (size_t i = 0; i < BUS_CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_busClocks[i]);
        if (err)
            return err;
    }

    err = whal_Clock_Deinit(&g_whalClock);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
