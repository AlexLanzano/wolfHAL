/* irq.c
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include <wolfHAL/irq/irq.h>

inline whal_Error whal_Irq_Init(whal_Irq *irqDev)
{
    if (!irqDev || !irqDev->driver || !irqDev->driver->Init) {
        return WHAL_EINVAL;
    }

    return irqDev->driver->Init(irqDev);
}

inline whal_Error whal_Irq_Deinit(whal_Irq *irqDev)
{
    if (!irqDev || !irqDev->driver || !irqDev->driver->Deinit) {
        return WHAL_EINVAL;
    }

    return irqDev->driver->Deinit(irqDev);
}

inline whal_Error whal_Irq_Enable(whal_Irq *irqDev, size_t irq,
                                   const void *irqCfg)
{
    if (!irqDev || !irqDev->driver || !irqDev->driver->Enable) {
        return WHAL_EINVAL;
    }

    return irqDev->driver->Enable(irqDev, irq, irqCfg);
}

inline whal_Error whal_Irq_Disable(whal_Irq *irqDev, size_t irq)
{
    if (!irqDev || !irqDev->driver || !irqDev->driver->Disable) {
        return WHAL_EINVAL;
    }

    return irqDev->driver->Disable(irqDev, irq);
}
