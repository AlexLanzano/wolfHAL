#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/microchip/pic32cz.h>

whal_Supply supply = {
    WHAL_PIC32CZ_SUPPLY_DEVICE,
};

whal_Clock clock = {
    WHAL_PIC32CZ_CLOCK_PLL_DEVICE,

    .cfg = &(whal_Pic32czClock_Cfg) {
        /* 300MHz clock */
        .oscCtrlCfg = &(whal_Pic32czClockPll_OscCtrlCfg) {
            .supplyCtrl = &supply,
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

whal_Gpio gpio = {
    WHAL_PIC32CZ_GPIO_DEVICE,
    
    .cfg = &(whal_Pic32czGpio_Cfg) {
        .pinCfgCount = 1,
        .pinCfg = &(whal_Pic32czGpio_PinCfg) {
            .port = 1,
            .pin = 21,
            .dir = WHAL_PIC32CZ_DIR_OUTPUT,
            .out = 0,
        },
    },
};

whal_Pic32czClock_Clk periphClock = {
    .gclkPeriphChannel = 0,
    .gclkPeriphSrc = 0,
    .mclkEnableInst = 0,
    .mclkEnableMask = 0,
};

whal_Timer g_whalTimer = {
    WHAL_CORTEX_M7_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 300000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};
