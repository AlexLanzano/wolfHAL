#include <wolfHAL/irq/cortex_m4_nvic.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>

/*
 * ARM Cortex-M4 NVIC register offsets (relative to 0xE000E100).
 *
 * ISER: Interrupt Set-Enable Registers (0x000-0x01C, 32 IRQs per register)
 * ICER: Interrupt Clear-Enable Registers (0x080-0x09C)
 * IPR:  Interrupt Priority Registers (0x300-0x37F, 4 IRQs per register)
 */
#define NVIC_ISER_REG(irq) (0x000 + (((irq) >> 5) << 2))
#define NVIC_ICER_REG(irq) (0x080 + (((irq) >> 5) << 2))
#define NVIC_IPR_REG(irq)    (0x300 + (((irq) >> 2) << 2))
#define NVIC_IPR_SHIFT(irq)  (((irq) & 0x3) << 3)

static whal_Error whal_Nvic_Init(whal_Irq *irqDev)
{
    if (!irqDev) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

static whal_Error whal_Nvic_Deinit(whal_Irq *irqDev)
{
    if (!irqDev) {
        return WHAL_EINVAL;
    }

    return WHAL_SUCCESS;
}

static whal_Error whal_Nvic_Enable(whal_Irq *irqDev, size_t irq,
                                    const void *irqCfg)
{
    if (!irqDev) {
        return WHAL_EINVAL;
    }

    size_t base = irqDev->regmap.base;

    /* Set priority if config provided */
    if (irqCfg) {
        const whal_Nvic_Cfg *cfg = (const whal_Nvic_Cfg *)irqCfg;
        size_t shift = NVIC_IPR_SHIFT(irq);
        size_t mask = (0xFFUL << shift);
        whal_Reg_Update(base, NVIC_IPR_REG(irq), mask,
                        (size_t)(cfg->priority << 4) << shift);
    }

    /* Enable the interrupt */
    whal_Reg_Write(base, NVIC_ISER_REG(irq), (1UL << (irq & 0x1F)));

    return WHAL_SUCCESS;
}

static whal_Error whal_Nvic_Disable(whal_Irq *irqDev, size_t irq)
{
    if (!irqDev) {
        return WHAL_EINVAL;
    }

    size_t base = irqDev->regmap.base;

    whal_Reg_Write(base, NVIC_ICER_REG(irq), (1UL << (irq & 0x1F)));

    return WHAL_SUCCESS;
}

const whal_IrqDriver whal_Nvic_Driver = {
    .Init = whal_Nvic_Init,
    .Deinit = whal_Nvic_Deinit,
    .Enable = whal_Nvic_Enable,
    .Disable = whal_Nvic_Disable,
};
