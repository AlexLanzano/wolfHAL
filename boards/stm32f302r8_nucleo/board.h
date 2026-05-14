#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32f302r8.h>

extern whal_Clock g_whalClock;
extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
extern whal_I2c g_whalI2c;
extern whal_Flash g_whalFlash;
extern whal_Watchdog g_whalWatchdog;

extern whal_Timeout g_whalTimeout;
extern volatile uint32_t g_tick;

enum {
    LED_PIN,
    UART_TX_PIN,
    UART_RX_PIN,
    SPI_SCK_PIN,
    SPI_MISO_PIN,
    SPI_MOSI_PIN,
    SPI_CS_PIN,
    I2C_SCL_PIN,
    I2C_SDA_PIN,
    PIN_COUNT,
};

#define BOARD_LED_PIN 0

#define BOARD_FLASH_START_ADDR 0x08000000
#define BOARD_FLASH_SIZE       0x10000
#define BOARD_FLASH_TEST_ADDR  0x0800F800
#define BOARD_FLASH_SECTOR_SZ  0x800

/* BOARD_*_DEV: how this board reaches each peripheral. */
#define BOARD_GPIO_DEV       WHAL_SINGLETON
#define BOARD_UART_DEV       (&g_whalUart)
#define BOARD_SPI_DEV        (&g_whalSpi)
#define BOARD_I2C_DEV        (&g_whalI2c)
#define BOARD_FLASH_DEV      (&g_whalFlash)
#define BOARD_CLOCK_DEV      (&g_whalClock)
#define BOARD_WATCHDOG_DEV   (&g_whalWatchdog)

/* WWDG singleton — referenced by stm32f0_wwdg.c via stm32f3_wwdg.c alias.
 * Declared unconditionally because the compiled .c always references it. */
static const whal_Watchdog whal_Stm32f3_Wwdg_Dev = {
    .base = WHAL_STM32F302_WWDG_BASE,
    .cfg  = (void *)&(const whal_Stm32f3_Wwdg_Cfg){
        .prescaler = 3,
        .window = 0x7F,
        .counter = 0x7F,
    },
};

/* IWDG singleton — referenced by stm32wb_iwdg.c via stm32f3_iwdg.c alias. */
static const whal_Watchdog whal_Stm32f3_Iwdg_Dev = {
    .base = WHAL_STM32F302_IWDG_BASE,
    .cfg  = (void *)&(const whal_Stm32f3_Iwdg_Cfg){
        .prescaler = WHAL_STM32F3_IWDG_PR_64,
        .reload = 500,
        .timeout = &g_whalTimeout,
    },
};

/* Flash singleton — referenced by stm32f0_flash.c (compiled via stm32f3_flash.c
 * alias). Const cfg lives here; the dispatcher stub g_whalFlash in board.c
 * carries only .driver so whal_Flash_* can be vtable-dispatched alongside
 * other flash drivers (e.g. SPI NOR W25Q64). */
static const whal_Flash whal_Stm32f3_Flash_Dev = {
    .base = WHAL_STM32F302_FLASH_BASE,

    .cfg = (void *)&(const whal_Stm32f3_Flash_Cfg){
        .startAddr = 0x08000000,
        .size = 0x10000,
        .timeout = &g_whalTimeout,
    },
};

static const whal_Gpio whal_Stm32f3_Gpio_Dev = {
    .base = WHAL_STM32F302_GPIO_BASE,

    .cfg = (void *)&(const whal_Stm32f3_Gpio_Cfg){
        .pinCfg = (const whal_Stm32f3_Gpio_PinCfg[PIN_COUNT]){
            /* LD2 Green LED on PB13 (per UM1724 Figure 14, NUCLEO-F302R8) */
            [LED_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_B, 13, WHAL_STM32F3_GPIO_MODE_OUT,
                WHAL_STM32F3_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F3_GPIO_SPEED_LOW,
                WHAL_STM32F3_GPIO_PULL_NONE, 0),
            /* USART2 TX on PA2, AF7 */
            [UART_TX_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_A, 2, WHAL_STM32F3_GPIO_MODE_ALTFN,
                WHAL_STM32F3_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F3_GPIO_SPEED_FAST,
                WHAL_STM32F3_GPIO_PULL_UP, 7),
            /* USART2 RX on PA3, AF7 */
            [UART_RX_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_A, 3, WHAL_STM32F3_GPIO_MODE_ALTFN,
                WHAL_STM32F3_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F3_GPIO_SPEED_FAST,
                WHAL_STM32F3_GPIO_PULL_UP, 7),
            /* SPI3 SCK on PB3, AF6 */
            [SPI_SCK_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_B, 3, WHAL_STM32F3_GPIO_MODE_ALTFN,
                WHAL_STM32F3_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F3_GPIO_SPEED_FAST,
                WHAL_STM32F3_GPIO_PULL_NONE, 6),
            /* SPI3 MISO on PB4, AF6 */
            [SPI_MISO_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_B, 4, WHAL_STM32F3_GPIO_MODE_ALTFN,
                WHAL_STM32F3_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F3_GPIO_SPEED_FAST,
                WHAL_STM32F3_GPIO_PULL_NONE, 6),
            /* SPI3 MOSI on PB5, AF6 */
            [SPI_MOSI_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_B, 5, WHAL_STM32F3_GPIO_MODE_ALTFN,
                WHAL_STM32F3_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F3_GPIO_SPEED_FAST,
                WHAL_STM32F3_GPIO_PULL_NONE, 6),
            /* SPI CS on PB12, output, push-pull */
            [SPI_CS_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_B, 12, WHAL_STM32F3_GPIO_MODE_OUT,
                WHAL_STM32F3_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F3_GPIO_SPEED_FAST,
                WHAL_STM32F3_GPIO_PULL_UP, 0),
            /* I2C1 SCL on PB8, AF4, open-drain */
            [I2C_SCL_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_B, 8, WHAL_STM32F3_GPIO_MODE_ALTFN,
                WHAL_STM32F3_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32F3_GPIO_SPEED_FAST,
                WHAL_STM32F3_GPIO_PULL_UP, 4),
            /* I2C1 SDA on PB9, AF4, open-drain */
            [I2C_SDA_PIN] = WHAL_STM32F3_GPIO_PIN(
                WHAL_STM32F3_GPIO_PORT_B, 9, WHAL_STM32F3_GPIO_MODE_ALTFN,
                WHAL_STM32F3_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32F3_GPIO_SPEED_FAST,
                WHAL_STM32F3_GPIO_PULL_UP, 4),
        },
        .pinCount = PIN_COUNT,
    },
};

/* SysTick singleton — referenced by systick.c directly. */
static const whal_Timer whal_SysTick_Dev = {
    .base = WHAL_CORTEX_M4_SYSTICK_BASE,
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
