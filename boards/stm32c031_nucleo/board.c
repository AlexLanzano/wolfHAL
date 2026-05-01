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
    .regmap = { WHAL_STM32C031_RCC_REGMAP },
};

static const whal_Stm32c0_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32C031_GPIOA_CLOCK},
    {WHAL_STM32C031_GPIOB_CLOCK},
    {WHAL_STM32C031_GPIOC_CLOCK},
    {WHAL_STM32C031_USART2_CLOCK},
    {WHAL_STM32C031_SPI1_CLOCK},
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    .regmap = { WHAL_STM32C031_GPIO_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32c0_Gpio_Cfg) {
        .pinCfg = (whal_Stm32c0_Gpio_PinCfg[PIN_COUNT]) {
            /* LD4 Green LED on PA5 */
            [LED_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 5, WHAL_STM32C0_GPIO_MODE_OUT,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_LOW,
                WHAL_STM32C0_GPIO_PULL_NONE, 0),
            /* USART2 TX on PA2, AF1 (ST-Link VCP) */
            [UART_TX_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 2, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_UP, 1),
            /* USART2 RX on PA3, AF1 (ST-Link VCP) */
            [UART_RX_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 3, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_UP, 1),
            /* SPI1 SCK on PA1, AF0 */
            [SPI_SCK_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 1, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_NONE, 0),
            /* SPI1 MISO on PA6, AF0 */
            [SPI_MISO_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 6, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_NONE, 0),
            /* SPI1 MOSI on PA7, AF0 */
            [SPI_MOSI_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 7, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_NONE, 0),
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    .regmap = { WHAL_CORTEX_M0PLUS_SYSTICK_REGMAP },
    .driver = WHAL_CORTEX_M0PLUS_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 48000000 / 1000, /* 48 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART */
whal_Uart g_whalUart = {
    .regmap = { WHAL_STM32C031_USART2_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32c0_Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32C0_UART_BRR(48000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    .regmap = { WHAL_STM32C031_SPI1_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32c0_Spi_Cfg) {
        .pclk = 48000000,
        .timeout = &g_whalTimeout,
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    .regmap = { WHAL_STM32C031_FLASH_REGMAP },
    .driver = WHAL_STM32C031_FLASH_DRIVER,

    .cfg = &(whal_Stm32c0_Flash_Cfg) {
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
    err = whal_Stm32c0_Flash_Ext_SetLatency(&g_whalFlash, WHAL_STM32C0_FLASH_LATENCY_1);
    if (err)
        return err;

    /* HSI48 / 1 = 48 MHz, then SYSCLK = HSISYS. */
    err = whal_Stm32c0_Rcc_EnableHsi(&g_whalClock, WHAL_STM32C0_RCC_HSIDIV_1);
    if (err)
        return err;
    err = whal_Stm32c0_Rcc_SetSysClock(&g_whalClock, WHAL_STM32C0_RCC_SYSCLK_SRC_HSISYS);
    if (err)
        return err;

    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32c0_Rcc_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
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

    for (size_t i = PERIPH_CLK_COUNT; i-- > 0; ) {
        err = whal_Stm32c0_Rcc_DisablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    return WHAL_SUCCESS;
}
