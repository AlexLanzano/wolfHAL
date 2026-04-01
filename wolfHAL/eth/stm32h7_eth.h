#ifndef WHAL_STM32H7_ETH_H
#define WHAL_STM32H7_ETH_H

/*
 * @file stm32h7_eth.h
 * @brief STM32H7 Ethernet MAC driver (alias for STM32H5 Ethernet).
 *
 * The STM32H7 Ethernet peripheral uses the same Synopsys DWC Ethernet MAC
 * as the STM32H5, with identical register offsets and DMA descriptor formats.
 * This header re-exports the STM32H5 Ethernet driver types and symbols under
 * STM32H7-specific names.
 */

#include <wolfHAL/eth/stm32h5_eth.h>

typedef whal_Stm32h5Eth_TxDesc whal_Stm32h7Eth_TxDesc;
typedef whal_Stm32h5Eth_RxDesc whal_Stm32h7Eth_RxDesc;
typedef whal_Stm32h5Eth_Cfg    whal_Stm32h7Eth_Cfg;

#define whal_Stm32h7Eth_Driver              whal_Stm32h5Eth_Driver
#define whal_Stm32h7Eth_Init                whal_Stm32h5Eth_Init
#define whal_Stm32h7Eth_Deinit              whal_Stm32h5Eth_Deinit
#define whal_Stm32h7Eth_Start               whal_Stm32h5Eth_Start
#define whal_Stm32h7Eth_Stop                whal_Stm32h5Eth_Stop
#define whal_Stm32h7Eth_Send                whal_Stm32h5Eth_Send
#define whal_Stm32h7Eth_Recv                whal_Stm32h5Eth_Recv
#define whal_Stm32h7Eth_MdioRead            whal_Stm32h5Eth_MdioRead
#define whal_Stm32h7Eth_MdioWrite           whal_Stm32h5Eth_MdioWrite
#define whal_Stm32h7Eth_Ext_EnableLoopback  whal_Stm32h5Eth_Ext_EnableLoopback

#endif /* WHAL_STM32H7_ETH_H */
