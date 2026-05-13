#ifndef WOLFHAL_H
#define WOLFHAL_H

/*
 * @file wolfHAL.h
 * @brief Convenience umbrella header that pulls in all core wolfHAL modules.
 */

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>

/* Handle sentinel for singleton-internal drivers: the driver references its
 * singleton (e.g., `whal_Stm32h5_Eth_Dev`) directly and ignores the handle
 * argument. Pass WHAL_SINGLETON to make that intent explicit at the call
 * site. */
#define WHAL_SINGLETON  ((void *)0)

#include <wolfHAL/clock/clock.h>
#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/block/block.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/i2c/i2c.h>
#include <wolfHAL/timer/timer.h>
#include <wolfHAL/ipc/ipc.h>
#include <wolfHAL/power/power.h>
#include <wolfHAL/timeout.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/dma/dma.h>
#include <wolfHAL/irq/irq.h>
#include <wolfHAL/watchdog/watchdog.h>
#include <wolfHAL/eth/eth.h>
#include <wolfHAL/eth_phy/eth_phy.h>
#include <wolfHAL/sensor/sensor.h>

#endif /* WOLFHAL_H */
