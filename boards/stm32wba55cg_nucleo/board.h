#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32wba55cg.h>
#include <wolfHAL/crypto/stm32wba_aes.h>
#include <wolfHAL/crypto/stm32wba_hash.h>

extern whal_Clock g_whalClock;
extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
extern whal_Flash g_whalFlash;
extern whal_Rng g_whalRng;
extern whal_I2c g_whalI2c;
extern whal_Crypto g_whalCrypto;
extern whal_AesEcb g_whalAesEcb;
extern whal_AesCbc g_whalAesCbc;
extern whal_AesCtr g_whalAesCtr;
extern whal_AesGcm g_whalAesGcm;
extern whal_AesGmac g_whalAesGmac;
extern whal_AesCcm g_whalAesCcm;
extern whal_Crypto g_whalHash;
extern whal_Sha1 g_whalSha1;
extern whal_Sha224 g_whalSha224;
extern whal_Sha256 g_whalSha256;
extern whal_HmacSha1 g_whalHmacSha1;
extern whal_HmacSha224 g_whalHmacSha224;
extern whal_HmacSha256 g_whalHmacSha256;
extern whal_Watchdog g_whalWatchdog;
#ifdef BOARD_DMA
extern whal_Dma g_whalDma1;
#endif

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

/* LD2 (Green) on PA9 -- avoids conflict with SPI1_SCK on PB4 (LD1) */
#define BOARD_LED_PIN             0
#define BOARD_FLASH_START_ADDR    0x08000000
#define BOARD_FLASH_SIZE          0x100000  /* 1 MB */
#define BOARD_FLASH_TEST_ADDR     0x080FE000
#define BOARD_FLASH_SECTOR_SZ     0x2000    /* 8 KB */

static const whal_Gpio whal_Stm32wba_Gpio_Dev = {
    .base = WHAL_STM32WBA55_GPIO_BASE,
    .driver = WHAL_STM32WBA55_GPIO_DRIVER,

    .cfg = (void *)&(const whal_Stm32wba_Gpio_Cfg){
        .pinCfg = (const whal_Stm32wba_Gpio_PinCfg[PIN_COUNT]){
            /* LED: PA9 (LD2, Green), output, push-pull, low speed, pull-up */
            [LED_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_A, 9, WHAL_STM32WBA_GPIO_MODE_OUT,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_LOW,
                WHAL_STM32WBA_GPIO_PULL_UP, 0),
            /* USART1 TX: PB12, AF7 */
            [UART_TX_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 12, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 7),
            /* USART1 RX: PA8, AF7 */
            [UART_RX_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_A, 8, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 7),
            /* SPI1 SCK: PB4, AF5 */
            [SPI_SCK_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 4, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_NONE, 5),
            /* SPI1 MISO: PB3, AF5 */
            [SPI_MISO_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 3, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_NONE, 5),
            /* SPI1 MOSI: PA15, AF5 */
            [SPI_MOSI_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_A, 15, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_NONE, 5),
            /* SPI CS: PA12, output, push-pull */
            [SPI_CS_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_A, 12, WHAL_STM32WBA_GPIO_MODE_OUT,
                WHAL_STM32WBA_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 0),
            /* I2C1 SCL: PB2, AF4, open-drain */
            [I2C_SCL_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 2, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 4),
            /* I2C1 SDA: PB1, AF4, open-drain */
            [I2C_SDA_PIN] = WHAL_STM32WBA_GPIO_PIN(
                WHAL_STM32WBA_GPIO_PORT_B, 1, WHAL_STM32WBA_GPIO_MODE_ALTFN,
                WHAL_STM32WBA_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32WBA_GPIO_SPEED_FAST,
                WHAL_STM32WBA_GPIO_PULL_UP, 4),
        },
        .pinCount = PIN_COUNT,
    },
};

/* NVIC singleton — referenced by cortex_m4_nvic.c directly. */
static const whal_Irq whal_Nvic_Dev = {
    .base = WHAL_CORTEX_M33_NVIC_BASE,
    /* .driver: direct API mapping */
};

/* SysTick singleton — referenced by systick.c directly. */
static const whal_Timer whal_SysTick_Dev = {
    .base = WHAL_CORTEX_M33_SYSTICK_BASE,
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
