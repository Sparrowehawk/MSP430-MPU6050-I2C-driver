#include "drivers/i2c.h"
#include <msp430.h>


/**
 * i2c.c
 */


bool start_transfer(addr_size_t addr_size, uint16_t addr)
{
    bool success = false;
    while (UCB0CTL1 & UCTXSTP);
    UCB0CTL1 |= UCTR + UCTXSTT;     // Set to transmit mode and transmit start condition

    switch(addr_size) {
    case ADDR_SIZE_8BIT:
        UCB0TXBUF = addr & 0xFF;
        break;
    case ADDR_SIZE_16BIT:
        UCB0TXBUF = (addr >> 8) & 0xFF;
        break;
    }

    while (UCB0CTL1 & UCTXSTT); // MAY BREAK THINGS

    // Essentially a loop to continue sending the message
    success = !(UCB0STAT & UCNACKIFG);
    if (success) {
        while (!(IFG2 & UCB0TXIFG)); // Wait for byte to be sent
        success = !(UCB0STAT & UCNACKIFG);
    }

    if (success){
        switch(addr_size){
        case ADDR_SIZE_8BIT:
            break;
        case ADDR_SIZE_16BIT:
            UCB0TXBUF = addr & 0xFF;
            while (!(IFG2 & UCB0TXIFG)); // Wait for byte to be sent
            success = !(UCB0STAT & UCNACKIFG);
            break;
        }
    }

    return success;
}


void stop_transfer()
{
    UCB0CTL1 |= UCTXSTP;            // Send a stop condition
    while(UCB0CTL1 & UCTXSTP);      // Send it
}



bool read_reg(addr_size_t addr_size, uint16_t addr, reg_size_t reg_size, uint8_t* data)
{
    bool success = false;

    if(!start_transfer(addr_size, addr)){
        return false;
    }

    UCB0CTL1 &= ~UCTR;              // Put in recieve mode
    UCB0CTL1 |= UCTXSTT;            // Send start condition

    while(UCB0CTL1 & UCTXSTT);      // Wait for the bandwitdth to be free

    success = !(UCB0STAT & UCNACKIFG);

    if(success){
       switch(reg_size){
       case REG_SIZE_8BIT:
           break;
       case REG_SIZE_16BIT:
           while ((IFG2 & UCB0RXIFG) == 0);
           data[1] = UCB0RXBUF;
           break;
       case REG_SIZE_32BIT:
           while ((IFG2 & UCB0RXIFG) == 0);
           data[3] = UCB0RXBUF;
           while ((IFG2 & UCB0RXIFG) == 0);
           data[2] = UCB0RXBUF;
           while ((IFG2 & UCB0RXIFG) == 0);
           data[1] = UCB0RXBUF;
           break;
       }
       stop_transfer();

       while ((IFG2 & UCB0RXIFG) == 0);
       data[0] = UCB0RXBUF;
    }

    return success;
}

bool read_reg_bytes(addr_size_t addr_size, uint8_t addr, uint8_t *bytes, uint16_t byte_count)
{
    bool success = false;
    bool transfer_complete = false;

    if(!start_transfer(addr_size, addr)){
        return false;
    }

    UCB0CTL1 &= ~UCTR;              // Put in recieve mode
    UCB0CTL1 |= UCTXSTT;            // Send start condition

    while(UCB0CTL1 & UCTXSTT);      // Wait for the bandwitdth to be free

    success = !(UCB0STAT & UCNACKIFG);

    uint16_t i;

    if(success){
        for(i = 0; i < byte_count; i++){
            if(i + 1 == byte_count){
                stop_transfer();
                transfer_complete = true;
            }

            success = !(UCB0STAT & UCNACKIFG);
            if(success){
                while ((IFG2 & UCB0RXIFG) == 0);
                bytes[i] = UCB0RXBUF;
            } else {
                break;
            }
        }
    }

    if(!transfer_complete){
        stop_transfer();
    }

    return success;

}

bool read_reg_addr8_data8(uint8_t addr, uint8_t* data)
{
    return read_reg(ADDR_SIZE_8BIT, addr, REG_SIZE_8BIT, data);
}

bool read_reg_addr8_data16(uint8_t addr, uint16_t* data)
{
    return read_reg(ADDR_SIZE_8BIT, addr, REG_SIZE_16BIT, (uint8_t*) data);
}

bool read_reg_addr8_data32(uint8_t addr, uint32_t* data)
{
    return read_reg(ADDR_SIZE_8BIT, addr, REG_SIZE_32BIT, (uint8_t*) data);
}

bool read_reg_addr16_data8(uint16_t addr, uint8_t* data)
{
    return read_reg(ADDR_SIZE_16BIT, addr, REG_SIZE_8BIT, data);
}

bool read_reg_addr16_data16(uint16_t addr, uint16_t* data)
{
    return read_reg(ADDR_SIZE_16BIT, addr, REG_SIZE_16BIT, (uint8_t*) data);
}

bool read_reg_addr16_data32(uint16_t addr, uint32_t* data)
{
    return read_reg(ADDR_SIZE_16BIT, addr, REG_SIZE_32BIT, (uint8_t*) data);
}

bool read_addr8_bytes(uint8_t start_addr, uint8_t *bytes, uint16_t byte_count){
    return read_reg_bytes(ADDR_SIZE_8BIT, start_addr, bytes, byte_count);
}

bool write_reg(addr_size_t addr_size, uint16_t addr, reg_size_t reg_size, uint32_t data)
{
    bool success = false;;

    if(!start_transfer(addr_size, addr)){
        return false;
    }

    switch(reg_size){
    case REG_SIZE_8BIT:
        success = true;
        break;
    case REG_SIZE_16BIT:
        UCB0TXBUF = (data>>8) & 0xFF;
        while (!(IFG2 & UCB0TXIFG));        // Wait for bit to be sent
        success = !(UCB0STAT & UCNACKIFG);
        break;
    case REG_SIZE_32BIT:
        UCB0TXBUF = (data>>24) & 0xFF;
        while (!(IFG2 & UCB0TXIFG));        // Wait for bit to be sent
        UCB0TXBUF = (data>>16) & 0xFF;
        while (!(IFG2 & UCB0TXIFG));        // Wait for bit to be sent
        UCB0TXBUF = (data>>8) & 0xFF;
        while (!(IFG2 & UCB0TXIFG));        // Wait for bit to be sent
        success = !(UCB0STAT & UCNACKIFG);
        break;
    }

    if(success){
        UCB0TXBUF = data;
        while (!(IFG2 & UCB0TXIFG));        // Wait for bit to be sent
        success = !(UCB0STAT & UCNACKIFG);
    }

    stop_transfer();
    return success;
}

bool write_reg_bytes(addr_size_t addr_size, uint8_t addr, uint8_t *bytes, uint16_t byte_count)
{
    bool success = true;

    if(!start_transfer(addr_size, addr)){
        return false;
    }

    uint16_t i;

    // Write data bytes
    for (i = 0; i < byte_count; i++) {
        UCB0TXBUF = bytes[i];
        while (!(IFG2 & UCB0TXIFG));        // Wait for byte to be sent
        success = !(UCB0STAT & UCNACKIFG);
        if (!success) {
            break;
        }
    }

    stop_transfer();
    return success;
}

bool write_reg_addr8_data8(uint8_t addr, uint8_t data)
{
    return write_reg(ADDR_SIZE_8BIT, addr, REG_SIZE_8BIT, data);
}

bool write_reg_addr8_data16(uint8_t addr, uint16_t data)
{
    return write_reg(ADDR_SIZE_8BIT, addr, REG_SIZE_16BIT, data);
}

bool write_reg_addr8_data32(uint8_t addr, uint32_t data)
{
    return write_reg(ADDR_SIZE_8BIT, addr, REG_SIZE_32BIT, data);
}

bool write_reg_addr16_data8(uint16_t addr, uint8_t data)
{
    return write_reg(ADDR_SIZE_16BIT, addr, REG_SIZE_8BIT, data);
}

bool write_reg_addr16_data16(uint16_t addr, uint16_t data)
{
    return write_reg(ADDR_SIZE_16BIT, addr, REG_SIZE_16BIT, data);
}

bool write_reg_addr16_data32(uint16_t addr, uint32_t data)
{
    return write_reg(ADDR_SIZE_16BIT, addr, REG_SIZE_32BIT, data);
}

void i2c_init()
{

    UCB0CTL1 |= UCSWRST;                  // Put USCI in reset
    UCB0CTL1 |= UCSSEL_2;                 // Select SMCLK as the clock source
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC; // Selects UCSI to be i2c mode

    // Sets timing for 100KHz as 1MHz (SMLK) / 10 = 100k
    UCB0BR0 = 10;
    UCB0BR1 = 0;

    // To enable for I2C (secondary function)
    P1SEL |= BIT6 + BIT7;
    P1SEL2 |= BIT6 + BIT7;


    UCB0I2CSA = I2C_RECIEVER_ADDR;          // Slave address

    UCB0CTL1 &= ~UCSWRST;      // Release from reset, enabling I2C
}


void msp430_init()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    BCSCTL1 = CALBC1_1MHZ;      // Set freq to 1MHz
    DCOCTL = CALDCO_1MHZ;
}

