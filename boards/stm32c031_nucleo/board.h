#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32c031xx.h>

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

#define BOARD_FLASH_START_ADDR 0x08000000
#define BOARD_FLASH_SIZE       0x8000
#define BOARD_FLASH_TEST_ADDR  0x08007800
#define BOARD_FLASH_SECTOR_SZ  0x800

static const whal_Gpio whal_Stm32c0_Gpio_Dev = {
    .base = WHAL_STM32C031_GPIO_BASE,

    .cfg = (void *)&(const whal_Stm32c0_Gpio_Cfg){
        .pinCfg = (const whal_Stm32c0_Gpio_PinCfg[PIN_COUNT]){
            /* LD4 Green LED on PA5 */
            [LED_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 5, WHAL_STM32C0_GPIO_MODE_OUT,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_LOW,
                WHAL_STM32C0_GPIO_PULL_NONE, 0),
            /* USART2 TX on PA2, AF1 (ST-Link VCP) */
            [UART_TX_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 2, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_UP, 1),
            /* USART2 RX on PA3, AF1 (ST-Link VCP) */
            [UART_RX_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 3, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_UP, 1),
            /* SPI1 SCK on PA1, AF0 */
            [SPI_SCK_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 1, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_NONE, 0),
            /* SPI1 MISO on PA6, AF0 */
            [SPI_MISO_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 6, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_NONE, 0),
            /* SPI1 MOSI on PA7, AF0 */
            [SPI_MOSI_PIN] = WHAL_STM32C0_GPIO_PIN(
                WHAL_STM32C0_GPIO_PORT_A, 7, WHAL_STM32C0_GPIO_MODE_ALTFN,
                WHAL_STM32C0_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32C0_GPIO_SPEED_FAST,
                WHAL_STM32C0_GPIO_PULL_NONE, 0),
        },
        .pinCount = PIN_COUNT,
    },
};

/* SysTick singleton — referenced by systick.c directly. */
static const whal_Timer whal_SysTick_Dev = {
    .base = WHAL_CORTEX_M0PLUS_SYSTICK_BASE,
    /* .driver: direct API mapping */

    .cfg = (void *)&(const whal_SysTick_Cfg){
        .cyclesPerTick = 48000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
