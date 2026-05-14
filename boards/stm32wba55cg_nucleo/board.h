#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32wba55cg.h>
#include <wolfHAL/crypto/stm32wba_aes.h>
#include <wolfHAL/crypto/stm32wba_hash.h>
#include <wolfHAL/rng/stm32wba_rng.h>

extern whal_Clock g_whalClock;
extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
extern whal_Flash g_whalFlash;
extern whal_I2c g_whalI2c;
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

/* IWDG/WWDG singletons — referenced by stm32wb_iwdg.c/stm32wb_wwdg.c directly. */
static const whal_Watchdog whal_Stm32wba_Iwdg_Dev = {
    .base = WHAL_STM32WBA55_IWDG_BASE,
    .cfg  = (void *)&(const whal_Stm32wba_Iwdg_Cfg){
        .prescaler = WHAL_STM32WBA_IWDG_PR_32,
        .reload = 100,
        .timeout = &g_whalTimeout,
    },
};

static const whal_Watchdog whal_Stm32wba_Wwdg_Dev = {
    .base = WHAL_STM32WBA55_WWDG_BASE,
    .cfg  = (void *)&(const whal_Stm32wba_Wwdg_Cfg){
        .prescaler = WHAL_STM32WBA_WWDG_TB_128,
        .window = 0x7F,
        .counter = 0x7F,
    },
};

/* AES + mode singletons — referenced by stm32wb_aes.c directly. */
static const whal_Crypto whal_Stm32wba_Aes_Dev = {
    .base = WHAL_STM32WBA55_AES_BASE,
    .cfg  = (void *)&(const whal_Stm32wba_Aes_Cfg){
        .timeout = &g_whalTimeout,
    },
};

static const whal_AesEcb whal_Stm32wba_AesEcb_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Aes_Dev,
};

static const whal_AesCbc whal_Stm32wba_AesCbc_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Aes_Dev,
};

static const whal_AesCtr whal_Stm32wba_AesCtr_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Aes_Dev,
};

static whal_Stm32wba_AesGcm_State g_wbaAesGcmDevState;
static const whal_AesGcm whal_Stm32wba_AesGcm_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Aes_Dev,
    .state  = &g_wbaAesGcmDevState,
};

static const whal_AesGmac whal_Stm32wba_AesGmac_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Aes_Dev,
};

static whal_Stm32wba_AesCcm_State g_wbaAesCcmDevState;
static const whal_AesCcm whal_Stm32wba_AesCcm_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Aes_Dev,
    .state  = &g_wbaAesCcmDevState,
};

/* RNG singleton — referenced by stm32wba_rng.c directly. */
static const whal_Rng whal_Stm32wba_Rng_Dev = {
    .base = WHAL_STM32WBA55_RNG_BASE,
    .cfg  = (void *)&(const whal_Stm32wba_Rng_Cfg){
        .timeout = &g_whalTimeout,
    },
};

/* Flash singleton — referenced by stm32wba_flash.c directly. Const cfg lives
 * here; the dispatcher stub g_whalFlash in board.c carries only .driver so
 * whal_Flash_* can be vtable-dispatched alongside other flash drivers (e.g.
 * SPI NOR W25Q64). */
static const whal_Flash whal_Stm32wba_Flash_Dev = {
    .base = WHAL_STM32WBA55_FLASH_BASE,

    .cfg = (void *)&(const whal_Stm32wba_Flash_Cfg){
        .timeout = &g_whalTimeout,
        .startAddr = 0x08000000,
        .size = 0x100000, /* 1 MB */
    },
};

/* HASH + algorithm singletons — referenced by stm32wba_hash.c directly. */
static const whal_Crypto whal_Stm32wba_Hash_Dev = {
    .base = WHAL_STM32WBA55_HASH_BASE,
    .cfg  = (void *)&(const whal_Stm32wba_Hash_Cfg){
        .timeout = &g_whalTimeout,
    },
};

static const whal_Sha1 whal_Stm32wba_Sha1_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Hash_Dev,
};

static const whal_Sha224 whal_Stm32wba_Sha224_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Hash_Dev,
};

static const whal_Sha256 whal_Stm32wba_Sha256_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Hash_Dev,
};

static const whal_HmacSha1 whal_Stm32wba_HmacSha1_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Hash_Dev,
};

static const whal_HmacSha224 whal_Stm32wba_HmacSha224_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Hash_Dev,
};

static const whal_HmacSha256 whal_Stm32wba_HmacSha256_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wba_Hash_Dev,
};

/* BOARD_*_DEV: how this board reaches each peripheral. */
#define BOARD_GPIO_DEV         WHAL_SINGLETON
#define BOARD_UART_DEV         (&g_whalUart)
#define BOARD_SPI_DEV          (&g_whalSpi)
#define BOARD_I2C_DEV          (&g_whalI2c)
#define BOARD_FLASH_DEV        (&g_whalFlash)
#define BOARD_CLOCK_DEV        (&g_whalClock)
#define BOARD_WATCHDOG_DEV     (&g_whalWatchdog)
#define BOARD_RNG_DEV          WHAL_SINGLETON
#define BOARD_AES_ECB_DEV      WHAL_SINGLETON
#define BOARD_AES_CBC_DEV      WHAL_SINGLETON
#define BOARD_AES_CTR_DEV      WHAL_SINGLETON
#define BOARD_AES_GCM_DEV      WHAL_SINGLETON
#define BOARD_AES_GMAC_DEV     WHAL_SINGLETON
#define BOARD_AES_CCM_DEV      WHAL_SINGLETON
#define BOARD_SHA1_DEV         WHAL_SINGLETON
#define BOARD_SHA224_DEV       WHAL_SINGLETON
#define BOARD_SHA256_DEV       WHAL_SINGLETON
#define BOARD_HMAC_SHA1_DEV    WHAL_SINGLETON
#define BOARD_HMAC_SHA224_DEV  WHAL_SINGLETON
#define BOARD_HMAC_SHA256_DEV  WHAL_SINGLETON

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
