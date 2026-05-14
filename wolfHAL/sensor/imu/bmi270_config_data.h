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
