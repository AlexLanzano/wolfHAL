#ifndef WHAL_PIC32CZ_SUPC_H
#define WHAL_PIC32CZ_SUPC_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <wolfHAL/bitops.h>
#include <wolfHAL/power/power.h>

/*
 * @file pic32cz_supc.h
 * @brief PIC32CZ supply controller (SUPC) driver.
 *
 * Boards toggle SUPC voltage-regulator outputs imperatively from
 * Board_Init by calling the helpers below. Power is a board-level
 * driver — there is no generic whal_Power_* API or vtable.
 */

#define WHAL_PIC32CZ_SUPC_VREGCTRL_REG          0x1C
#define WHAL_PIC32CZ_SUPC_VREGCTRL_AVREGEN_Pos  16
#define WHAL_PIC32CZ_SUPC_VREGCTRL_AVREGEN_Msk  (WHAL_BITMASK(3) << WHAL_PIC32CZ_SUPC_VREGCTRL_AVREGEN_Pos)

/*
 * @brief Descriptor for one SUPC voltage-regulator output (mask + bit position
 *        within VREGCTRL.AVREGEN).
 */
typedef struct whal_Pic32cz_Supc_Supply {
    size_t enableMask;
    size_t enablePos;
} whal_Pic32cz_Supc_Supply;

/*
 * @brief Set a regulator's enable bit in SUPC VREGCTRL.AVREGEN.
 *
 * @param powerDev Power device instance.
 * @param supply   Descriptor for the regulator output to enable.
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Pic32cz_Supc_EnableSupply(
    const whal_Power *powerDev, const whal_Pic32cz_Supc_Supply *supply)
{
    whal_Reg_Update(powerDev->base, WHAL_PIC32CZ_SUPC_VREGCTRL_REG,
                    supply->enableMask,
                    whal_SetBits(supply->enableMask, supply->enablePos, 1));
    return WHAL_SUCCESS;
}

/*
 * @brief Clear a regulator's enable bit in SUPC VREGCTRL.AVREGEN.
 *
 * @param powerDev Power device instance.
 * @param supply   Descriptor for the regulator output to disable.
 *
 * @retval WHAL_SUCCESS Always.
 */
static inline whal_Error whal_Pic32cz_Supc_DisableSupply(
    const whal_Power *powerDev, const whal_Pic32cz_Supc_Supply *supply)
{
    whal_Reg_Update(powerDev->base, WHAL_PIC32CZ_SUPC_VREGCTRL_REG,
                    supply->enableMask,
                    whal_SetBits(supply->enableMask, supply->enablePos, 0));
    return WHAL_SUCCESS;
}

#endif /* WHAL_PIC32CZ_SUPC_H */
