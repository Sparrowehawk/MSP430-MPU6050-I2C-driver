#include "drivers/i2c.h"
#include <msp430.h>

/**
 * main.c
 * Main driver code using the I2C driver code in combination with specific code for the MPU6050
 * Use a logic analyzer to analyse values
 */


#define LED       BIT6              // Red LED on P2.1 for success indication

// Very basic LED code to blink to show the status of the system
void led_init()
{
    P2DIR |= LED;
    P2OUT &= LED;
}

void toggle_leds()
{
    P2OUT ^= LED;
}


int main(void)
{
    msp430_init();                  // System init
    i2c_init();                     // I2C setup
    led_init();                     // LED pin setup

    bool success = mpu6050_init();  // Init MPU6050

    accel_data_t accel;
    temp_data_t temp;
    gyro_data_t gyro;

    // Run the code in a loop once init is successful
    if (success)
    {
        while (1)
        {
            if (mpu6050_read_accel(&accel))
            {
                toggle_leds();
            }

            if (mpu6050_read_temp(&temp))
            {
                toggle_leds();
            }

            if (mpu6050_read_gyro(&gyro))
            {
                toggle_leds();
            }

            __delay_cycles(100000);     // Delay between readings
        }
    }
    else
    {
        while (1)
        {
            P2OUT |= LED;               // Flat red for error
            __delay_cycles(1000000);
        }
    }

}
