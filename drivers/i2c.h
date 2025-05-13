/*
 * i2c.h
 *
 *  Created on: 12 May 2025
 *      Author: Hamid
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stdbool.h>

#define I2C_RECIEVER_ADDR 0x68

typedef enum
{
    ADDR_SIZE_8BIT,
    ADDR_SIZE_16BIT
} addr_size_t;

typedef enum
{
    REG_SIZE_8BIT,
    REG_SIZE_16BIT,
    REG_SIZE_32BIT
} reg_size_t;

bool start_transfer(addr_size_t addr_size, uint16_t addr);
void stop_transfer();

bool read_reg(addr_size_t addr_size, uint16_t addr, reg_size_t reg_size, uint8_t* data);
bool write_reg(addr_size_t addr_size, uint16_t addr, reg_size_t reg_size, uint32_t data);

bool read_reg_addr8_data8(uint8_t addr, uint8_t* data);
bool read_reg_addr8_data16(uint8_t addr, uint16_t* data);
bool read_reg_addr8_data32(uint8_t addr, uint32_t* data);

bool read_reg_addr16_data8(uint16_t addr, uint8_t* data);
bool read_reg_addr16_data16(uint16_t addr, uint16_t* data);
bool read_reg_addr16_data32(uint16_t addr, uint32_t* data);

bool write_reg_addr8_data8(uint8_t addr, uint8_t data);
bool write_reg_addr8_data16(uint8_t addr, uint16_t data);
bool write_reg_addr8_data32(uint8_t addr, uint32_t data);

bool write_reg_addr16_data8(uint16_t addr, uint8_t data);
bool write_reg_addr16_data16(uint16_t addr, uint16_t data);
bool write_reg_addr16_data32(uint16_t addr, uint32_t data);


bool read_reg_bytes(addr_size_t addr_size, uint8_t addr, uint8_t *bytes, uint16_t byte_count);
bool write_reg_bytes(addr_size_t addr_size, uint8_t addr, uint8_t *bytes, uint16_t byte_count);

bool read_addr8_bytes(uint8_t start_addr, uint8_t *bytes, uint16_t byte_count);


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
