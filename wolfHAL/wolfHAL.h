#ifndef WOLFHAL_H
#define WOLFHAL_H

/*
 * @file wolfHAL.h
 * @brief Convenience umbrella header that pulls in all core wolfHAL modules.
 */

#include <wolfHAL/error.h>
#include <wolfHAL/regmap.h>

#include <wolfHAL/clock/clock.h>
#include <wolfHAL/gpio/gpio.h>
#include <wolfHAL/uart/uart.h>
#include <wolfHAL/flash/flash.h>
#include <wolfHAL/block/block.h>
#include <wolfHAL/rng/rng.h>
#include <wolfHAL/spi/spi.h>
#include <wolfHAL/timer/timer.h>
#include <wolfHAL/ipc/ipc.h>
#include <wolfHAL/supply/supply.h>
#include <wolfHAL/timeout.h>
#include <wolfHAL/crypto/crypto.h>
#include <wolfHAL/eth/eth.h>
#include <wolfHAL/eth_phy/eth_phy.h>

#endif /* WOLFHAL_H */
