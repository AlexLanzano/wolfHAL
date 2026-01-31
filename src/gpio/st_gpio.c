#include <stdint.h>
#include <wolfHAL/error.h>
#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/gpio/st_gpio.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>

#define STGPIO_PORT_SIZE 0x400

#define STGPIO_GPIOx_MODE_REG     0x00
#define STGPIO_GPIOx_OUTTYPE_REG  0x04
#define STGPIO_GPIOx_SPEED_REG    0x08
#define STGPIO_GPIOx_PULL_REG     0x0C
#define STGPIO_GPIOx_IDR_REG      0x10
#define STGPIO_GPIOx_ODR_REG      0x14
#define STGPIO_GPIOx_ALTFNL_REG   0x20
#define STGPIO_GPIOx_ALTFNH_REG   0x24

static inline void whal_StGpio_InitAltFn(whal_Regmap *portReg, whal_StGpio_PinCfg *pinCfg)
{
    uint8_t pin = pinCfg->pin;
    uint8_t maskBit;
    uint64_t mask;

    maskBit = pin << 2;
    mask = WHAL_MASK_RANGE(maskBit + 3, maskBit);

    uint64_t *reg = (uint64_t *)(portReg->base + STGPIO_GPIOx_ALTFNL_REG);
    *reg = (*reg & ~mask) | (whal_SetBits(mask, pinCfg->altFn) & mask);
}


static inline whal_Error whal_StGpio_InitPin(whal_Gpio *gpioDev, whal_StGpio_PinCfg *pinCfg)
{
    whal_Regmap portReg;

    if (pinCfg->pin > 15) {
        return WHAL_EINVAL;
    }

    portReg.size = STGPIO_PORT_SIZE;
    portReg.base = (size_t)(gpioDev->regmap.base + (pinCfg->port * STGPIO_PORT_SIZE)); 

    uint8_t pin = pinCfg->pin;
    uint8_t maskBit = pin << 1;
    size_t mask1 = WHAL_MASK_RANGE(maskBit + 1, maskBit);
    size_t mask2 = WHAL_MASK(pin);

    whal_Reg_Update(portReg.base, STGPIO_GPIOx_MODE_REG, mask1,
                    whal_SetBits(mask1, pinCfg->mode));

    whal_Reg_Update(portReg.base, STGPIO_GPIOx_SPEED_REG, mask1,
                    whal_SetBits(mask1, pinCfg->speed));

    whal_Reg_Update(portReg.base, STGPIO_GPIOx_OUTTYPE_REG, mask2,
                    whal_SetBits(mask2, pinCfg->outType));

    if (pinCfg->mode == WHAL_STGPIO_MODE_ALTFN) {
        whal_StGpio_InitAltFn(&portReg, pinCfg);
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StGpio_Init(whal_Gpio *gpioDev)
{
    whal_Error err;
    whal_StGpio_Cfg *cfg;
    whal_StGpio_PinCfg *pinCfg;

    if (!gpioDev || !gpioDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StGpio_Cfg *)gpioDev->cfg;
    pinCfg = cfg->pinCfg;

    err = whal_Clock_Enable(cfg->clkCtrl, cfg->clk);
    if (err) {
        return err;
    }
    
    for (size_t pin = 0; pin < cfg->pinCount; ++pin) {
        err = whal_StGpio_InitPin(gpioDev, &pinCfg[pin]);
        if (err) {
            return err;
        }
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StGpio_Deinit(whal_Gpio *gpioDev)
{
    whal_Error err;
    whal_StGpio_Cfg *cfg;
    cfg = (whal_StGpio_Cfg *)gpioDev->cfg;

    err = whal_Clock_Disable(cfg->clkCtrl, cfg->clk);
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}

static whal_Error whal_StGpio_SetOrGet(whal_Gpio *gpioDev, size_t pin, size_t *value, uint8_t set)
{
    whal_Regmap portReg;
    whal_StGpio_Cfg *cfg;
    whal_StGpio_PinCfg *pinCfg;
    size_t mask;

    if (!gpioDev || !gpioDev->cfg) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StGpio_Cfg *)gpioDev->cfg;
    pinCfg = cfg->pinCfg;

    if (pinCfg[pin].pin > 15) {
        return WHAL_EINVAL;
    }

    portReg.size = STGPIO_PORT_SIZE;
    portReg.base = (size_t)(gpioDev->regmap.base + (pinCfg[pin].port * STGPIO_PORT_SIZE)); 

    mask = WHAL_MASK(pinCfg[pin].pin);
    if (set) {
        whal_Reg_Update(portReg.base, STGPIO_GPIOx_ODR_REG, mask,
                        whal_SetBits(mask, *value));
    }
    else {
        whal_Reg_Get(portReg.base, STGPIO_GPIOx_IDR_REG, mask, value);
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StGpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value)
{
    return whal_StGpio_SetOrGet(gpioDev, pin, value, 0);
}

whal_Error whal_StGpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value)
{
    return whal_StGpio_SetOrGet(gpioDev, pin, &value, 1);
}

const whal_GpioDriver whal_StGpio_Driver = {
    .Init = whal_StGpio_Init,
    .Deinit = whal_StGpio_Deinit,
    .Get = whal_StGpio_Get,
    .Set = whal_StGpio_Set,
};
