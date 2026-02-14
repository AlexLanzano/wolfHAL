#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/microchip/pic32cz.h>

whal_Supply g_whalSupply = {
    WHAL_PIC32CZ_SUPPLY_DEVICE,
};

whal_Clock g_whalClock = {
    WHAL_PIC32CZ_CLOCK_PLL_DEVICE,

    .cfg = &(whal_Pic32czClock_Cfg) {
        /* 300MHz clock */
        .oscCtrlCfg = &(whal_Pic32czClockPll_OscCtrlCfg) {
            .supplyCtrl = &g_whalSupply,
            .supply = &(whal_Pic32czSupc_Supply){WHAL_PIC32CZ_SUPPLY_PLL},

            .pllInst = WHAL_PIC32CZ_PLL0,
            .refSel = WHAL_PIC32CZ_REFSEL_DFLL48M,
            .bwSel = WHAL_PIC32CZ_BWSEL_10MHz_TO_20MHz,

            .fbDiv = 225,
            .refDiv = 12,

            .outCfgCount = 1,
            .outCfg = &(whal_Pic32czClockPll_OutCfg) {
                .postDivMask = WHAL_PIC32CZ_POSTDIVMASK0,
                .outEnMask = WHAL_PIC32CZ_OUTENMASK0,
                .postDiv = 3,
            },
        },
        .mclkCfg = &(whal_Pic32czClock_MclkCfg) {
            .div = 2,
        },
        .gclkCfgCount = 1,
        .gclkCfg = &(whal_Pic32czClock_GclkCfg) {
            .gen = 0,
            .genSrc = WHAL_PIC32CZ_GENSRC_PLL0_CLOCKOUT0,
            .genDiv = 1,
        },
    },
};

whal_Gpio g_whalGpio = {
    WHAL_PIC32CZ_GPIO_DEVICE,
    
    .cfg = &(whal_Pic32czGpio_Cfg) {
        .pinCfgCount = 3,
        .pinCfg = (whal_Pic32czGpio_PinCfg[]) {
            { /* LED */
                .port = 1,
                .pin = 21,
                .dir = WHAL_PIC32CZ_DIR_OUTPUT,
                .out = 0,
            },
            { /* UART TX */
                .port = 2,
                .pin = 21,
                .pmuxEn = 1,
                .pmux = WHAL_PIC32CZ_PMUX_SERCOM_ALT,
            },
            { /* UART RX */
                .port = 2,
                .pin = 22,
                .pmuxEn = 1,
                .pmux = WHAL_PIC32CZ_PMUX_SERCOM_ALT,
            },
        },
    },
};

static whal_Pic32czClock_Clk uartClk = {
    .gclkPeriphChannel = 25, /* SERCOM 4 */
    .gclkPeriphSrc = 0, /* GEN 0 */
    .mclkEnableInst = 1, /* Peripheral BUS Clock Enable Mask1 Register */
    .mclkEnableMask = WHAL_MASK(3), /* SERCOM 4 enable mask */
};

whal_Uart g_whalUart = {
    WHAL_PIC32CZ_SERCOM4_UART_DEVICE,

    .cfg = &(whal_Pic32czUart_Cfg) {
        .clkCtrl = &g_whalClock,
        .clk = &uartClk,
        .baud = WHAL_PIC32CZ_UART_BAUD(115200, 300000000),
        .txPad = WHAL_PIC32CZ_UART_TXPO_PAD0,
        .rxPad = WHAL_PIC32CZ_UART_RXPO_PAD1,
    },
};

whal_Timer g_whalTimer = {
    WHAL_CORTEX_M7_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 300000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

whal_Flash g_whalFlash = {
    WHAL_PIC32CZ_FLASH_DEVICE,
};
