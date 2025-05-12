/*
 * i2c.h
 * Header file containing all function and structs used across the board
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stdbool.h>

#define I2C_RECIEVER_ADDR 0x68

bool start_transfer(uint8_t addr);
void stop_transfer();

bool read_reg(uint8_t addr, uint8_t* data);
bool write_reg(uint8_t addr, uint8_t data);

bool read_reg_bytes(uint8_t addr, uint8_t *bytes, uint16_t byte_count);
bool write_reg_bytes(uint8_t addr, uint8_t *bytes, uint16_t byte_count);

void i2c_init();
void msp430_init();

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} accel_data_t;

typedef struct {
    int16_t temperature;
} temp_data_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} gyro_data_t;

bool mpu6050_init();

bool mpu6050_read_accel(accel_data_t* accel);
bool mpu6050_read_temp(temp_data_t* temp);
bool mpu6050_read_gyro(gyro_data_t* gyro);

#endif /* I2C_H_ */
