#include <stdint.h>
#include <wolfHAL/rng/stm32wb_rng.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

/*
 * STM32WB RNG Register Definitions
 *
 * The RNG peripheral uses an analog noise source to produce 32-bit
 * random values. One value is available at a time in DR, signaled
 * by the DRDY flag in SR.
 */

/* Control Register */
#define RNG_CR_REG   0x00
#define RNG_CR_RNGEN_Pos 2                                              /* RNG enable */
#define RNG_CR_RNGEN_Msk (1UL << RNG_CR_RNGEN_Pos)

#define RNG_CR_CED_Pos   5                                              /* Clock error detection disable */
#define RNG_CR_CED_Msk   (1UL << RNG_CR_CED_Pos)

/* Status Register */
#define RNG_SR_REG  0x04
#define RNG_SR_DRDY_Pos 0                                               /* Data ready */
#define RNG_SR_DRDY_Msk (1UL << RNG_SR_DRDY_Pos)

#define RNG_SR_CECS_Pos 1                                               /* Clock error current status */
#define RNG_SR_CECS_Msk (1UL << RNG_SR_CECS_Pos)

#define RNG_SR_SECS_Pos 2                                               /* Seed error current status */
#define RNG_SR_SECS_Msk (1UL << RNG_SR_SECS_Pos)

#define RNG_SR_CEIS_Pos 5                                               /* Clock error interrupt status */
#define RNG_SR_CEIS_Msk (1UL << RNG_SR_CEIS_Pos)

#define RNG_SR_SEIS_Pos 6                                               /* Seed error interrupt status */
#define RNG_SR_SEIS_Msk (1UL << RNG_SR_SEIS_Pos)

/* Data Register - 32-bit random value */
#define RNG_DR_REG  0x08

whal_Error whal_Stm32wbRng_Init(whal_Rng *rngDev)
{
    if (!rngDev || !rngDev->cfg) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRng_Deinit(whal_Rng *rngDev)
{
    if (!rngDev || !rngDev->cfg) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_Stm32wbRng_Generate(whal_Rng *rngDev, uint8_t *rngData, size_t rngDataSz)
{

    whal_Error err = WHAL_SUCCESS;
    whal_Stm32wbRng_Cfg *cfg = (whal_Stm32wbRng_Cfg *)rngDev->cfg;
    const whal_Regmap *reg = &rngDev->regmap;
    size_t sr;
    size_t offset = 0;
#ifdef WHAL_CFG_NO_TIMEOUT
    (void)(cfg);
#endif

    if (!rngDev || !rngData) {
        return WHAL_EINVAL;
    }

    /* Enable the RNG peripheral */
    whal_Reg_Update(reg->base, RNG_CR_REG, RNG_CR_RNGEN_Msk,
                    whal_SetBits(RNG_CR_RNGEN_Msk, RNG_CR_RNGEN_Pos, 1));

    while (offset < rngDataSz) {
        /* Wait for a random value to be ready */
        WHAL_TIMEOUT_START(cfg->timeout);
        while (1) {
            if (WHAL_TIMEOUT_EXPIRED(cfg->timeout)) {
                err = WHAL_ETIMEOUT;
                goto exit;
            }

            sr = whal_Reg_Read(reg->base, RNG_SR_REG);

            /* Check for seed or clock error */
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
        uint32_t rnd = *(volatile uint32_t *)(reg->base + RNG_DR_REG);

        /* Copy bytes into output buffer */
        for (size_t i = 0; i < 4 && offset < rngDataSz; i++, offset++) {
            rngData[offset] = (uint8_t)(rnd >> (i * 8));
        }
    }

exit:
    /* Disable the RNG peripheral */
    whal_Reg_Update(reg->base, RNG_CR_REG, RNG_CR_RNGEN_Msk,
                    whal_SetBits(RNG_CR_RNGEN_Msk, RNG_CR_RNGEN_Pos, 0));

    return err;
}

const whal_RngDriver whal_Stm32wbRng_Driver = {
    .Init = whal_Stm32wbRng_Init,
    .Deinit = whal_Stm32wbRng_Deinit,
    .Generate = whal_Stm32wbRng_Generate,
};
