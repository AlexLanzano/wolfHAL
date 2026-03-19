#include <stdint.h>
#include <wolfHAL/spi/stm32wb_spi.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32WB SPI Register Definitions
 *
 * The SPI peripheral provides full-duplex synchronous serial communication.
 * This driver configures it in master mode with software slave management
 * and 8-bit data frames.
 */

/* Control Register 1 - master config, clock, enable */
#define SPI_CR1_REG  0x00
#define SPI_CR1_CPHA_Pos 0                                              /* Clock phase */
#define SPI_CR1_CPHA_Msk (1UL << SPI_CR1_CPHA_Pos)

#define SPI_CR1_CPOL_Pos 1                                              /* Clock polarity */
#define SPI_CR1_CPOL_Msk (1UL << SPI_CR1_CPOL_Pos)

#define SPI_CR1_MSTR_Pos 2                                              /* Master selection */
#define SPI_CR1_MSTR_Msk (1UL << SPI_CR1_MSTR_Pos)

#define SPI_CR1_BR_Pos   3                                              /* Baud rate prescaler */
#define SPI_CR1_BR_Msk   (WHAL_BITMASK(3) << SPI_CR1_BR_Pos)

#define SPI_CR1_SPE_Pos  6                                              /* SPI enable */
#define SPI_CR1_SPE_Msk  (1UL << SPI_CR1_SPE_Pos)

#define SPI_CR1_SSI_Pos  8                                              /* Internal slave select */
#define SPI_CR1_SSI_Msk  (1UL << SPI_CR1_SSI_Pos)

#define SPI_CR1_SSM_Pos  9                                              /* Software slave management */
#define SPI_CR1_SSM_Msk  (1UL << SPI_CR1_SSM_Pos)

/* Control Register 2 - data size, FIFO threshold */
#define SPI_CR2_REG   0x04
#define SPI_CR2_DS_Pos    8                                             /* Data size (0111 = 8-bit) */
#define SPI_CR2_DS_Msk    (WHAL_BITMASK(4) << SPI_CR2_DS_Pos)

#define SPI_CR2_FRXTH_Pos 12                                           /* FIFO reception threshold */
#define SPI_CR2_FRXTH_Msk (1UL << SPI_CR2_FRXTH_Pos)

/* Status Register */
#define SPI_SR_REG  0x08
#define SPI_SR_RXNE_Pos 0                                               /* Receive buffer not empty */
#define SPI_SR_RXNE_Msk (1UL << SPI_SR_RXNE_Pos)

#define SPI_SR_TXE_Pos  1                                               /* Transmit buffer empty */
#define SPI_SR_TXE_Msk  (1UL << SPI_SR_TXE_Pos)

#define SPI_SR_BSY_Pos  7                                               /* Busy flag */
#define SPI_SR_BSY_Msk  (1UL << SPI_SR_BSY_Pos)

/* Data Register - 8/16-bit access */
#define SPI_DR_REG  0x0C
#define SPI_DR_Pos 0
#define SPI_DR_Msk (WHAL_BITMASK(8) << SPI_DR_Pos)

/* 8-bit data size value for DS field */
#define SPI_DS_8BIT 0x7

/*
 * Calculate the baud rate prescaler index for a target baud rate.
 *
 * SPI baud rate = fPCLK / (2 ^ (BR + 1))
 *   BR=0 -> /2,  BR=1 -> /4,  BR=2 -> /8, ... BR=7 -> /256
 *
 * Returns the smallest prescaler that does not exceed the target baud.
 */
static uint32_t whal_Stm32wbSpi_CalcBr(size_t pclk, uint32_t targetBaud)
{
    uint32_t br;

    for (br = 0; br < 7; br++) {
        if ((pclk / (2u << br)) <= targetBaud) {
            return br;
        }
    }

    return 7;
}

/*
 * Configure SPI mode and baud rate, then enable the peripheral.
 * Must be called with SPE disabled.
 */
static void whal_Stm32wbSpi_ApplyComCfg(const whal_Regmap *reg,
                                        whal_Stm32wbSpi_Cfg *cfg,
                                        whal_Stm32wbSpi_ComCfg *comCfg)
{
    uint32_t cpol, cpha, br;

    br = whal_Stm32wbSpi_CalcBr(cfg->pclk, comCfg->baud);

    cpol = (comCfg->mode >> 1) & 1;
    cpha = comCfg->mode & 1;

    /* Disable SPE before reconfiguring */
    whal_Reg_Update(reg->base, SPI_CR1_REG, SPI_CR1_SPE_Msk,
                    whal_SetBits(SPI_CR1_SPE_Msk, SPI_CR1_SPE_Pos, 0));

    /* Set mode and baud rate */
    whal_Reg_Update(reg->base, SPI_CR1_REG,
                    SPI_CR1_CPOL_Msk | SPI_CR1_CPHA_Msk | SPI_CR1_BR_Msk,
                    whal_SetBits(SPI_CR1_CPOL_Msk, SPI_CR1_CPOL_Pos, cpol) |
                    whal_SetBits(SPI_CR1_CPHA_Msk, SPI_CR1_CPHA_Pos, cpha) |
                    whal_SetBits(SPI_CR1_BR_Msk, SPI_CR1_BR_Pos, br));

    /* Re-enable SPE */
    whal_Reg_Update(reg->base, SPI_CR1_REG, SPI_CR1_SPE_Msk,
                    whal_SetBits(SPI_CR1_SPE_Msk, SPI_CR1_SPE_Pos, 1));
}

whal_Error whal_Stm32wbSpi_Init(whal_Spi *spiDev)
{
    const whal_Regmap *reg;

    if (!spiDev || !spiDev->cfg) {
        return WHAL_EINVAL;
    }

    reg = &spiDev->regmap;

    /* Master mode with software slave management */
    whal_Reg_Update(reg->base, SPI_CR1_REG,
                    SPI_CR1_MSTR_Msk | SPI_CR1_SSM_Msk | SPI_CR1_SSI_Msk,
                    whal_SetBits(SPI_CR1_MSTR_Msk, SPI_CR1_MSTR_Pos, 1) |
                    whal_SetBits(SPI_CR1_SSM_Msk, SPI_CR1_SSM_Pos, 1) |
                    whal_SetBits(SPI_CR1_SSI_Msk, SPI_CR1_SSI_Pos, 1));

    /* 8-bit data size and FIFO receive threshold for 8-bit */
    whal_Reg_Update(reg->base, SPI_CR2_REG,
                    SPI_CR2_DS_Msk | SPI_CR2_FRXTH_Msk,
                    whal_SetBits(SPI_CR2_DS_Msk, SPI_CR2_DS_Pos, SPI_DS_8BIT) |
                    whal_SetBits(SPI_CR2_FRXTH_Msk, SPI_CR2_FRXTH_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbSpi_Deinit(whal_Spi *spiDev)
{
    const whal_Regmap *reg;

    if (!spiDev || !spiDev->cfg) {
        return WHAL_EINVAL;
    }

    reg = &spiDev->regmap;

    /* Disable SPI */
    whal_Reg_Update(reg->base, SPI_CR1_REG, SPI_CR1_SPE_Msk,
                    whal_SetBits(SPI_CR1_SPE_Msk, SPI_CR1_SPE_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbSpi_SendRecv(whal_Spi *spiDev, void *spiComCfg, const uint8_t *tx,
                                    size_t txLen, uint8_t *rx, size_t rxLen)
{
    const whal_Regmap *reg;
    whal_Stm32wbSpi_Cfg *cfg;
    whal_Stm32wbSpi_ComCfg *comCfg;

    if (!spiDev || !spiDev->cfg || !spiComCfg) {
        return WHAL_EINVAL;
    }

    reg = &spiDev->regmap;
    cfg = (whal_Stm32wbSpi_Cfg *)spiDev->cfg;
    comCfg = (whal_Stm32wbSpi_ComCfg *)spiComCfg;
    size_t totalLen = txLen > rxLen ? txLen : rxLen;
    size_t d;

    whal_Stm32wbSpi_ApplyComCfg(reg, cfg, comCfg);

    for (size_t i = 0; i < totalLen; i++) {
        if (txLen && tx) {
            /* Wait for TX buffer empty */
            whal_Error txErr = whal_Reg_ReadPoll(reg->base, SPI_SR_REG,
                                                  SPI_SR_TXE_Msk,
                                                  SPI_SR_TXE_Msk,
                                                  cfg->timeout);
            if (txErr)
                return txErr;

            /* Write data or dummy byte */
            uint8_t txByte = (i >= txLen) ? tx[txLen-1] : tx[i];
            *(volatile uint8_t *)(reg->base + SPI_DR_REG) = txByte;
        }

        if (rxLen && rx) {
            /* Wait for RX buffer not empty */
            whal_Error rxErr = whal_Reg_ReadPoll(reg->base, SPI_SR_REG,
                                                  SPI_SR_RXNE_Msk,
                                                  SPI_SR_RXNE_Msk,
                                                  cfg->timeout);
            if (rxErr)
                return rxErr;

            /* Read received byte */
            d = *(volatile uint8_t *)(reg->base + SPI_DR_REG);
            if (i < rxLen) {
                rx[i] = (uint8_t)d;
            }
        }
    }

    /* Wait for not busy */
    return whal_Reg_ReadPoll(reg->base, SPI_SR_REG, SPI_SR_BSY_Msk, 0,
                             cfg->timeout);
}

whal_Error whal_Stm32wbSpi_Send(whal_Spi *spiDev, void *spiComCfg, const uint8_t *data,
                                size_t dataSz)
{
    return whal_Stm32wbSpi_SendRecv(spiDev, spiComCfg, data, dataSz, NULL, 0);
}

whal_Error whal_Stm32wbSpi_Recv(whal_Spi *spiDev, void *spiComCfg, uint8_t *data,
                                size_t dataSz)
{
    uint8_t dummyByte = 0xFF;
    return whal_Stm32wbSpi_SendRecv(spiDev, spiComCfg, &dummyByte, 1, data, dataSz);
}

const whal_SpiDriver whal_Stm32wbSpi_Driver = {
    .Init = whal_Stm32wbSpi_Init,
    .Deinit = whal_Stm32wbSpi_Deinit,
    .SendRecv = whal_Stm32wbSpi_SendRecv,
    .Send = whal_Stm32wbSpi_Send,
    .Recv = whal_Stm32wbSpi_Recv,
};
