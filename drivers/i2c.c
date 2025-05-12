#include "drivers/i2c.h"
#include <msp430.h>


/**
 * i2c.c
 */

// Function for the generic start of a stransfer
bool start_transfer(uint8_t addr)
{
    bool success = false;
    while (UCB0CTL1 & UCTXSTP);                 // Wait for previous transmissions to end
    UCB0CTL1 |= UCTR + UCTXSTT;                 // Set to transmit mode and transmit start condition

    UCB0TXBUF = addr;                           // Set buffer to the address 

    // Essentially a loop to continue sending the message
    success = !(UCB0STAT & UCNACKIFG);
    if (success) {
        while (!(IFG2 & UCB0TXIFG));            // Wait for byte to be sent
        success = !(UCB0STAT & UCNACKIFG);
    }

    return success;
}


void stop_transfer()
{
    UCB0CTL1 |= UCTXSTP;                        // Send a stop condition
    while(UCB0CTL1 & UCTXSTP);                  // Wait until it is sent
}



bool read_reg(uint8_t addr, uint8_t* data)
{
    bool success = false;

    if(!start_transfer(addr)){                  // The first part of any read is to start a transfer to the addr
        return false;
    }

    UCB0CTL1 &= ~UCTR;                          // Put in recieve mode
    UCB0CTL1 |= UCTXSTT;                        // Send start condition

    while(UCB0CTL1 & UCTXSTT);                  // Wait for the bandwitdth to be free

    stop_transfer();                            // Stops transfer

    success = !(UCB0STAT & UCNACKIFG);          // If the status is not NAK

    while ((IFG2 & UCB0RXIFG) == 0);            // Wait for transmission to recieve
    data[0] = UCB0RXBUF;                        // Data

    return success;
}

bool read_reg_bytes(uint8_t addr, uint8_t *bytes, uint16_t byte_count)
{
    // Same method as read_reg mostly

    bool success = false;
    bool transfer_complete = false;

    if(!start_transfer(addr)){
        return false;
    }

    UCB0CTL1 &= ~UCTR;              
    UCB0CTL1 |= UCTXSTT;            

    while(UCB0CTL1 & UCTXSTT);      

    success = !(UCB0STAT & UCNACKIFG);

    uint16_t i;

    if(success){                                 // Only difference is here, where it loops per byte to read the data
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

bool write_reg(uint8_t addr, uint8_t data)
{
    bool success = true;

    if(!start_transfer(addr)){                      // Start transmission
        return false;
    }

    UCB0TXBUF = data;
    while (!(IFG2 & UCB0TXIFG));                    // Wait for bit to be sent
    success = !(UCB0STAT & UCNACKIFG);

    stop_transfer();
    return success;
}

bool write_reg_bytes(uint8_t addr, uint8_t *bytes, uint16_t byte_count)
{
    bool success = true;

    if(!start_transfer(addr)){
        return false;
    }

    uint16_t i;

    // Write data bits in a for loop
    for (i = 0; i < byte_count; i++) {
        UCB0TXBUF = bytes[i];
        while (!(IFG2 & UCB0TXIFG));        
        success = !(UCB0STAT & UCNACKIFG);
        if (!success) {
            break;
        }
    }

    stop_transfer();
    return success;
}


void i2c_init()
{

    UCB0CTL1 |= UCSWRST;                            // Put USCI in reset
    UCB0CTL1 |= UCSSEL_2;                           // Select SMCLK as the clock source
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;           // Selects UCSI to be i2c mode

    // Sets timing for 100KHz as 1MHz (SMLK) / 10 = 100k
    UCB0BR0 = 10;
    UCB0BR1 = 0;

    // To enable for I2C (secondary function)
    P1SEL |= BIT6 + BIT7;
    P1SEL2 |= BIT6 + BIT7;


    UCB0I2CSA = I2C_RECIEVER_ADDR;                   // Reciever address

    UCB0CTL1 &= ~UCSWRST;                            // Release from reset, enabling I2C
}


void msp430_init()
{
    WDTCTL = WDTPW | WDTHOLD;                        // stop watchdog timer

    BCSCTL1 = CALBC1_1MHZ;                           // Set freq to 1MHz
    DCOCTL = CALDCO_1MHZ;
}
