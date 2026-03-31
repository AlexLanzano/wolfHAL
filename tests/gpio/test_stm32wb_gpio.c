#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/gpio/stm32wb_gpio.h>
#include <wolfHAL/bitops.h>
#include "board.h"
#include "test.h"

/*
 * GPIO register offsets.
 * LED port and pin come from board.h (BOARD_LED_PORT_OFFSET, BOARD_LED_PIN_NUM).
 */
#define GPIOx_MODE_REG    0x00
#define GPIOx_ODR_REG     0x14

static void Test_Gpio_NoDuplicatePins(void)
{
    whal_Stm32wbGpio_Cfg *cfg = (whal_Stm32wbGpio_Cfg *)g_whalGpio.cfg;
    whal_Stm32wbGpio_PinCfg *pins = cfg->pinCfg;

    for (size_t i = 0; i < cfg->pinCount; i++) {
        for (size_t j = i + 1; j < cfg->pinCount; j++) {
            if (pins[i].port == pins[j].port &&
                pins[i].pin == pins[j].pin) {
                WHAL_ASSERT_NEQ(pins[i].port, pins[j].port);
            }
        }
    }
}

static void Test_Gpio_ModeRegister(void)
{
    size_t portBase = g_whalGpio.regmap.base + BOARD_LED_PORT_OFFSET;
    size_t bitPos = BOARD_LED_PIN_NUM << 1;
    size_t mask = (WHAL_BITMASK(2) << bitPos);
    size_t val = 0;

    whal_Reg_Get(portBase, GPIOx_MODE_REG, mask, bitPos, &val);
    WHAL_ASSERT_EQ(val, WHAL_STM32WB_GPIO_MODE_OUT);
}

static void Test_Gpio_SetHighReg(void)
{
    WHAL_ASSERT_EQ(whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 1), WHAL_SUCCESS);

    size_t portBase = g_whalGpio.regmap.base + BOARD_LED_PORT_OFFSET;
    size_t val = 0;
    whal_Reg_Get(portBase, GPIOx_ODR_REG, (1UL << BOARD_LED_PIN_NUM),
                 BOARD_LED_PIN_NUM, &val);
    WHAL_ASSERT_EQ(val, 1);
}

static void Test_Gpio_SetLowReg(void)
{
    WHAL_ASSERT_EQ(whal_Gpio_Set(&g_whalGpio, BOARD_LED_PIN, 0), WHAL_SUCCESS);

    size_t portBase = g_whalGpio.regmap.base + BOARD_LED_PORT_OFFSET;
    size_t val = 0;
    whal_Reg_Get(portBase, GPIOx_ODR_REG, (1UL << BOARD_LED_PIN_NUM),
                 BOARD_LED_PIN_NUM, &val);
    WHAL_ASSERT_EQ(val, 0);
}

void whal_Test_Gpio_Platform(void)
{
    WHAL_TEST(Test_Gpio_NoDuplicatePins);
    WHAL_TEST(Test_Gpio_ModeRegister);
    WHAL_TEST(Test_Gpio_SetHighReg);
    WHAL_TEST(Test_Gpio_SetLowReg);
}
