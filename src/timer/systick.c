#include <wolfHAL/error.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/timer/timer.h>
#include <wolfHAL/timer/systick.h>

#define SYSTICK_CSR_REG 0x00
#define SYSTICK_CSR_ENABLE_Pos 0
#define SYSTICK_CSR_ENABLE_Msk (1UL << SYSTICK_CSR_ENABLE_Pos)

#define SYSTICK_CSR_TICKINT_Pos 1
#define SYSTICK_CSR_TICKINT_Msk (1UL << SYSTICK_CSR_TICKINT_Pos)

#define SYSTICK_CSR_CLKSOURCE_Pos 2
#define SYSTICK_CSR_CLKSOURCE_Msk (1UL << SYSTICK_CSR_CLKSOURCE_Pos)

#define SYSTICK_CSR_COUNTFLAG_Pos 16
#define SYSTICK_CSR_COUNTFLAG_Msk (1UL << SYSTICK_CSR_COUNTFLAG_Pos)

#define SYSTICK_RVR_REG 0x04
#define SYSTICK_RVR_RELOAD_Pos 0
#define SYSTICK_RVR_RELOAD_Msk (WHAL_BITMASK(24) << SYSTICK_RVR_RELOAD_Pos)

whal_Error SysTick_Init(whal_Timer *timerDev)
{
    whal_SysTick_Cfg *cfg;
    const whal_Regmap *reg = &timerDev->regmap;

    if (!timerDev || !timerDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_SysTick_Cfg *)timerDev->cfg;

    whal_Reg_Update(reg->base, SYSTICK_CSR_REG,
                          SYSTICK_CSR_CLKSOURCE_Msk | SYSTICK_CSR_TICKINT_Msk,
                          whal_SetBits(SYSTICK_CSR_CLKSOURCE_Msk, SYSTICK_CSR_CLKSOURCE_Pos, cfg->clkSrc) |
                          whal_SetBits(SYSTICK_CSR_TICKINT_Msk, SYSTICK_CSR_TICKINT_Pos, cfg->tickInt));

    whal_Reg_Update(reg->base, SYSTICK_RVR_REG,
                    SYSTICK_RVR_RELOAD_Msk,
                    whal_SetBits(SYSTICK_RVR_RELOAD_Msk, SYSTICK_RVR_RELOAD_Pos, cfg->cyclesPerTick));

    return WHAL_SUCCESS;
}

whal_Error SysTick_Deinit(whal_Timer *timerDev)
{
    return WHAL_SUCCESS;
}

whal_Error SysTick_Start(whal_Timer *timerDev)
{
    const whal_Regmap *reg = &timerDev->regmap;

    if (!timerDev || !timerDev->cfg) {
        return WHAL_EINVAL;
    }

    whal_Reg_Update(reg->base, SYSTICK_CSR_REG, SYSTICK_CSR_ENABLE_Msk,
                    whal_SetBits(SYSTICK_CSR_ENABLE_Msk, SYSTICK_CSR_ENABLE_Pos, 1));

    return WHAL_SUCCESS;
}

whal_Error SysTick_Stop(whal_Timer *timerDev)
{
    const whal_Regmap *reg = &timerDev->regmap;

    if (!timerDev || !timerDev->cfg) {
        return WHAL_EINVAL;
    }

    whal_Reg_Update(reg->base, SYSTICK_CSR_REG, SYSTICK_CSR_ENABLE_Msk,
                    whal_SetBits(SYSTICK_CSR_ENABLE_Msk, SYSTICK_CSR_ENABLE_Pos, 0));

    return WHAL_SUCCESS;
}

whal_Error SysTick_Reset(whal_Timer *timerDev)
{

    return WHAL_SUCCESS;
}

const whal_TimerDriver whal_SysTick_Driver = {
    .Init = SysTick_Init,
    .Deinit = SysTick_Deinit,
    .Start = SysTick_Start,
    .Stop = SysTick_Stop,
    .Reset = SysTick_Reset,
};
