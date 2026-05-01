#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32f091xx.h>
#include "peripheral.h"

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
    .timeoutTicks = 1000,
    .GetTick = Board_GetTick,
};

/* Clock — PLL at 48 MHz (HSI/2 * 12) */
whal_Clock g_whalClock = {
    .regmap = { WHAL_STM32F091_RCC_REGMAP },
};

static const whal_Stm32f0_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32F091_GPIOA_CLOCK},
    {WHAL_STM32F091_GPIOB_CLOCK},
    {WHAL_STM32F091_GPIOC_CLOCK},
    {WHAL_STM32F091_USART2_CLOCK},
    {WHAL_STM32F091_SPI1_CLOCK},
    {WHAL_STM32F091_I2C1_CLOCK},
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    .regmap = { WHAL_STM32F091_GPIO_REGMAP },

    .cfg = &(whal_Stm32f0_Gpio_Cfg) {
        .pinCfg = (whal_Stm32f0_Gpio_PinCfg[PIN_COUNT]) {
            /* LD2 Green LED on PA5 */
            [LED_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_A, 5, WHAL_STM32F0_GPIO_MODE_OUT,
                WHAL_STM32F0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F0_GPIO_SPEED_LOW,
                WHAL_STM32F0_GPIO_PULL_NONE, 0),
            /* USART2 TX on PA2, AF1 */
            [UART_TX_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_A, 2, WHAL_STM32F0_GPIO_MODE_ALTFN,
                WHAL_STM32F0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F0_GPIO_SPEED_FAST,
                WHAL_STM32F0_GPIO_PULL_UP, 1),
            /* USART2 RX on PA3, AF1 */
            [UART_RX_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_A, 3, WHAL_STM32F0_GPIO_MODE_ALTFN,
                WHAL_STM32F0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F0_GPIO_SPEED_FAST,
                WHAL_STM32F0_GPIO_PULL_UP, 1),
            /* SPI1 SCK on PB3, AF0 */
            [SPI_SCK_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_B, 3, WHAL_STM32F0_GPIO_MODE_ALTFN,
                WHAL_STM32F0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F0_GPIO_SPEED_FAST,
                WHAL_STM32F0_GPIO_PULL_NONE, 0),
            /* SPI1 MISO on PB4, AF0 */
            [SPI_MISO_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_B, 4, WHAL_STM32F0_GPIO_MODE_ALTFN,
                WHAL_STM32F0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F0_GPIO_SPEED_FAST,
                WHAL_STM32F0_GPIO_PULL_NONE, 0),
            /* SPI1 MOSI on PB5, AF0 */
            [SPI_MOSI_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_B, 5, WHAL_STM32F0_GPIO_MODE_ALTFN,
                WHAL_STM32F0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F0_GPIO_SPEED_FAST,
                WHAL_STM32F0_GPIO_PULL_NONE, 0),
            /* SPI CS on PB6, output, push-pull */
            [SPI_CS_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_B, 6, WHAL_STM32F0_GPIO_MODE_OUT,
                WHAL_STM32F0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F0_GPIO_SPEED_FAST,
                WHAL_STM32F0_GPIO_PULL_UP, 0),
            /* I2C1 SCL on PB8, AF1, open-drain */
            [I2C_SCL_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_B, 8, WHAL_STM32F0_GPIO_MODE_ALTFN,
                WHAL_STM32F0_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32F0_GPIO_SPEED_FAST,
                WHAL_STM32F0_GPIO_PULL_UP, 1),
            /* I2C1 SDA on PB9, AF1, open-drain */
            [I2C_SDA_PIN] = WHAL_STM32F0_GPIO_PIN(
                WHAL_STM32F0_GPIO_PORT_B, 9, WHAL_STM32F0_GPIO_MODE_ALTFN,
                WHAL_STM32F0_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32F0_GPIO_SPEED_FAST,
                WHAL_STM32F0_GPIO_PULL_UP, 1),
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer — SysTick at 1 ms */
whal_Timer g_whalTimer = {
    .regmap = { WHAL_CORTEX_M0_SYSTICK_REGMAP },
    .driver = WHAL_CORTEX_M0_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 48000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART — USART2 at 115200 baud */
whal_Uart g_whalUart = {
    .regmap = { WHAL_STM32F091_USART2_REGMAP },

    .cfg = &(whal_Stm32f0_Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32F0_UART_BRR(48000000, 115200),
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    .regmap = { WHAL_STM32F091_SPI1_REGMAP },

    .cfg = &(whal_Stm32f0_Spi_Cfg) {
        .pclk = 48000000,
        .timeout = &g_whalTimeout,
    },
};

/* I2C — I2C1 */
whal_I2c g_whalI2c = {
    .regmap = { WHAL_STM32F091_I2C1_REGMAP },

    .cfg = &(whal_Stm32f0_I2c_Cfg) {
        .pclk = 48000000,
        .timeout = &g_whalTimeout,
    },
};

/* Flash — 256 KB */
whal_Flash g_whalFlash = {
    .regmap = { WHAL_STM32F091_FLASH_REGMAP },
    .driver = WHAL_STM32F091_FLASH_DRIVER,

    .cfg = &(whal_Stm32f0_Flash_Cfg) {
        .startAddr = 0x08000000,
        .size = 0x40000,
        .timeout = &g_whalTimeout,
    },
};

#ifdef BOARD_WATCHDOG_IWDG
whal_Watchdog g_whalWatchdog = {
    .regmap = { WHAL_STM32F091_IWDG_REGMAP },
    .driver = WHAL_STM32F091_IWDG_DRIVER,

    .cfg = &(whal_Stm32f0_Iwdg_Cfg) {
        .prescaler = WHAL_STM32F0_IWDG_PR_64,
        .reload = 500,
        .timeout = &g_whalTimeout,
    },
};
#endif

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
    err = whal_Stm32f0_Flash_Ext_SetLatency(&g_whalFlash,
                                             WHAL_STM32F0_FLASH_LATENCY_1);
    if (err)
        return err;

    /* HSI -> PLL (HSI/2 * 12 = 48 MHz) -> SYSCLK = PLL */
    err = whal_Stm32f0_Rcc_EnableOsc(&g_whalClock,
        &(whal_Stm32f0_Rcc_OscCfg){WHAL_STM32F0_RCC_HSI_CFG});
    if (err)
        return err;
    err = whal_Stm32f0_Rcc_EnablePll(&g_whalClock, &(whal_Stm32f0_Rcc_PllCfg){
        .clkSrc = WHAL_STM32F0_RCC_PLLSRC_HSI_DIV2,
        .prediv = 1,
        .pllmul = 12,
    });
    if (err)
        return err;
    err = whal_Stm32f0_Rcc_SetSysClock(&g_whalClock, WHAL_STM32F0_RCC_SYSCLK_SRC_PLL);
    if (err)
        return err;

    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32f0_Rcc_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
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

    err = whal_I2c_Init(&g_whalI2c);
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

    err = whal_I2c_Deinit(&g_whalI2c);
    if (err)
        return err;

    err = whal_Uart_Deinit(&g_whalUart);
    if (err)
        return err;

    err = whal_Gpio_Deinit(&g_whalGpio);
    if (err)
        return err;

    for (size_t i = PERIPH_CLK_COUNT; i-- > 0; ) {
        err = whal_Stm32f0_Rcc_DisablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    err = whal_Stm32f0_Rcc_SetSysClock(&g_whalClock, WHAL_STM32F0_RCC_SYSCLK_SRC_HSI);
    if (err)
        return err;
    err = whal_Stm32f0_Rcc_DisablePll(&g_whalClock);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
