# I2C Driver for MSP430G2553 with an MPU6050

A very basic I2C driver implementation that enables communication between an MSP430G2553 microcontroller and an MPU6050 IMU sensor. While tailored for the MPU6050, parts of the code are modular enough to be adapted for other I2C devices.

## Parts Used
- [MSP430G2553](https://www.ti.com/product/MSP430G2553)
- [MPU6050 Accelerometer + Gyroscope IMU](https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/)
- Logic Analyzer (optional but useful for debugging I2C communication)

## Setup

1. **Wiring:**
   - Connect the MPU6050 to the MSP430 using the following pin configuration:
     - `P1.6` — I2C Clock (SCL)
     - `P1.7` — I2C Data (SDA)
   - Power the MPU6050 appropriately:
     - `VCC` to `3.3V` or `5V` (depending on the board variant)
     - `GND` to `GND`

2. **Flashing the MSP430:**
   - Use Code Composer Studio or `mspdebug` to flash the code.
   - Make sure the USCI_B0 module is used for I2C communication in the MSP430.

3. **Running:**
   - Once the code is running, the logic analyzer can be used to verify I2C communication and data transactions.
   - IMU data (acceleration, temperature, gyroscope) will be read over I2C.

## What the Data Means

### Accelerometer
- The MPU6050 outputs raw accelerometer values as signed 16-bit integers across three axes: X, Y, and Z.
- These values can be converted to `g` units by using the configured full-scale range.
  - For ±2g:  
    \[
    \text{Acceleration (g)} = \frac{\text{Raw Value}}{16384.0}
    \]

### Temperature
- The temperature is reported as a signed 16-bit value.
- Example: If bytes received are `0xF2` and `0x6D`, combine to get `0xF26D`.
- Convert to decimal:
  - `0xF26D` = `-3475`
- Use the formula from the [MPU6050 Register Map](https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf):
  \[
  \text{Temperature (°C)} = \frac{\text{Raw Value}}{340.0} + 36.53
  \]
  - `-3475 / 340 + 36.53 ≈ 26.31°C`

### Gyroscope
- Gyroscope data is also provided as signed 16-bit values for the X, Y, and Z axes.
- To convert raw values to degrees per second (°/s), use:
  - For ±250°/s:
    \[
    \text{Gyro (°/s)} = \frac{\text{Raw Value}}{131.0}
    \]

## Notes

- The driver uses polling-based I2C transfers (not interrupt-driven).
- Only 8-bit and 16-bit register addressing is supported; 32-bit addresses are rarely used and are not implemented here.
- This driver avoids using master/slave terminology where possible in favor of more inclusive language like controller/target.

---

## License
MIT License

---

## Author
[Sparrowehawk](https://github.com/Sparrowehawk)
