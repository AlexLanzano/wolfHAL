#include <wolfHAL/platform/st/stm32wb55xx.h>
#include "stm32wb55xx_nucleo.h"

whal_Clock g_whalClock;
whal_Flash g_whalFlash;

whal_Clock g_whalClock = {
    WHAL_STM32WB55_RCC_PLL_DEVICE,

    .cfg = &(whal_Stm32wbRcc_Cfg) {
        .flash = &g_whalFlash,
        .flashLatency = WHAL_STM32WB_FLASH_LATENCY_3,

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

whal_Gpio g_whalGpio = {
    WHAL_STM32WB55_GPIO_DEVICE,

    .cfg = &(whal_Stm32wbGpio_Cfg) {
        .clkCtrl = &g_whalClock,
        .clk = (const void *[2]) {
            &(whal_Stm32wbRcc_Clk){WHAL_STM32WB55_GPIOA_CLOCK},
            &(whal_Stm32wbRcc_Clk){WHAL_STM32WB55_GPIOB_CLOCK},
        },
        .clkCount = 2,

        .pinCfg = (whal_Stm32wbGpio_PinCfg[7]) {
            [LED_PIN] = { /* LED */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 5,
                .mode = WHAL_STM32WB_GPIO_MODE_OUT,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_LOW,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 0,
            },
            [LPUART1_TX_PIN] = { /* UART1 TX */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 6,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 7,
            },
            [LPUART1_RX_PIN] = { /* UART1 RX */
                .port = WHAL_STM32WB_GPIO_PORT_B,
                .pin = 7,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 7,
            },
            [SPI1_SCK_PIN] = { /* SPI1 SCK */
                .port = WHAL_STM32WB_GPIO_PORT_A,
                .pin = 5,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 5,
            },
            [SPI1_MISO_PIN] = { /* SPI1 MISO */
                .port = WHAL_STM32WB_GPIO_PORT_A,
                .pin = 6,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 5,
            },
            [SPI1_MOSI_PIN] = { /* SPI1 MOSI */
                .port = WHAL_STM32WB_GPIO_PORT_A,
                .pin = 7,
                .mode = WHAL_STM32WB_GPIO_MODE_ALTFN,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_FAST,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 5,
            },
            [CS_PIN] = { /* SPI1 CS */
                .port = WHAL_STM32WB_GPIO_PORT_A,
                .pin = 4,
                .mode = WHAL_STM32WB_GPIO_MODE_OUT,
                .outType = WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL,
                .speed = WHAL_STM32WB_GPIO_SPEED_LOW,
                .pull = WHAL_STM32WB_GPIO_PULL_UP,
                .altFn = 0,
            },
        },
        .pinCount = 7,
    },
};

whal_Timer g_whalTimer = {
    WHAL_CORTEX_M4_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 64000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

whal_Uart g_whalUart = {
    WHAL_STM32WB55_UART1_DEVICE,

    .cfg = &(whal_Stm32wbUart_Cfg) {
        .clkCtrl = &g_whalClock,
        .clk = &(whal_Stm32wbRcc_Clk) {WHAL_STM32WB55_UART1_CLOCK},

        .baud = 115200,
    },
};

whal_Flash g_whalFlash = {
    WHAL_STM32WB55_FLASH_DEVICE,

    .cfg = &(whal_Stm32wbFlash_Cfg) {
        .clkCtrl = &g_whalClock,
        .clk = &(whal_Stm32wbRcc_Clk) {WHAL_STM32WB55_FLASH_CLOCK},

        .startAddr = 0x08000000,
        .size = 0x100000,
    },
};

whal_Spi g_whalSpi = {
    WHAL_STM32WB55_SPI1_DEVICE,

    .cfg = &(whal_Stm32wbSpi_Cfg) {
        .clkCtrl = &g_whalClock,
        .clk = &(whal_Stm32wbRcc_Clk) {WHAL_STM32WB55_SPI1_CLOCK},
    },
};
