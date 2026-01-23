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

static whal_Error whal_StGpio_GetPortAddr(whal_Gpio *gpioDev, whal_StGpio_Cfg *pinCfg, size_t *portAddr)
{
    whal_StGpio_Port port = pinCfg->port;
    size_t regBase = gpioDev->regmap.base;

    if (port < WHAL_STGPIO_PORT_A || port > WHAL_STGPIO_PORT_H || !portAddr) {
        return WHAL_EINVAL;
    }

    *portAddr = (size_t)(regBase + (port * STGPIO_PORT_SIZE)); 

    return WHAL_SUCCESS;
}

static whal_Error whal_StGpio_InitMode(whal_Regmap *portReg, whal_StGpio_Cfg *pinCfg)
{
    whal_Error err;
    uint8_t pin = pinCfg->pin;
    uint8_t maskBit = pin << 1;
    size_t mask = WHAL_MASK_RANGE(maskBit + 1, maskBit);

    err = whal_Reg_Update(portReg, STGPIO_GPIOx_MODE_REG, mask,
                          whal_SetBits(mask, pinCfg->mode));

    return err;
}

static whal_Error whal_StGpio_InitOutType(whal_Regmap *portReg, whal_StGpio_Cfg *pinCfg)
{
    whal_Error err;
    uint8_t pin = pinCfg->pin;
    size_t mask = WHAL_MASK(pin);

    err = whal_Reg_Update(portReg, STGPIO_GPIOx_OUTTYPE_REG, mask,
                          whal_SetBits(mask, pinCfg->outType));

    return err;
}

static whal_Error whal_StGpio_InitSpeed(whal_Regmap *portReg, whal_StGpio_Cfg *pinCfg)
{
    whal_Error err;
    uint8_t pin = pinCfg->pin;
    uint8_t maskBit = pin << 1;
    size_t mask = WHAL_MASK_RANGE(maskBit + 1, maskBit);

    err = whal_Reg_Update(portReg, STGPIO_GPIOx_SPEED_REG, mask,
                          whal_SetBits(mask, pinCfg->mode));

    return err;
}

static whal_Error whal_StGpio_InitAltFn(whal_Regmap *portReg, whal_StGpio_Cfg *pinCfg)
{
    whal_Error err;
    uint8_t pin = pinCfg->pin;
    size_t regOffset;
    uint8_t maskBit;
    size_t mask;

    if (pin >= 8) {
        regOffset = STGPIO_GPIOx_ALTFNH_REG;
        pin -= 8;
    } else {
        regOffset = STGPIO_GPIOx_ALTFNL_REG;
    }

    maskBit = pin << 2;
    mask = WHAL_MASK_RANGE(maskBit + 3, maskBit);

    err = whal_Reg_Update(portReg, regOffset, mask,
                          whal_SetBits(mask, pinCfg->altFn));

    return err;
}


static whal_Error whal_StGpio_InitPin(whal_Gpio *gpioDev, whal_StGpio_Cfg *pinCfg)
{
    whal_Error err;
    whal_Regmap portReg;

    if (pinCfg->pin > 15) {
        return WHAL_EINVAL;
    }

    portReg.size = STGPIO_PORT_SIZE;
    err = whal_StGpio_GetPortAddr(gpioDev, pinCfg, &portReg.base);
    if (err) {
        return err;
    }

    err = whal_StGpio_InitMode(&portReg, pinCfg); 
    if (err) {
        return err;
    }

    err = whal_StGpio_InitOutType(&portReg, pinCfg);
    if (err) {
        return err;
    }

    err = whal_StGpio_InitSpeed(&portReg, pinCfg);
    if (err) {
        return err;
    }

    if (pinCfg->mode == WHAL_STGPIO_MODE_ALTFN) {
        err = whal_StGpio_InitAltFn(&portReg, pinCfg);
        if (err) {
            return err;
        }
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StGpio_Init(whal_Gpio *gpioDev)
{
    whal_Error err;
    whal_StGpio_Cfg *cfg;

    if (!gpioDev || !gpioDev->pinCfg || !gpioDev->pinCount) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StGpio_Cfg *)gpioDev->pinCfg;
    
    for (size_t pin = 0; pin < gpioDev->pinCount; ++pin) {
        err = whal_StGpio_InitPin(gpioDev, &cfg[pin]);
        if (err) {
            return err;
        }

    }

    return WHAL_SUCCESS;
}

whal_Error whal_StGpio_Deinit(whal_Gpio *gpioDev)
{
    return WHAL_SUCCESS;
}

whal_Error whal_StGpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value)
{
    whal_Error err;
    whal_Regmap portReg;
    whal_StGpio_Cfg *cfg;
    size_t mask;

    if (!gpioDev || !gpioDev->pinCfg || !gpioDev->pinCount) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StGpio_Cfg *)gpioDev->pinCfg;


    if (cfg[pin].pin > 15) {
        return WHAL_EINVAL;
    }

    portReg.size = STGPIO_PORT_SIZE;
    err = whal_StGpio_GetPortAddr(gpioDev, cfg, &portReg.base);
    if (err) {
        return err;
    }

    mask = WHAL_MASK(cfg[pin].pin);
    err = whal_Reg_Get(&portReg, STGPIO_GPIOx_IDR_REG, mask, value);
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StGpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value)
{
    whal_Error err;
    whal_Regmap portReg;
    whal_StGpio_Cfg *cfg;
    size_t mask;

    if (!gpioDev || !gpioDev->pinCfg || !gpioDev->pinCount) {
        return WHAL_EINVAL;
    }

    cfg = (whal_StGpio_Cfg *)gpioDev->pinCfg;


    if (cfg[pin].pin > 15) {
        return WHAL_EINVAL;
    }

    portReg.size = STGPIO_PORT_SIZE;
    err = whal_StGpio_GetPortAddr(gpioDev, cfg, &portReg.base);
    if (err) {
        return err;
    }

    mask = WHAL_MASK(cfg[pin].pin);
    err = whal_Reg_Update(&portReg, STGPIO_GPIOx_ODR_REG, mask,
                          whal_SetBits(mask, value));
    if (err) {
        return err;
    }

    return WHAL_SUCCESS;
}

whal_Error whal_StGpio_Cmd(whal_Gpio *gpioDev, size_t cmd, void *args)
{
    return WHAL_SUCCESS;
}

whal_GpioDriver whal_StGpio_Driver = {
    .Init = whal_StGpio_Init,
    .Deinit = whal_StGpio_Deinit,
    .Get = whal_StGpio_Get,
    .Set = whal_StGpio_Set,
    .Cmd = whal_StGpio_Cmd,
};
