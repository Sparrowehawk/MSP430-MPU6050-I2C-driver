#include <msp430.h> 


/**
 * main.c
 * To see the I2C communication, either toggle an LED or use a logic analyzer
 * As an MPU6050 is the device of choice, we know from the datasheet that the who_am_i reg is 0x75 and it should return 0x65, the reciever addr
 */

 

static void msp430_init()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    BCSCTL1 = CALBC1_1MHZ;      // Set freq to 1MHz
    DCOCTL = CALDCO_1MHZ;
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


    UCB0I2CSA = 0x68;          // Slave address

    UCB0CTL1 &= ~UCSWRST;      // Release from reset, enabling I2C
}

void i2c_write(unsigned char reg_addr)
{
    while(UCB0CTL1 & UCTXSTP);      // Wait for all prev communication to end

    UCB0CTL1 |= UCTR + UCTXSTT;     // Set to transmit mode and transmit start conditio

    while(!(UCB0TXIFG & IFG2));     // Wait for the bandwidth to be ready

    UCB0TXBUF= reg_addr;            // Write to the reg of choice
                                    // as it is the WHO_AM_I reg, it just needs a sigle bit to return the value

    while (!(IFG2 & UCB0TXIFG));    // Wait for the transmission
}


unsigned char i2c_read(){

    UCB0CTL1 &= ~UCTR;              // Put in recieve mode
    UCB0CTL1 |= UCTXSTT;            // Send start condition 

    while(UCB0CTL1 & UCTXSTT);      // Wait for the bandwitdth to be free
    UCB0CTL1 |= UCTXNACK;           // Send a NACK
    while (!(IFG2 & UCB0RXIFG));    // Wait for the transmission

    unsigned char value = UCB0RXBUF;// Buffer value is the one we want (return from WHO_AM_I reg)

    UCB0CTL1 |= UCTXSTP;            // Send a stop condition
    while(UCB0CTL1 & UCTXSTP);      // Send it

    return value;
}

// TESTING CODE TO PROVE I2C WORKS
unsigned char mpu6050_whoami()
{
    unsigned char value;

    while (UCB0CTL1 & UCTXSTP);            // Ensure no STOP is pending
    UCB0CTL1 |= UCTR + UCTXSTT;            // I2C start + transmit mode
    while (!(IFG2 & UCB0TXIFG));           // Wait for TX buffer

    UCB0TXBUF = 0x75;                      // Send WHO_AM_I register address
    while (!(IFG2 & UCB0TXIFG));           // Wait for buffer

    UCB0CTL1 &= ~UCTR;                     // Switch to receiver mode
    UCB0CTL1 |= UCTXSTT;                   // Repeated start

    while (UCB0CTL1 & UCTXSTT);            // Wait for start to complete
    UCB0CTL1 |= UCTXNACK;                  // Send NACK after next byte
    while (!(IFG2 & UCB0RXIFG));           // Wait for byte

    value = UCB0RXBUF;                     // Read byte
    UCB0CTL1 |= UCTXSTP;                   // Send STOP
    while (UCB0CTL1 & UCTXSTP);            // Wait for stop


    return value;
}


int main(void)          
{
    msp430_init();
    i2c_init();

   unsigned char who_am_i_value = 0;

    // Write the register address (0x75) to start reading WHO_AM_I
    i2c_write(0x75);

    // Read the WHO_AM_I value
    who_am_i_value = i2c_read();

    // Turn LED on if the value is as expected
    if (who_am_i_value == 0x68) 
        P1OUT |= BIT0;  

    while(1);  
}
