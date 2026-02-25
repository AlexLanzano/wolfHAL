#include <stdint.h>
#include <wolfHAL/uart/pic32cz_uart.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/clock/clock.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/* SERCOM USART Register Offsets */
#define USART_CTRLA_REG     0x00
#define USART_CTRLB_REG     0x04
#define USART_CTRLC_REG     0x08
#define USART_BAUD_REG      0x0C
#define USART_INTENCLR_REG  0x14
#define USART_INTENSET_REG  0x16
#define USART_INTFLAG_REG   0x18
#define USART_STATUS_REG    0x1A
#define USART_SYNCBUSY_REG  0x1C
#define USART_DATA_REG      0x28

/* CTRLA Register Bit Definitions */
#define USART_CTRLA_SWRST_Pos       0
#define USART_CTRLA_SWRST_Msk       (1UL << USART_CTRLA_SWRST_Pos)

#define USART_CTRLA_ENABLE_Pos      1
#define USART_CTRLA_ENABLE_Msk      (1UL << USART_CTRLA_ENABLE_Pos)

#define USART_CTRLA_MODE_Pos        2
#define USART_CTRLA_MODE_Msk        (WHAL_BITMASK(3) << USART_CTRLA_MODE_Pos)
#define USART_CTRLA_MODE_USART_INT_CLK 0x1

#define USART_CTRLA_RUNSTDBY_Pos    7
#define USART_CTRLA_RUNSTDBY_Msk    (1UL << USART_CTRLA_RUNSTDBY_Pos)

#define USART_CTRLA_IBON_Pos        8
#define USART_CTRLA_IBON_Msk        (1UL << USART_CTRLA_IBON_Pos)

#define USART_CTRLA_TXINV_Pos       9
#define USART_CTRLA_TXINV_Msk       (1UL << USART_CTRLA_TXINV_Pos)

#define USART_CTRLA_RXINV_Pos       10
#define USART_CTRLA_RXINV_Msk       (1UL << USART_CTRLA_RXINV_Pos)

#define USART_CTRLA_SAMPR_Pos       13
#define USART_CTRLA_SAMPR_Msk       (WHAL_BITMASK(3) << USART_CTRLA_SAMPR_Pos)
#define USART_CTRLA_SAMPR_16X_ARITH 0x0
#define USART_CTRLA_SAMPR_16X_FRAC  0x1
#define USART_CTRLA_SAMPR_8X_ARITH  0x2
#define USART_CTRLA_SAMPR_8X_FRAC   0x3
#define USART_CTRLA_SAMPR_3X_ARITH  0x4

#define USART_CTRLA_TXPO_Pos        16
#define USART_CTRLA_TXPO_Msk        (WHAL_BITMASK(2) << USART_CTRLA_TXPO_Pos)

#define USART_CTRLA_RXPO_Pos        20
#define USART_CTRLA_RXPO_Msk        (WHAL_BITMASK(2) << USART_CTRLA_RXPO_Pos)

#define USART_CTRLA_SAMPA_Pos       22
#define USART_CTRLA_SAMPA_Msk       (WHAL_BITMASK(2) << USART_CTRLA_SAMPA_Pos)

#define USART_CTRLA_FORM_Pos        24
#define USART_CTRLA_FORM_Msk        (WHAL_BITMASK(4) << USART_CTRLA_FORM_Pos)
#define USART_CTRLA_FORM_USART  0x0
#define USART_CTRLA_FORM_PARITY 0x1

#define USART_CTRLA_CMODE_Pos       28
#define USART_CTRLA_CMODE_Msk       (1UL << USART_CTRLA_CMODE_Pos)

#define USART_CTRLA_CPOL_Pos        29
#define USART_CTRLA_CPOL_Msk        (1UL << USART_CTRLA_CPOL_Pos)

#define USART_CTRLA_DORD_Pos        30
#define USART_CTRLA_DORD_Msk        (1UL << USART_CTRLA_DORD_Pos)

/* CTRLB Register Bit Definitions */
#define USART_CTRLB_CHSIZE_Pos      0
#define USART_CTRLB_CHSIZE_Msk      (WHAL_BITMASK(3) << USART_CTRLB_CHSIZE_Pos)
#define USART_CTRLB_CHSIZE_8BIT 0x0
#define USART_CTRLB_CHSIZE_9BIT 0x1
#define USART_CTRLB_CHSIZE_5BIT 0x5
#define USART_CTRLB_CHSIZE_6BIT 0x6
#define USART_CTRLB_CHSIZE_7BIT 0x7

#define USART_CTRLB_SBMODE_Pos      6
#define USART_CTRLB_SBMODE_Msk      (1UL << USART_CTRLB_SBMODE_Pos)

#define USART_CTRLB_COLDEN_Pos      8
#define USART_CTRLB_COLDEN_Msk      (1UL << USART_CTRLB_COLDEN_Pos)

#define USART_CTRLB_SFDE_Pos        9
#define USART_CTRLB_SFDE_Msk        (1UL << USART_CTRLB_SFDE_Pos)

#define USART_CTRLB_ENC_Pos         10
#define USART_CTRLB_ENC_Msk         (1UL << USART_CTRLB_ENC_Pos)

#define USART_CTRLB_PMODE_Pos       13
#define USART_CTRLB_PMODE_Msk       (1UL << USART_CTRLB_PMODE_Pos)

#define USART_CTRLB_TXEN_Pos        16
#define USART_CTRLB_TXEN_Msk        (1UL << USART_CTRLB_TXEN_Pos)

#define USART_CTRLB_RXEN_Pos        17
#define USART_CTRLB_RXEN_Msk        (1UL << USART_CTRLB_RXEN_Pos)

/* INTFLAG Register Bit Definitions */
#define USART_INTFLAG_DRE_Pos       0
#define USART_INTFLAG_DRE_Msk       (1UL << USART_INTFLAG_DRE_Pos)

#define USART_INTFLAG_TXC_Pos       1
#define USART_INTFLAG_TXC_Msk       (1UL << USART_INTFLAG_TXC_Pos)

#define USART_INTFLAG_RXC_Pos       2
#define USART_INTFLAG_RXC_Msk       (1UL << USART_INTFLAG_RXC_Pos)

#define USART_INTFLAG_RXS_Pos       3
#define USART_INTFLAG_RXS_Msk       (1UL << USART_INTFLAG_RXS_Pos)

#define USART_INTFLAG_CTSIC_Pos     4
#define USART_INTFLAG_CTSIC_Msk     (1UL << USART_INTFLAG_CTSIC_Pos)

#define USART_INTFLAG_RXBRK_Pos     5
#define USART_INTFLAG_RXBRK_Msk     (1UL << USART_INTFLAG_RXBRK_Pos)

#define USART_INTFLAG_ERROR_Pos     7
#define USART_INTFLAG_ERROR_Msk     (1UL << USART_INTFLAG_ERROR_Pos)

/* STATUS Register Bit Definitions */
#define USART_STATUS_PERR_Pos       0
#define USART_STATUS_PERR_Msk       (1UL << USART_STATUS_PERR_Pos)

#define USART_STATUS_FERR_Pos       1
#define USART_STATUS_FERR_Msk       (1UL << USART_STATUS_FERR_Pos)

#define USART_STATUS_BUFOVF_Pos     2
#define USART_STATUS_BUFOVF_Msk     (1UL << USART_STATUS_BUFOVF_Pos)

#define USART_STATUS_CTS_Pos        3
#define USART_STATUS_CTS_Msk        (1UL << USART_STATUS_CTS_Pos)

#define USART_STATUS_ISF_Pos        4
#define USART_STATUS_ISF_Msk        (1UL << USART_STATUS_ISF_Pos)

#define USART_STATUS_COLL_Pos       5
#define USART_STATUS_COLL_Msk       (1UL << USART_STATUS_COLL_Pos)

#define USART_STATUS_TXE_Pos        6
#define USART_STATUS_TXE_Msk        (1UL << USART_STATUS_TXE_Pos)

/* SYNCBUSY Register Bit Definitions */
#define USART_SYNCBUSY_SWRST_Pos    0
#define USART_SYNCBUSY_SWRST_Msk    (1UL << USART_SYNCBUSY_SWRST_Pos)

#define USART_SYNCBUSY_ENABLE_Pos   1
#define USART_SYNCBUSY_ENABLE_Msk   (1UL << USART_SYNCBUSY_ENABLE_Pos)

#define USART_SYNCBUSY_CTRLB_Pos    2
#define USART_SYNCBUSY_CTRLB_Msk    (1UL << USART_SYNCBUSY_CTRLB_Pos)

/* DATA Register */
#define USART_DATA_Pos              0
#define USART_DATA_Msk              (WHAL_BITMASK(9) << USART_DATA_Pos)

static void whal_Pic32czUart_WaitSync(const whal_Regmap *reg, size_t mask, size_t pos)
{
    size_t busy = 1;
    while (busy) {
        whal_Reg_Get(reg->base, USART_SYNCBUSY_REG, mask, pos, &busy);
    }
}

whal_Error whal_Pic32czUart_Init(whal_Uart *uartDev)
{
    whal_Error err;
    whal_Pic32czUart_Cfg *cfg;
    const whal_Regmap *reg;

    if (!uartDev) {
        return WHAL_EINVAL;
    }

    reg = &uartDev->regmap;
    cfg = (whal_Pic32czUart_Cfg *)uartDev->cfg;

    /* Enable peripheral clock */
    err = whal_Clock_Enable(cfg->clkCtrl, cfg->clk);
    if (err != WHAL_SUCCESS) {
        return err;
    }

    /* Configure CTRLA: internal clock, async mode, LSB first, 16x sampling */
    whal_Reg_Update(reg->base, USART_CTRLA_REG,
                    USART_CTRLA_MODE_Msk |
                    USART_CTRLA_SAMPR_Msk |
                    USART_CTRLA_TXPO_Msk |
                    USART_CTRLA_RXPO_Msk |
                    USART_CTRLA_FORM_Msk |
                    USART_CTRLA_CMODE_Msk |
                    USART_CTRLA_DORD_Msk,
                    whal_SetBits(USART_CTRLA_MODE_Msk, USART_CTRLA_MODE_Pos, USART_CTRLA_MODE_USART_INT_CLK) |
                    whal_SetBits(USART_CTRLA_SAMPR_Msk, USART_CTRLA_SAMPR_Pos, USART_CTRLA_SAMPR_16X_ARITH) |
                    whal_SetBits(USART_CTRLA_TXPO_Msk, USART_CTRLA_TXPO_Pos, cfg->txPad) |
                    whal_SetBits(USART_CTRLA_RXPO_Msk, USART_CTRLA_RXPO_Pos, cfg->rxPad) |
                    whal_SetBits(USART_CTRLA_FORM_Msk, USART_CTRLA_FORM_Pos, USART_CTRLA_FORM_USART) |
                    whal_SetBits(USART_CTRLA_CMODE_Msk, USART_CTRLA_CMODE_Pos, 0) |  /* Async mode */
                    whal_SetBits(USART_CTRLA_DORD_Msk, USART_CTRLA_DORD_Pos, 1));   /* LSB first */

    /* Configure CTRLB: 8-bit char size, 1 stop bit, no parity */
    whal_Reg_Update(reg->base, USART_CTRLB_REG,
                    USART_CTRLB_CHSIZE_Msk |
                    USART_CTRLB_SBMODE_Msk |
                    USART_CTRLB_PMODE_Msk,
                    whal_SetBits(USART_CTRLB_CHSIZE_Msk, USART_CTRLB_CHSIZE_Pos, USART_CTRLB_CHSIZE_8BIT) |
                    whal_SetBits(USART_CTRLB_SBMODE_Msk, USART_CTRLB_SBMODE_Pos, 0) |
                    whal_SetBits(USART_CTRLB_PMODE_Msk, USART_CTRLB_PMODE_Pos, 0));

    /* Wait for CTRLB sync */
    whal_Pic32czUart_WaitSync(reg, USART_SYNCBUSY_CTRLB_Msk, USART_SYNCBUSY_CTRLB_Pos);

    /* Set baud rate */
    whal_Reg_Update(reg->base, USART_BAUD_REG,
                    0xFFFF,
                    cfg->baud);

    /* Enable transmitter and receiver */
    whal_Reg_Update(reg->base, USART_CTRLB_REG,
                    USART_CTRLB_TXEN_Msk | USART_CTRLB_RXEN_Msk,
                    whal_SetBits(USART_CTRLB_TXEN_Msk, USART_CTRLB_TXEN_Pos, 1) |
                    whal_SetBits(USART_CTRLB_RXEN_Msk, USART_CTRLB_RXEN_Pos, 1));

    /* Wait for CTRLB sync */
    whal_Pic32czUart_WaitSync(reg, USART_SYNCBUSY_CTRLB_Msk, USART_SYNCBUSY_CTRLB_Pos);

    /* Enable SERCOM USART */
    whal_Reg_Update(reg->base, USART_CTRLA_REG,
                    USART_CTRLA_ENABLE_Msk,
                    whal_SetBits(USART_CTRLA_ENABLE_Msk, USART_CTRLA_ENABLE_Pos, 1));

    /* Wait for enable sync */
    whal_Pic32czUart_WaitSync(reg, USART_SYNCBUSY_ENABLE_Msk, USART_SYNCBUSY_ENABLE_Pos);

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czUart_Deinit(whal_Uart *uartDev)
{
    whal_Error err;
    const whal_Regmap *reg;
    whal_Pic32czUart_Cfg *cfg;

    if (!uartDev) {
        return WHAL_EINVAL;
    }

    reg = &uartDev->regmap;
    cfg = (whal_Pic32czUart_Cfg *)uartDev->cfg;

    /* Disable SERCOM USART */
    whal_Reg_Update(reg->base, USART_CTRLA_REG,
                    USART_CTRLA_ENABLE_Msk,
                    whal_SetBits(USART_CTRLA_ENABLE_Msk, USART_CTRLA_ENABLE_Pos, 0));

    /* Wait for disable sync */
    whal_Pic32czUart_WaitSync(reg, USART_SYNCBUSY_ENABLE_Msk, USART_SYNCBUSY_ENABLE_Pos);

    /* Disable transmitter and receiver */
    whal_Reg_Update(reg->base, USART_CTRLB_REG,
                    USART_CTRLB_TXEN_Msk | USART_CTRLB_RXEN_Msk,
                    whal_SetBits(USART_CTRLB_TXEN_Msk, USART_CTRLB_TXEN_Pos, 0) |
                    whal_SetBits(USART_CTRLB_RXEN_Msk, USART_CTRLB_RXEN_Pos, 0));

    /* Wait for CTRLB sync */
    whal_Pic32czUart_WaitSync(reg, USART_SYNCBUSY_CTRLB_Msk, USART_SYNCBUSY_CTRLB_Pos);

    /* Disable peripheral clock */
    err = whal_Clock_Disable(cfg->clkCtrl, cfg->clk);
    if (err != WHAL_SUCCESS) {
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czUart_Send(whal_Uart *uartDev, const uint8_t *data, size_t dataSz)
{
    const whal_Regmap *reg;

    if (!uartDev || !data) {
        return WHAL_EINVAL;
    }

    reg = &uartDev->regmap;

    for (size_t i = 0; i < dataSz; ++i) {
        size_t dataRegEmpty = 0;

        /* Wait for data register to be empty */
        while (!dataRegEmpty) {
            whal_Reg_Get(reg->base, USART_INTFLAG_REG,
                         USART_INTFLAG_DRE_Msk, USART_INTFLAG_DRE_Pos, &dataRegEmpty);
        }

        /* Write data to transmit register */
        whal_Reg_Update(reg->base, USART_DATA_REG,
                        USART_DATA_Msk,
                        whal_SetBits(USART_DATA_Msk, USART_DATA_Pos, data[i]));
    }

    /* Wait for transmission complete */
    {
        size_t txComplete = 0;
        while (!txComplete) {
            whal_Reg_Get(reg->base, USART_INTFLAG_REG,
                         USART_INTFLAG_TXC_Msk, USART_INTFLAG_TXC_Pos, &txComplete);
        }
        /* Clear TXC flag by writing 1 */
        whal_Reg_Update(reg->base, USART_INTFLAG_REG,
                        USART_INTFLAG_TXC_Msk,
                        whal_SetBits(USART_INTFLAG_TXC_Msk, USART_INTFLAG_TXC_Pos, 1));
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Pic32czUart_Recv(whal_Uart *uartDev, uint8_t *data, size_t dataSz)
{
    const whal_Regmap *reg;

    if (!uartDev || !data) {
        return WHAL_EINVAL;
    }

    reg = &uartDev->regmap;

    for (size_t i = 0; i < dataSz; ++i) {
        size_t dataReceived = 0;
        size_t rxData;

        /* Wait for receive complete */
        while (!dataReceived) {
            whal_Reg_Get(reg->base, USART_INTFLAG_REG,
                         USART_INTFLAG_RXC_Msk, USART_INTFLAG_RXC_Pos, &dataReceived);
        }

        /* Read received data */
        whal_Reg_Get(reg->base, USART_DATA_REG,
                     USART_DATA_Msk, USART_DATA_Pos, &rxData);

        data[i] = (uint8_t)rxData;
    }

    return WHAL_SUCCESS;
}

const whal_UartDriver whal_Pic32czUart_Driver = {
    .Init = whal_Pic32czUart_Init,
    .Deinit = whal_Pic32czUart_Deinit,
    .Send = whal_Pic32czUart_Send,
    .Recv = whal_Pic32czUart_Recv,
};
