#include <stdint.h>
#include <wolfHAL/rng/stm32n6_rng.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32N6 RNG Register Definitions (from RM0486 Section 47.7)
 *
 * The STM32N6 RNG has the same register offsets as the STM32H5 RNG:
 *   CR(0x00), SR(0x04), DR(0x08), NSCR(0x0C), HTCR(0x10)
 *
 * However, the bit fields in RNG_CR differ and the NIST configuration
 * values are device-specific. Configuration B is used here:
 *   NISTC=1, CONFIG1=0x18, CLKDIV=0, CONFIG2=0x0, CONFIG3=0x0, N=1
 */

/* Control Register */
#define RNG_CR_REG       0x00
#define RNG_CR_RNGEN_Pos 2
#define RNG_CR_RNGEN_Msk (1UL << RNG_CR_RNGEN_Pos)

#define RNG_CR_CONDRST_Pos 30
#define RNG_CR_CONDRST_Msk (1UL << RNG_CR_CONDRST_Pos)

/*
 * Configuration B from RM0486 Table 398:
 *   NISTC = 1 (bit 12)
 *   RNG_CONFIG1[7:0] = 0x18 (bits 27:20)
 *   CLKDIV[3:0] = 0x0 (bits 19:16) -- must be 32 MHz or less
 *   RNG_CONFIG2[2:0] = 0x0 (bits 15:13)
 *   RNG_CONFIG3[3:0] = 0x0 (bits 11:8)
 *   HTCR = default (0x000072AC)
 *   NSCR = default (0x0003FFFF)
 */
#define RNG_CR_CFG_B     0x01801000UL

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

whal_Error whal_Stm32n6Rng_Init(whal_Rng *rngDev)
{
    whal_Stm32n6Rng_Cfg *cfg;
    const whal_Regmap *reg;
    whal_Error err;

    if (!rngDev || !rngDev->cfg)
        return WHAL_EINVAL;

    cfg = (whal_Stm32n6Rng_Cfg *)rngDev->cfg;
    reg = &rngDev->regmap;

    /*
     * Apply configuration via CONDRST sequence:
     * 1. Write CONDRST=1 with configuration bits, RNGEN=0
     * 2. Write CONDRST=0 with RNGEN=1 to start
     * 3. Wait for CONDRST to clear
     */
    whal_Reg_Write(reg->base, RNG_CR_REG,
                   RNG_CR_CFG_B | RNG_CR_CONDRST_Msk);

    whal_Reg_Write(reg->base, RNG_CR_REG,
                   RNG_CR_CFG_B | RNG_CR_RNGEN_Msk);

    /* Wait for CONDRST to clear (reset complete) */
    err = whal_Reg_ReadPoll(reg->base, RNG_CR_REG,
                            RNG_CR_CONDRST_Msk, 0, cfg->timeout);

    return err;
}

whal_Error whal_Stm32n6Rng_Deinit(whal_Rng *rngDev)
{
    if (!rngDev || !rngDev->cfg)
        return WHAL_EINVAL;

    /* Disable the RNG peripheral */
    whal_Reg_Update(rngDev->regmap.base, RNG_CR_REG, RNG_CR_RNGEN_Msk,
                    whal_SetBits(RNG_CR_RNGEN_Msk, RNG_CR_RNGEN_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32n6Rng_Generate(whal_Rng *rngDev, void *rngData,
                                     size_t rngDataSz)
{
    uint8_t *rngBuf = (uint8_t *)rngData;
    whal_Error err = WHAL_SUCCESS;
    whal_Stm32n6Rng_Cfg *cfg;
    const whal_Regmap *reg;
    size_t sr;
    size_t offset = 0;

    if (!rngDev || !rngDev->cfg || !rngData)
        return WHAL_EINVAL;

    cfg = (whal_Stm32n6Rng_Cfg *)rngDev->cfg;
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
            rngBuf[offset] = (uint8_t)(rnd >> (i * 8));
    }

exit:
    return err;
}

const whal_RngDriver whal_Stm32n6Rng_Driver = {
    .Init = whal_Stm32n6Rng_Init,
    .Deinit = whal_Stm32n6Rng_Deinit,
    .Generate = whal_Stm32n6Rng_Generate,
};
