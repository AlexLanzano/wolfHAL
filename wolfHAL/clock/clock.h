#ifndef WHAL_CLOCK_H
#define WHAL_CLOCK_H

#include <stddef.h>

/*
 * @file clock.h
 * @brief Clock device handle.
 *
 * wolfHAL doesn't provide a generic clock API — clock-tree shape and
 * peripheral-gate semantics vary too much across vendors. Each chip's
 * clock driver header (e.g. <wolfHAL/clock/stm32wb_rcc.h>) defines its
 * own imperative bring-up and gate-toggle helpers. This file just
 * provides a typed handle so those helpers have something to take.
 */
typedef struct {
    const size_t base;
} whal_Clock;

#endif /* WHAL_CLOCK_H */
