/* test_bmi270_sensor.c
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

#include <wolfHAL/wolfHAL.h>
#include <wolfHAL/sensor/sensor.h>
#include <wolfHAL/sensor/imu/bmi270_sensor.h>
#include "wolfHAL_board.h"
#include "wolfHAL_peripheral.h"
#include "test.h"

/*
 * BMI270 IMU test suite.
 *
 * Requires a BMI270 connected to the board's I2C bus.
 * The board must provide g_whalBmi270 via the peripheral config.
 */

/* The driver runs the accelerometer at +/-16g, 2048 LSB per g */
#define ACCEL_LSB_PER_G     2048

/* At rest the accelerometer measures 1 g in any orientation; accept 0.8 g to
 * 1.2 g, compared as squared magnitudes */
#define GRAVITY_SQ_MIN      ((uint32_t)(ACCEL_LSB_PER_G * 8 / 10) * \
                             (uint32_t)(ACCEL_LSB_PER_G * 8 / 10))
#define GRAVITY_SQ_MAX      ((uint32_t)(ACCEL_LSB_PER_G * 12 / 10) * \
                             (uint32_t)(ACCEL_LSB_PER_G * 12 / 10))

static uint32_t AccelMagnitudeSq(const whal_Bmi270_Data *d)
{
    int32_t x = d->accelX;
    int32_t y = d->accelY;
    int32_t z = d->accelZ;

    return (uint32_t)(x * x) + (uint32_t)(y * y) + (uint32_t)(z * z);
}

/* Each field decodes from its own little-endian byte pair, including the
 * sign bit; the bytes differ per field so a swapped index is caught. */
static void Test_Bmi270_Decode(void)
{
    static const uint8_t raw[WHAL_BMI270_RAW_LEN] = {
        0x01, 0x02,     /* accelX  0x0201 */
        0x03, 0x84,     /* accelY  0x8403 */
        0xFF, 0x7F,     /* accelZ  0x7FFF */
        0x05, 0x06,     /* gyroX   0x0605 */
        0x07, 0xF8,     /* gyroY   0xF807 */
        0x00, 0x80,     /* gyroZ   0x8000 */
    };
    whal_Bmi270_Data d = {0};

    whal_Bmi270_Decode(raw, &d);

    WHAL_ASSERT_EQ(d.accelX, 0x0201);
    WHAL_ASSERT_EQ(d.accelY, -31741);
    WHAL_ASSERT_EQ(d.accelZ, 32767);
    WHAL_ASSERT_EQ(d.gyroX, 0x0605);
    WHAL_ASSERT_EQ(d.gyroY, -2041);
    WHAL_ASSERT_EQ(d.gyroZ, -32768);
}

static void Test_Bmi270_Read(void)
{
    whal_Bmi270_Data data = {0};
    uint32_t magSq;

    WHAL_ASSERT_EQ(whal_Sensor_Read(
                       g_peripheralSensor[PERIPHERAL_SENSOR_BMI270].dev,
                       &data),
                   WHAL_SUCCESS);

    magSq = AccelMagnitudeSq(&data);
    WHAL_ASSERT_EQ(magSq >= GRAVITY_SQ_MIN, 1);
    WHAL_ASSERT_EQ(magSq <= GRAVITY_SQ_MAX, 1);
}

static void Test_Bmi270_ReadMultiple(void)
{
    whal_Bmi270_Data data1 = {0};
    whal_Bmi270_Data data2 = {0};

    WHAL_ASSERT_EQ(whal_Sensor_Read(
                       g_peripheralSensor[PERIPHERAL_SENSOR_BMI270].dev,
                       &data1),
                   WHAL_SUCCESS);
    WHAL_ASSERT_EQ(whal_Sensor_Read(
                       g_peripheralSensor[PERIPHERAL_SENSOR_BMI270].dev,
                       &data2),
                   WHAL_SUCCESS);

    /* Both reads should measure gravity */
    WHAL_ASSERT_EQ(AccelMagnitudeSq(&data1) >= GRAVITY_SQ_MIN, 1);
    WHAL_ASSERT_EQ(AccelMagnitudeSq(&data1) <= GRAVITY_SQ_MAX, 1);
    WHAL_ASSERT_EQ(AccelMagnitudeSq(&data2) >= GRAVITY_SQ_MIN, 1);
    WHAL_ASSERT_EQ(AccelMagnitudeSq(&data2) <= GRAVITY_SQ_MAX, 1);
}

void whal_Test_Bmi270_Sensor(void)
{
    WHAL_TEST_SUITE_START("bmi270");
    WHAL_TEST(Test_Bmi270_Decode);
    WHAL_TEST(Test_Bmi270_Read);
    WHAL_TEST(Test_Bmi270_ReadMultiple);
    WHAL_TEST_SUITE_END();
}
