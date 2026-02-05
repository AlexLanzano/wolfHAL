#ifndef WHAL_PIC32CZ_GPIO_H
#define WHAL_PIC32CZ_GPIO_H

#include <stdint.h>
#include <wolfHAL/gpio/gpio.h>

/*
 * @file pic32cz_gpio.h
 * @brief PIC32CZ PORT (GPIO) driver configuration.
 *
 * The PIC32CZ PORT module provides GPIO functionality with:
 * - Multiple ports (A, B, C, etc.) each with up to 32 pins
 * - Configurable direction (input/output)
 * - Optional input enable and pull resistors
 * - Peripheral multiplexing (PMUX) for alternate functions
 *
 * Each pin can either be used as GPIO or connected to a peripheral
 * function via the PMUX registers. When pmuxEn is set, the pin is
 * controlled by the selected peripheral rather than GPIO.
 */

/*
 * @brief Pin direction selection.
 */
enum {
    WHAL_PIC32CZ_DIR_INPUT,  /* Configure pin as input */
    WHAL_PIC32CZ_DIR_OUTPUT, /* Configure pin as output */
};

/*
 * @brief Peripheral multiplexer function selection.
 *
 * Each pin can be assigned to one of several peripheral functions (A-N).
 * The available functions vary by pin - consult the datasheet's
 * I/O Multiplexing table for valid assignments.
 */
enum {
    WHAL_PIC32CZ_PMUX_A = 0x0,  /* Peripheral function A */
    WHAL_PIC32CZ_PMUX_B = 0x1,  /* Peripheral function B */
    WHAL_PIC32CZ_PMUX_C = 0x2,  /* Peripheral function C */
    WHAL_PIC32CZ_PMUX_D = 0x3,  /* Peripheral function D */
    WHAL_PIC32CZ_PMUX_E = 0x4,  /* Peripheral function E */
    WHAL_PIC32CZ_PMUX_F = 0x5,  /* Peripheral function F */
    WHAL_PIC32CZ_PMUX_G = 0x6,  /* Peripheral function G */
    WHAL_PIC32CZ_PMUX_H = 0x7,  /* Peripheral function H */
    WHAL_PIC32CZ_PMUX_I = 0x8,  /* Peripheral function I */
    WHAL_PIC32CZ_PMUX_J = 0x9,  /* Peripheral function J */
    WHAL_PIC32CZ_PMUX_K = 0xA,  /* Peripheral function K */
    WHAL_PIC32CZ_PMUX_L = 0xB,  /* Peripheral function L */
    WHAL_PIC32CZ_PMUX_M = 0xC,  /* Peripheral function M */
    WHAL_PIC32CZ_PMUX_N = 0xD,  /* Peripheral function N */
};

/*
 * @brief Single pin configuration.
 *
 * Describes the configuration for one GPIO pin. Pins can be configured
 * either as standard GPIO (input/output) or as peripheral function pins.
 *
 * For GPIO mode (pmuxEn = 0):
 *   - dir: Input or output direction
 *   - inEn: Enable input buffer (required for reading input pins)
 *   - pullEn: Enable internal pull resistor
 *   - out: Initial output value (for output pins)
 *
 * For peripheral mode (pmuxEn = 1):
 *   - pmux: Peripheral function (A-N) to assign
 *   - Other fields are ignored; peripheral controls the pin
 */
typedef struct whal_Pic32czGpio_PinCfg {
    uint8_t port;   /* Port index (0=A, 1=B, 2=C, ...) */
    uint8_t pin;    /* Pin number within port (0-31) */
    uint8_t dir;    /* Direction: WHAL_PIC32CZ_DIR_INPUT/OUTPUT */
    uint8_t inEn;   /* Input buffer enable (1=enabled) */
    uint8_t pullEn; /* Pull resistor enable (1=enabled) */
    uint8_t out;    /* Initial output value (0 or 1) */
    uint8_t pmuxEn; /* Peripheral mux enable (1=use pmux function) */
    uint8_t pmux;   /* Peripheral function (WHAL_PIC32CZ_PMUX_x) */
} whal_Pic32czGpio_PinCfg;

/*
 * @brief GPIO device configuration.
 *
 * Contains an array of pin configurations to be applied during Init.
 */
typedef struct whal_Pic32czGpio_Cfg {
    size_t pinCfgCount;              /* Number of pins to configure */
    whal_Pic32czGpio_PinCfg *pinCfg; /* Array of pin configurations */
} whal_Pic32czGpio_Cfg;

/*
 * @brief Driver instance for PIC32CZ GPIO.
 */
extern const whal_GpioDriver whal_Pic32czGpio_Driver;

/*
 * @brief Initialize the PIC32CZ GPIO peripheral.
 *
 * @param gpioDev GPIO device instance.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Pic32czGpio_Init(whal_Gpio *gpioDev);
/*
 * @brief Deinitialize the PIC32CZ GPIO peripheral.
 *
 * @param gpioDev GPIO device instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Pic32czGpio_Deinit(whal_Gpio *gpioDev);
/*
 * @brief Read a GPIO pin value.
 *
 * @param gpioDev GPIO device instance.
 * @param pin     Pin index in the configured pin table.
 * @param value   Output for the sampled pin value.
 *
 * @retval WHAL_SUCCESS Pin value read.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Pic32czGpio_Get(whal_Gpio *gpioDev, size_t pin, size_t *value);
/*
 * @brief Set a GPIO pin value.
 *
 * @param gpioDev GPIO device instance.
 * @param pin     Pin index in the configured pin table.
 * @param value   Value to drive.
 *
 * @retval WHAL_SUCCESS Pin updated.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Pic32czGpio_Set(whal_Gpio *gpioDev, size_t pin, size_t value);

#endif /* WHAL_PIC32CZ_GPIO_H */
