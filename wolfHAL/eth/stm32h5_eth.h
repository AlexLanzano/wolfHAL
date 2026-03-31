#ifndef WHAL_STM32H5_ETH_H
#define WHAL_STM32H5_ETH_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/eth/eth.h>
#include <wolfHAL/timeout.h>

/*
 * @file stm32h5_eth.h
 * @brief STM32H5 Ethernet MAC driver configuration.
 *
 * The STM32H5 Ethernet peripheral is a Synopsys DWC Ethernet MAC with
 * integrated DMA controller. It supports 10/100 Mbps via MII or RMII
 * interface, hardware checksum offload, and IEEE 1588 timestamping.
 *
 * The driver uses descriptor rings in RAM for TX and RX DMA transfers.
 * Descriptor and buffer memory must be provided by the board configuration.
 */

/* TX DMA descriptor (4 x 32-bit words, 16 bytes) */
typedef struct {
    volatile uint32_t des[4];
} whal_Stm32h5Eth_TxDesc;

/* RX DMA descriptor (4 x 32-bit words, 16 bytes) */
typedef struct {
    volatile uint32_t des[4];
} whal_Stm32h5Eth_RxDesc;

/*
 * @brief STM32H5 Ethernet MAC configuration.
 */
typedef struct whal_Stm32h5Eth_Cfg {
    uint8_t macAddr[6];                   /* MAC address */
    whal_Stm32h5Eth_TxDesc *txDescs;     /* TX descriptor ring (pre-allocated) */
    uint8_t *txBufs;                      /* TX frame buffers (pre-allocated) */
    size_t txDescCount;                   /* Number of TX descriptors */
    size_t txBufSize;                     /* Size of each TX buffer in bytes */
    whal_Stm32h5Eth_RxDesc *rxDescs;     /* RX descriptor ring (pre-allocated) */
    uint8_t *rxBufs;                      /* RX frame buffers (pre-allocated) */
    size_t rxDescCount;                   /* Number of RX descriptors */
    size_t rxBufSize;                     /* Size of each RX buffer in bytes */
    whal_Timeout *timeout;
    /* Runtime state (set by driver, not by user) */
    size_t txHead;                        /* Next TX descriptor to use */
    size_t rxHead;                        /* Next RX descriptor to check */
} whal_Stm32h5Eth_Cfg;

/*
 * @brief Driver instance for STM32H5 Ethernet MAC.
 */
extern const whal_EthDriver whal_Stm32h5Eth_Driver;

/*
 * @brief Initialize the STM32H5 Ethernet MAC.
 *
 * Configures the MAC, MTL, and DMA. Sets up descriptor rings and
 * MAC address. Does not start TX/RX.
 *
 * @param ethDev Ethernet device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Eth_Init(whal_Eth *ethDev);

/*
 * @brief Deinitialize the STM32H5 Ethernet MAC.
 *
 * @param ethDev Ethernet device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Eth_Deinit(whal_Eth *ethDev);

/*
 * @brief Start the Ethernet MAC TX/RX and DMA engines.
 *
 * Configures MAC speed and duplex, then enables TX/RX and starts DMA.
 *
 * @param ethDev Ethernet device instance.
 * @param speed  Link speed: WHAL_ETH_SPEED_10 or WHAL_ETH_SPEED_100.
 * @param duplex Duplex mode: WHAL_ETH_DUPLEX_HALF or WHAL_ETH_DUPLEX_FULL.
 *
 * @retval WHAL_SUCCESS MAC started.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Eth_Start(whal_Eth *ethDev, uint8_t speed,
                                  uint8_t duplex);

/*
 * @brief Stop the Ethernet MAC TX/RX and DMA engines.
 *
 * @param ethDev Ethernet device instance.
 *
 * @retval WHAL_SUCCESS MAC stopped.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Eth_Stop(whal_Eth *ethDev);

/*
 * @brief Transmit an Ethernet frame.
 *
 * Copies frame data into the next available TX descriptor buffer,
 * sets the OWN bit, and advances the DMA tail pointer.
 *
 * @param ethDev Ethernet device instance.
 * @param frame  Frame data to transmit.
 * @param len    Length of the frame in bytes.
 *
 * @retval WHAL_SUCCESS   Frame queued for transmission.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTREADY No TX descriptor available.
 */
whal_Error whal_Stm32h5Eth_Send(whal_Eth *ethDev, const void *frame,
                                 size_t len);

/*
 * @brief Receive an Ethernet frame.
 *
 * Checks if the DMA has completed an RX descriptor. If so, copies
 * the frame data into the caller's buffer and returns the descriptor
 * to DMA.
 *
 * @param ethDev Ethernet device instance.
 * @param frame  Buffer to receive frame data into.
 * @param len    On entry, buffer size. On exit, received frame length.
 *
 * @retval WHAL_SUCCESS   Frame received.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ENOTREADY No frame available.
 */
whal_Error whal_Stm32h5Eth_Recv(whal_Eth *ethDev, void *frame,
                                 size_t *len);

/*
 * @brief Read a PHY register via MDIO.
 *
 * @param ethDev  Ethernet device instance.
 * @param phyAddr PHY address on the MDIO bus (0-31).
 * @param reg     PHY register address (0-31).
 * @param val     Output for the 16-bit register value.
 *
 * @retval WHAL_SUCCESS   Register read completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ETIMEOUT  MDIO busy timeout.
 */
whal_Error whal_Stm32h5Eth_MdioRead(whal_Eth *ethDev, uint8_t phyAddr,
                                      uint8_t reg, uint16_t *val);

/*
 * @brief Write a PHY register via MDIO.
 *
 * @param ethDev  Ethernet device instance.
 * @param phyAddr PHY address on the MDIO bus (0-31).
 * @param reg     PHY register address (0-31).
 * @param val     16-bit value to write.
 *
 * @retval WHAL_SUCCESS   Register write completed.
 * @retval WHAL_EINVAL    Invalid arguments.
 * @retval WHAL_ETIMEOUT  MDIO busy timeout.
 */
whal_Error whal_Stm32h5Eth_MdioWrite(whal_Eth *ethDev, uint8_t phyAddr,
                                       uint8_t reg, uint16_t val);

/*
 * @brief Enable or disable MAC-internal loopback.
 *
 * When enabled, the TX data path feeds directly into the RX data path
 * inside the MAC. No PHY, cable, or link partner is needed.
 *
 * @param ethDev Ethernet device instance.
 * @param enable 1 to enable loopback, 0 to disable.
 *
 * @retval WHAL_SUCCESS Loopback state changed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Stm32h5Eth_Ext_EnableLoopback(whal_Eth *ethDev,
                                                uint8_t enable);

#endif /* WHAL_STM32H5_ETH_H */
