#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/microchip/pic32cz.h>

extern whal_Clock g_whalClock;
extern whal_Uart g_whalUart;
extern whal_Flash g_whalFlash;

extern volatile uint32_t g_tick;

#define BOARD_LED_PIN         0
#define BOARD_FLASH_TEST_ADDR 0x0C000000
#define BOARD_FLASH_SECTOR_SZ 0x1000

static const whal_Gpio whal_Pic32cz_Gpio_Dev = {
    .base = WHAL_PIC32CZ_GPIO_BASE,

    .cfg = (void *)&(const whal_Pic32cz_Gpio_Cfg){
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

/* SysTick singleton — referenced by systick.c directly. */
static const whal_Timer whal_SysTick_Dev = {
    .base = WHAL_CORTEX_M7_SYSTICK_BASE,
    /* .driver: direct API mapping */

    .cfg = (void *)&(const whal_SysTick_Cfg){
        .cyclesPerTick = 300000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
