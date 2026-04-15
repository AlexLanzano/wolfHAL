#include <stdint.h>
#include <wolfHAL/rng/stm32wba_rng.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32WBA RNG Register Definitions (RM0493 section 24.7)
 */

/* Control Register (offset 0x000, reset 0x0080 0D00) */
#define RNG_CR_REG            0x00
#define RNG_CR_RNGEN_Pos      2
#define RNG_CR_RNGEN_Msk      (1UL << RNG_CR_RNGEN_Pos)
#define RNG_CR_IE_Pos         3
#define RNG_CR_IE_Msk         (1UL << RNG_CR_IE_Pos)
#define RNG_CR_CED_Pos        5
#define RNG_CR_CED_Msk        (1UL << RNG_CR_CED_Pos)
#define RNG_CR_ARDIS_Pos      7
#define RNG_CR_ARDIS_Msk      (1UL << RNG_CR_ARDIS_Pos)
#define RNG_CR_RNG_CONFIG3_Pos 8
#define RNG_CR_RNG_CONFIG3_Msk (0xFUL << RNG_CR_RNG_CONFIG3_Pos)
#define RNG_CR_NISTC_Pos      12
#define RNG_CR_NISTC_Msk      (1UL << RNG_CR_NISTC_Pos)
#define RNG_CR_RNG_CONFIG2_Pos 13
#define RNG_CR_RNG_CONFIG2_Msk (7UL << RNG_CR_RNG_CONFIG2_Pos)
#define RNG_CR_CLKDIV_Pos     16
#define RNG_CR_CLKDIV_Msk     (0xFUL << RNG_CR_CLKDIV_Pos)
#define RNG_CR_RNG_CONFIG1_Pos 20
#define RNG_CR_RNG_CONFIG1_Msk (0x3FUL << RNG_CR_RNG_CONFIG1_Pos)
#define RNG_CR_CONDRST_Pos    30
#define RNG_CR_CONDRST_Msk    (1UL << RNG_CR_CONDRST_Pos)
#define RNG_CR_CONFIGLOCK_Pos 31
#define RNG_CR_CONFIGLOCK_Msk (1UL << RNG_CR_CONFIGLOCK_Pos)

/* Status Register (offset 0x004) */
#define RNG_SR_REG            0x04
#define RNG_SR_DRDY_Pos       0
#define RNG_SR_DRDY_Msk       (1UL << RNG_SR_DRDY_Pos)
#define RNG_SR_CECS_Pos       1
#define RNG_SR_CECS_Msk       (1UL << RNG_SR_CECS_Pos)
#define RNG_SR_SECS_Pos       2
#define RNG_SR_SECS_Msk       (1UL << RNG_SR_SECS_Pos)
#define RNG_SR_CEIS_Pos       5
#define RNG_SR_CEIS_Msk       (1UL << RNG_SR_CEIS_Pos)
#define RNG_SR_SEIS_Pos       6
#define RNG_SR_SEIS_Msk       (1UL << RNG_SR_SEIS_Pos)

/* Data Register (offset 0x008) */
#define RNG_DR_REG            0x08

/*
 * Configuration C values (RM0493 Table 178):
 *   NISTC=0, RNG_CONFIG1=0x0F, CLKDIV=0x0, RNG_CONFIG2=0x0,
 *   RNG_CONFIG3=0xD, CED=0, N=2
 */
#define RNG_CR_CONFIG_C  (whal_SetBits(RNG_CR_RNG_CONFIG1_Msk, RNG_CR_RNG_CONFIG1_Pos, 0x0F) | \
                          whal_SetBits(RNG_CR_RNG_CONFIG3_Msk, RNG_CR_RNG_CONFIG3_Pos, 0x0D))

whal_Error whal_Stm32wbaRng_Init(whal_Rng *rngDev)
{
    const whal_Stm32wbaRng_Cfg *cfg;
    const whal_Regmap *reg;

    if (!rngDev || !rngDev->cfg)
        return WHAL_EINVAL;

    cfg = (const whal_Stm32wbaRng_Cfg *)rngDev->cfg;
    reg = &rngDev->regmap;

    /* Apply Configuration C with CONDRST=1 and RNGEN=1 */
    whal_Reg_Write(reg->base, RNG_CR_REG,
                   RNG_CR_CONDRST_Msk | RNG_CR_CONFIG_C | RNG_CR_RNGEN_Msk);

    /* Clear CONDRST to start conditioning */
    whal_Reg_Write(reg->base, RNG_CR_REG,
                   RNG_CR_CONFIG_C | RNG_CR_RNGEN_Msk);

    /* Wait for CONDRST to self-clear */
    return whal_Reg_ReadPoll(reg->base, RNG_CR_REG, RNG_CR_CONDRST_Msk, 0,
                             cfg->timeout);
}

whal_Error whal_Stm32wbaRng_Deinit(whal_Rng *rngDev)
{
    if (!rngDev || !rngDev->cfg)
        return WHAL_EINVAL;

    /* Disable RNG */
    whal_Reg_Update(rngDev->regmap.base, RNG_CR_REG, RNG_CR_RNGEN_Msk, 0);

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbaRng_Generate(whal_Rng *rngDev, void *rngData, size_t rngDataSz)
{
    uint8_t *rngBuf = (uint8_t *)rngData;
    whal_Error err = WHAL_SUCCESS;
    whal_Stm32wbaRng_Cfg *cfg;
    const whal_Regmap *reg;
    size_t sr;
    size_t offset = 0;

    if (!rngDev || !rngDev->cfg || !rngData)
        return WHAL_EINVAL;

    cfg = (whal_Stm32wbaRng_Cfg *)rngDev->cfg;
    reg = &rngDev->regmap;
#ifdef WHAL_CFG_NO_TIMEOUT
    (void)(cfg);
#endif

    while (offset < rngDataSz) {
        WHAL_TIMEOUT_START(cfg->timeout);
        while (1) {
            if (WHAL_TIMEOUT_EXPIRED(cfg->timeout)) {
                err = WHAL_ETIMEOUT;
                goto exit;
            }

            sr = whal_Reg_Read(reg->base, RNG_SR_REG);

            if (sr & RNG_SR_SECS_Msk) {
                err = WHAL_EHARDWARE;
                goto exit;
            }
            if (sr & RNG_SR_CECS_Msk) {
                err = WHAL_EHARDWARE;
                goto exit;
            }

            if (sr & RNG_SR_DRDY_Msk)
                break;
        }

        uint32_t rnd = *(volatile uint32_t *)(reg->base + RNG_DR_REG);

        for (size_t i = 0; i < 4 && offset < rngDataSz; i++, offset++)
            rngBuf[offset] = (uint8_t)(rnd >> (i * 8));
    }

exit:
    return err;
}

const whal_RngDriver whal_Stm32wbaRng_Driver = {
    .Init = whal_Stm32wbaRng_Init,
    .Deinit = whal_Stm32wbaRng_Deinit,
    .Generate = whal_Stm32wbaRng_Generate,
};
