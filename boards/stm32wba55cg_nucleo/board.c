/* Board configuration for the STM32WBA55CG Nucleo (NUCLEO-WBA55CG) */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32wba55cg.h>
#include "peripheral.h"

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

/* IRQ */
whal_Irq g_whalIrq = {
    .regmap = { WHAL_CORTEX_M33_NVIC_REGMAP },
    .driver = WHAL_CORTEX_M33_NVIC_DRIVER,
};

/* Clock: PLL1 from HSE32, targeting 100 MHz
 *   HSE32 = 32 MHz
 *   PLL1M = 1 (div 2) -> ref_ck = 16 MHz
 *   PLL1N = 25 -> VCO = 16 * 25 = 400 MHz
 *   PLL1R = 3 (div 4) -> pll1rclk = 100 MHz
 *   PLL1RGE = 3 (8-16 MHz range)
 */
whal_Clock g_whalClock = {
    .regmap = { WHAL_STM32WBA55_RCC_PLL_REGMAP },
    .driver = WHAL_STM32WBA55_RCC_PLL_DRIVER,

    .cfg = &(whal_Stm32wbaRcc_Cfg) {
        .sysClkSrc = WHAL_STM32WBA_RCC_SYSCLK_SRC_PLL1,
        .sysClkCfg = &(whal_Stm32wbaRcc_Pll1Cfg) {
            .clkSrc = WHAL_STM32WBA_RCC_PLL1SRC_HSE32,
            .rge = WHAL_STM32WBA_RCC_PLL1RGE_8_16,
            .m = 1,   /* div 2 */
            .n = 25,  /* VCO = 400 MHz */
            .r = 3,   /* div 4 -> 100 MHz */
            .q = 0,
            .p = 0,
        },
    },
};

static const whal_Stm32wbaRcc_Clk g_flashClock = {WHAL_STM32WBA55_FLASH_CLOCK};

static const whal_Stm32wbaRcc_Clk g_clocks[] = {
    {WHAL_STM32WBA55_GPIOA_CLOCK},
    {WHAL_STM32WBA55_GPIOB_CLOCK},
    {WHAL_STM32WBA55_GPIOC_CLOCK},
    {WHAL_STM32WBA55_USART1_CLOCK},
    {WHAL_STM32WBA55_SPI1_CLOCK},
    {WHAL_STM32WBA55_RNG_CLOCK},
    {WHAL_STM32WBA55_AES_CLOCK},
    {WHAL_STM32WBA55_HASH_CLOCK},
    {WHAL_STM32WBA55_I2C1_CLOCK},
#ifdef BOARD_WATCHDOG_WWDG
    {WHAL_STM32WBA55_WWDG_CLOCK},
#endif
};
#define CLOCK_COUNT (sizeof(g_clocks) / sizeof(g_clocks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    .regmap = { WHAL_STM32WBA55_GPIO_REGMAP },
    .driver = WHAL_STM32WBA55_GPIO_DRIVER,

    .cfg = &(whal_Stm32wbaGpio_Cfg) {
        .pinCfg = (whal_Stm32wbaGpio_PinCfg[PIN_COUNT]) {
            /* LED: PA9 (LD2, Green), output, push-pull, low speed, pull-up */
            [LED_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_A, 9, WHAL_STM32WBA_GPIO_MODE_OUT,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_LOW,
                WHAL_STM32WBA_GPIO_PULL_UP, 0),
            /* USART1 TX: PB12, AF7 */
            [UART_TX_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 12, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 7),
            /* USART1 RX: PA8, AF7 */
            [UART_RX_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_A, 8, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 7),
            /* SPI1 SCK: PB4, AF5 */
            [SPI_SCK_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 4, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_NONE, 5),
            /* SPI1 MISO: PB3, AF5 */
            [SPI_MISO_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 3, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_NONE, 5),
            /* SPI1 MOSI: PA15, AF5 */
            [SPI_MOSI_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_A, 15, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_NONE, 5),
            /* SPI CS: PA12, output, push-pull */
            [SPI_CS_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_A, 12, WHAL_STM32WBA_GPIO_MODE_OUT,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 0),
            /* I2C1 SCL: PB2, AF4, open-drain */
            [I2C_SCL_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 2, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 4),
            /* I2C1 SDA: PB1, AF4, open-drain */
            [I2C_SDA_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 1, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 4),
        },
        .pinCount = PIN_COUNT,
    },
};

/* I2C */
whal_I2c g_whalI2c = {
    .regmap = { WHAL_STM32WBA55_I2C1_REGMAP },
    .driver = WHAL_STM32WBA55_I2C1_DRIVER,

    .cfg = &(whal_Stm32wbaI2c_Cfg) {
        .pclk = 100000000,
        .timeout = &g_whalTimeout,
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    .regmap = { WHAL_STM32WBA55_SPI1_REGMAP },
    .driver = WHAL_STM32WBA55_SPI1_DRIVER,

    .cfg = &(whal_Stm32wbaSpi_Cfg) {
        .pclk = 100000000,
        .timeout = &g_whalTimeout,
    },
};

/* Timer (SysTick at 100 MHz) */
whal_Timer g_whalTimer = {
    .regmap = { WHAL_CORTEX_M33_SYSTICK_REGMAP },
    .driver = WHAL_CORTEX_M33_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 100000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* DMA */
#ifdef BOARD_DMA

whal_Dma g_whalDma1 = {
    .regmap = { WHAL_STM32WBA55_GPDMA1_REGMAP },
    .driver = WHAL_STM32WBA55_GPDMA1_DRIVER,
    .cfg = &(whal_Stm32wbaGpdma_Cfg){
        .numChannels = 8,
        .timeout = &g_whalTimeout,
    },
};

static const whal_Stm32wbaRcc_Clk g_dmaClock = {WHAL_STM32WBA55_GPDMA1_CLOCK};

void GPDMA1_Channel0_IRQHandler(void)
{
    whal_Stm32wbaGpdma_IRQHandler(&g_whalDma1, 0,
                                   whal_Stm32wbaUartDma_TxCallback,
                                   g_whalUart.cfg);
}

void GPDMA1_Channel1_IRQHandler(void)
{
    whal_Stm32wbaGpdma_IRQHandler(&g_whalDma1, 1,
                                   whal_Stm32wbaUartDma_RxCallback,
                                   g_whalUart.cfg);
}
#endif

/* UART (USART1 via VCP at 115200 baud) */
#ifdef BOARD_DMA
whal_Uart g_whalUart = {
    .regmap = { WHAL_STM32WBA55_USART1_REGMAP },
    .driver = &whal_Stm32wbaUartDma_Driver,
    .cfg = &(whal_Stm32wbaUartDma_Cfg) {
        .base = {
            .brr = WHAL_STM32WBA_UART_BRR(100000000, 115200),
            .timeout = &g_whalTimeout,
        },
        .dma = &g_whalDma1,
        .txCh = 0,
        .rxCh = 1,
        .txChCfg = &(whal_Stm32wbaGpdma_ChCfg){WHAL_STM32WBA55_USART1_TX_DMA_CFG},
        .rxChCfg = &(whal_Stm32wbaGpdma_ChCfg){WHAL_STM32WBA55_USART1_RX_DMA_CFG},
    },
};
#else
whal_Uart g_whalUart = {
    .regmap = { WHAL_STM32WBA55_USART1_REGMAP },
    .driver = WHAL_STM32WBA55_USART1_DRIVER,

    .cfg = &(whal_Stm32wbaUart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32WBA_UART_BRR(100000000, 115200),
    },
};
#endif

/* Flash */
whal_Flash g_whalFlash = {
    .regmap = { WHAL_STM32WBA55_FLASH_REGMAP },
    .driver = WHAL_STM32WBA55_FLASH_DRIVER,

    .cfg = &(whal_Stm32wbaFlash_Cfg) {
        .timeout = &g_whalTimeout,
        .startAddr = 0x08000000,
        .size = 0x100000, /* 1 MB */
    },
};

/* RNG */
whal_Rng g_whalRng = {
    .regmap = { WHAL_STM32WBA55_RNG_REGMAP },
    .driver = WHAL_STM32WBA55_RNG_DRIVER,

    .cfg = &(whal_Stm32wbaRng_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

/* Crypto (AES hardware accelerator) */
whal_Crypto g_whalCrypto = {
    .regmap = { WHAL_STM32WBA55_AES_REGMAP },
    .driver = WHAL_STM32WBA55_AES_DRIVER,

    .cfg = &(whal_Stm32wbaAes_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

/* Hash (HASH hardware accelerator) */
whal_Crypto g_whalHash = {
    .regmap = { WHAL_STM32WBA55_HASH_REGMAP },
    .driver = WHAL_STM32WBA55_HASH_DRIVER,

    .cfg = &(whal_Stm32wbaHash_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

#ifdef BOARD_WATCHDOG_IWDG
whal_Watchdog g_whalWatchdog = {
    .regmap = { WHAL_STM32WBA55_IWDG_REGMAP },
    .driver = WHAL_STM32WBA55_IWDG_DRIVER,

    .cfg = &(whal_Stm32wbaIwdg_Cfg) {
        .prescaler = WHAL_STM32WBA_IWDG_PR_32,
        .reload = 100,
        .timeout = &g_whalTimeout,
    },
};
#elif defined(BOARD_WATCHDOG_WWDG)
whal_Watchdog g_whalWatchdog = {
    .regmap = { WHAL_STM32WBA55_WWDG_REGMAP },
    .driver = WHAL_STM32WBA55_WWDG_DRIVER,

    .cfg = &(whal_Stm32wbaWwdg_Cfg) {
        .prescaler = WHAL_STM32WBA_WWDG_TB_128,
        .window = 0x7F,
        .counter = 0x7F,
    },
};
#endif

void Board_WaitMs(size_t ms)
{
    uint32_t startCount = g_tick;
    while (g_tick - startCount < ms);
}

/*
 * Switch PWR voltage scaling to Range 1 (required for >16 MHz operation).
 * After reset the device is in Range 2 (max 16 MHz). Must switch to Range 1
 * before configuring PLL or increasing SYSCLK.
 *
 * PWR base: 0x46020800
 * PWR_VOSR offset: 0x00C
 *   bit 16 VOS: 0=Range2, 1=Range1
 *   bit 15 VOSRDY: read-only, 1 when stable
 */
#define PWR_BASE       0x46020800
#define PWR_VOSR_REG   0x00C
#define PWR_VOSR_VOS_Msk    (1UL << 16)
#define PWR_VOSR_VOSRDY_Msk (1UL << 15)

static whal_Error Board_SetVosRange1(void)
{
    /* Set VOS to Range 1 */
    whal_Reg_Update(PWR_BASE, PWR_VOSR_REG, PWR_VOSR_VOS_Msk, PWR_VOSR_VOS_Msk);

    /* Wait for VOSRDY */
    while (!(whal_Reg_Read(PWR_BASE, PWR_VOSR_REG) & PWR_VOSR_VOSRDY_Msk))
        ;

    return WHAL_SUCCESS;
}

whal_Error Board_Init(void)
{
    whal_Error err;

    /* Enable PWR clock (RCC_AHB4ENR bit 2) -- required to access PWR registers */
    static const whal_Stm32wbaRcc_Clk pwrClock = {WHAL_STM32WBA55_PWR_CLOCK};
    err = whal_Clock_Enable(&g_whalClock, &pwrClock);
    if (err)
        return err;

    /* Switch to voltage Range 1 (required for >16 MHz operation).
     * After reset device is in Range 2 which caps SYSCLK at 16 MHz. */
    err = Board_SetVosRange1();
    if (err)
        return err;

    /* Enable flash clock and set latency before increasing clock speed.
     * At 100 MHz, 3.3V: 3 wait states required (RM0493 Table 69). */
    err = whal_Clock_Enable(&g_whalClock, &g_flashClock);
    if (err)
        return err;

    err = whal_Stm32wbaFlash_Ext_SetLatency(&g_whalFlash, 3);
    if (err)
        return err;

    /* Set HPRE5 to div 4 before switching to PLL (AHB5 max 32 MHz).
     * 100 MHz / 4 = 25 MHz. Value 0b101 = div 4. */
    err = whal_Stm32wbaRcc_Ext_SetHpre5(&g_whalClock, 5);
    if (err)
        return err;

    err = whal_Clock_Init(&g_whalClock);
    if (err)
        return err;

#ifdef BOARD_WATCHDOG_IWDG
    /* Enable LSI oscillator required by IWDG */
    err = whal_Stm32wbaRcc_Ext_EnableLsi(&g_whalClock, 1);
    if (err)
        return err;
#endif

    /* Select HSI16 as RNG kernel clock source.
     * Default after reset is LSE which is not enabled. HSI16 is always on. */
    err = whal_Stm32wbaRcc_Ext_SetRngClockSrc(&g_whalClock,
                                               WHAL_STM32WBA_RCC_RNGSEL_HSI16);
    if (err)
        return err;

    /* Enable peripheral clocks */
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_clocks[i]);
        if (err)
            return err;
    }

    err = whal_Irq_Init(&g_whalIrq);
    if (err)
        return err;

#ifdef BOARD_DMA
    err = whal_Clock_Enable(&g_whalClock, &g_dmaClock);
    if (err)
        return err;
    err = whal_Dma_Init(&g_whalDma1);
    if (err)
        return err;

    /* Enable NVIC interrupts for GPDMA1 channel 0 (IRQ 29) and channel 1 (IRQ 30) */
    err = whal_Irq_Enable(&g_whalIrq, 29, NULL);
    if (err)
        return err;
    err = whal_Irq_Enable(&g_whalIrq, 30, NULL);
    if (err)
        return err;
#endif

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

    err = whal_Flash_Init(&g_whalFlash);
    if (err)
        return err;

    err = whal_Rng_Init(&g_whalRng);
    if (err)
        return err;

    err = whal_Crypto_Init(&g_whalCrypto);
    if (err)
        return err;

    err = whal_Crypto_Init(&g_whalHash);
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

    err = whal_Crypto_Deinit(&g_whalHash);
    if (err)
        return err;

    err = whal_Crypto_Deinit(&g_whalCrypto);
    if (err)
        return err;

    err = whal_Rng_Deinit(&g_whalRng);
    if (err)
        return err;

    err = whal_Flash_Deinit(&g_whalFlash);
    if (err)
        return err;

    err = whal_I2c_Deinit(&g_whalI2c);
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

#ifdef BOARD_DMA
    whal_Irq_Disable(&g_whalIrq, 29);
    whal_Irq_Disable(&g_whalIrq, 30);

    err = whal_Dma_Deinit(&g_whalDma1);
    if (err)
        return err;
    err = whal_Clock_Disable(&g_whalClock, &g_dmaClock);
    if (err)
        return err;
#endif

    err = whal_Irq_Deinit(&g_whalIrq);
    if (err)
        return err;

    /* Disable peripheral clocks */
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_clocks[i]);
        if (err)
            return err;
    }

#ifdef BOARD_WATCHDOG_IWDG
    err = whal_Stm32wbaRcc_Ext_EnableLsi(&g_whalClock, 0);
    if (err)
        return err;
#endif

    err = whal_Clock_Deinit(&g_whalClock);
    if (err)
        return err;

    /* Reduce flash latency then disable flash clock */
    err = whal_Stm32wbaFlash_Ext_SetLatency(&g_whalFlash, 0);
    if (err)
        return err;

    err = whal_Clock_Disable(&g_whalClock, &g_flashClock);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
