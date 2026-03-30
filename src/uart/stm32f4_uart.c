#include <stdint.h>
#include <wolfHAL/uart/stm32f4_uart.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timeout.h>

/*
 * STM32F4 USART Register Definitions (USARTv1)
 *
 * The STM32F4 USART uses SR/DR style registers which differ from the
 * ISR/TDR/RDR layout on newer STM32 families.
 */

/* Status Register */
#define UART_SR_REG 0x00
#define UART_SR_RXNE_Pos 5                                             /* Receive data register not empty */
#define UART_SR_RXNE_Msk (1UL << UART_SR_RXNE_Pos)

#define UART_SR_TC_Pos 6                                               /* Transmission complete */
#define UART_SR_TC_Msk (1UL << UART_SR_TC_Pos)

#define UART_SR_TXE_Pos 7                                              /* Transmit data register empty */
#define UART_SR_TXE_Msk (1UL << UART_SR_TXE_Pos)

/* Data Register - shared TX/RX */
#define UART_DR_REG 0x04
#define UART_DR_Pos 0
#define UART_DR_Msk (WHAL_BITMASK(9) << UART_DR_Pos)

/* Baud Rate Register */
#define UART_BRR_REG 0x08
#define UART_BRR_Pos 0
#define UART_BRR_Msk (WHAL_BITMASK(16) << UART_BRR_Pos)

/* Control Register 1 */
#define UART_CR1_REG 0x0C
#define UART_CR1_RE_Pos 2                                              /* Receiver enable */
#define UART_CR1_RE_Msk (1UL << UART_CR1_RE_Pos)

#define UART_CR1_TE_Pos 3                                              /* Transmitter enable */
#define UART_CR1_TE_Msk (1UL << UART_CR1_TE_Pos)

#define UART_CR1_UE_Pos 13                                             /* USART enable */
#define UART_CR1_UE_Msk (1UL << UART_CR1_UE_Pos)

whal_Error whal_Stm32f4Uart_Init(whal_Uart *uartDev)
{
    whal_Stm32f4Uart_Cfg *cfg;
    const whal_Regmap *reg;
    uint32_t brr;

    if (!uartDev || !uartDev->cfg)
        return WHAL_EINVAL;

    reg = &uartDev->regmap;
    cfg = (whal_Stm32f4Uart_Cfg *)uartDev->cfg;

    brr = cfg->brr;

    /* Set baud rate */
    whal_Reg_Update(reg->base, UART_BRR_REG,
                    UART_BRR_Msk,
                    whal_SetBits(UART_BRR_Msk, UART_BRR_Pos, brr));

    /* Enable USART, transmitter, and receiver */
    whal_Reg_Update(reg->base, UART_CR1_REG,
                    UART_CR1_UE_Msk | UART_CR1_RE_Msk | UART_CR1_TE_Msk,
                    whal_SetBits(UART_CR1_UE_Msk, UART_CR1_UE_Pos, 1) |
                    whal_SetBits(UART_CR1_RE_Msk, UART_CR1_RE_Pos, 1) |
                    whal_SetBits(UART_CR1_TE_Msk, UART_CR1_TE_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4Uart_Deinit(whal_Uart *uartDev)
{
    const whal_Regmap *reg;

    if (!uartDev)
        return WHAL_EINVAL;

    reg = &uartDev->regmap;

    /* Disable USART, transmitter, and receiver */
    whal_Reg_Update(reg->base, UART_CR1_REG,
                    UART_CR1_UE_Msk | UART_CR1_RE_Msk | UART_CR1_TE_Msk,
                    whal_SetBits(UART_CR1_UE_Msk, UART_CR1_UE_Pos, 0) |
                    whal_SetBits(UART_CR1_RE_Msk, UART_CR1_RE_Pos, 0) |
                    whal_SetBits(UART_CR1_TE_Msk, UART_CR1_TE_Pos, 0));

    /* Clear baud rate */
    whal_Reg_Update(reg->base, UART_BRR_REG,
                    UART_BRR_Msk,
                    whal_SetBits(UART_BRR_Msk, UART_BRR_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4Uart_Send(whal_Uart *uartDev, const void *data, size_t dataSz)
{
    const whal_Regmap *reg;
    whal_Stm32f4Uart_Cfg *cfg;
    const uint8_t *buf = data;

    if (!uartDev || !uartDev->cfg || !data)
        return WHAL_EINVAL;

    reg = &uartDev->regmap;
    cfg = (whal_Stm32f4Uart_Cfg *)uartDev->cfg;

    for (size_t i = 0; i < dataSz; ++i) {
        whal_Error err;

        /* Wait for transmit data register empty */
        err = whal_Reg_ReadPoll(reg->base, UART_SR_REG, UART_SR_TXE_Msk,
                                UART_SR_TXE_Msk, cfg->timeout);
        if (err)
            return err;

        /* Write byte to data register (must not read DR — use pure write) */
        whal_Reg_Write(reg->base, UART_DR_REG, buf[i]);
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32f4Uart_Recv(whal_Uart *uartDev, void *data, size_t dataSz)
{
    const whal_Regmap *reg;
    whal_Stm32f4Uart_Cfg *cfg;
    uint8_t *buf = data;

    if (!uartDev || !uartDev->cfg || !data)
        return WHAL_EINVAL;

    reg = &uartDev->regmap;
    cfg = (whal_Stm32f4Uart_Cfg *)uartDev->cfg;
    size_t d;

    for (size_t i = 0; i < dataSz; ++i) {
        /* Wait for receive data register not empty */
        whal_Error err = whal_Reg_ReadPoll(reg->base, UART_SR_REG,
                                           UART_SR_RXNE_Msk,
                                           UART_SR_RXNE_Msk, cfg->timeout);
        if (err)
            return err;

        /* Read received byte */
        whal_Reg_Get(reg->base, UART_DR_REG,
                     UART_DR_Msk, UART_DR_Pos, &d);

        buf[i] = d;
    }

    return WHAL_SUCCESS;
}

const whal_UartDriver whal_Stm32f4Uart_Driver = {
    .Init = whal_Stm32f4Uart_Init,
    .Deinit = whal_Stm32f4Uart_Deinit,
    .Send = whal_Stm32f4Uart_Send,
    .Recv = whal_Stm32f4Uart_Recv,
};
