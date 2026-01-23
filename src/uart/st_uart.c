#include <stdint.h>
#include <wolfHAL/uart/st_uart.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define STUART_CR1_REG 0x00
#define STUART_CR1_UE WHAL_MASK(0)
#define STUART_CR1_RE WHAL_MASK(2)
#define STUART_CR1_TE WHAL_MASK(3)

#define STUART_BRR_REG 0x0C
#define STUART_BRR_BRR_MASK WHAL_MASK_RANGE(19, 0)

#define STUART_ISR_REG 0x1C
#define STUART_ISR_RXFNE_MASK WHAL_MASK(5)
#define STUART_ISR_TC_MASK WHAL_MASK(6)
#define STUART_ISR_TXE_MASK WHAL_MASK(7)

#define STUART_RDR_REG 0x24
#define STUART_RDR_RDR_MASK WHAL_MASK_RANGE(8, 0)

#define STUART_TDR_REG 0x28
#define STUART_TDR_TDR_MASK WHAL_MASK_RANGE(8, 0)

whal_Error whal_StUart_Init(whal_Uart *uartDev)
{
    whal_Error err;
    whal_StUart_Cfg *cfg;
    const whal_Regmap *reg = &uartDev->regmap;
    size_t clockFreq;
    uint32_t brr;

    cfg = (whal_StUart_Cfg *)uartDev->cfg;

    err = whal_Clock_GetRate(cfg->sysClk, &clockFreq);
    if (err != WHAL_SUCCESS) {
        return err;
    }

    brr = (clockFreq / cfg->baud) * 256;
    
    err = whal_Reg_Update(reg, STUART_BRR_REG,
                          STUART_BRR_BRR_MASK,
                          whal_SetBits(STUART_BRR_BRR_MASK, brr));
    if (err) {
        return err;
    }

    err = whal_Reg_Update(reg, STUART_CR1_REG,
                          STUART_CR1_UE | STUART_CR1_RE | STUART_CR1_TE,
                          whal_SetBits(STUART_CR1_UE, 1) |
                          whal_SetBits(STUART_CR1_RE, 1) |
                          whal_SetBits(STUART_CR1_TE, 1));
    if (err) {
        return err;
    }
    
    return WHAL_SUCCESS;
}

whal_Error whal_StUart_Deinit(whal_Uart *uartDev)
{

    return WHAL_SUCCESS;
}
extern void WaitMs(size_t ms);
whal_Error whal_StUart_Send(whal_Uart *uartDev, uint8_t *data, size_t dataSz)
{
    whal_Error err;
    const whal_Regmap *reg = &uartDev->regmap;
    
    for (size_t i = 0; i < dataSz; ++i) {
        size_t txComplete = 0;

        err = whal_Reg_Update(reg, STUART_TDR_REG, STUART_TDR_TDR_MASK, 
                              whal_SetBits(STUART_TDR_TDR_MASK, data[i]));
        if (err) {
            return err;
        }

        while (!txComplete) {
            err = whal_Reg_Get(reg, STUART_ISR_REG, STUART_ISR_TC_MASK, &txComplete);
            if (err) {
                return err;
            }
        }
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StUart_Recv(whal_Uart *uartDev, uint8_t *data, size_t dataSz)
{
    whal_Error err;
    const whal_Regmap *reg = &uartDev->regmap;
    size_t d;
    
    for (size_t i = 0; i < dataSz; ++i) {
        size_t dataReceived = 0;

        while (!dataReceived) {
            err = whal_Reg_Get(reg, STUART_ISR_REG, STUART_ISR_RXFNE_MASK, &dataReceived);
            if (err) {
                return err;
            }
        }

        err = whal_Reg_Get(reg, STUART_RDR_REG, 
                           STUART_RDR_RDR_MASK, &d);
        if (err) {
            return err;
        }

        data[i] = d;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StUart_Cmd(whal_Uart *uartDev, size_t cmd, void *args)
{

    return WHAL_SUCCESS;
}

whal_UartDriver whal_StUart_Driver = {
    .Init = whal_StUart_Init,
    .Deinit = whal_StUart_Deinit,
    .Send = whal_StUart_Send,
    .Recv = whal_StUart_Recv,
    .Cmd = whal_StUart_Cmd,
};
