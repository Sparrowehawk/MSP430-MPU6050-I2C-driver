#include "drivers/i2c.h"
#include <msp430.h>

// MPU6050 Register Addresses
#define MPU6050_PWR_MGMT_1     0x6B
#define MPU6050_SMPLRT_DIV     0x19
#define MPU6050_CONFIG         0x1A
#define MPU6050_GYRO_CONFIG    0x1B
#define MPU6050_ACCEL_CONFIG   0x1C
#define MPU6050_WHO_AM_I       0x75

// Reg addr to collect data from
#define MPU6050_ACCEL          0x3B
#define MPU6050_TEMP           0x41
#define MPU6050_GYRO           0x43

// Used to give the config reg values specified in datasheet
bool mpu6050_init(){
    bool success = true;

    success &= write_reg(MPU6050_PWR_MGMT_1, 0x00);  // Wakes up all measuring  registers
    __delay_cycles(50000);                           // Give enough time to wake reg up

    success &= write_reg(MPU6050_CONFIG, 0x01);      // Config reg: DLP_CFG = 1
    success &= write_reg(MPU6050_SMPLRT_DIV, 0x09);  // SMPLRT_DIV = 9 → Sample rate = 1kHz / (1 + 9) = 100Hz

    // Set both ACCEL and GYRO to be measured with the lowest range but the highst sensitivity
    success &= write_reg(MPU6050_ACCEL_CONFIG, 0x00);
    success &= write_reg(MPU6050_GYRO_CONFIG, 0x00);

    return success;
}


/*
 * All the of the reg reading functions work the same
 * Reads the  first register and the consequative ones specified in the datasheet
 * Combination canbe used to gain the full values needed
 */ 

bool mpu6050_read_accel(accel_data_t* accel)
{
    uint8_t raw_data[6];
    bool success = read_reg_bytes(MPU6050_ACCEL, raw_data, 6);
    if (!success) return false;

    // Finds each value by taking the first 8 bits, shifting to the left and inserting the least sig 8 bits
    accel->x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    accel->y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    accel->z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

    // Accel is given with a sensitivity of 16384 LSB/g
    // Therefore divide the values by 16384f to get true value

    return true;
}

bool mpu6050_read_temp(temp_data_t* temp)
{
    uint8_t raw_data[2];
    bool success = read_reg_bytes(MPU6050_TEMP, raw_data, 2);
    if (!success) return false;

    temp->temperature = (int16_t)((raw_data[0] << 8) | raw_data[1]);

    // Temp is given as 2 hex values, combined to give a 16 signed bit value
    // Tempreture = (signed 16 bit value / 340) + 36.53

    return true;
}

bool mpu6050_read_gyro(gyro_data_t* gyro)
{
    uint8_t raw_data[6];
    bool success = read_reg_bytes(MPU6050_GYRO, raw_data, 6);
    if (!success) return false;

    gyro->x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    gyro->y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    gyro->z = (int16_t)((raw_data[4] << 8) | raw_data[5]);


    // Gyro is given with a sensitivity of 131 LSB/g
    // Therefore divide the values by 131f to get true value

    return true;
}
