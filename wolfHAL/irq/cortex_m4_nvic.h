#ifndef WHAL_CORTEX_M4_NVIC_H
#define WHAL_CORTEX_M4_NVIC_H

#include <wolfHAL/irq/irq.h>
#include <stdint.h>

/*
 * @file cortex_m4_nvic.h
 * @brief ARM Cortex-M4 NVIC driver.
 *
 * Uses a 4-bit priority field in bits[7:4] of each IPR byte.
 * The regmap base should be 0xE000E100 (NVIC_ISER0).
 */

/*
 * @brief Optional per-interrupt configuration.
 *
 * Pass to whal_Irq_Enable() to set priority, or pass NULL for default (0).
 */
typedef struct {
    uint8_t priority;
} whal_Nvic_Cfg;

/*
 * @brief Driver instance for Cortex-M4 NVIC.
 */
extern const whal_IrqDriver whal_Nvic_Driver;

#endif /* WHAL_CORTEX_M4_NVIC_H */
