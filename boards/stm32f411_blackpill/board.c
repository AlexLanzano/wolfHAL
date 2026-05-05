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
static const whal_Stm32f4_Flash_Sector g_flashSectors[] = {
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
    .base = WHAL_STM32F411_RCC_BASE,
};

static const whal_Stm32f4_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32F411_GPIOA_CLOCK},
    {WHAL_STM32F411_GPIOC_CLOCK},
    {WHAL_STM32F411_USART2_CLOCK},
    {WHAL_STM32F411_SPI1_CLOCK},
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    .base = WHAL_STM32F411_GPIO_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32f4_Gpio_Cfg) {
        .pinCfg = (whal_Stm32f4_Gpio_PinCfg[PIN_COUNT]) {
            /* LED on PC13 (active low) */
            [LED_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_C, 13, WHAL_STM32F4_GPIO_MODE_OUT,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_LOW,
                WHAL_STM32F4_GPIO_PULL_NONE, 0),
            /* USART2 TX on PA2 (AF7) */
            [UART_TX_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 2, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_UP, 7),
            /* USART2 RX on PA3 (AF7) */
            [UART_RX_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 3, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_UP, 7),
            /* SPI1 SCK on PA5 (AF5) */
            [SPI_SCK_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 5, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_NONE, 5),
            /* SPI1 MISO on PA6 (AF5) */
            [SPI_MISO_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 6, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_NONE, 5),
            /* SPI1 MOSI on PA7 (AF5) */
            [SPI_MOSI_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 7, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_NONE, 5),
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    .base = WHAL_CORTEX_M4_SYSTICK_BASE,
    .driver = WHAL_CORTEX_M4_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 100000000 / 1000, /* 100 MHz / 1 kHz = 1 ms tick */
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART */
whal_Uart g_whalUart = {
    .base = WHAL_STM32F411_USART2_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32f4_Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32F4_UART_BRR(50000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    .base = WHAL_STM32F411_SPI1_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32f4_Spi_Cfg) {
        .pclk = 100000000,
        .timeout = &g_whalTimeout,
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    .base = WHAL_STM32F411_FLASH_BASE,
    .driver = WHAL_STM32F411_FLASH_DRIVER,

    .cfg = &(whal_Stm32f4_Flash_Cfg) {
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
    err = whal_Stm32f4_Flash_Ext_SetLatency(&g_whalFlash,
                                            WHAL_STM32F4_FLASH_LATENCY_3);
    if (err)
        return err;

    /* HSE 25 MHz -> PLL (25/25 * 200 / 2 = 100 MHz) -> SYSCLK = PLL */
    err = whal_Stm32f4_Rcc_EnableOsc(&g_whalClock,
        &(whal_Stm32f4_Rcc_OscCfg){WHAL_STM32F4_RCC_HSE_CFG});
    if (err)
        return err;
    err = whal_Stm32f4_Rcc_EnablePll(&g_whalClock, &(whal_Stm32f4_Rcc_PllCfg){
        .clkSrc = WHAL_STM32F4_RCC_PLLCLK_SRC_HSE,
        .m = 25, .n = 200, .p = 0, .q = 4,
    });
    if (err)
        return err;
    /* APB1 = SYSCLK/2 = 50 MHz, APB2 = SYSCLK/1 = 100 MHz */
    err = whal_Stm32f4_Rcc_SetBusPrescalers(&g_whalClock,
        &(whal_Stm32f4_Rcc_BusCfg){.ppre1 = 4, .ppre2 = 0});
    if (err)
        return err;
    err = whal_Stm32f4_Rcc_SetSysClock(&g_whalClock, WHAL_STM32F4_RCC_SYSCLK_SRC_PLL);
    if (err)
        return err;

    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32f4_Rcc_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
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
        err = whal_Stm32f4_Rcc_DisablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    err = whal_Stm32f4_Rcc_SetSysClock(&g_whalClock, WHAL_STM32F4_RCC_SYSCLK_SRC_HSI);
    if (err)
        return err;
    err = whal_Stm32f4_Rcc_DisablePll(&g_whalClock);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
