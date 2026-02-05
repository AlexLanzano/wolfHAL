#ifndef WHAL_SUPPLY_H
#define WHAL_SUPPLY_H

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <stddef.h>

/*
 * @file supply.h
 * @brief Generic supply abstraction for power rail control.
 */

typedef struct whal_Supply whal_Supply;

/*
 * @brief Driver vtable for supply devices.
 */
typedef struct {
    /* Initialize the supply hardware. */
    whal_Error (*Init)(whal_Supply *supplyDev);
    /* Deinitialize the supply hardware. */
    whal_Error (*Deinit)(whal_Supply *supplyDev);
    /* Enable a supply output. */
    whal_Error (*Enable)(whal_Supply *supplyDev, void *supply);
    /* Disable a supply output. */
    whal_Error (*Disable)(whal_Supply *supplyDev, void *supply);
} whal_SupplyDriver;

/*
 * @brief Supply device instance pairing configuration with a driver.
 */
struct whal_Supply {
    const whal_Regmap regmap;
    const whal_SupplyDriver *driver;
    void *cfg;
};

/*
 * @brief Initialize a supply device.
 *
 * @param supplyDev Supply instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
#ifdef WHAL_CFG_NO_CALLBACKS
#define whal_Supply_Init(supplyDev) ((supplyDev)->driver->Init((supplyDev)))
#define whal_Supply_Deinit(supplyDev) ((supplyDev)->driver->Deinit((supplyDev)))
#define whal_Supply_Enable(supplyDev, supply) ((supplyDev)->driver->Enable((supplyDev), (supply)))
#define whal_Supply_Disable(supplyDev, supply) ((supplyDev)->driver->Disable((supplyDev), (supply)))
#else
/*
 * @brief Initialize a supply device.
 *
 * @param supplyDev Supply instance to initialize.
 *
 * @retval WHAL_SUCCESS Driver-specific init completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Supply_Init(whal_Supply *supplyDev);
/*
 * @brief Deinitialize a supply device.
 *
 * @param supplyDev Supply instance to deinitialize.
 *
 * @retval WHAL_SUCCESS Driver-specific deinit completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Supply_Deinit(whal_Supply *supplyDev);
/*
 * @brief Enable a supply output.
 *
 * @param supplyDev Supply instance to enable.
 *
 * @retval WHAL_SUCCESS Supply enabled.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Supply_Enable(whal_Supply *supplyDev, void *supply);
/*
 * @brief Disable a supply output.
 *
 * @param supplyDev Supply instance to disable.
 *
 * @retval WHAL_SUCCESS Supply disabled.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Supply_Disable(whal_Supply *supplyDev, void *supply);
#endif

#endif /* WHAL_SUPPLY_H */
