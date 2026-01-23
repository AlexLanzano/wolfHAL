#include <wolfHAL/platform/st/stm32wb55xx.h>
#include "stm32wb55xx_nucleo.h"

whal_StRcc_PeriphClk periphClkEn[] =
{
    WHAL_ST_RCC_PERIPH_GPIOA,
    WHAL_ST_RCC_PERIPH_GPIOB,
    WHAL_ST_RCC_PERIPH_LPUART1,
};

whal_Clock rcc = {
    WHAL_STM32WB55_RCC_DEVICE,

    .cfg = &(whal_StRcc_Cfg){
        .sysClkSrc = WHAL_ST_RCC_SYSCLK_SRC_PLL,
        .sysClkCfg.pll =
        {
            .clkSrc = WHAL_ST_RCC_PLLCLK_SRC_MSI,
            /* 64 MHz */
            .n = 32,
            .m = 0,
            .r = 1,
            .q = 0,
            .p = 0,
        },
        .periphClkEn = periphClkEn, 
        .periphClkEnCount = sizeof(periphClkEn) / sizeof(whal_StRcc_PeriphClk),
    },
};

whal_Gpio gpio = {
    WHAL_STM32WB55_GPIO_DEVICE,

    .pinCfg = &(whal_StGpio_Cfg[3]){
        [LED_PIN] = { /* LED */
            .port = WHAL_STGPIO_PORT_B,
            .pin = 5,
            .mode = WHAL_STGPIO_MODE_OUT,
            .outType = WHAL_STGPIO_OUTTYPE_PUSHPULL,
            .speed = WHAL_STGPIO_SPEED_LOW,
            .pull = WHAL_STGPIO_PULL_UP,
            .altFn = 0,
        },
        [LPUART1_TX_PIN] = { /* LPUART1 TX */
            .port = WHAL_STGPIO_PORT_A,
            .pin = 2,
            .mode = WHAL_STGPIO_MODE_ALTFN,
            .outType = WHAL_STGPIO_OUTTYPE_PUSHPULL,
            .speed = WHAL_STGPIO_SPEED_FAST,
            .pull = WHAL_STGPIO_PULL_UP,
            .altFn = 8,
        },
        [LPUART1_RX_PIN] = { /* LPUART1 RX */
            .port = WHAL_STGPIO_PORT_A,
            .pin = 3,
            .mode = WHAL_STGPIO_MODE_ALTFN,
            .outType = WHAL_STGPIO_OUTTYPE_PUSHPULL,
            .speed = WHAL_STGPIO_SPEED_FAST,
            .pull = WHAL_STGPIO_PULL_UP,
            .altFn = 8,
        },
    },
    .pinCount = 3,
};


whal_Timer sysTickTimer = {
    WHAL_CORTEX_M4_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg){
        .cyclesPerTick = 64000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

whal_StUart_Cfg lpuart1Cfg = {
};

whal_Uart lpuart1 = {
    WHAL_STM32WB55_LPUART1_DEVICE,

    .cfg = &(whal_StUart_Cfg){
        .baud = 115200,
        .sysClk = &rcc,
    },
};

whal_Flash flash = {
    WHAL_STM32WB55_FLASH_DEVICE,
};
