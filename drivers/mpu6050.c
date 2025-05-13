#include "drivers/i2c.h"
#include <msp430.h>

// MPU6050 Register Addresses
#define MPU6050_PWR_MGMT_1     0x6B
#define MPU6050_SMPLRT_DIV     0x19
#define MPU6050_CONFIG         0x1A
#define MPU6050_GYRO_CONFIG    0x1B
#define MPU6050_ACCEL_CONFIG   0x1C
#define MPU6050_WHO_AM_I       0x75

#define MPU6050_ACCEL          0x3B
#define MPU6050_TEMP           0x41
#define MPU6050_GYRO           0x43

bool mpu6050_init(){
    bool success = true;

    success &= write_reg_addr8_data8(MPU6050_PWR_MGMT_1, 0x00);  // Wake up device
    __delay_cycles(50000);  // Give enough time after waking

    success &= write_reg_addr8_data8(MPU6050_CONFIG, 0x01); // Config reg: DLP_CFG = 1
    success &= write_reg_addr8_data8(MPU6050_SMPLRT_DIV, 0x09);  // SMPLRT_DIV = 9 → Sample rate = 1kHz / (1 + 9) = 100Hz

    success &= write_reg_addr8_data8(MPU6050_GYRO_CONFIG, 0x00);
    success &= write_reg_addr8_data8(MPU6050_GYRO_CONFIG, 0x00);

    return success;
}

bool mpu6050_read_accel(accel_data_t* accel)
{
    uint8_t raw_data[6];
    bool success = read_addr8_bytes(MPU6050_ACCEL, raw_data, 6);
    if (!success) return false;

    accel->x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    accel->y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    accel->z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

    return true;
}

bool mpu6050_read_temp(temp_data_t* temp)
{
    uint8_t raw_data[2];
    bool success = read_addr8_bytes(MPU6050_TEMP, raw_data, 2);
    if (!success) return false;

    temp->temperature = (int16_t)((raw_data[0] << 8) | raw_data[1]);

    return true;
}

bool mpu6050_read_gyro(gyro_data_t* gyro)
{
    uint8_t raw_data[6];
    bool success = read_addr8_bytes(MPU6050_GYRO, raw_data, 6);
    if (!success) return false;

    gyro->x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    gyro->y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    gyro->z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

    return true;
}
