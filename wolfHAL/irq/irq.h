#ifndef WHAL_IRQ_H
#define WHAL_IRQ_H

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>
#include <stddef.h>

/*
 * @file irq.h
 * @brief Generic interrupt controller abstraction and driver interface.
 *
 * A whal_Irq device represents an interrupt controller (e.g., ARM Cortex-M
 * NVIC). The API allows enabling and disabling individual interrupt lines,
 * with optional platform-specific configuration (e.g., priority) passed
 * through the Enable call.
 */

typedef struct whal_Irq whal_Irq;

/*
 * @brief Driver vtable for interrupt controllers.
 */
typedef struct {
    /* Initialize the interrupt controller. */
    whal_Error (*Init)(whal_Irq *irqDev);
    /* Deinitialize the interrupt controller. */
    whal_Error (*Deinit)(whal_Irq *irqDev);
    /* Enable an interrupt line. irqCfg is platform-specific (or NULL). */
    whal_Error (*Enable)(whal_Irq *irqDev, size_t irq, const void *irqCfg);
    /* Disable an interrupt line. */
    whal_Error (*Disable)(whal_Irq *irqDev, size_t irq);
} whal_IrqDriver;

/*
 * @brief Interrupt controller device instance.
 */
struct whal_Irq {
    const whal_Regmap regmap;
    const whal_IrqDriver *driver;
    const void *cfg;
};

#ifdef WHAL_CFG_DIRECT_CALLBACKS
#define whal_Irq_Init(irqDev) ((irqDev)->driver->Init((irqDev)))
#define whal_Irq_Deinit(irqDev) ((irqDev)->driver->Deinit((irqDev)))
#define whal_Irq_Enable(irqDev, irq, irqCfg) ((irqDev)->driver->Enable((irqDev), (irq), (irqCfg)))
#define whal_Irq_Disable(irqDev, irq) ((irqDev)->driver->Disable((irqDev), (irq)))
#else
/*
 * @brief Initialize the interrupt controller.
 *
 * @param irqDev Interrupt controller instance.
 *
 * @retval WHAL_SUCCESS Init completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Irq_Init(whal_Irq *irqDev);
/*
 * @brief Deinitialize the interrupt controller.
 *
 * @param irqDev Interrupt controller instance.
 *
 * @retval WHAL_SUCCESS Deinit completed.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Irq_Deinit(whal_Irq *irqDev);
/*
 * @brief Enable an interrupt line.
 *
 * @param irqDev Interrupt controller instance.
 * @param irq    Interrupt number.
 * @param irqCfg Platform-specific config (e.g., priority), or NULL for defaults.
 *
 * @retval WHAL_SUCCESS Interrupt enabled.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Irq_Enable(whal_Irq *irqDev, size_t irq, const void *irqCfg);
/*
 * @brief Disable an interrupt line.
 *
 * @param irqDev Interrupt controller instance.
 * @param irq    Interrupt number.
 *
 * @retval WHAL_SUCCESS Interrupt disabled.
 * @retval WHAL_EINVAL  Null pointer or missing driver function.
 */
whal_Error whal_Irq_Disable(whal_Irq *irqDev, size_t irq);
#endif

#endif /* WHAL_IRQ_H */
