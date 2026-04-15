#ifndef WHAL_STM32WBA_UART_DMA_H
#define WHAL_STM32WBA_UART_DMA_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/uart/stm32wb_uart.h>
#include <wolfHAL/dma/dma.h>
#include <wolfHAL/dma/stm32wba_gpdma.h>

/*
 * @file stm32wba_uart_dma.h
 * @brief STM32WBA UART driver -- GPDMA-backed variant.
 *
 * Uses the STM32WBA GPDMA controller to move bytes between memory and the
 * USART TDR/RDR registers. Init/Deinit reuse the polled UART driver; Send/Recv
 * block using DMA; SendAsync/RecvAsync return immediately.
 *
 * The GPDMA request lines for USART are (RM0493 Table 208):
 *   REQSEL 11 = USART1_RX
 *   REQSEL 12 = USART1_TX
 *   REQSEL 13 = USART2_RX
 *   REQSEL 14 = USART2_TX
 */

typedef struct {
    whal_Stm32wbUart_Cfg base;
    whal_Dma *dma;
    size_t txCh;
    size_t rxCh;
    whal_Stm32wbaGpdma_ChCfg *txChCfg;
    whal_Stm32wbaGpdma_ChCfg *rxChCfg;
    volatile whal_Error txResult;
    volatile whal_Error rxResult;
} whal_Stm32wbaUartDma_Cfg;

extern const whal_UartDriver whal_Stm32wbaUartDma_Driver;

whal_Error whal_Stm32wbaUartDma_Send(whal_Uart *uartDev, const void *data,
                                      size_t dataSz);
whal_Error whal_Stm32wbaUartDma_Recv(whal_Uart *uartDev, void *data,
                                      size_t dataSz);
whal_Error whal_Stm32wbaUartDma_SendAsync(whal_Uart *uartDev, const void *data,
                                           size_t dataSz);
whal_Error whal_Stm32wbaUartDma_RecvAsync(whal_Uart *uartDev, void *data,
                                           size_t dataSz);

/* DMA channel completion callbacks. Pass the UART DMA cfg as ctx. */
void whal_Stm32wbaUartDma_TxCallback(void *ctx, whal_Error err);
void whal_Stm32wbaUartDma_RxCallback(void *ctx, whal_Error err);

#endif /* WHAL_STM32WBA_UART_DMA_H */
