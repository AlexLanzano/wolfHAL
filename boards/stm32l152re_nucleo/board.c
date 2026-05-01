#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32l152re.h>
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

/*
 * Clock -- PLL at 32 MHz (HSI 16 MHz * 4 / 2)
 *
 * PLLVCO = HSI * PLLMUL = 16 MHz * 4 = 64 MHz
 * SYSCLK = PLLVCO / PLLDIV = 64 MHz / 2 = 32 MHz
 */
whal_Clock g_whalClock = {
    .regmap = { WHAL_STM32L152_RCC_REGMAP },
};

static const whal_Stm32l1_Rcc_PeriphClk g_pwrClock = {WHAL_STM32L152_PWR_CLOCK};

static const whal_Stm32l1_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32L152_GPIOA_CLOCK},
    {WHAL_STM32L152_GPIOB_CLOCK},
    {WHAL_STM32L152_GPIOC_CLOCK},
    {WHAL_STM32L152_USART2_CLOCK},
    {WHAL_STM32L152_SPI3_CLOCK},
    {WHAL_STM32L152_I2C1_CLOCK},
#ifdef BOARD_WATCHDOG_WWDG
    {WHAL_STM32L152_WWDG_CLOCK},
#endif
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

/* PWR -- Range 1 (1.8 V) to allow SYSCLK up to 32 MHz. */
whal_Power g_whalPower = {
    .regmap = { WHAL_STM32L152_PWR_REGMAP },
};

/* GPIO */
whal_Gpio g_whalGpio = {
    .regmap = { WHAL_STM32L152_GPIO_REGMAP },

    .cfg = &(whal_Stm32l1_Gpio_Cfg) {
        .pinCfg = (whal_Stm32l1_Gpio_PinCfg[PIN_COUNT]) {
            /* LD2 Green LED on PA5 (per UM1724, NUCLEO-L152RE) */
            [LED_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_A, 5, WHAL_STM32L1_GPIO_MODE_OUT,
                WHAL_STM32L1_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32L1_GPIO_SPEED_LOW,
                WHAL_STM32L1_GPIO_PULL_NONE, 0),
            /* USART2 TX on PA2, AF7 */
            [UART_TX_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_A, 2, WHAL_STM32L1_GPIO_MODE_ALTFN,
                WHAL_STM32L1_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32L1_GPIO_SPEED_FAST,
                WHAL_STM32L1_GPIO_PULL_UP, 7),
            /* USART2 RX on PA3, AF7 */
            [UART_RX_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_A, 3, WHAL_STM32L1_GPIO_MODE_ALTFN,
                WHAL_STM32L1_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32L1_GPIO_SPEED_FAST,
                WHAL_STM32L1_GPIO_PULL_UP, 7),
            /* SPI3 SCK on PB3, AF6 (avoids LD2 conflict on PA5) */
            [SPI_SCK_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_B, 3, WHAL_STM32L1_GPIO_MODE_ALTFN,
                WHAL_STM32L1_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32L1_GPIO_SPEED_FAST,
                WHAL_STM32L1_GPIO_PULL_NONE, 6),
            /* SPI3 MISO on PB4, AF6 */
            [SPI_MISO_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_B, 4, WHAL_STM32L1_GPIO_MODE_ALTFN,
                WHAL_STM32L1_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32L1_GPIO_SPEED_FAST,
                WHAL_STM32L1_GPIO_PULL_NONE, 6),
            /* SPI3 MOSI on PB5, AF6 */
            [SPI_MOSI_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_B, 5, WHAL_STM32L1_GPIO_MODE_ALTFN,
                WHAL_STM32L1_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32L1_GPIO_SPEED_FAST,
                WHAL_STM32L1_GPIO_PULL_NONE, 6),
            /* SPI CS on PB12, output, push-pull */
            [SPI_CS_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_B, 12, WHAL_STM32L1_GPIO_MODE_OUT,
                WHAL_STM32L1_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32L1_GPIO_SPEED_FAST,
                WHAL_STM32L1_GPIO_PULL_UP, 0),
            /* I2C1 SCL on PB8, AF4, open-drain */
            [I2C_SCL_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_B, 8, WHAL_STM32L1_GPIO_MODE_ALTFN,
                WHAL_STM32L1_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32L1_GPIO_SPEED_FAST,
                WHAL_STM32L1_GPIO_PULL_UP, 4),
            /* I2C1 SDA on PB9, AF4, open-drain */
            [I2C_SDA_PIN] = WHAL_STM32L1_GPIO_PIN(
                WHAL_STM32L1_GPIO_PORT_B, 9, WHAL_STM32L1_GPIO_MODE_ALTFN,
                WHAL_STM32L1_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32L1_GPIO_SPEED_FAST,
                WHAL_STM32L1_GPIO_PULL_UP, 4),
        },
        .pinCount = PIN_COUNT,
    },
};

/* Timer -- SysTick at 1 ms (32 MHz / 1000) */
whal_Timer g_whalTimer = {
    .regmap = { WHAL_CORTEX_M3_SYSTICK_REGMAP },
    .driver = WHAL_CORTEX_M3_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 32000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* UART -- USART2 at 115200 baud */
whal_Uart g_whalUart = {
    .regmap = { WHAL_STM32L152_USART2_REGMAP },

    .cfg = &(whal_Stm32l1_Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32L1_UART_BRR(32000000, 115200),
    },
};

/* SPI -- SPI3 */
whal_Spi g_whalSpi = {
    .regmap = { WHAL_STM32L152_SPI3_REGMAP },

    .cfg = &(whal_Stm32l1_Spi_Cfg) {
        .pclk = 32000000,
        .timeout = &g_whalTimeout,
    },
};

/* I2C -- I2C1 */
whal_I2c g_whalI2c = {
    .regmap = { WHAL_STM32L152_I2C1_REGMAP },

    .cfg = &(whal_Stm32l1_I2c_Cfg) {
        .pclk = 32000000,
        .timeout = &g_whalTimeout,
    },
};

/* Flash -- 512 KB */
whal_Flash g_whalFlash = {
    .regmap = { WHAL_STM32L152_FLASH_REGMAP },
    .driver = WHAL_STM32L152_FLASH_DRIVER,

    .cfg = &(whal_Stm32l1_Flash_Cfg) {
        .startAddr = 0x08000000,
        .size = 0x80000,
        .timeout = &g_whalTimeout,
    },
};

#ifdef BOARD_WATCHDOG_IWDG
whal_Watchdog g_whalWatchdog = {
    .regmap = { WHAL_STM32L152_IWDG_REGMAP },
    .driver = WHAL_STM32L152_IWDG_DRIVER,

    .cfg = &(whal_Stm32l1_Iwdg_Cfg) {
        .prescaler = WHAL_STM32L1_IWDG_PR_64,
        .reload = 500,
        .timeout = &g_whalTimeout,
    },
};
#elif defined(BOARD_WATCHDOG_WWDG)
whal_Watchdog g_whalWatchdog = {
    .regmap = { WHAL_STM32L152_WWDG_REGMAP },
    .driver = WHAL_STM32L152_WWDG_DRIVER,

    .cfg = &(whal_Stm32l1_Wwdg_Cfg) {
        .prescaler = 3,
        .window = 0x7F,
        .counter = 0x7F,
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

    /* Enable PWR peripheral clock so the power driver can program VOS.
     * The RCC device only uses regmap.base for clock gating, so calling
     * Enable before Init is safe. */
    err = whal_Stm32l1_Rcc_EnablePeriphClk(&g_whalClock, &g_pwrClock);
    if (err)
        return err;

    /* Switch regulator to range 1 (1.8 V) so the PLL can reach 32 MHz.
     * Reset default is range 2, which caps PLL VCO at 48 MHz and SYSCLK at
     * 16 MHz. */
    err = whal_Stm32l1_Pwr_SetVosRange(&g_whalPower,
                                       WHAL_STM32L1_PWR_VOS_RANGE_1,
                                       &g_whalTimeout);
    if (err)
        return err;

    /* Set flash latency before increasing clock speed.
     * STM32L1: 0 WS for HCLK <= 16 MHz, 1 WS for 16 < HCLK <= 32 MHz. */
    err = whal_Stm32l1_Flash_Ext_SetLatency(WHAL_STM32L1_FLASH_LATENCY_1);
    if (err)
        return err;

    /* HSI 16 MHz -> PLL (HSI * 4 / 2 = 32 MHz) -> SYSCLK = PLL */
    err = whal_Stm32l1_Rcc_EnableOsc(&g_whalClock,
        &(whal_Stm32l1_Rcc_OscCfg){WHAL_STM32L1_RCC_HSI_CFG});
    if (err)
        return err;
    err = whal_Stm32l1_Rcc_EnablePll(&g_whalClock, &(whal_Stm32l1_Rcc_PllCfg){
        .clkSrc = WHAL_STM32L1_RCC_PLLSRC_HSI,
        .pllmul = WHAL_STM32L1_RCC_PLLMUL_4,
        .plldiv = WHAL_STM32L1_RCC_PLLDIV_2,
    });
    if (err)
        return err;
    err = whal_Stm32l1_Rcc_SetSysClock(&g_whalClock, WHAL_STM32L1_RCC_SYSCLK_SRC_PLL);
    if (err)
        return err;

    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32l1_Rcc_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
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
        err = whal_Stm32l1_Rcc_DisablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    /* PWR is left in its current voltage range; no Deinit operation. */

    err = whal_Stm32l1_Rcc_DisablePeriphClk(&g_whalClock, &g_pwrClock);
    if (err)
        return err;

    err = whal_Stm32l1_Rcc_SetSysClock(&g_whalClock, WHAL_STM32L1_RCC_SYSCLK_SRC_MSI);
    if (err)
        return err;
    err = whal_Stm32l1_Rcc_DisablePll(&g_whalClock);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
