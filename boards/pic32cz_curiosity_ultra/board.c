/* Example board configuration for the PIC32CZ CA Curiosity Ultra dev board */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/microchip/pic32cz.h>

/* Supply */
static whal_Supply g_whalSupply = {
    WHAL_PIC32CZ_SUPPLY_DEVICE,
};

/* Clock */
whal_Clock g_whalClock = {
    WHAL_PIC32CZ_CLOCK_PLL_DEVICE,

    .cfg = &(whal_Pic32czClock_Cfg) {
        /* 300MHz clock */
        .oscCtrlCfg = &(whal_Pic32czClockPll_OscCtrlCfg) {
            .pllInst = WHAL_PIC32CZ_PLL0,
            .refSel = WHAL_PIC32CZ_REFSEL_DFLL48M,
            .bwSel = WHAL_PIC32CZ_BWSEL_10MHz_TO_20MHz,

            .fbDiv = 225,
            .refDiv = 12,

            .outCfgCount = 1,
            .outCfg = &(whal_Pic32czClockPll_OutCfg) {
                .postDivMask = WHAL_PIC32CZ_POSTDIV0_Msk,
                .postDivPos = WHAL_PIC32CZ_POSTDIV0_Pos,
                .outEnMask = WHAL_PIC32CZ_OUTEN0_Msk,
                .outEnPos = WHAL_PIC32CZ_OUTEN0_Pos,
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

/* Peripheral clocks */
static const whal_Pic32czClock_Clk g_peripheralClocks[] = {
    { /* SERCOM 4 (UART) */
        .gclkPeriphChannel = 25,
        .gclkPeriphSrc = 0, /* GEN 0 */
        .mclkEnableInst = 1,
        .mclkEnableMask = (1UL << 3),
        .mclkEnablePos = 3,
    },
};
#define CLOCK_COUNT (sizeof(g_peripheralClocks) / sizeof(g_peripheralClocks[0]))

/* GPIO */
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

/* UART */
whal_Uart g_whalUart = {
    WHAL_PIC32CZ_SERCOM4_UART_DEVICE,

    .cfg = &(whal_Pic32czUart_Cfg) {
        .baud = WHAL_PIC32CZ_UART_BAUD(115200, 300000000),
        .txPad = WHAL_PIC32CZ_UART_TXPO_PAD0,
        .rxPad = WHAL_PIC32CZ_UART_RXPO_PAD1,
    },
};

/* Timer */
whal_Timer g_whalTimer = {
    WHAL_CORTEX_M7_SYSTICK_DEVICE,

    .cfg = &(whal_SysTick_Cfg) {
        .cyclesPerTick = 300000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

/* Flash */
whal_Flash g_whalFlash = {
    WHAL_PIC32CZ_FLASH_DEVICE,
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

    err = whal_Supply_Init(&g_whalSupply);
    if (err) {
        return err;
    }

    /* Enable PLL power supply before clock init */
    err = whal_Supply_Enable(&g_whalSupply,
                             &(whal_Pic32czSupc_Supply){WHAL_PIC32CZ_SUPPLY_PLL});
    if (err) {
        return err;
    }

    err = whal_Clock_Init(&g_whalClock);
    if (err) {
        return err;
    }

    /* Enable peripheral clocks */
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
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
    for (size_t i = 0; i < CLOCK_COUNT; i++) {
        err = whal_Clock_Disable(&g_whalClock, &g_peripheralClocks[i]);
        if (err)
            return err;
    }

    err = whal_Clock_Deinit(&g_whalClock);
    if (err) {
        return err;
    }

    err = whal_Supply_Deinit(&g_whalSupply);
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}
