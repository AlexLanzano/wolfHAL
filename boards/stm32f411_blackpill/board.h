#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32f411xx.h>

extern whal_Clock g_whalClock;
extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
extern whal_Flash g_whalFlash;

extern whal_Timeout g_whalTimeout;
extern volatile uint32_t g_tick;

enum {
    LED_PIN,
    UART_TX_PIN,
    UART_RX_PIN,
    SPI_SCK_PIN,
    SPI_MISO_PIN,
    SPI_MOSI_PIN,
    PIN_COUNT,
};

#define BOARD_LED_PIN 0

/* Flash test address: last sector (sector 7, 128KB at 0x08060000) */
#define BOARD_FLASH_TEST_ADDR 0x08060000
#define BOARD_FLASH_SECTOR_SZ 0x20000
#define BOARD_FLASH_WRITE_SZ  4

static const whal_Gpio whal_Stm32f4_Gpio_Dev = {
    .base = WHAL_STM32F411_GPIO_BASE,

    .cfg = (void *)&(const whal_Stm32f4_Gpio_Cfg){
        .pinCfg = (const whal_Stm32f4_Gpio_PinCfg[PIN_COUNT]){
            /* LED on PC13 (active low) */
            [LED_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_C, 13, WHAL_STM32F4_GPIO_MODE_OUT,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_LOW,
                WHAL_STM32F4_GPIO_PULL_NONE, 0),
            /* USART2 TX on PA2 (AF7) */
            [UART_TX_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 2, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_UP, 7),
            /* USART2 RX on PA3 (AF7) */
            [UART_RX_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 3, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_UP, 7),
            /* SPI1 SCK on PA5 (AF5) */
            [SPI_SCK_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 5, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_NONE, 5),
            /* SPI1 MISO on PA6 (AF5) */
            [SPI_MISO_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 6, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_NONE, 5),
            /* SPI1 MOSI on PA7 (AF5) */
            [SPI_MOSI_PIN] = WHAL_STM32F4_GPIO_PIN(
                WHAL_STM32F4_GPIO_PORT_A, 7, WHAL_STM32F4_GPIO_MODE_ALTFN,
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST,
                WHAL_STM32F4_GPIO_PULL_NONE, 5),
        },
        .pinCount = PIN_COUNT,
    },
};

/* SysTick singleton — referenced by systick.c directly. */
static const whal_Timer whal_SysTick_Dev = {
    .base = WHAL_CORTEX_M4_SYSTICK_BASE,
    /* .driver: direct API mapping */

    .cfg = (void *)&(const whal_SysTick_Cfg){
        .cyclesPerTick = 100000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
