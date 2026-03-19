/* Example board configuration for the STM32WB55 Nucleo dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32wb55xx.h>

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
    .timeoutTicks = 5, /* 5ms timeout */
    .GetTick = Board_GetTick,
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

static const whal_Stm32wbRcc_Clk g_peripheralClocks[] = {
    {WHAL_STM32WB55_GPIOB_CLOCK},
    {WHAL_STM32WB55_UART1_CLOCK},
    {WHAL_STM32WB55_RNG_CLOCK},
    {WHAL_STM32WB55_AES1_CLOCK},
};
#define PERIPHERAL_CLOCK_COUNT (sizeof(g_peripheralClocks) / sizeof(g_peripheralClocks[0]))

/* GPIO */
enum {
    LED_PIN,
    UART_TX_PIN,
    UART_RX_PIN,
};

whal_Gpio g_whalGpio = {
    WHAL_STM32WB55_GPIO_DEVICE,

    .cfg = &(whal_Stm32wbGpio_Cfg) {
        .pinCfg = (whal_Stm32wbGpio_PinCfg[3]) {
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
        },
        .pinCount = 3,
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

/* UART */
whal_Uart g_whalUart = {
    WHAL_STM32WB55_UART1_DEVICE,

    .cfg = &(whal_Stm32wbUart_Cfg) {
        .timeout = &g_whalTimeout,

        .brr = WHAL_STM32WB_UART_BRR(64000000, 115200),
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    WHAL_STM32WB55_FLASH_DEVICE,

    .cfg = &(whal_Stm32wbFlash_Cfg) {
        .timeout = &g_whalTimeout,

        .startAddr = 0x08000000,
        .size = 0x100000,
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
    g_waiting = 1;
    while (1) {
        uint32_t currentCount = g_tick;
        if (g_tickOverflow) {
            if ((UINT32_MAX - startCount) + currentCount > ms) {
                break;
            }
        } else if (currentCount - startCount > ms) {
            break;
        }
    }

    g_waiting = 0;
    g_tickOverflow = 0;
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

    /* Enable peripheral clocks */
    for (size_t i = 0; i < PERIPHERAL_CLOCK_COUNT; i++) {
        err = whal_Clock_Enable(&g_whalClock, &g_peripheralClocks[i]);
        if (err)
            return err;
    }

    err = whal_Gpio_Init(&g_whalGpio);
    if (err) {
        return err;
    }

    err = whal_Uart_Init(&g_whalUart);
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

    return WHAL_SUCCESS;
}

whal_Error Board_Deinit(void)
{
    whal_Error err;

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

    err = whal_Uart_Deinit(&g_whalUart);
    if (err) {
        return err;
    }

    err = whal_Gpio_Deinit(&g_whalGpio);
    if (err) {
        return err;
    }

    /* Disable peripheral clocks */
    for (size_t i = 0; i < PERIPHERAL_CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_peripheralClocks[i]);
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
