/* Example board configuration for the PIC32CZ CA Curiosity Ultra dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/microchip/pic32cz.h>
#include "peripheral.h"

/* Power */
static whal_Power g_whalPower = {
    .regmap = { WHAL_PIC32CZ_SUPC_REGMAP },
};

/* Clock */
whal_Clock g_whalClock = {
    .regmap = { WHAL_PIC32CZ_CLOCK_REGMAP },
};

/* Peripheral clocks */
static const whal_Pic32cz_Clock_PeriphClk g_periphClks[] = {
    { /* SERCOM 4 (UART) */
        .gclkPeriphChannel = 25,
        .gclkPeriphSrc = 0, /* GEN 0 */
        .mclkEnableInst = 1,
        .mclkEnableMask = (1UL << 3),
        .mclkEnablePos = 3,
    },
};
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))

/* GPIO */
whal_Gpio g_whalGpio = {
    .regmap = { WHAL_PIC32CZ_GPIO_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Pic32cz_Gpio_Cfg) {
        .pinCfgCount = 3,
        .pinCfg = (whal_Pic32cz_Gpio_PinCfg[]) {
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

/* UART */
whal_Uart g_whalUart = {
    .regmap = { WHAL_PIC32CZ_SERCOM4_UART_REGMAP },
    /* .driver: direct API mapping */

    .cfg = &(whal_Pic32cz_Uart_Cfg) {
        .baud = WHAL_PIC32CZ_UART_BAUD(115200, 300000000),
        .txPad = WHAL_PIC32CZ_UART_TXPO_PAD0,
        .rxPad = WHAL_PIC32CZ_UART_RXPO_PAD1,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    .regmap = { WHAL_CORTEX_M7_SYSTICK_REGMAP },
    .driver = WHAL_CORTEX_M7_SYSTICK_DRIVER,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 300000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    .regmap = { WHAL_PIC32CZ_FLASH_REGMAP },
    .driver = WHAL_PIC32CZ_FLASH_DRIVER,
};

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

    /* Enable PLL power supply before clock init */
    err = whal_Pic32cz_Supc_EnableSupply(&g_whalPower,
            &(whal_Pic32cz_Supc_Supply){WHAL_PIC32CZ_SUPC_PLL});
    if (err) {
        return err;
    }

    /* PLL0: DFLL48 / 12 * 225 / 3 = 300 MHz, then GCLK0 from PLL0 OUT0,
     * then MCLK CPU divider /2 = 150 MHz CPU. */
    err = whal_Pic32cz_Clock_EnablePll(&g_whalClock, &(whal_Pic32cz_Clock_PllCfg){
        .pllInst = WHAL_PIC32CZ_PLL0,
        .refSel = WHAL_PIC32CZ_REFSEL_DFLL48M,
        .bwSel = WHAL_PIC32CZ_BWSEL_10MHz_TO_20MHz,
        .fbDiv = 225,
        .refDiv = 12,
        .outCfgCount = 1,
        .outCfg = &(whal_Pic32cz_Clock_PllOutCfg){
            .postDivMask = WHAL_PIC32CZ_POSTDIV0_Msk,
            .postDivPos  = WHAL_PIC32CZ_POSTDIV0_Pos,
            .outEnMask   = WHAL_PIC32CZ_OUTEN0_Msk,
            .outEnPos    = WHAL_PIC32CZ_OUTEN0_Pos,
            .postDiv = 3,
        },
    });
    if (err)
        return err;
    err = whal_Pic32cz_Clock_SetMclkDiv(&g_whalClock, 2);
    if (err)
        return err;
    err = whal_Pic32cz_Clock_EnableGclkGen(&g_whalClock, &(whal_Pic32cz_Clock_GenCfg){
        .gen = 0,
        .genSrc = WHAL_PIC32CZ_GENSRC_PLL0_CLOCKOUT0,
        .genDiv = 1,
    });
    if (err)
        return err;

    /* Enable peripheral clocks */
    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Pic32cz_Clock_EnablePeriphClk(&g_whalClock, &g_periphClks[i]);
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
    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Pic32cz_Clock_DisablePeriphClk(&g_whalClock, &g_periphClks[i]);
        if (err)
            return err;
    }

    /* SUPC outputs are left as-is; no Deinit operation. */

    return WHAL_SUCCESS;
}
