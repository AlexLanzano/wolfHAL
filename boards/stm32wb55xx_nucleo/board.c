/* Example board configuration for the STM32WB55 Nucleo dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32wb55xx.h>

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

/* Clock */
whal_Clock g_whalClock = {
    .regmap = { WHAL_STM32WB55_RCC_REGMAP },
};

static const whal_Stm32wb_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32WB55_GPIOA_GATE},
    {WHAL_STM32WB55_GPIOB_GATE},
    {WHAL_STM32WB55_UART1_GATE},
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    .regmap = { WHAL_STM32WB55_GPIO_REGMAP },

    .cfg = &(whal_Stm32wb_Gpio_Cfg) {
        .pinCfg = (whal_Stm32wb_Gpio_PinCfg[PIN_COUNT]) {
            /* LED: PB5, output, push-pull, low speed, pull-up */
            [LED_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 5, WHAL_STM32WB_GPIO_MODE_OUT,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_LOW,
                WHAL_STM32WB_GPIO_PULL_UP, 0),
            /* UART1 TX: PB6, AF7 */
            [UART_TX_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 6, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 7),
            /* UART1 RX: PB7, AF7 */
            [UART_RX_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 7, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 7),
        },
        .pinCount = PIN_COUNT,
    },
};

/* UART */
whal_Uart g_whalUart = {
    .regmap = { WHAL_STM32WB55_UART1_REGMAP },

    .cfg = &(whal_Stm32wb_Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32WB_UART_BRR(64000000, 115200),
    },
};

/* Timer (SysTick) */
whal_Timer g_whalTimer = {
    .regmap = { WHAL_CORTEX_M4_SYSTICK_REGMAP },
    .driver = WHAL_CORTEX_M4_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 64000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    .regmap = { WHAL_STM32WB55_FLASH_REGMAP },
    .driver = WHAL_STM32WB55_FLASH_DRIVER,

    .cfg = &(whal_Stm32wb_Flash_Cfg) {
        .timeout = &g_whalTimeout,
        .startAddr = 0x08000000,
        .size = 0x80000, /* 512 KB (upper half reserved for BLE stack) */
    },
};

void Board_WaitMs(size_t ms)
{
    uint32_t startCount = g_tick;
    while (g_tick - startCount < ms);
}

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Flash latency must be set before SYSCLK rises above 16 MHz. */
    err = whal_Stm32wb_Flash_Ext_SetLatency(&g_whalFlash, WHAL_STM32WB_FLASH_LATENCY_3);
    if (err)
        return err;

    /* MSI 4 MHz -> PLL VCO 128 MHz -> PLLR /2 = 64 MHz -> SYSCLK */
    err = whal_Stm32wb_Rcc_EnableMsi(&g_whalClock, WHAL_STM32WB_RCC_MSIRANGE_4MHz);
    if (err)
        return err;

    err = whal_Stm32wb_Rcc_EnablePll(&g_whalClock, &(whal_Stm32wb_Rcc_PllCfg){
        .clkSrc = WHAL_STM32WB_RCC_PLLCLK_SRC_MSI,
        .n = 32, .m = 0, .r = 1, .q = 0, .p = 0,
    });
    if (err)
        return err;

    err = whal_Stm32wb_Rcc_SetSysClock(&g_whalClock, WHAL_STM32WB_RCC_SYSCLK_SRC_PLL);
    if (err)
        return err;

    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32wb_Rcc_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    err = whal_Gpio_Init(&g_whalGpio);
    if (err)
        return err;

    err = whal_Uart_Init(&g_whalUart);
    if (err)
        return err;

    err = whal_Flash_Init(&g_whalFlash);
    if (err)
        return err;

    err = whal_Timer_Init(&g_whalTimer);
    if (err)
        return err;

    err = whal_Timer_Start(&g_whalTimer);
    if (err)
        return err;

    return WHAL_SUCCESS;
}

whal_Error Board_Deinit(void)
{
    whal_Error err;

    err = whal_Timer_Stop(&g_whalTimer);
    if (err)
        return err;

    err = whal_Timer_Deinit(&g_whalTimer);
    if (err)
        return err;

    err = whal_Flash_Deinit(&g_whalFlash);
    if (err)
        return err;

    err = whal_Uart_Deinit(&g_whalUart);
    if (err)
        return err;

    err = whal_Gpio_Deinit(&g_whalGpio);
    if (err)
        return err;

    for (size_t i = PERIPH_CLK_COUNT; i-- > 0; ) {
        err = whal_Stm32wb_Rcc_DisablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    err = whal_Stm32wb_Rcc_SetSysClock(&g_whalClock, WHAL_STM32WB_RCC_SYSCLK_SRC_MSI);
    if (err)
        return err;

    err = whal_Stm32wb_Rcc_DisablePll(&g_whalClock);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
