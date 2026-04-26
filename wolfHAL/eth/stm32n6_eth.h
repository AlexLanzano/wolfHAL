#ifndef WHAL_STM32N6_ETH_H
#define WHAL_STM32N6_ETH_H

/**
 * @file stm32n6_eth.h
 * @brief STM32N6 Ethernet MAC driver configuration.
 *
 * The STM32N6 Ethernet peripheral uses the same Synopsys DWC EQOS GMAC IP
 * as the STM32H5, with these differences:
 * - AXI 64-bit bus (vs AHB 32-bit) with ACE coherency registers
 * - 2 DMA channels with 0x80 stride (this driver uses channel 0 only)
 * - Descriptor alignment: bits 2:0 reserved (8-byte aligned)
 * - New AXI registers at DMA offsets 0x1020-0x1028
 *
 * ETH1 base address: 0x48036000
 *
 * The driver uses descriptor rings in RAM for TX and RX DMA transfers.
 * Descriptor and buffer memory must be provided by the board configuration.
 * Descriptors must be 8-byte aligned.
 */

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/eth/eth.h>
#include <wolfHAL/timeout.h>

/** TX DMA descriptor (4 x 32-bit words, 16 bytes, must be 8-byte aligned) */
typedef struct {
    volatile uint32_t des[4];
} whal_Stm32n6Eth_TxDesc;

/** RX DMA descriptor (4 x 32-bit words, 16 bytes, must be 8-byte aligned) */
typedef struct {
    volatile uint32_t des[4];
} whal_Stm32n6Eth_RxDesc;

/**
 * @brief STM32N6 Ethernet MAC configuration.
 */
typedef struct whal_Stm32n6Eth_Cfg {
    whal_Stm32n6Eth_TxDesc *txDescs;     /**< TX descriptor ring (8-byte aligned) */
    uint8_t *txBufs;                      /**< TX frame buffers */
    size_t txDescCount;                   /**< Number of TX descriptors */
    size_t txBufSize;                     /**< Size of each TX buffer in bytes */
    whal_Stm32n6Eth_RxDesc *rxDescs;     /**< RX descriptor ring (8-byte aligned) */
    uint8_t *rxBufs;                      /**< RX frame buffers */
    size_t rxDescCount;                   /**< Number of RX descriptors */
    size_t rxBufSize;                     /**< Size of each RX buffer in bytes */
    whal_Timeout *timeout;
    /* Runtime state (set by driver, not by user) */
    size_t txHead;                        /**< Next TX descriptor to use */
    size_t rxHead;                        /**< Next RX descriptor to check */
} whal_Stm32n6Eth_Cfg;

#ifndef WHAL_CFG_ETH_API_MAPPING_STM32N6
/**
 * @brief Driver instance for STM32N6 Ethernet MAC.
 */
extern const whal_EthDriver whal_Stm32n6Eth_Driver;

whal_Error whal_Stm32n6Eth_Init(whal_Eth *ethDev);
whal_Error whal_Stm32n6Eth_Deinit(whal_Eth *ethDev);
whal_Error whal_Stm32n6Eth_Start(whal_Eth *ethDev, uint8_t speed,
                                  uint8_t duplex);
whal_Error whal_Stm32n6Eth_Stop(whal_Eth *ethDev);
whal_Error whal_Stm32n6Eth_Send(whal_Eth *ethDev, const void *frame,
                                 size_t len);
whal_Error whal_Stm32n6Eth_Recv(whal_Eth *ethDev, void *frame,
                                 size_t *len);
whal_Error whal_Stm32n6Eth_MdioRead(whal_Eth *ethDev, uint8_t phyAddr,
                                      uint8_t reg, uint16_t *val);
whal_Error whal_Stm32n6Eth_MdioWrite(whal_Eth *ethDev, uint8_t phyAddr,
                                       uint8_t reg, uint16_t val);
#endif /* !WHAL_CFG_ETH_API_MAPPING_STM32N6 */

/**
 * @brief Enable or disable MAC-internal loopback.
 *
 * @param ethDev Ethernet device instance.
 * @param enable 1 to enable loopback, 0 to disable.
 * @retval WHAL_SUCCESS Loopback state changed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32n6Eth_Ext_EnableLoopback(whal_Eth *ethDev,
                                                uint8_t enable);

#endif /* WHAL_STM32N6_ETH_H */
