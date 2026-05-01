#ifndef WHAL_POWER_H
#define WHAL_POWER_H

#include <wolfHAL/regmap.h>

/*
 * @file power.h
 * @brief Power-controller device handle.
 *
 * wolfHAL doesn't provide a generic power API — the operations chips
 * expose around their power controllers (voltage scaling, low-power mode
 * entry, regulator on/off) are too chip-specific to abstract usefully.
 * Each chip's power driver header (e.g. <wolfHAL/power/stm32l1_pwr.h>,
 * <wolfHAL/power/pic32cz_supc.h>) defines its own imperative helpers.
 * Power is a board-level driver — boards call those helpers directly
 * from Board_Init, and applications reach board-level behavior through
 * Board_<Operation>() wrappers.
 */
typedef struct {
    const whal_Regmap regmap;
} whal_Power;

#endif /* WHAL_POWER_H */
