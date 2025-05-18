# I2C Driver for MSP430G2553 with an MPU6050 

A very basic I2C driver implementation that enables communication between an MSP430G2553 and an MPU6050. While tailored for the MPU6050, parts of the code are modular enough to be adapted for other I2C devices.

## Parts Used
- MSP430G2553
- MPU6050
- Logic Analyzer (used to see the communications)

## Setup

1. **Wiring:**
   - Connect the MPU6050 to the MSP430 via the I2C pins and the relevent power pins
     - `P1.6` — I2C Clock (SCL)
     - `P1.7` — I2C Data (SDA)

2. **Flashing the MSP430:**
   - Use Code Composer Studio or `mspdebug` to flash the code.
   - Make sure the USCI_B0 module is used for I2C communication in the MSP430.

3. **Running:**
   - Once the code is running, the on board LED (P2.1) can be used to verify I2C communication.
   - IMU data (acceleration, temperature, gyroscope) will be read over I2C.
   - Can be seen visually via a logic analyzer

## What the Data Means

### Accelerometer
- The MPU6050 outputs raw accelerometer values as signed 16-bit integers across three axes: X, Y, and Z
- Individually, they're given as 6 8-bit hex values, they can be combined in pairs to get the X, Y and Z values
- These values can be converted to `g` depending on the sensitivity set during initialisation (found in the register map as well)
  - For ±2g:  
     - `Acceleration (g) = Raw Value / 16384.0f`
    

### Temperature
- The temperature is reported as a signed 16-bit value
- Example: If bytes received are `0xF2` and `0x6D`, combine to get `0xF26D`
- Convert to decimal:
   - `0xF26D` = `-3475`
- Use the formula from the MPU6050 Register Map:
   - `Temperature (°C) = (Raw Value / 340.0) + 36.53`
   - `-3475 / 340 + 36.53 ≈ 26.31°C`

### Gyroscope
- Gyroscope data is also provided as signed 16-bit values for the X, Y, and Z axes
- Individually, they're given as 6 8-bit hex values, they can be combined in pairs to get the X, Y and Z values
- To convert raw values to degrees per second (°/s), use:
  - For ±250°/s:
    - `Gyro (°/s) = Raw Value/131.0f`
    

## Notes

- The driver uses polling-based I2C transfers (not interrupt-driven).

---

## License
MIT License

---

## Author
[Sparrowehawk](https://github.com/Sparrowehawk)
