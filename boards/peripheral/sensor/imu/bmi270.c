#include "bmi270.h"
#include <wolfHAL/sensor/imu/bmi270.h>
#include <wolfHAL/sensor/imu/bmi270_config_data.h>
#include "board.h"

/*
 * Bosch BMI270 — 6-axis IMU (accelerometer + gyroscope)
 *
 * - I2C address: 0x68 (SDO low)
 * - Standard mode (100 kHz) or Fast mode (400 kHz)
 * - Requires 8192-byte config blob upload during init
 */

whal_I2c_ComCfg g_bmi270ComCfg = {
    .freq = 400000, /* 400 kHz fast mode */
    .addr = WHAL_BMI270_ADDR_LOW,
    .addrSz = 7,
};

whal_Sensor g_whalBmi270 = {
    .driver = &whal_Bmi270_Driver,
    .cfg = &(whal_Bmi270_Cfg) {
        .i2c = &g_whalI2c,
        .comCfg = &g_bmi270ComCfg,
        .configData = whal_bmi270_config_data,
        .configDataSz = WHAL_BMI270_CONFIG_DATA_SZ,
        .DelayMs = Board_WaitMs,
    },
};
