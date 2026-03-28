#include <stdint.h>
#include <wolfHAL/rng/stm32h5_rng.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32H5 RNG Register Definitions
 *
 * The STM32H5 RNG has a 4-word output FIFO and requires a CONDRST
 * sequence to apply configuration. NIST-certified values from AN4230
 * are used for the STM32H563/573/562 family.
 */

/* Control Register */
#define RNG_CR_REG       0x00
#define RNG_CR_RNGEN_Pos 2
#define RNG_CR_RNGEN_Msk (1UL << RNG_CR_RNGEN_Pos)

#define RNG_CR_CONDRST_Pos 30
#define RNG_CR_CONDRST_Msk (1UL << RNG_CR_CONDRST_Pos)

/*
 * NIST-certified RNG_CR configuration for STM32H563/573/562 (from AN4230).
 * This value encodes CONFIG1=0x0F, CONFIG2=0x0, CONFIG3=0xE, NISTC=1,
 * CLKDIV=0, ARDIS=0, CED=0.
 */
#define RNG_CR_NIST_CFG  0x00F01E00UL

/* Status Register */
#define RNG_SR_REG      0x04
#define RNG_SR_DRDY_Pos 0
#define RNG_SR_DRDY_Msk (1UL << RNG_SR_DRDY_Pos)

#define RNG_SR_CECS_Pos 1
#define RNG_SR_CECS_Msk (1UL << RNG_SR_CECS_Pos)

#define RNG_SR_SECS_Pos 2
#define RNG_SR_SECS_Msk (1UL << RNG_SR_SECS_Pos)

/* Data Register - 32-bit random value */
#define RNG_DR_REG      0x08

/* Noise Source Control Register */
#define RNG_NSCR_REG    0x0C

/* Health Test Control Register */
#define RNG_HTCR_REG    0x10

/* NIST-certified HTCR and NSCR values for STM32H563/573/562 (from AN4230) */
#define RNG_HTCR_NIST_VAL  0x00006A91UL
#define RNG_NSCR_NIST_VAL  0x0003AF66UL

/* Magic value required to unlock HTCR writes */
#define RNG_HTCR_MAGIC     0x17590ABCUL

whal_Error whal_Stm32h5Rng_Init(whal_Rng *rngDev)
{
    whal_Stm32h5Rng_Cfg *cfg;
    const whal_Regmap *reg;
    whal_Error err;

    if (!rngDev || !rngDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Rng_Cfg *)rngDev->cfg;
    reg = &rngDev->regmap;

    /*
     * Apply NIST-certified configuration via CONDRST sequence:
     * 1. Write CONDRST=1 with configuration bits, RNGEN=0
     * 2. Write HTCR magic key then HTCR value (while CONDRST=1)
     * 3. Write NSCR value (while CONDRST=1)
     * 4. Write CONDRST=0 with RNGEN=1 to start
     */
    whal_Reg_Write(reg->base, RNG_CR_REG,
                   RNG_CR_NIST_CFG | RNG_CR_CONDRST_Msk);

    whal_Reg_Write(reg->base, RNG_HTCR_REG, RNG_HTCR_MAGIC);
    whal_Reg_Write(reg->base, RNG_HTCR_REG, RNG_HTCR_NIST_VAL);
    whal_Reg_Write(reg->base, RNG_NSCR_REG, RNG_NSCR_NIST_VAL);

    whal_Reg_Write(reg->base, RNG_CR_REG,
                   RNG_CR_NIST_CFG | RNG_CR_RNGEN_Msk);

    /* Wait for CONDRST to clear (reset complete) */
    err = whal_Reg_ReadPoll(reg->base, RNG_CR_REG,
                            RNG_CR_CONDRST_Msk, 0, cfg->timeout);

    return err;
}

whal_Error whal_Stm32h5Rng_Deinit(whal_Rng *rngDev)
{
    if (!rngDev || !rngDev->cfg)
        return WHAL_EINVAL;

    /* Disable the RNG peripheral */
    whal_Reg_Update(rngDev->regmap.base, RNG_CR_REG, RNG_CR_RNGEN_Msk,
                    whal_SetBits(RNG_CR_RNGEN_Msk, RNG_CR_RNGEN_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32h5Rng_Generate(whal_Rng *rngDev, uint8_t *rngData,
                                     size_t rngDataSz)
{
    whal_Error err = WHAL_SUCCESS;
    whal_Stm32h5Rng_Cfg *cfg;
    const whal_Regmap *reg;
    size_t sr;
    size_t offset = 0;

    if (!rngDev || !rngDev->cfg || !rngData)
        return WHAL_EINVAL;

    cfg = (whal_Stm32h5Rng_Cfg *)rngDev->cfg;
    reg = &rngDev->regmap;
#ifdef WHAL_CFG_NO_TIMEOUT
    (void)(cfg);
#endif

    while (offset < rngDataSz) {
        /* Wait for a random value to be ready */
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

        /* Read 32-bit random value */
        uint32_t rnd = (uint32_t)whal_Reg_Read(reg->base, RNG_DR_REG);

        /* Copy bytes into output buffer */
        for (size_t i = 0; i < 4 && offset < rngDataSz; i++, offset++)
            rngData[offset] = (uint8_t)(rnd >> (i * 8));
    }

exit:
    return err;
}

const whal_RngDriver whal_Stm32h5Rng_Driver = {
    .Init = whal_Stm32h5Rng_Init,
    .Deinit = whal_Stm32h5Rng_Deinit,
    .Generate = whal_Stm32h5Rng_Generate,
};
