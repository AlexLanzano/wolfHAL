/* bmi270_config_data.h
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

#ifndef WHAL_BMI270_CONFIG_DATA_H
#define WHAL_BMI270_CONFIG_DATA_H

#include <stdint.h>
#include <stddef.h>

/*
 * @file bmi270_config_data.h
 * @brief Bosch BMI270 configuration data blob.
 *
 * Extracted from the Bosch Sensortec BMI270_SensorAPI repository under
 * BSD-3-Clause license. See src/sensor/imu/bmi270_config_data.c for the
 * full license text and data.
 */

/*
 * @brief Size in bytes of the BMI270 configuration data blob.
 */
#define WHAL_BMI270_CONFIG_DATA_SZ 8192

/*
 * @brief BMI270 configuration data blob. Uploaded to the sensor during init
 *        via the INIT_DATA register.
 */
extern const uint8_t whal_bmi270_config_data[WHAL_BMI270_CONFIG_DATA_SZ];

#endif /* WHAL_BMI270_CONFIG_DATA_H */
