#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32wb55xx.h>

extern whal_Clock g_whalClock;
extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
extern whal_Flash g_whalFlash;
extern whal_Crypto g_whalCrypto;
extern whal_AesEcb g_whalAesEcb;
extern whal_AesCbc g_whalAesCbc;
extern whal_AesCtr g_whalAesCtr;
extern whal_AesGcm g_whalAesGcm;
extern whal_AesGmac g_whalAesGmac;
extern whal_AesCcm g_whalAesCcm;
extern whal_I2c g_whalI2c;

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

#define BOARD_LED_PIN             0
#define BOARD_FLASH_START_ADDR    0x08000000
#define BOARD_FLASH_SIZE          0x80000  /* 512 KB (upper half reserved for BLE stack) */
#define BOARD_FLASH_TEST_ADDR     0x0807F000
#define BOARD_FLASH_SECTOR_SZ     0x1000


/* GPIO singleton — referenced by stm32wb_gpio.c directly. */
static const whal_Gpio whal_Stm32wb_Gpio_Dev = {
    .base = WHAL_STM32WB55_GPIO_BASE,
    .cfg  = (void *)&(const whal_Stm32wb_Gpio_Cfg){
        .pinCfg = (const whal_Stm32wb_Gpio_PinCfg[PIN_COUNT]){
            [LED_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 5, WHAL_STM32WB_GPIO_MODE_OUT,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_LOW,
                WHAL_STM32WB_GPIO_PULL_UP, 0),
            [UART_TX_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 6, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 7),
            [UART_RX_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 7, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 7),
            [SPI_SCK_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_A, 5, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_NONE, 5),
            [SPI_MISO_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_A, 6, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_NONE, 5),
            [SPI_MOSI_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_A, 7, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_NONE, 5),
            [SPI_CS_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_A, 4, WHAL_STM32WB_GPIO_MODE_OUT,
                WHAL_STM32WB_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 0),
            [I2C_SCL_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 8, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 4),
            [I2C_SDA_PIN] = WHAL_STM32WB_GPIO_PIN(
                WHAL_STM32WB_GPIO_PORT_B, 9, WHAL_STM32WB_GPIO_MODE_ALTFN,
                WHAL_STM32WB_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32WB_GPIO_SPEED_FAST,
                WHAL_STM32WB_GPIO_PULL_UP, 4),
        },
        .pinCount = PIN_COUNT,
    },
};

/* AES crypto + mode singletons — referenced by stm32wb_aes.c directly. */
static const whal_Crypto whal_Stm32wb_Aes_Dev = {
    .base = WHAL_STM32WB55_AES1_BASE,
    /* .driver: direct API mapping */
    .cfg = (void *)&(const whal_Stm32wb_Aes_Cfg){
        .timeout = &g_whalTimeout,
    },
};

static const whal_AesEcb whal_Stm32wb_AesEcb_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wb_Aes_Dev,
    /* .driver: direct API mapping */
};

static const whal_AesCbc whal_Stm32wb_AesCbc_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wb_Aes_Dev,
    /* .driver: direct API mapping */
};

static const whal_AesCtr whal_Stm32wb_AesCtr_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wb_Aes_Dev,
    /* .driver: direct API mapping */
};

static whal_Stm32wb_AesGcm_State g_aesGcmDevState;
static const whal_AesGcm whal_Stm32wb_AesGcm_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wb_Aes_Dev,
    /* .driver: direct API mapping */
    .state = &g_aesGcmDevState,
};

static const whal_AesGmac whal_Stm32wb_AesGmac_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wb_Aes_Dev,
    /* .driver: direct API mapping */
};

static whal_Stm32wb_AesCcm_State g_aesCcmDevState;
static const whal_AesCcm whal_Stm32wb_AesCcm_Dev = {
    .crypto = (whal_Crypto *)&whal_Stm32wb_Aes_Dev,
    /* .driver: direct API mapping */
    .state = &g_aesCcmDevState,
};

/* Flash singleton — referenced by stm32wb_flash.c directly. Const cfg lives
 * here; the dispatcher stub g_whalFlash in board.c carries only .driver so
 * whal_Flash_* can be vtable-dispatched alongside other flash drivers (e.g.
 * SPI NOR W25Q64). */
static const whal_Flash whal_Stm32wb_Flash_Dev = {
    .base = WHAL_STM32WB55_FLASH_BASE,

    .cfg = (void *)&(const whal_Stm32wb_Flash_Cfg){
        .timeout = &g_whalTimeout,
        .startAddr = 0x08000000,
        .size = 0x80000, /* 512 KB (upper half reserved for BLE stack) */
    },
};

/* IWDG singleton — referenced by stm32wb_iwdg.c directly. */
static const whal_Watchdog whal_Stm32wb_Iwdg_Dev = {
    .base = WHAL_STM32WB55_IWDG_BASE,
    /* .driver: direct API mapping */

    .cfg = (void *)&(const whal_Stm32wb_Iwdg_Cfg){
        .prescaler = WHAL_STM32WB_IWDG_PR_32,
        .reload = 100,
        .timeout = &g_whalTimeout,
    },
};

/* WWDG singleton — referenced by stm32wb_wwdg.c directly. */
static const whal_Watchdog whal_Stm32wb_Wwdg_Dev = {
    .base = WHAL_STM32WB55_WWDG_BASE,
    /* .driver: direct API mapping */

    .cfg = (void *)&(const whal_Stm32wb_Wwdg_Cfg){
        .prescaler = WHAL_STM32WB_WWDG_TB_128,
        .window = 0x7F,
        .counter = 0x7F,
    },
};

/* RNG singleton — referenced by stm32wb_rng.c directly. */
static const whal_Rng whal_Stm32wb_Rng_Dev = {
    .base = WHAL_STM32WB55_RNG_BASE,
    /* .driver: direct API mapping */

    .cfg = (void *)&(const whal_Stm32wb_Rng_Cfg){
        .timeout = &g_whalTimeout,
    },
};

/* NVIC singleton — referenced by cortex_m4_nvic.c directly. */
static const whal_Irq whal_Nvic_Dev = {
    .base = WHAL_CORTEX_M4_NVIC_BASE,
    /* .driver: direct API mapping */
};

/* SysTick singleton — referenced by systick.c directly. */
static const whal_Timer whal_SysTick_Dev = {
    .base = WHAL_CORTEX_M4_SYSTICK_BASE,
    /* .driver: direct API mapping */

    .cfg = (void *)&(const whal_SysTick_Cfg){
        .cyclesPerTick = 64000000 / 1000,
        .clkSrc = WHAL_SYSTICK_CLKSRC_SYSCLK,
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED,
    },
};

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
