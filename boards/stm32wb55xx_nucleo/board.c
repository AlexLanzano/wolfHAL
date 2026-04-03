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
    WHAL_CORTEX_M4_NVIC_DEVICE,
};

/* Clock */
whal_Clock g_whalClock = {
    WHAL_STM32WB55_RCC_PLL_DEVICE,

    .cfg = &(whal_Stm32wbRcc_Cfg) {
        .sysClkSrc = WHAL_STM32WB_RCC_SYSCLK_SRC_PLL,
        .sysClkCfg = &(whal_Stm32wbRcc_PllClkCfg)
        {
            .clkSrc = WHAL_STM32WB_RCC_PLLCLK_SRC_MSI,
            /* 64 MHz */
            .n = 32,
            .m = 0,
            .r = 1,
            .q = 0,
            .p = 0,
        },
    },
};

static const whal_Stm32wbRcc_Clk g_flashClock = {WHAL_STM32WB55_FLASH_CLOCK};

static const whal_Stm32wbRcc_Clk g_clocks[] = {
    {WHAL_STM32WB55_GPIOA_CLOCK},
    {WHAL_STM32WB55_GPIOB_CLOCK},
    {WHAL_STM32WB55_UART1_CLOCK},
    {WHAL_STM32WB55_SPI1_CLOCK},
    {WHAL_STM32WB55_RNG_CLOCK},
    {WHAL_STM32WB55_AES1_CLOCK},
    {WHAL_STM32WB55_I2C1_CLOCK},
};
#define CLOCK_COUNT (sizeof(g_clocks) / sizeof(g_clocks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    WHAL_STM32WB55_GPIO_DEVICE,

    .cfg = &(whal_Stm32wbGpio_Cfg) {
        .pinCfg = (whal_Stm32wbGpio_PinCfg[PIN_COUNT]) {
            [LED_PIN] = { /* LED */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 5,
                .mode = WHAL_STM32WB_GPIO_MODE_OUT,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_LOW,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 0,
            },
            [UART_TX_PIN] = { /* UART1 TX */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 6,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 7,
            },
            [UART_RX_PIN] = { /* UART1 RX */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 7,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 7,
            },
            [SPI_SCK_PIN] = { /* SPI1 SCK */
                .port = WHAL_STM32WB_GPIO_PORT_A,
                .pin = 5,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MISO_PIN] = { /* SPI1 MISO */
                .port = WHAL_STM32WB_GPIO_PORT_A,
                .pin = 6,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_MOSI_PIN] = { /* SPI1 MOSI */
                .port = WHAL_STM32WB_GPIO_PORT_A,
                .pin = 7,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_NONE,
                .altFn = 5,
            },
            [SPI_CS_PIN] = { /* SPI CS */
                .port = WHAL_STM32WB_GPIO_PORT_A,
                .pin = 4,
                .mode = WHAL_STM32WB_GPIO_MODE_OUT,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
            },
            [I2C_SCL_PIN] = { /* I2C1 SCL */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 8,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_OPENDRAIN,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 4,
            },
            [I2C_SDA_PIN] = { /* I2C1 SDA */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 9,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_OPENDRAIN,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 4,
            },
        },
        .pinCount = PIN_COUNT,
    },
};

/* I2C */
whal_I2c g_whalI2c = {
    WHAL_STM32WB55_I2C1_DEVICE,

    .cfg = &(whal_Stm32wbI2c_Cfg) {
        .pclk = 64000000,
        .timeout = &g_whalTimeout,
    },
};

/* SPI */
whal_Spi g_whalSpi = {
    WHAL_STM32WB55_SPI1_DEVICE,

    .cfg = &(whal_Stm32wbSpi_Cfg) {
        .pclk = 64000000,
        .timeout = &g_whalTimeout,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    WHAL_CORTEX_M4_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 64000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* DMA */
#ifdef BOARD_DMA

whal_Dma g_whalDma1 = {
    WHAL_STM32WB55_DMA1_DEVICE,
    .cfg = &(whal_Stm32wbDma_Cfg){WHAL_STM32WB55_DMA1_CFG},
};

static const whal_Stm32wbRcc_Clk g_dmaClock = {WHAL_STM32WB55_DMA1_CLOCK};
static const whal_Stm32wbRcc_Clk g_dmamuxClock = {WHAL_STM32WB55_DMAMUX1_CLOCK};

void DMA1_Channel4_IRQHandler(void)
{
    whal_Stm32wbDma_IRQHandler(&g_whalDma1, 3,
                                whal_Stm32wbUartDma_TxCallback, g_whalUart.cfg);
}

void DMA1_Channel5_IRQHandler(void)
{
    whal_Stm32wbDma_IRQHandler(&g_whalDma1, 4,
                                whal_Stm32wbUartDma_RxCallback, g_whalUart.cfg);
}
#endif

/* UART */
#ifdef BOARD_DMA
whal_Uart g_whalUart = {
    WHAL_STM32WB55_UART1_DEVICE,
    .driver = &whal_Stm32wbUartDma_Driver,
    .cfg = &(whal_Stm32wbUartDma_Cfg) {
        .base = {
            .brr = WHAL_STM32WB_UART_BRR(64000000, 115200),
            .timeout = &g_whalTimeout,
        },
        .dma = &g_whalDma1,
        .txCh = 3,
        .rxCh = 4,
        .txChCfg = &(whal_Stm32wbDma_ChCfg){WHAL_STM32WB55_UART1_TX_DMA_CFG},
        .rxChCfg = &(whal_Stm32wbDma_ChCfg){WHAL_STM32WB55_UART1_RX_DMA_CFG},
    },
};
#else
whal_Uart g_whalUart = {
    WHAL_STM32WB55_UART1_DEVICE,

    .cfg = &(whal_Stm32wbUart_Cfg) {
        .timeout = &g_whalTimeout,

        .brr = WHAL_STM32WB_UART_BRR(64000000, 115200),
    },
};
#endif

/* Flash */
whal_Flash g_whalFlash = {
    WHAL_STM32WB55_FLASH_DEVICE,

    .cfg = &(whal_Stm32wbFlash_Cfg) {
        .timeout = &g_whalTimeout,

        .startAddr = 0x08000000,
        .size = 0x80000, /* 512 KB (upper half reserved for BLE stack) */
    },
};

/* RNG */
whal_Rng g_whalRng = {
    WHAL_STM32WB55_RNG_DEVICE,

    .cfg = &(whal_Stm32wbRng_Cfg) {
        .timeout = &g_whalTimeout,
    },
};

/* Crypto */
static const whal_Crypto_OpFunc cryptoOps[BOARD_CRYPTO_OP_COUNT] = {
    [BOARD_CRYPTO_AES_ECB]  = whal_Stm32wbAes_AesEcb,
    [BOARD_CRYPTO_AES_CBC]  = whal_Stm32wbAes_AesCbc,
    [BOARD_CRYPTO_AES_CTR]  = whal_Stm32wbAes_AesCtr,
    [BOARD_CRYPTO_AES_GCM]  = whal_Stm32wbAes_AesGcm,
    [BOARD_CRYPTO_AES_GMAC] = whal_Stm32wbAes_AesGmac,
    [BOARD_CRYPTO_AES_CCM]  = whal_Stm32wbAes_AesCcm,
};

whal_Crypto g_whalCrypto = {
    WHAL_STM32WB55_AES1_DEVICE,

    .ops = cryptoOps,
    .opsCount = BOARD_CRYPTO_OP_COUNT,

    .cfg = &(whal_Stm32wbAes_Cfg) {
        .timeout = &g_whalTimeout,
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

    /* Enable flash clock and set latency before increasing clock speed */
    err = whal_Clock_Enable(&g_whalClock, &g_flashClock);
    if (err) {
        return err;
    }

    err = whal_Stm32wbFlash_Ext_SetLatency(&g_whalFlash, WHAL_STM32WB_FLASH_LATENCY_3);
    if (err) {
        return err;
    }

    err = whal_Clock_Init(&g_whalClock);
    if (err) {
        return err;
    }

    /* Enable HSI48 osc required by the RNG */
    err = whal_Stm32wbRcc_Ext_EnableHsi48(&g_whalClock, 1);
    if (err) {
        return err;
    }

    /* Enable clocks */
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
    err = whal_Clock_Enable(&g_whalClock, &g_dmamuxClock);
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
    err = whal_Clock_Disable(&g_whalClock, &g_dmamuxClock);
    if (err)
        return err;
    err = whal_Clock_Disable(&g_whalClock, &g_dmaClock);
    if (err)
        return err;
#endif

    err = whal_Irq_Deinit(&g_whalIrq);
    if (err)
        return err;

    /* Disable clocks */
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_clocks[i]);
        if (err)
            return err;
    }

    err = whal_Stm32wbRcc_Ext_EnableHsi48(&g_whalClock, 0);
    if (err) {
        return err;
    }

    err = whal_Clock_Deinit(&g_whalClock);
    if (err) {
        return err;
    }

    /* Reduce flash latency then disable flash clock */
    err = whal_Stm32wbFlash_Ext_SetLatency(&g_whalFlash, WHAL_STM32WB_FLASH_LATENCY_0);
    if (err) {
        return err;
    }

    err = whal_Clock_Disable(&g_whalClock, &g_flashClock);
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}
