#ifndef WHAL_STM32WB_UART_DMA_H
#define WHAL_STM32WB_UART_DMA_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/uart/stm32wb_uart.h>
#include <wolfHAL/dma/dma.h>
#include <wolfHAL/dma/stm32wb_dma.h>

/*
 * @file stm32wb_uart_dma.h
 * @brief STM32WB UART driver — DMA-backed variant.
 */

/*
 * @brief DMA-backed UART configuration.
 *
 * Used with whal_Stm32wbUartDma_Driver. Init/Deinit reuse the polled driver.
 * Send/Recv block using DMA. SendAsync/RecvAsync return immediately.
 */
typedef struct {
    whal_Stm32wbUart_Cfg base;
    whal_Dma *dma;
    size_t txCh;
    size_t rxCh;
    whal_Stm32wbDma_ChCfg *txChCfg;
    whal_Stm32wbDma_ChCfg *rxChCfg;
    volatile whal_Error txResult;
    volatile whal_Error rxResult;
} whal_Stm32wbUartDma_Cfg;

/*
 * @brief DMA-backed UART driver. Implements Init, Deinit, Send, Recv,
 *        SendAsync, RecvAsync.
 */
extern const whal_UartDriver whal_Stm32wbUartDma_Driver;

/*
 * @brief Transmit a buffer over UART using DMA (blocking).
 *
 * Starts a DMA transfer and waits for completion with timeout.
 *
 * @param uartDev UART device instance.
 * @param data    Buffer to transmit.
 * @param dataSz  Number of bytes to transmit.
 *
 * @retval WHAL_SUCCESS  All bytes transmitted.
 * @retval WHAL_ETIMEOUT DMA transfer or UART TC timed out.
 * @retval WHAL_EINVAL   Invalid arguments.
 */
whal_Error whal_Stm32wbUartDma_Send(whal_Uart *uartDev, const void *data,
                                     size_t dataSz);

/*
 * @brief Receive a buffer over UART using DMA (blocking).
 *
 * Starts a DMA transfer and waits for completion with timeout.
 *
 * @param uartDev UART device instance.
 * @param data    Receive buffer.
 * @param dataSz  Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS  All bytes received.
 * @retval WHAL_ETIMEOUT DMA transfer timed out.
 * @retval WHAL_EINVAL   Invalid arguments.
 */
whal_Error whal_Stm32wbUartDma_Recv(whal_Uart *uartDev, void *data,
                                     size_t dataSz);

/*
 * @brief Start an asynchronous DMA transmit.
 *
 * Returns immediately after starting the DMA transfer. The buffer must
 * remain valid until cfg->txResult is no longer WHAL_ENOTREADY.
 *
 * @param uartDev UART device instance.
 * @param data    Buffer to transmit. Must remain valid until completion.
 * @param dataSz  Number of bytes to transmit.
 *
 * @retval WHAL_SUCCESS   Transfer started.
 * @retval WHAL_ENOTREADY A transfer is already in progress.
 * @retval WHAL_EINVAL    Invalid arguments.
 */
whal_Error whal_Stm32wbUartDma_SendAsync(whal_Uart *uartDev, const void *data,
                                          size_t dataSz);

/*
 * @brief Start an asynchronous DMA receive.
 *
 * Returns immediately after starting the DMA transfer. The buffer must
 * remain valid until cfg->rxResult is no longer WHAL_ENOTREADY.
 *
 * @param uartDev UART device instance.
 * @param data    Receive buffer. Must remain valid until completion.
 * @param dataSz  Number of bytes to receive.
 *
 * @retval WHAL_SUCCESS   Receive started.
 * @retval WHAL_ENOTREADY A transfer is already in progress.
 * @retval WHAL_EINVAL    Invalid arguments.
 */
whal_Error whal_Stm32wbUartDma_RecvAsync(whal_Uart *uartDev, void *data,
                                          size_t dataSz);

/*
 * @brief DMA TX completion callback for use with whal_Stm32wbDma_IRQHandler.
 *
 * Sets cfg->txResult from the DMA error code. Pass the UART DMA cfg as ctx.
 */
void whal_Stm32wbUartDma_TxCallback(void *ctx, whal_Error err);

/*
 * @brief DMA RX completion callback for use with whal_Stm32wbDma_IRQHandler.
 *
 * Sets cfg->rxResult from the DMA error code. Pass the UART DMA cfg as ctx.
 */
void whal_Stm32wbUartDma_RxCallback(void *ctx, whal_Error err);

#endif /* WHAL_STM32WB_UART_DMA_H */
