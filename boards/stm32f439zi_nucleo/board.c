/* board.c
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

/* Board configuration for the NUCLEO-F439ZI (STM32F439ZIT6, 144-pin). */

#include <stdint.h>
#include <stddef.h>
#include "board.h"
#include <wolfHAL/platform/st/stm32f439zi.h>
#include <wolfHAL/eth_phy/lan8742a_eth_phy.h>
#include "peripheral.h"

/* SysTick timing */
volatile uint32_t g_tick = 0;

void SysTick_Handler(void)
{
    g_tick++;
}

uint32_t Board_GetTick(void)
{
    return g_tick;
}

whal_Timeout g_whalTimeout = {
    .timeoutTicks = 1000,
    .GetTick = Board_GetTick,
};

/* Bank-1 sector layout (24-sector dual-bank F439ZI restricted to bank 1). */
const whal_Stm32f4_Flash_Sector g_flashSectors[FLASH_SECTOR_COUNT] = {
    { .addr = 0x08000000, .size = 0x04000 }, /* Sector 0:  16 KB */
    { .addr = 0x08004000, .size = 0x04000 }, /* Sector 1:  16 KB */
    { .addr = 0x08008000, .size = 0x04000 }, /* Sector 2:  16 KB */
    { .addr = 0x0800C000, .size = 0x04000 }, /* Sector 3:  16 KB */
    { .addr = 0x08010000, .size = 0x10000 }, /* Sector 4:  64 KB */
    { .addr = 0x08020000, .size = 0x20000 }, /* Sector 5: 128 KB */
    { .addr = 0x08040000, .size = 0x20000 }, /* Sector 6: 128 KB */
    { .addr = 0x08060000, .size = 0x20000 }, /* Sector 7: 128 KB */
    { .addr = 0x08080000, .size = 0x20000 }, /* Sector 8: 128 KB */
    { .addr = 0x080A0000, .size = 0x20000 }, /* Sector 9: 128 KB */
    { .addr = 0x080C0000, .size = 0x20000 }, /* Sector 10: 128 KB */
    { .addr = 0x080E0000, .size = 0x20000 }, /* Sector 11: 128 KB */
};

static const whal_Stm32f4_Rcc_PeriphClk g_periphClks[] = {
    {WHAL_STM32F439_GPIOA_CLOCK},
    {WHAL_STM32F439_GPIOB_CLOCK},
    {WHAL_STM32F439_GPIOC_CLOCK},
    {WHAL_STM32F439_GPIOD_CLOCK},
    {WHAL_STM32F439_GPIOG_CLOCK},
    {WHAL_STM32F439_USART3_CLOCK},
    {WHAL_STM32F439_SPI1_CLOCK},
    {WHAL_STM32F439_I2C1_CLOCK},
    {WHAL_STM32F439_RNG_CLOCK},
    {WHAL_STM32F439_HASH_CLOCK},
    {WHAL_STM32F439_SYSCFG_CLOCK},
};

/* ETH clocks are gated separately so SYSCFG.PMC can configure RMII before
 * the MAC observes its RX clock. */
static const whal_Stm32f4_Rcc_PeriphClk g_ethClocks[] = {
    {WHAL_STM32F439_ETHMAC_CLOCK},
    {WHAL_STM32F439_ETHMACTX_CLOCK},
    {WHAL_STM32F439_ETHMACRX_CLOCK},
};

/* Ethernet descriptor rings + buffer pool (referenced by the ETH singleton
 * cfg in board.h). 16-byte alignment matches DMA descriptor format. */
whal_Stm32f4_Eth_TxDesc ethTxDescs[BOARD_ETH_TX_DESC_COUNT]
    __attribute__((aligned(16)));
whal_Stm32f4_Eth_RxDesc ethRxDescs[BOARD_ETH_RX_DESC_COUNT]
    __attribute__((aligned(16)));
uint8_t ethTxBufs[BOARD_ETH_TX_DESC_COUNT * BOARD_ETH_TX_BUF_SIZE]
    __attribute__((aligned(4)));
uint8_t ethRxBufs[BOARD_ETH_RX_DESC_COUNT * BOARD_ETH_RX_BUF_SIZE]
    __attribute__((aligned(4)));
#define PERIPH_CLK_COUNT (sizeof(g_periphClks) / sizeof(g_periphClks[0]))
#define ETH_CLOCK_COUNT  (sizeof(g_ethClocks) / sizeof(g_ethClocks[0]))

/* I2C1 — 50 MHz APB1 feeds the peripheral. */
whal_I2c g_whalI2c = {
    .base = WHAL_STM32F439_I2C1_BASE,

    .cfg = &(whal_Stm32f4_I2c_Cfg) {
        .pclk1 = 50000000,
        .timeout = &g_whalTimeout,
    },
};

/* SPI1 — 100 MHz APB2. */
whal_Spi g_whalSpi = {
    .base = WHAL_STM32F439_SPI1_BASE,

    .cfg = &(whal_Stm32f4_Spi_Cfg) {
        .pclk = 100000000,
        .timeout = &g_whalTimeout,
    },
};

/* USART3 (ST-LINK VCP) — 50 MHz APB1, 115200 baud. */
whal_Uart g_whalUart = {
    .base = WHAL_STM32F439_USART3_BASE,

    .cfg = &(whal_Stm32f4_Uart_Cfg) {
        .timeout = &g_whalTimeout,
        .brr = WHAL_STM32F4_UART_BRR(50000000, 115200),
    },
};

void Board_WaitMs(size_t ms)
{
    uint32_t startCount = g_tick;
    while ((g_tick - startCount) < ms)
        ;
}

/*
 * Clock tree: HSE 8 MHz (from ST-LINK MCO) -> PLL -> SYSCLK = 100 MHz.
 *   M = 8   (VCO_in = 1 MHz)
 *   N = 200 (VCO    = 200 MHz)
 *   P = 0   (SYSCLK = VCO/2 = 100 MHz)
 *   Q = 4   (PLL48  = 50 MHz — drives RNG, > HCLK/16 so valid)
 * APB1 = SYSCLK/2 = 50 MHz, APB2 = SYSCLK/1 = 100 MHz.
 * Flash latency = 3 WS at 100 MHz / 2.7-3.6 V (RM0090 Table 11).
 * Default PWR_CR.VOS = Scale 1 (reset value), supports up to 144/168 MHz.
 */
whal_Error Board_Init(void)
{
    whal_Error err;

    err = whal_Stm32f4_Flash_Ext_SetLatency(BOARD_FLASH_DEV,
                                            WHAL_STM32F4_FLASH_LATENCY_3);
    if (err)
        return err;

    err = whal_Stm32f4_Rcc_EnableOsc(
        &(whal_Stm32f4_Rcc_OscCfg){WHAL_STM32F4_RCC_HSE_CFG});
    if (err)
        return err;
    err = whal_Stm32f4_Rcc_EnablePll(&(whal_Stm32f4_Rcc_PllCfg){
        .clkSrc = WHAL_STM32F4_RCC_PLLCLK_SRC_HSE,
        .m = 8, .n = 200, .p = 0, .q = 4,
    });
    if (err)
        return err;
    err = whal_Stm32f4_Rcc_SetBusPrescalers(
        &(whal_Stm32f4_Rcc_BusCfg){.ppre1 = 4, .ppre2 = 0});
    if (err)
        return err;
    err = whal_Stm32f4_Rcc_SetSysClock(WHAL_STM32F4_RCC_SYSCLK_SRC_PLL);
    if (err)
        return err;

    for (size_t i = 0; i < PERIPH_CLK_COUNT; i++) {
        err = whal_Stm32f4_Rcc_EnablePeriphClk(&g_periphClks[i]);
        if (err)
            return err;
    }

    /* Select RMII mode in SYSCFG_PMC.MII_RMII_SEL (bit 23) BEFORE enabling
     * the ETH MAC/TX/RX clocks — the IP samples this select line at first
     * clock edge after reset (RM0090 §33.4.4). SYSCFG_PMC is at offset 0x04
     * of SYSCFG_BASE. */
    *(volatile uint32_t *)(WHAL_STM32F439_SYSCFG_BASE + 0x04) |=
        (1UL << 23);

    /* Now enable the ETH peripheral clocks. */
    for (size_t i = 0; i < ETH_CLOCK_COUNT; i++) {
        err = whal_Stm32f4_Rcc_EnablePeriphClk(&g_ethClocks[i]);
        if (err)
            return err;
    }

    err = whal_Gpio_Init(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_Uart_Init(&g_whalUart);
    if (err)
        return err;

    err = whal_Spi_Init(&g_whalSpi);
    if (err)
        return err;

    err = whal_I2c_Init(&g_whalI2c);
    if (err)
        return err;

    err = whal_Flash_Init(BOARD_FLASH_DEV);
    if (err)
        return err;

    err = whal_Rng_Init(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_Stm32f4_Hash_Init((whal_Crypto *)&whal_Stm32f4_Hash_Dev);
    if (err)
        return err;

    err = whal_Eth_Init(BOARD_ETH_DEV);
    if (err)
        return err;

    err = whal_EthPhy_Init(BOARD_ETH_PHY_DEV);
    if (err)
        return err;

    err = whal_Timer_Init(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_Timer_Start(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = Peripheral_Init();
    if (err)
        return err;

    return WHAL_SUCCESS;
}

whal_Error Board_Deinit(void)
{
    whal_Error err;

    err = Peripheral_Deinit();
    if (err)
        return err;

    err = whal_Timer_Stop(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_Timer_Deinit(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_EthPhy_Deinit(BOARD_ETH_PHY_DEV);
    if (err)
        return err;

    err = whal_Eth_Deinit(BOARD_ETH_DEV);
    if (err)
        return err;

    err = whal_Stm32f4_Hash_Deinit((whal_Crypto *)&whal_Stm32f4_Hash_Dev);
    if (err)
        return err;

    err = whal_Rng_Deinit(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    err = whal_Flash_Deinit(BOARD_FLASH_DEV);
    if (err)
        return err;

    err = whal_I2c_Deinit(&g_whalI2c);
    if (err)
        return err;

    err = whal_Spi_Deinit(&g_whalSpi);
    if (err)
        return err;

    err = whal_Uart_Deinit(&g_whalUart);
    if (err)
        return err;

    err = whal_Gpio_Deinit(WHAL_INTERNAL_DEV);
    if (err)
        return err;

    for (size_t i = ETH_CLOCK_COUNT; i-- > 0; ) {
        err = whal_Stm32f4_Rcc_DisablePeriphClk(&g_ethClocks[i]);
        if (err)
            return err;
    }

    for (size_t i = PERIPH_CLK_COUNT; i-- > 0; ) {
        err = whal_Stm32f4_Rcc_DisablePeriphClk(&g_periphClks[i]);
        if (err)
            return err;
    }

    err = whal_Stm32f4_Rcc_SetSysClock(WHAL_STM32F4_RCC_SYSCLK_SRC_HSI);
    if (err)
        return err;
    err = whal_Stm32f4_Rcc_DisablePll();
    if (err)
        return err;

    return WHAL_SUCCESS;
}
