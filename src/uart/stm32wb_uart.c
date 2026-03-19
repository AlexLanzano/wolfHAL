#include <stdint.h>
#include <wolfHAL/uart/stm32wb_uart.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

#define UART_CR1_REG 0x00
#define UART_CR1_UE_Pos 0
#define UART_CR1_UE_Msk (1UL << UART_CR1_UE_Pos)

#define UART_CR1_RE_Pos 2
#define UART_CR1_RE_Msk (1UL << UART_CR1_RE_Pos)

#define UART_CR1_TE_Pos 3
#define UART_CR1_TE_Msk (1UL << UART_CR1_TE_Pos)

#define UART_BRR_REG 0x0C
#define UART_BRR_Pos 0
#define UART_BRR_Msk (WHAL_BITMASK(20) << UART_BRR_Pos)

#define UART_ISR_REG 0x1C
#define UART_ISR_RXFNE_Pos 5
#define UART_ISR_RXFNE_Msk (1UL << UART_ISR_RXFNE_Pos)

#define UART_ISR_TC_Pos 6
#define UART_ISR_TC_Msk (1UL << UART_ISR_TC_Pos)

#define UART_ISR_TXE_Pos 7
#define UART_ISR_TXE_Msk (1UL << UART_ISR_TXE_Pos)

#define UART_RDR_REG 0x24
#define UART_RDR_Pos 0
#define UART_RDR_Msk (WHAL_BITMASK(9) << UART_RDR_Pos)

#define UART_TDR_REG 0x28
#define UART_TDR_Pos 0
#define UART_TDR_Msk (WHAL_BITMASK(9) << UART_TDR_Pos)

whal_Error whal_Stm32wbUart_Init(whal_Uart *uartDev)
{
    whal_Stm32wbUart_Cfg *cfg;
    const whal_Regmap *reg = &uartDev->regmap;
    uint32_t brr;

    cfg = (whal_Stm32wbUart_Cfg *)uartDev->cfg;

    brr = cfg->baud;

    whal_Reg_Update(reg->base, UART_BRR_REG,
                    UART_BRR_Msk,
                    whal_SetBits(UART_BRR_Msk, UART_BRR_Pos, brr));
    whal_Reg_Update(reg->base, UART_CR1_REG,
                    UART_CR1_UE_Msk | UART_CR1_RE_Msk | UART_CR1_TE_Msk,
                    whal_SetBits(UART_CR1_UE_Msk, UART_CR1_UE_Pos, 1) |
                    whal_SetBits(UART_CR1_RE_Msk, UART_CR1_RE_Pos, 1) |
                    whal_SetBits(UART_CR1_TE_Msk, UART_CR1_TE_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbUart_Deinit(whal_Uart *uartDev)
{
    const whal_Regmap *reg = &uartDev->regmap;

    whal_Reg_Update(reg->base, UART_CR1_REG,
                    UART_CR1_UE_Msk | UART_CR1_RE_Msk | UART_CR1_TE_Msk,
                    whal_SetBits(UART_CR1_UE_Msk, UART_CR1_UE_Pos, 0) |
                    whal_SetBits(UART_CR1_RE_Msk, UART_CR1_RE_Pos, 0) |
                    whal_SetBits(UART_CR1_TE_Msk, UART_CR1_TE_Pos, 0));

    whal_Reg_Update(reg->base, UART_BRR_REG,
                          UART_BRR_Msk,
                          whal_SetBits(UART_BRR_Msk, UART_BRR_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbUart_Send(whal_Uart *uartDev, const void *data, size_t dataSz)
{
    const whal_Regmap *reg = &uartDev->regmap;
    whal_Stm32wbUart_Cfg *cfg = (whal_Stm32wbUart_Cfg *)uartDev->cfg;
    const uint8_t *buf = data;

    for (size_t i = 0; i < dataSz; ++i) {
        whal_Error err;
        whal_Reg_Update(reg->base, UART_TDR_REG, UART_TDR_Msk,
                        whal_SetBits(UART_TDR_Msk, UART_TDR_Pos, buf[i]));

        err = whal_Reg_ReadPoll(reg->base, UART_ISR_REG, UART_ISR_TC_Msk,
                                UART_ISR_TC_Msk, cfg->timeout);
        if (err)
            return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbUart_Recv(whal_Uart *uartDev, void *data, size_t dataSz)
{
    const whal_Regmap *reg = &uartDev->regmap;
    whal_Stm32wbUart_Cfg *cfg = (whal_Stm32wbUart_Cfg *)uartDev->cfg;
    uint8_t *buf = data;
    size_t d;

    for (size_t i = 0; i < dataSz; ++i) {
        whal_Error err = whal_Reg_ReadPoll(reg->base, UART_ISR_REG,
                                           UART_ISR_RXFNE_Msk,
                                           UART_ISR_RXFNE_Msk, cfg->timeout);
        if (err)
            return err;

        whal_Reg_Get(reg->base, UART_RDR_REG,
                     UART_RDR_Msk, UART_RDR_Pos, &d);

        buf[i] = d;
    }

    return WHAL_SUCCESS;
}

const whal_UartDriver whal_Stm32wbUart_Driver = {
    .Init = whal_Stm32wbUart_Init,
    .Deinit = whal_Stm32wbUart_Deinit,
    .Send = whal_Stm32wbUart_Send,
    .Recv = whal_Stm32wbUart_Recv,
};

