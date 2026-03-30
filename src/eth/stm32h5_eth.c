#include <stdint.h>
#include <wolfHAL/eth/stm32h5_eth.h>
#include <wolfHAL/eth/eth.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32H5 Ethernet MAC Register Definitions
 *
 * The peripheral has three register blocks at offsets from the ETH base:
 *   MAC: 0x0000   MTL: 0x0C00   DMA: 0x1000
 */

/* --- MAC registers --- */
#define ETH_MACCR_REG       0x0000
#define ETH_MACCR_RE_Pos    0
#define ETH_MACCR_RE_Msk    (1UL << ETH_MACCR_RE_Pos)
#define ETH_MACCR_TE_Pos    1
#define ETH_MACCR_TE_Msk    (1UL << ETH_MACCR_TE_Pos)
#define ETH_MACCR_DM_Pos    13
#define ETH_MACCR_DM_Msk    (1UL << ETH_MACCR_DM_Pos)
#define ETH_MACCR_FES_Pos   14
#define ETH_MACCR_FES_Msk   (1UL << ETH_MACCR_FES_Pos)

#define ETH_MACPFR_REG      0x0008

#define ETH_MACA0HR_REG     0x0300
#define ETH_MACA0LR_REG     0x0304

#define ETH_MACMDIOAR_REG   0x0200
#define ETH_MACMDIOAR_MB_Pos  0
#define ETH_MACMDIOAR_MB_Msk  (1UL << ETH_MACMDIOAR_MB_Pos)
#define ETH_MACMDIOAR_GOC_Pos 2
#define ETH_MACMDIOAR_GOC_Msk (3UL << ETH_MACMDIOAR_GOC_Pos)
#define ETH_MACMDIOAR_CR_Pos  8
#define ETH_MACMDIOAR_CR_Msk  (0xFUL << ETH_MACMDIOAR_CR_Pos)
#define ETH_MACMDIOAR_RDA_Pos 16
#define ETH_MACMDIOAR_RDA_Msk (0x1FUL << ETH_MACMDIOAR_RDA_Pos)
#define ETH_MACMDIOAR_PA_Pos  21
#define ETH_MACMDIOAR_PA_Msk  (0x1FUL << ETH_MACMDIOAR_PA_Pos)

#define ETH_MACMDIODR_REG   0x0204
#define ETH_MACMDIODR_MD_Msk 0xFFFFUL

#define ETH_MDIO_GOC_WRITE  0x1
#define ETH_MDIO_GOC_READ   0x3

/* --- MTL registers --- */
#define ETH_MTLTXQOMR_REG   0x0D00
#define ETH_MTLTXQOMR_TSF_Pos  1
#define ETH_MTLTXQOMR_TSF_Msk  (1UL << ETH_MTLTXQOMR_TSF_Pos)
#define ETH_MTLTXQOMR_TXQEN_Pos 2
#define ETH_MTLTXQOMR_TXQEN_Msk (3UL << ETH_MTLTXQOMR_TXQEN_Pos)

#define ETH_MTLRXQOMR_REG   0x0D30
#define ETH_MTLRXQOMR_FUP_Pos  3
#define ETH_MTLRXQOMR_FUP_Msk  (1UL << ETH_MTLRXQOMR_FUP_Pos)
#define ETH_MTLRXQOMR_FEP_Pos  4
#define ETH_MTLRXQOMR_FEP_Msk  (1UL << ETH_MTLRXQOMR_FEP_Pos)
#define ETH_MTLRXQOMR_RSF_Pos  5
#define ETH_MTLRXQOMR_RSF_Msk  (1UL << ETH_MTLRXQOMR_RSF_Pos)

/* --- DMA registers --- */
#define ETH_DMAMR_REG        0x1000
#define ETH_DMAMR_SWR_Pos    0
#define ETH_DMAMR_SWR_Msk    (1UL << ETH_DMAMR_SWR_Pos)

#define ETH_DMASBMR_REG     0x1004
#define ETH_DMASBMR_FB_Pos  0
#define ETH_DMASBMR_FB_Msk  (1UL << ETH_DMASBMR_FB_Pos)
#define ETH_DMASBMR_AAL_Pos 12
#define ETH_DMASBMR_AAL_Msk (1UL << ETH_DMASBMR_AAL_Pos)

#define ETH_DMACCR_REG       0x1100

#define ETH_DMACTXCR_REG     0x1104
#define ETH_DMACTXCR_ST_Pos  0
#define ETH_DMACTXCR_ST_Msk  (1UL << ETH_DMACTXCR_ST_Pos)
#define ETH_DMACTXCR_TXPBL_Pos 16
#define ETH_DMACTXCR_TXPBL_Msk (0x3FUL << ETH_DMACTXCR_TXPBL_Pos)

#define ETH_DMACRXCR_REG     0x1108
#define ETH_DMACRXCR_SR_Pos  0
#define ETH_DMACRXCR_SR_Msk  (1UL << ETH_DMACRXCR_SR_Pos)
#define ETH_DMACRXCR_RBSZ_Pos 1
#define ETH_DMACRXCR_RBSZ_Msk (0x3FFFUL << ETH_DMACRXCR_RBSZ_Pos)
#define ETH_DMACRXCR_RXPBL_Pos 16
#define ETH_DMACRXCR_RXPBL_Msk (0x3FUL << ETH_DMACRXCR_RXPBL_Pos)

#define ETH_DMACTXDLAR_REG   0x1114
#define ETH_DMACRXDLAR_REG   0x111C
#define ETH_DMACTXDTPR_REG   0x1120
#define ETH_DMACRXDTPR_REG   0x1128
#define ETH_DMACTXRLR_REG    0x112C
#define ETH_DMACRXRLR_REG    0x1130

#define ETH_DMACSR_REG       0x1160
#define ETH_DMACSR_TI_Msk    (1UL << 0)
#define ETH_DMACSR_RI_Msk    (1UL << 6)

/* --- TX descriptor bits (TDES3) --- */
#define TDES3_OWN  (1UL << 31)
#define TDES3_FD   (1UL << 29)
#define TDES3_LD   (1UL << 28)

/* TX descriptor bits (TDES2) */
#define TDES2_IOC  (1UL << 31)

/* --- RX descriptor bits (RDES3) --- */
#define RDES3_OWN   (1UL << 31)
#define RDES3_IOC   (1UL << 30)
#define RDES3_BUF1V (1UL << 24)
#define RDES3_ES    (1UL << 15)
#define RDES3_PL_Msk 0x7FFFUL

/* Default burst length */
#define ETH_PBL    32

/* Max TX frame size */
#define ETH_MAX_FRAME_SIZE 1536

/* MDIO clock range for 168 MHz AHB: CR=4 gives /102 ≈ 1.6 MHz */
#define ETH_MDIO_CR 4

static whal_Error MdioPoll(size_t base, whal_Timeout *timeout)
{
    return whal_Reg_ReadPoll(base, ETH_MACMDIOAR_REG,
                             ETH_MACMDIOAR_MB_Msk, 0, timeout);
}

whal_Error whal_Stm32h5Eth_Init(whal_Eth *ethDev)
{
    whal_Stm32h5Eth_Cfg *cfg;
    size_t base;
    whal_Error err;

    if (!ethDev || !ethDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Eth_Cfg *)ethDev->cfg;
    base = ethDev->regmap.base;

    if (!cfg->txDescs || !cfg->txBufs || cfg->txDescCount == 0 ||
        !cfg->rxDescs || !cfg->rxBufs || cfg->rxDescCount == 0 ||
        cfg->txBufSize == 0 || cfg->rxBufSize == 0)
        return WHAL_EINVAL;

    /* DMA software reset */
    whal_Reg_Update(base, ETH_DMAMR_REG, ETH_DMAMR_SWR_Msk,
                    ETH_DMAMR_SWR_Msk);
    err = whal_Reg_ReadPoll(base, ETH_DMAMR_REG, ETH_DMAMR_SWR_Msk, 0,
                            cfg->timeout);
    if (err)
        return err;

    /* DMA bus mode: fixed burst, address-aligned */
    whal_Reg_Write(base, ETH_DMASBMR_REG,
                   ETH_DMASBMR_FB_Msk | ETH_DMASBMR_AAL_Msk);

    /* DMA channel: contiguous descriptors (DSL=0) */
    whal_Reg_Write(base, ETH_DMACCR_REG, 0);

    /* DMA TX: burst length, not started yet */
    whal_Reg_Write(base, ETH_DMACTXCR_REG,
                   whal_SetBits(ETH_DMACTXCR_TXPBL_Msk,
                                ETH_DMACTXCR_TXPBL_Pos, ETH_PBL));

    /* DMA RX: burst length, buffer size, not started yet */
    whal_Reg_Write(base, ETH_DMACRXCR_REG,
                   whal_SetBits(ETH_DMACRXCR_RXPBL_Msk,
                                ETH_DMACRXCR_RXPBL_Pos, ETH_PBL) |
                   whal_SetBits(ETH_DMACRXCR_RBSZ_Msk,
                                ETH_DMACRXCR_RBSZ_Pos, cfg->rxBufSize));

    /* Set up TX descriptor ring */
    for (size_t i = 0; i < cfg->txDescCount; i++) {
        cfg->txDescs[i].des[0] = 0;
        cfg->txDescs[i].des[1] = 0;
        cfg->txDescs[i].des[2] = 0;
        cfg->txDescs[i].des[3] = 0;
    }

    /* Set up RX descriptor ring with pre-allocated buffers */
    for (size_t i = 0; i < cfg->rxDescCount; i++) {
        cfg->rxDescs[i].des[0] = (uintptr_t)(cfg->rxBufs + i * cfg->rxBufSize);
        cfg->rxDescs[i].des[1] = 0;
        cfg->rxDescs[i].des[2] = 0;
        cfg->rxDescs[i].des[3] = RDES3_OWN | RDES3_IOC | RDES3_BUF1V;
    }

    /* Program descriptor ring addresses and lengths */
    whal_Reg_Write(base, ETH_DMACTXDLAR_REG, (uintptr_t)cfg->txDescs);
    whal_Reg_Write(base, ETH_DMACRXDLAR_REG, (uintptr_t)cfg->rxDescs);
    whal_Reg_Write(base, ETH_DMACTXRLR_REG, cfg->txDescCount - 1);
    whal_Reg_Write(base, ETH_DMACRXRLR_REG, cfg->rxDescCount - 1);

    /* MTL: store-and-forward for TX and RX, enable TX queue */
    whal_Reg_Update(base, ETH_MTLTXQOMR_REG,
                    ETH_MTLTXQOMR_TSF_Msk | ETH_MTLTXQOMR_TXQEN_Msk,
                    ETH_MTLTXQOMR_TSF_Msk |
                    whal_SetBits(ETH_MTLTXQOMR_TXQEN_Msk,
                                 ETH_MTLTXQOMR_TXQEN_Pos, 2));
    whal_Reg_Update(base, ETH_MTLRXQOMR_REG,
                    ETH_MTLRXQOMR_RSF_Msk, ETH_MTLRXQOMR_RSF_Msk);

    /* MAC address */
    whal_Reg_Write(base, ETH_MACA0LR_REG,
                   ((uint32_t)cfg->macAddr[3] << 24) |
                   ((uint32_t)cfg->macAddr[2] << 16) |
                   ((uint32_t)cfg->macAddr[1] << 8) |
                   ((uint32_t)cfg->macAddr[0]));
    whal_Reg_Write(base, ETH_MACA0HR_REG,
                   ((uint32_t)cfg->macAddr[5] << 8) |
                   ((uint32_t)cfg->macAddr[4]));

    /* Reset ring tracking state */
    cfg->txHead = 0;
    cfg->rxHead = 0;

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Eth_Deinit(whal_Eth *ethDev)
{
    if (!ethDev)
        return WHAL_EINVAL;

    whal_Reg_Update(ethDev->regmap.base, ETH_DMAMR_REG,
                    ETH_DMAMR_SWR_Msk, ETH_DMAMR_SWR_Msk);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Eth_Start(whal_Eth *ethDev, uint8_t speed,
                                  uint8_t duplex)
{
    whal_Stm32h5Eth_Cfg *cfg;
    size_t base;

    if (!ethDev || !ethDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Eth_Cfg *)ethDev->cfg;
    base = ethDev->regmap.base;

    /* Configure MAC speed and duplex to match PHY */
    whal_Reg_Update(base, ETH_MACCR_REG,
                    ETH_MACCR_FES_Msk | ETH_MACCR_DM_Msk,
                    ((speed == 100) ? ETH_MACCR_FES_Msk : 0) |
                    (duplex ? ETH_MACCR_DM_Msk : 0));

    /* Enable MAC TX and RX */
    whal_Reg_Update(base, ETH_MACCR_REG,
                    ETH_MACCR_TE_Msk | ETH_MACCR_RE_Msk,
                    ETH_MACCR_TE_Msk | ETH_MACCR_RE_Msk);

    /* Start DMA TX */
    whal_Reg_Update(base, ETH_DMACTXCR_REG,
                    ETH_DMACTXCR_ST_Msk, ETH_DMACTXCR_ST_Msk);

    /* Start DMA RX */
    whal_Reg_Update(base, ETH_DMACRXCR_REG,
                    ETH_DMACRXCR_SR_Msk, ETH_DMACRXCR_SR_Msk);

    /* Kick RX DMA by writing tail pointer past last descriptor */
    whal_Reg_Write(base, ETH_DMACRXDTPR_REG,
                   (uintptr_t)&cfg->rxDescs[cfg->rxDescCount]);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Eth_Stop(whal_Eth *ethDev)
{
    size_t base;

    if (!ethDev)
        return WHAL_EINVAL;

    base = ethDev->regmap.base;

    /* Stop DMA TX */
    whal_Reg_Update(base, ETH_DMACTXCR_REG, ETH_DMACTXCR_ST_Msk, 0);

    /* Stop DMA RX */
    whal_Reg_Update(base, ETH_DMACRXCR_REG, ETH_DMACRXCR_SR_Msk, 0);

    /* Disable MAC TX and RX */
    whal_Reg_Update(base, ETH_MACCR_REG,
                    ETH_MACCR_TE_Msk | ETH_MACCR_RE_Msk, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Eth_Send(whal_Eth *ethDev, const uint8_t *frame,
                                 size_t len)
{
    whal_Stm32h5Eth_Cfg *cfg;
    whal_Stm32h5Eth_TxDesc *desc;
    size_t base;
    size_t idx;

    if (!ethDev || !ethDev->cfg || !frame || len == 0)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Eth_Cfg *)ethDev->cfg;

    if (len > cfg->txBufSize)
        return WHAL_EINVAL;
    base = ethDev->regmap.base;
    idx = cfg->txHead;
    desc = &cfg->txDescs[idx];

    /* Check if descriptor is available (OWN must be 0) */
    if (desc->des[3] & TDES3_OWN)
        return WHAL_ENOTREADY;

    /* Copy frame into TX buffer */
    uint8_t *txBuf = cfg->txBufs + idx * cfg->txBufSize;
    for (size_t i = 0; i < len; i++)
        txBuf[i] = frame[i];

    /* Set up descriptor */
    desc->des[0] = (uintptr_t)txBuf;
    desc->des[1] = 0;
    desc->des[2] = (len & 0x3FFF) | TDES2_IOC;
    desc->des[3] = TDES3_OWN | TDES3_FD | TDES3_LD | (len & 0x7FFF);

    /* Advance ring position */
    cfg->txHead = (idx + 1) % cfg->txDescCount;

    /* Kick DMA — tail pointer past the end of the ring */
    whal_Reg_Write(base, ETH_DMACTXDTPR_REG,
                   (uintptr_t)&cfg->txDescs[cfg->txDescCount]);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Eth_Recv(whal_Eth *ethDev, uint8_t *frame,
                                 size_t *len)
{
    whal_Stm32h5Eth_Cfg *cfg;
    whal_Stm32h5Eth_RxDesc *desc;
    size_t base;
    size_t idx;
    uint32_t rdes3;
    size_t pktLen;

    if (!ethDev || !ethDev->cfg || !frame || !len)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Eth_Cfg *)ethDev->cfg;
    base = ethDev->regmap.base;
    idx = cfg->rxHead;
    desc = &cfg->rxDescs[idx];

    rdes3 = desc->des[3];

    /* Check if DMA has released this descriptor */
    if (rdes3 & RDES3_OWN)
        return WHAL_ENOTREADY;

    /* Check for errors */
    if (rdes3 & RDES3_ES) {
        desc->des[0] = (uintptr_t)(cfg->rxBufs + idx * cfg->rxBufSize);
        desc->des[3] = RDES3_OWN | RDES3_IOC | RDES3_BUF1V;
        cfg->rxHead = (idx + 1) % cfg->rxDescCount;
        whal_Reg_Write(base, ETH_DMACRXDTPR_REG,
                       (uintptr_t)&cfg->rxDescs[cfg->rxDescCount]);
        return WHAL_EHARDWARE;
    }

    /* Extract packet length (includes CRC) */
    pktLen = rdes3 & RDES3_PL_Msk;
    if (pktLen > *len)
        pktLen = *len;

    /* Copy frame data */
    uint8_t *rxBuf = (uint8_t *)(cfg->rxBufs + idx * cfg->rxBufSize);
    for (size_t i = 0; i < pktLen; i++)
        frame[i] = rxBuf[i];
    *len = pktLen;

    /* Re-arm descriptor for DMA */
    desc->des[0] = (uintptr_t)rxBuf;
    desc->des[1] = 0;
    desc->des[2] = 0;
    desc->des[3] = RDES3_OWN | RDES3_IOC | RDES3_BUF1V;

    /* Advance ring position */
    cfg->rxHead = (idx + 1) % cfg->rxDescCount;

    /* Update RX tail pointer — always past end of ring */
    whal_Reg_Write(base, ETH_DMACRXDTPR_REG,
                   (uintptr_t)&cfg->rxDescs[cfg->rxDescCount]);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Eth_MdioRead(whal_Eth *ethDev, uint8_t phyAddr,
                                      uint8_t reg, uint16_t *val)
{
    whal_Stm32h5Eth_Cfg *cfg;
    size_t base;
    whal_Error err;

    if (!ethDev || !ethDev->cfg || !val)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Eth_Cfg *)ethDev->cfg;
    base = ethDev->regmap.base;

    err = MdioPoll(base, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Write(base, ETH_MACMDIOAR_REG,
                   whal_SetBits(ETH_MACMDIOAR_PA_Msk, ETH_MACMDIOAR_PA_Pos,
                                phyAddr) |
                   whal_SetBits(ETH_MACMDIOAR_RDA_Msk, ETH_MACMDIOAR_RDA_Pos,
                                reg) |
                   whal_SetBits(ETH_MACMDIOAR_CR_Msk, ETH_MACMDIOAR_CR_Pos,
                                ETH_MDIO_CR) |
                   whal_SetBits(ETH_MACMDIOAR_GOC_Msk, ETH_MACMDIOAR_GOC_Pos,
                                ETH_MDIO_GOC_READ) |
                   ETH_MACMDIOAR_MB_Msk);

    err = MdioPoll(base, cfg->timeout);
    if (err)
        return err;

    *val = (uint16_t)(whal_Reg_Read(base, ETH_MACMDIODR_REG) &
                      ETH_MACMDIODR_MD_Msk);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Eth_MdioWrite(whal_Eth *ethDev, uint8_t phyAddr,
                                       uint8_t reg, uint16_t val)
{
    whal_Stm32h5Eth_Cfg *cfg;
    size_t base;
    whal_Error err;

    if (!ethDev || !ethDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Eth_Cfg *)ethDev->cfg;
    base = ethDev->regmap.base;

    err = MdioPoll(base, cfg->timeout);
    if (err)
        return err;

    whal_Reg_Write(base, ETH_MACMDIODR_REG, val);

    whal_Reg_Write(base, ETH_MACMDIOAR_REG,
                   whal_SetBits(ETH_MACMDIOAR_PA_Msk, ETH_MACMDIOAR_PA_Pos,
                                phyAddr) |
                   whal_SetBits(ETH_MACMDIOAR_RDA_Msk, ETH_MACMDIOAR_RDA_Pos,
                                reg) |
                   whal_SetBits(ETH_MACMDIOAR_CR_Msk, ETH_MACMDIOAR_CR_Pos,
                                ETH_MDIO_CR) |
                   whal_SetBits(ETH_MACMDIOAR_GOC_Msk, ETH_MACMDIOAR_GOC_Pos,
                                ETH_MDIO_GOC_WRITE) |
                   ETH_MACMDIOAR_MB_Msk);

    err = MdioPoll(base, cfg->timeout);
    if (err)
        return err;

    return WHAL_SUCCESS;
}

#define ETH_MACCR_LM_Pos    12
#define ETH_MACCR_LM_Msk    (1UL << ETH_MACCR_LM_Pos)

whal_Error whal_Stm32h5Eth_Ext_EnableLoopback(whal_Eth *ethDev,
                                                uint8_t enable)
{
    if (!ethDev)
        return WHAL_EINVAL;

    whal_Reg_Update(ethDev->regmap.base, ETH_MACCR_REG, ETH_MACCR_LM_Msk,
                    whal_SetBits(ETH_MACCR_LM_Msk, ETH_MACCR_LM_Pos,
                                 enable ? 1 : 0));

    return WHAL_SUCCESS;
}

const whal_EthDriver whal_Stm32h5Eth_Driver = {
    .Init = whal_Stm32h5Eth_Init,
    .Deinit = whal_Stm32h5Eth_Deinit,
    .Start = whal_Stm32h5Eth_Start,
    .Stop = whal_Stm32h5Eth_Stop,
    .Send = whal_Stm32h5Eth_Send,
    .Recv = whal_Stm32h5Eth_Recv,
    .MdioRead = whal_Stm32h5Eth_MdioRead,
    .MdioWrite = whal_Stm32h5Eth_MdioWrite,
};
