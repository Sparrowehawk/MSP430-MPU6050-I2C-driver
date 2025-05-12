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

#define I2C_SLAVE_ADDR 0x68

bool start_transfer(uint8_t addr);
void stop_transfer();

bool read_reg(uint8_t addr, uint8_t* data);
bool write_reg(uint8_t addr, uint8_t data);

bool read_reg_bytes(uint8_t addr, uint8_t *bytes, uint16_t byte_count);
bool write_reg_bytes(uint8_t addr, uint8_t *bytes, uint16_t byte_count);

void i2c_init();
static void msp430_init();

#endif /* I2C_H_ */
