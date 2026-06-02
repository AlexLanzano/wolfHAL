/* board.h
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/platform/st/stm32f439zi.h>
#include <wolfHAL/eth_phy/lan8742a_eth_phy.h>

extern whal_Uart g_whalUart;
extern whal_Spi g_whalSpi;
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
    ETH_RMII_REF_CLK_PIN,  /* PA1, AF11 */
    ETH_RMII_MDIO_PIN,     /* PA2, AF11 */
    ETH_RMII_MDC_PIN,      /* PC1, AF11 */
    ETH_RMII_CRS_DV_PIN,   /* PA7, AF11 (shared net with SPI1 MOSI on Nucleo) */
    ETH_RMII_RXD0_PIN,     /* PC4, AF11 */
    ETH_RMII_RXD1_PIN,     /* PC5, AF11 */
    ETH_RMII_TX_EN_PIN,    /* PG11, AF11 */
    ETH_RMII_TXD0_PIN,     /* PG13, AF11 */
    ETH_RMII_TXD1_PIN,     /* PG14, AF11 */
    PIN_COUNT,
};

#define BOARD_LED_PIN              0
#define BOARD_FLASH_START_ADDR     0x08000000
#define BOARD_FLASH_SIZE           0x100000  /* 1 MB — single-bank usage of F439ZI's 2 MB */
/* Flash test target: sector 11 of bank 1 (last 128 KB sector at 0x080E0000). */
#define BOARD_FLASH_TEST_ADDR      0x080E0000
#define BOARD_FLASH_SECTOR_SZ      0x20000   /* 128 KB */
#define BOARD_FLASH_WRITE_SZ       4

/* Flash sector layout — bank 1 only (sectors 0-11). */
#define FLASH_SECTOR_COUNT 12
extern const whal_Stm32f4_Flash_Sector g_flashSectors[FLASH_SECTOR_COUNT];

#define WHAL_CFG_STM32F4_FLASH_DEV { \
    .driver = WHAL_STM32F439_FLASH_DRIVER, \
    .base   = WHAL_STM32F439_FLASH_BASE, \
    .cfg    = (void *)&(const whal_Stm32f4_Flash_Cfg){ \
        .startAddr   = BOARD_FLASH_START_ADDR, \
        .size        = BOARD_FLASH_SIZE, \
        .sectors     = g_flashSectors, \
        .sectorCount = FLASH_SECTOR_COUNT, \
        .timeout     = &g_whalTimeout, \
    }, \
}

#define WHAL_CFG_STM32F4_RNG_DEV { \
    .base = WHAL_STM32F439_RNG_BASE, \
    .cfg  = (void *)&(const whal_Stm32f4_Rng_Cfg){ \
        .timeout = &g_whalTimeout, \
    }, \
}

#define WHAL_CFG_STM32F4_IWDG_DEV { \
    .base = WHAL_STM32F439_IWDG_BASE, \
    .cfg  = (void *)&(const whal_Stm32f4_Iwdg_Cfg){ \
        .prescaler = WHAL_STM32F4_IWDG_PR_32, \
        .reload    = 100, \
        .timeout   = &g_whalTimeout, \
    }, \
}

#define WHAL_CFG_STM32F4_WWDG_DEV { \
    .base = WHAL_STM32F439_WWDG_BASE, \
    .cfg  = (void *)&(const whal_Stm32f4_Wwdg_Cfg){ \
        .prescaler = WHAL_STM32F4_WWDG_TB_128, \
        .window    = 0x7F, \
        .counter   = 0x7F, \
    }, \
}

/* HASH + algorithm dev initializers — singletons defined in stm32f4_hash.c. */
#define WHAL_CFG_STM32F4_HASH_DEV { \
    .base = WHAL_STM32F439_HASH_BASE, \
    .cfg  = (void *)&(const whal_Stm32f4_Hash_Cfg){ \
        .timeout = &g_whalTimeout, \
    }, \
}

#define WHAL_CFG_STM32F4_HASH_SHA1_DEV { \
    .crypto = (whal_Crypto *)&whal_Stm32f4_Hash_Dev, \
}

#define WHAL_CFG_STM32F4_HASH_SHA224_DEV { \
    .crypto = (whal_Crypto *)&whal_Stm32f4_Hash_Dev, \
}

#define WHAL_CFG_STM32F4_HASH_SHA256_DEV { \
    .crypto = (whal_Crypto *)&whal_Stm32f4_Hash_Dev, \
}

#define WHAL_CFG_STM32F4_HASH_HMAC_SHA1_DEV { \
    .crypto = (whal_Crypto *)&whal_Stm32f4_Hash_Dev, \
}

#define WHAL_CFG_STM32F4_HASH_HMAC_SHA224_DEV { \
    .crypto = (whal_Crypto *)&whal_Stm32f4_Hash_Dev, \
}

#define WHAL_CFG_STM32F4_HASH_HMAC_SHA256_DEV { \
    .crypto = (whal_Crypto *)&whal_Stm32f4_Hash_Dev, \
}

/* Ethernet PHY: LAN8742A on the NUCLEO-F439ZI sits at MDIO address 0.
 * PHY ID registers (IEEE PHYIDR1/PHYIDR2 = regs 2/3) read back as
 * 0x0007/0xC131 for LAN8742A. */
#define BOARD_ETH_PHY_ADDR 0
#define BOARD_ETH_PHY_ID1  0x0007
#define BOARD_ETH_PHY_ID2  0xC131

/* ETH descriptor rings + buffer pool — defined in board.c, addresses
 * captured by the ETH singleton's cfg below at compile time. */
#define BOARD_ETH_TX_DESC_COUNT 4
#define BOARD_ETH_RX_DESC_COUNT 4
#define BOARD_ETH_TX_BUF_SIZE   1536
#define BOARD_ETH_RX_BUF_SIZE   1536

extern whal_Stm32f4_Eth_TxDesc ethTxDescs[BOARD_ETH_TX_DESC_COUNT];
extern whal_Stm32f4_Eth_RxDesc ethRxDescs[BOARD_ETH_RX_DESC_COUNT];
extern uint8_t ethTxBufs[BOARD_ETH_TX_DESC_COUNT * BOARD_ETH_TX_BUF_SIZE];
extern uint8_t ethRxBufs[BOARD_ETH_RX_DESC_COUNT * BOARD_ETH_RX_BUF_SIZE];

/* ETH dev initializer — singleton defined in stm32f4_eth.c. */
#define WHAL_CFG_STM32F4_ETH_DEV { \
    .base    = WHAL_STM32F439_ETH_BASE, \
    .macAddr = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x01 }, \
    .cfg     = (void *)&(const whal_Stm32f4_Eth_Cfg){ \
        .txDescs     = ethTxDescs, \
        .txBufs      = ethTxBufs, \
        .txDescCount = BOARD_ETH_TX_DESC_COUNT, \
        .txBufSize   = BOARD_ETH_TX_BUF_SIZE, \
        .rxDescs     = ethRxDescs, \
        .rxBufs      = ethRxBufs, \
        .rxDescCount = BOARD_ETH_RX_DESC_COUNT, \
        .rxBufSize   = BOARD_ETH_RX_BUF_SIZE, \
        .hclkHz      = 100000000, /* AHB1 = SYSCLK = 100 MHz */ \
        .timeout     = &g_whalTimeout, \
    }, \
}

/* LAN8742A PHY dev initializer — singleton defined in lan8742a_eth_phy.c.
 * The PHY does not own its own MDIO controller; MDIO calls dispatch
 * through whal_Eth_MdioRead/Write on the parent MAC, which the LAN8742A
 * driver already passes a NULL eth pointer to (singleton-resolves). */
#define WHAL_CFG_LAN8742A_DEV { \
    .eth  = NULL, \
    .addr = BOARD_ETH_PHY_ADDR, \
    .cfg  = (void *)&(const whal_Lan8742a_Cfg){ \
        .timeout = &g_whalTimeout, \
    }, \
}

/* BOARD_*_DEV: how this board reaches each peripheral. */
#define BOARD_GPIO_DEV         WHAL_INTERNAL_DEV
#define BOARD_UART_DEV         (&g_whalUart)
#define BOARD_SPI_DEV          (&g_whalSpi)
#define BOARD_I2C_DEV          (&g_whalI2c)
#define BOARD_FLASH_DEV        ((whal_Flash *)&whal_Stm32f4_Flash_Dev)
#define BOARD_RNG_DEV          WHAL_INTERNAL_DEV
#define BOARD_WATCHDOG_DEV     WHAL_INTERNAL_DEV
#define BOARD_SHA1_DEV         WHAL_INTERNAL_DEV
#define BOARD_SHA224_DEV       WHAL_INTERNAL_DEV
#define BOARD_SHA256_DEV       WHAL_INTERNAL_DEV
#define BOARD_HMAC_SHA1_DEV    WHAL_INTERNAL_DEV
#define BOARD_HMAC_SHA224_DEV  WHAL_INTERNAL_DEV
#define BOARD_HMAC_SHA256_DEV  WHAL_INTERNAL_DEV
#define BOARD_ETH_DEV          ((whal_Eth *)&whal_Stm32f4_Eth_Dev)
#define BOARD_ETH_PHY_DEV      ((whal_EthPhy *)&whal_Lan8742a_Dev)

#define WHAL_CFG_STM32F4_GPIO_DEV { \
    .base = WHAL_STM32F439_GPIO_BASE, \
    .cfg = (void *)&(const whal_Stm32f4_Gpio_Cfg){ \
        .pinCfg = (const whal_Stm32f4_Gpio_PinCfg[PIN_COUNT]){ \
            /* LED LD1 (green): PB0, output push-pull, no pull. */ \
            [LED_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_B, 0, WHAL_STM32F4_GPIO_MODE_OUT, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_LOW, \
                WHAL_STM32F4_GPIO_PULL_NONE, 0), \
            /* USART3 TX: PD8 (AF7) — NUCLEO ST-LINK VCP TX. */ \
            [UART_TX_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_D, 8, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_UP, 7), \
            /* USART3 RX: PD9 (AF7). */ \
            [UART_RX_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_D, 9, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_UP, 7), \
            /* SPI1 SCK: PA5 (AF5). */ \
            [SPI_SCK_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_A, 5, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 5), \
            /* SPI1 MISO: PA6 (AF5). */ \
            [SPI_MISO_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_A, 6, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 5), \
            /* SPI1 MOSI: PA7 (AF5). */ \
            [SPI_MOSI_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_A, 7, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 5), \
            /* SPI CS: PD14, software-driven. */ \
            [SPI_CS_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_D, 14, WHAL_STM32F4_GPIO_MODE_OUT, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_UP, 0), \
            /* I2C1 SCL: PB8 (AF4), open-drain. */ \
            [I2C_SCL_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_B, 8, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_UP, 4), \
            /* I2C1 SDA: PB9 (AF4), open-drain. */ \
            [I2C_SDA_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_B, 9, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_OPENDRAIN, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_UP, 4), \
            /* RMII REF_CLK: PA1 (AF11). */ \
            [ETH_RMII_REF_CLK_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_A, 1, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
            /* RMII MDIO: PA2 (AF11). */ \
            [ETH_RMII_MDIO_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_A, 2, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
            /* RMII MDC: PC1 (AF11). */ \
            [ETH_RMII_MDC_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_C, 1, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
            /* RMII CRS_DV: PA7 (AF11). Shares PA7 with SPI1 MOSI on the
             * Nucleo board — only one driver may run at a time. */ \
            [ETH_RMII_CRS_DV_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_A, 7, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
            /* RMII RXD0: PC4 (AF11). */ \
            [ETH_RMII_RXD0_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_C, 4, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
            /* RMII RXD1: PC5 (AF11). */ \
            [ETH_RMII_RXD1_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_C, 5, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
            /* RMII TX_EN: PG11 (AF11). */ \
            [ETH_RMII_TX_EN_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_G, 11, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
            /* RMII TXD0: PG13 (AF11). */ \
            [ETH_RMII_TXD0_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_G, 13, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
            /* RMII TXD1: PG14 (AF11). */ \
            [ETH_RMII_TXD1_PIN] = WHAL_STM32F4_GPIO_PIN( \
                WHAL_STM32F4_GPIO_PORT_G, 14, WHAL_STM32F4_GPIO_MODE_ALTFN, \
                WHAL_STM32F4_GPIO_OUTTYPE_PUSHPULL, WHAL_STM32F4_GPIO_SPEED_FAST, \
                WHAL_STM32F4_GPIO_PULL_NONE, 11), \
        }, \
        .pinCount = PIN_COUNT, \
    }, \
}

#define WHAL_CFG_SYSTICK_DEV { \
    .base = WHAL_CORTEX_M4_SYSTICK_BASE, \
    .cfg  = (void *)&(const whal_SysTick_Cfg){ \
        .cyclesPerTick = 100000000 / 1000, \
        .clkSrc  = WHAL_SYSTICK_CLKSRC_SYSCLK, \
        .tickInt = WHAL_SYSTICK_TICKINT_ENABLED, \
    }, \
}

whal_Error Board_Init(void);
whal_Error Board_Deinit(void);
void Board_WaitMs(size_t ms);

#endif /* BOARD_H */
