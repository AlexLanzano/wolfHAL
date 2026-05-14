#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/microchip/pic32cz.h>
#include <wolfHAL/power/pic32cz_supc.h>

extern whal_Clock g_whalClock;
extern whal_Uart g_whalUart;
extern whal_Flash g_whalFlash;
extern whal_Timeout g_whalTimeout;

extern volatile uint32_t g_tick;

#define BOARD_LED_PIN         0
#define BOARD_FLASH_TEST_ADDR 0x0C000000
#define BOARD_FLASH_SECTOR_SZ 0x1000

/* BOARD_*_DEV: how this board reaches each peripheral. */
#define BOARD_GPIO_DEV     WHAL_SINGLETON
#define BOARD_UART_DEV     (&g_whalUart)
#define BOARD_FLASH_DEV    (&g_whalFlash)
#define BOARD_CLOCK_DEV    (&g_whalClock)

/* SUPC singleton — referenced by pic32cz_supc.c directly. */
static const whal_Power whal_Pic32cz_Supc_Dev = {
    .base = WHAL_PIC32CZ_SUPC_BASE,
};

/* Flash singleton — referenced by pic32cz_flash.c directly. Const cfg lives
 * here; the dispatcher stub g_whalFlash in board.c carries only .driver so
 * whal_Flash_* can be vtable-dispatched alongside other flash drivers (e.g.
 * SPI NOR W25Q64). */
static const whal_Flash whal_Pic32cz_Flash_Dev = {
    .base = WHAL_PIC32CZ_FLASH_BASE,

    .cfg = (void *)&(const whal_Pic32cz_Flash_Cfg){
        .startAddr = 0x0C000000,
        .size = 0x00800000, /* 8 MB max */
        .timeout = &g_whalTimeout,
    },
};

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
