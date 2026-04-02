#include <stdint.h>
#include <wolfHAL/uart/stm32wb_uart_dma.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

#define UART_CR3_REG 0x08

#define UART_CR3_DMAR_Pos 6
#define UART_CR3_DMAR_Msk (1UL << UART_CR3_DMAR_Pos)

#define UART_CR3_DMAT_Pos 7
#define UART_CR3_DMAT_Msk (1UL << UART_CR3_DMAT_Pos)

#define UART_ISR_REG 0x1C
#define UART_ISR_TC_Pos 6
#define UART_ISR_TC_Msk (1UL << UART_ISR_TC_Pos)

#define UART_RDR_REG 0x24

#define UART_TDR_REG 0x28

whal_Error whal_Stm32wbUartDma_SendAsync(whal_Uart *uartDev, const void *data,
                                         size_t dataSz)
{
    whal_Stm32wbUartDma_Cfg *cfg;
    const whal_Regmap *reg;
    whal_Error err;

    if (!uartDev || !uartDev->cfg || !data || dataSz > UINT16_MAX)
        return WHAL_EINVAL;

    if (dataSz == 0)
        return WHAL_SUCCESS;

    cfg = (whal_Stm32wbUartDma_Cfg *)uartDev->cfg;

    if (cfg->txResult == WHAL_ENOTREADY)
        return WHAL_ENOTREADY;

    reg = &uartDev->regmap;

    cfg->txChCfg->srcAddr = (uint32_t)(uintptr_t)data;
    cfg->txChCfg->dstAddr = (uint32_t)(reg->base + UART_TDR_REG);
    cfg->txChCfg->length = dataSz;

    cfg->txResult = WHAL_ENOTREADY;

    err = whal_Dma_Configure(cfg->dma, cfg->txCh, cfg->txChCfg);
    if (err) {
        cfg->txResult = err;
        return err;
    }

    whal_Reg_Update(reg->base, UART_CR3_REG, UART_CR3_DMAT_Msk,
                    UART_CR3_DMAT_Msk);

    err = whal_Dma_Start(cfg->dma, cfg->txCh);
    if (err) {
        whal_Reg_Update(reg->base, UART_CR3_REG, UART_CR3_DMAT_Msk, 0);
        cfg->txResult = err;
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbUartDma_RecvAsync(whal_Uart *uartDev, void *data,
                                         size_t dataSz)
{
    whal_Stm32wbUartDma_Cfg *cfg;
    const whal_Regmap *reg;
    whal_Error err;

    if (!uartDev || !uartDev->cfg || !data || dataSz > UINT16_MAX)
        return WHAL_EINVAL;

    if (dataSz == 0)
        return WHAL_SUCCESS;

    cfg = (whal_Stm32wbUartDma_Cfg *)uartDev->cfg;

    if (cfg->rxResult == WHAL_ENOTREADY)
        return WHAL_ENOTREADY;

    reg = &uartDev->regmap;

    cfg->rxChCfg->srcAddr = (uint32_t)(reg->base + UART_RDR_REG);
    cfg->rxChCfg->dstAddr = (uint32_t)(uintptr_t)data;
    cfg->rxChCfg->length = dataSz;

    cfg->rxResult = WHAL_ENOTREADY;

    err = whal_Dma_Configure(cfg->dma, cfg->rxCh, cfg->rxChCfg);
    if (err) {
        cfg->rxResult = err;
        return err;
    }

    whal_Reg_Update(reg->base, UART_CR3_REG, UART_CR3_DMAR_Msk,
                    UART_CR3_DMAR_Msk);

    err = whal_Dma_Start(cfg->dma, cfg->rxCh);
    if (err) {
        whal_Reg_Update(reg->base, UART_CR3_REG, UART_CR3_DMAR_Msk, 0);
        cfg->rxResult = err;
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbUartDma_Send(whal_Uart *uartDev, const void *data,
                                    size_t dataSz)
{
    whal_Stm32wbUartDma_Cfg *cfg;
    whal_Error err;

    err = whal_Stm32wbUartDma_SendAsync(uartDev, data, dataSz);
    if (err)
        return err;

    cfg = (whal_Stm32wbUartDma_Cfg *)uartDev->cfg;

    WHAL_TIMEOUT_START(cfg->base.timeout);
    while (cfg->txResult == WHAL_ENOTREADY) {
        if (WHAL_TIMEOUT_EXPIRED(cfg->base.timeout)) {
            err = WHAL_ETIMEOUT;
            goto cleanup;
        }
    }

    if (cfg->txResult != WHAL_SUCCESS) {
        err = cfg->txResult;
        goto cleanup;
    }

    err = whal_Reg_ReadPoll(uartDev->regmap.base, UART_ISR_REG,
                            UART_ISR_TC_Msk, UART_ISR_TC_Msk, cfg->base.timeout);

cleanup:
    whal_Dma_Stop(cfg->dma, cfg->txCh);
    whal_Reg_Update(uartDev->regmap.base, UART_CR3_REG, UART_CR3_DMAT_Msk, 0);
    cfg->txResult = err;

    return err;
}

whal_Error whal_Stm32wbUartDma_Recv(whal_Uart *uartDev, void *data,
                                     size_t dataSz)
{
    whal_Stm32wbUartDma_Cfg *cfg;
    whal_Error err;

    err = whal_Stm32wbUartDma_RecvAsync(uartDev, data, dataSz);
    if (err)
        return err;

    cfg = (whal_Stm32wbUartDma_Cfg *)uartDev->cfg;

    WHAL_TIMEOUT_START(cfg->base.timeout);
    while (cfg->rxResult == WHAL_ENOTREADY) {
        if (WHAL_TIMEOUT_EXPIRED(cfg->base.timeout)) {
            err = WHAL_ETIMEOUT;
            goto cleanup;
        }
    }

    err = cfg->rxResult;

cleanup:
    whal_Dma_Stop(cfg->dma, cfg->rxCh);
    whal_Reg_Update(uartDev->regmap.base, UART_CR3_REG, UART_CR3_DMAR_Msk, 0);
    cfg->rxResult = err;

    return err;
}

void whal_Stm32wbUartDma_TxCallback(void *ctx, whal_Error err)
{
    whal_Stm32wbUartDma_Cfg *cfg = (whal_Stm32wbUartDma_Cfg *)ctx;
    cfg->txResult = err;
}

void whal_Stm32wbUartDma_RxCallback(void *ctx, whal_Error err)
{
    whal_Stm32wbUartDma_Cfg *cfg = (whal_Stm32wbUartDma_Cfg *)ctx;
    cfg->rxResult = err;
}

const whal_UartDriver whal_Stm32wbUartDma_Driver = {
    .Init = whal_Stm32wbUart_Init,
    .Deinit = whal_Stm32wbUart_Deinit,
    .Send = whal_Stm32wbUartDma_Send,
    .Recv = whal_Stm32wbUartDma_Recv,
    .SendAsync = whal_Stm32wbUartDma_SendAsync,
    .RecvAsync = whal_Stm32wbUartDma_RecvAsync,
};
