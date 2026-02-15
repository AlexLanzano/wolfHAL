#ifndef WHAL_PIC32CZ_SUPC_H
#define WHAL_PIC32CZ_SUPC_H

#include <stdint.h>
#include <wolfHAL/supply/supply.h>

/*
 * @file pic32cz_supc.h
 * @brief PIC32CZ supply controller (SUPC) driver configuration.
 */

/*
 * @brief PIC32CZ SUPC configuration parameters.
 */
typedef struct whal_Pic32czSupc_Cfg {
} whal_Pic32czSupc_Cfg;

typedef struct whal_Pic32czSupc_Supply {
    size_t enableMask;
} whal_Pic32czSupc_Supply;

/*
 * @brief Driver instance for PIC32CZ SUPC peripheral.
 */
extern const whal_SupplyDriver whal_Pic32czSupc_Driver;

/*
 * @brief Initialize the PIC32CZ SUPC peripheral.
 *
 * @param supplyCtrl Supply device instance to initialize.
 *
 * @retval WHAL_SUCCESS Initialization completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Pic32czSupc_Init(whal_Supply *supplyCtrl);
/*
 * @brief Deinitialize the PIC32CZ SUPC peripheral.
 *
 * @param supplyCtrl Supply device instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Pic32czSupc_Deinit(whal_Supply *supplyCtrl);
/*
 * @brief Enable the PIC32CZ SUPC supply output.
 *
 * @param supplyCtrl Supply device instance to enable.
 *
 * @retval WHAL_SUCCESS Supply enabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Pic32czSupc_Enable(whal_Supply *supplyCtrl, void *supply);
/*
 * @brief Disable the PIC32CZ SUPC supply output.
 *
 * @param supplyCtrl Supply device instance to disable.
 *
 * @retval WHAL_SUCCESS Supply disabled.
 * @retval WHAL_EINVAL  Invalid arguments.
 */
whal_Error whal_Pic32czSupc_Disable(whal_Supply *supplyCtrl, void *supply);

#endif /* WHAL_PIC32CZ_SUPC_H */
