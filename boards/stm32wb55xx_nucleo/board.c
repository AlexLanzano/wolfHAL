/* Example board configuration for the STM32WB55 Nucleo dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32wb55xx.h>
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
    .base = WHAL_CORTEX_M4_NVIC_BASE,
    .driver = WHAL_CORTEX_M4_NVIC_DRIVER,
};

/* Clock */
whal_Clock g_whalClock = {
    .base = WHAL_STM32WB55_RCC_BASE,
    /* .driver: direct API mapping */
};

static const whal_Stm32wb_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32WB55_GPIOA_GATE},
    {WHAL_STM32WB55_GPIOB_GATE},
    {WHAL_STM32WB55_UART1_GATE},
    {WHAL_STM32WB55_SPI1_GATE},
    {WHAL_STM32WB55_RNG_GATE},
    {WHAL_STM32WB55_AES1_GATE},
    {WHAL_STM32WB55_I2C1_GATE},
#ifdef BOARD_WATCHDOG_WWDG
    {WHAL_STM32WB55_WWDG_GATE},
#endif
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    .base = WHAL_STM32WB55_GPIO_BASE,
    /* .driver: direct API mapping */

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
            /* SPI1 SCK: PA5, AF5 */
            [SPI_SCK_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_A, 5, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_NONE, 5),
            /* SPI1 MISO: PA6, AF5 */
            [SPI_MISO_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_A, 6, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_NONE, 5),
            /* SPI1 MOSI: PA7, AF5 */
            [SPI_MOSI_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_A, 7, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_NONE, 5),
            /* SPI CS: PA4, output, push-pull */
            [SPI_CS_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_A, 4, WHAL_STM32WB_GPIO_MODE_OUT,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 0),
            /* I2C1 SCL: PB8, AF4, open-drain */
            [I2C_SCL_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 8, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 4),
            /* I2C1 SDA: PB9, AF4, open-drain */
            [I2C_SDA_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 9, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 4),
        },
        .pinCount = PIN_COUNT,
    },
};

/* I2C */
whal_I2c g_whalI2c = {
    .base = WHAL_STM32WB55_I2C1_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32wb_I2c_Cfg) {
        .pclk = 64000000,
        .timeout = &g_whalTimeout,
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    .base = WHAL_STM32WB55_SPI1_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32wb_Spi_Cfg) {
        .pclk = 64000000,
        .timeout = &g_whalTimeout,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    .base = WHAL_CORTEX_M4_SYSTICK_BASE,
    .driver = WHAL_CORTEX_M4_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 64000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* DMA */
#ifdef BOARD_DMA

whal_Dma g_whalDma1 = {
    .base = WHAL_STM32WB55_DMA1_BASE,
    /* .driver: direct API mapping */
    .cfg = &(whal_Stm32wb_Dma_Cfg){WHAL_STM32WB55_DMA1_CFG},
};

void DMA1_Channel4_IRQHandler(void)
{
    whal_Stm32wb_Dma_IRQHandler(&g_whalDma1, 3,
                                whal_Stm32wb_UartDma_TxCallback, g_whalUart.cfg);
}

void DMA1_Channel5_IRQHandler(void)
{
    whal_Stm32wb_Dma_IRQHandler(&g_whalDma1, 4,
                                whal_Stm32wb_UartDma_RxCallback, g_whalUart.cfg);
}
#endif

/* UART */
#ifdef BOARD_DMA
whal_Uart g_whalUart = {
    .base = WHAL_STM32WB55_UART1_BASE,
    /* .driver: direct API mapping */
    .cfg = &(whal_Stm32wb_UartDma_Cfg) {
        .base = {
            .brr = WHAL_STM32WB_UART_BRR(64000000, 115200),
            .timeout = &g_whalTimeout,
        },
        .dma = &g_whalDma1,
        .txCh = 3,
        .rxCh = 4,
        .txChCfg = &(whal_Stm32wb_Dma_ChCfg){WHAL_STM32WB55_UART1_TX_DMA_CFG},
        .rxChCfg = &(whal_Stm32wb_Dma_ChCfg){WHAL_STM32WB55_UART1_RX_DMA_CFG},
    },
};
#else
whal_Uart g_whalUart = {
    .base = WHAL_STM32WB55_UART1_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32wb_Uart_Cfg) {
        .timeout = &g_whalTimeout,

        .brr = WHAL_STM32WB_UART_BRR(64000000, 115200),
    },
};
#endif

/* Flash */
whal_Flash g_whalFlash = {
    .base = WHAL_STM32WB55_FLASH_BASE,
    .driver = WHAL_STM32WB55_FLASH_DRIVER,

    .cfg = &(whal_Stm32wb_Flash_Cfg) {
        .timeout = &g_whalTimeout,

        .startAddr = 0x08000000,
        .size = 0x80000, /* 512 KB (upper half reserved for BLE stack) */
    },
};

/* RNG */
whal_Rng g_whalRng = {
    .base = WHAL_STM32WB55_RNG_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32wb_Rng_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

/* Crypto */
whal_Crypto g_whalCrypto = {
    .base = WHAL_STM32WB55_AES1_BASE,
    /* .driver: direct API mapping */

    .cfg = &(whal_Stm32wb_Aes_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

#ifdef BOARD_WATCHDOG_IWDG
whal_Watchdog g_whalWatchdog = {
    .base = WHAL_STM32WB55_IWDG_BASE,
    .driver = WHAL_STM32WB55_IWDG_DRIVER,

    .cfg = &(whal_Stm32wb_Iwdg_Cfg) {
        .prescaler = WHAL_STM32WB_IWDG_PR_32,
        .reload = 100,
        .timeout = &g_whalTimeout,
    },
};
#elif defined(BOARD_WATCHDOG_WWDG)
whal_Watchdog g_whalWatchdog = {
    .base = WHAL_STM32WB55_WWDG_BASE,
    .driver = WHAL_STM32WB55_WWDG_DRIVER,

    .cfg = &(whal_Stm32wb_Wwdg_Cfg) {
        .prescaler = WHAL_STM32WB_WWDG_TB_128,
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

whal_Error Board_Init(void)
{
    whal_Error err;
    size_t i;

    /* Flash latency must be set before SYSCLK rises above ~16 MHz. */
    err = whal_Stm32wb_Flash_Ext_SetLatency(&g_whalFlash, WHAL_STM32WB_FLASH_LATENCY_3);
    if (err)
        return err;

    /* Bring up the clock tree: MSI -> PLL (sourced from MSI) -> HSI48 -> LSI -> SYSCLK->PLL */
    err = whal_Stm32wb_Rcc_EnableMsi(&g_whalClock, WHAL_STM32WB_RCC_MSIRANGE_4MHz);
    if (err)
        return err;

    /* MSI (4 MHz) -> VCO 128 MHz -> PLLR /2 = 64 MHz */
    err = whal_Stm32wb_Rcc_EnablePll(&g_whalClock, &(whal_Stm32wb_Rcc_PllCfg){
        .clkSrc = WHAL_STM32WB_RCC_PLLCLK_SRC_MSI,
        .n = 32, .m = 0, .r = 1, .q = 0, .p = 0,
    });
    if (err)
        return err;

    err = whal_Stm32wb_Rcc_EnableOsc(&g_whalClock,
        &(whal_Stm32wb_Rcc_OscCfg){WHAL_STM32WB_RCC_HSI48_CFG});
    if (err)
        return err;

#ifdef BOARD_WATCHDOG_IWDG
    err = whal_Stm32wb_Rcc_EnableOsc(&g_whalClock,
        &(whal_Stm32wb_Rcc_OscCfg){WHAL_STM32WB_RCC_LSI_CFG});
    if (err)
        return err;
#endif

    err = whal_Stm32wb_Rcc_SetSysClock(&g_whalClock, WHAL_STM32WB_RCC_SYSCLK_SRC_PLL);
    if (err)
        return err;

    for (i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32wb_Rcc_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    err = whal_Irq_Init(&g_whalIrq);
    if (err)
        return err;

#ifdef BOARD_DMA
    err = whal_Stm32wb_Rcc_EnablePeriphClk(&g_whalClock, &(whal_Stm32wb_Rcc_PeriphClk){WHAL_STM32WB55_DMA1_GATE});
    if (err)
        return err;

    err = whal_Stm32wb_Rcc_EnablePeriphClk(&g_whalClock, &(whal_Stm32wb_Rcc_PeriphClk){WHAL_STM32WB55_DMAMUX1_GATE});
    if (err)
        return err;

    err = whal_Dma_Init(&g_whalDma1);
    if (err)
        return err;

    /* Enable NVIC interrupts for DMA1 channel 4 (IRQ 14) and channel 5 (IRQ 15) */
    err = whal_Irq_Enable(&g_whalIrq, 14, NULL);
    if (err)
        return err;

    err = whal_Irq_Enable(&g_whalIrq, 15, NULL);
    if (err)
        return err;
#endif

    err = whal_Gpio_Init(&g_whalGpio);
    if (err) {
        return err;
    }

    err = whal_Uart_Init(&g_whalUart);
    if (err) {
        return err;
    }

    err = whal_Spi_Init(&g_whalSpi);
    if (err) {
        return err;
    }

    err = whal_I2c_Init(&g_whalI2c);
    if (err) {
        return err;
    }

    err = whal_Flash_Init(&g_whalFlash);
    if (err) {
        return err;
    }

    err = whal_Rng_Init(&g_whalRng);
    if (err) {
        return err;
    }

    err = whal_Crypto_Init(&g_whalCrypto);
    if (err) {
        return err;
    }

    err = whal_Timer_Init(&g_whalTimer);
    if (err) {
        return err;
    }

    err = whal_Timer_Start(&g_whalTimer);
    if (err) {
        return err;
    }

    err = Peripheral_Init();
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error Board_Deinit(void)
{
    whal_Error err;

    err = Peripheral_Deinit();
    if (err) {
        return err;
    }

    err = whal_Timer_Stop(&g_whalTimer);
    if (err) {
        return err;
    }

    err = whal_Timer_Deinit(&g_whalTimer);
    if (err) {
        return err;
    }

    err = whal_Crypto_Deinit(&g_whalCrypto);
    if (err) {
        return err;
    }

    err = whal_Rng_Deinit(&g_whalRng);
    if (err) {
        return err;
    }

    err = whal_Flash_Deinit(&g_whalFlash);
    if (err) {
        return err;
    }

    err = whal_I2c_Deinit(&g_whalI2c);
    if (err) {
        return err;
    }

    err = whal_Spi_Deinit(&g_whalSpi);
    if (err) {
        return err;
    }

    err = whal_Uart_Deinit(&g_whalUart);
    if (err) {
        return err;
    }

    err = whal_Gpio_Deinit(&g_whalGpio);
    if (err) {
        return err;
    }

#ifdef BOARD_DMA
    whal_Irq_Disable(&g_whalIrq, 14);
    whal_Irq_Disable(&g_whalIrq, 15);

    err = whal_Dma_Deinit(&g_whalDma1);
    if (err)
        return err;
    err = whal_Stm32wb_Rcc_DisablePeriphClk(&g_whalClock, &(whal_Stm32wb_Rcc_PeriphClk){WHAL_STM32WB55_DMAMUX1_GATE});
    if (err)
        return err;
    err = whal_Stm32wb_Rcc_DisablePeriphClk(&g_whalClock, &(whal_Stm32wb_Rcc_PeriphClk){WHAL_STM32WB55_DMA1_GATE});
    if (err)
        return err;
#endif

    err = whal_Irq_Deinit(&g_whalIrq);
    if (err)
        return err;

    /* Tear down the clock tree in reverse: peripheral gates off, SYSCLK -> MSI, sources off. */
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
#ifdef BOARD_WATCHDOG_IWDG
    err = whal_Stm32wb_Rcc_DisableOsc(&g_whalClock,
        &(whal_Stm32wb_Rcc_OscCfg){WHAL_STM32WB_RCC_LSI_CFG});
    if (err)
        return err;
#endif
    err = whal_Stm32wb_Rcc_DisableOsc(&g_whalClock,
        &(whal_Stm32wb_Rcc_OscCfg){WHAL_STM32WB_RCC_HSI48_CFG});
    if (err)
        return err;
    /* MSI stays on as the post-Deinit fallback. */

    err = whal_Stm32wb_Flash_Ext_SetLatency(&g_whalFlash, WHAL_STM32WB_FLASH_LATENCY_0);
    if (err)
        return err;

    return WHAL_SUCCESS;
}
