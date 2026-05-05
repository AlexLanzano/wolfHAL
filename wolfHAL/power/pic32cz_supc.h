#ifndef WHAL_PIC32CZ_SUPC_H
#define WHAL_PIC32CZ_SUPC_H

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/error.h>
#include <wolfHAL/power/power.h>

/*
 * @file pic32cz_supc.h
 * @brief PIC32CZ supply controller (SUPC) driver.
 *
 * Boards toggle SUPC voltage-regulator outputs imperatively from
 * Board_Init by calling the helpers below. Power is a board-level
 * driver — there is no generic whal_Power_* API or vtable.
 */

/*
 * @brief Descriptor for one SUPC voltage regulator output (VREGCTRL.AVREGEN).
 */
typedef struct whal_Pic32cz_Supc_Supply {
    size_t enableMask;
    size_t enablePos;
} whal_Pic32cz_Supc_Supply;

/*
 * @brief Enable a SUPC voltage regulator output.
 *
 * @param powerDev Power device instance.
 * @param supply   Supply descriptor identifying which regulator output.
 *
 * @retval WHAL_SUCCESS Output enabled.
 * @retval WHAL_EINVAL  Null pointer.
 */
whal_Error whal_Pic32cz_Supc_EnableSupply(whal_Power *powerDev,
                                         const whal_Pic32cz_Supc_Supply *supply);

/*
 * @brief Disable a SUPC voltage regulator output.
 *
 * @param powerDev Power device instance.
 * @param supply   Supply descriptor identifying which regulator output.
 *
 * @retval WHAL_SUCCESS Output disabled.
 * @retval WHAL_EINVAL  Null pointer.
 */
whal_Error whal_Pic32cz_Supc_DisableSupply(whal_Power *powerDev,
                                          const whal_Pic32cz_Supc_Supply *supply);

#endif /* WHAL_PIC32CZ_SUPC_H */
