#include "IMU.h"
#include <Wire.h>
#include "LSM9DS1.h"

void writeTo(int device, byte address, byte val)
{
  Wire.beginTransmission(device); // start transmission to device
  Wire.write(address);            // send register address
  Wire.write(val);                // send value to write
  Wire.endTransmission();         // end transmission
}

void readFrom(int device, byte address, int num, byte buff[])
{
  Wire.beginTransmission(device); // start transmission to device
  Wire.write(address);            // sends address to read from
  Wire.endTransmission();         // end transmission

  Wire.beginTransmission(device); // start transmission to device (initiate again)
  Wire.requestFrom(device, num);  // request 6 bytes from device

  int i = 0;
  while (Wire.available()) // device may send less than requested (abnormal)
  {
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); // end transmission
}

/*******************************************
 * enableIMU -- enables Accelerometer,
 * magntometer & gyroscope
 *
 * Parameters
 *  None
 *
 * Returns
 *  None
 *******************************************/
void enableIMU(int sda, int scl)
{
  Wire.begin(sda, scl);
  delay(1000);
  // Enable the gyroscope
  writeTo(LSM9DS1_GYR_ADDRESS, LSM9DS1_CTRL_REG4, 0b00111000);    // z, y, x axis enabled for gyro
  writeTo(LSM9DS1_GYR_ADDRESS, LSM9DS1_CTRL_REG1_G, 0b10111000);  // Gyro ODR = 476Hz, 2000 dps
  writeTo(LSM9DS1_GYR_ADDRESS, LSM9DS1_ORIENT_CFG_G, 0b10111000); // Swap orientation

  // Enable the accelerometer
  writeTo(LSM9DS1_ACC_ADDRESS, LSM9DS1_CTRL_REG5_XL, 0b00111000); // z, y, x axis enabled for accelerometer
  writeTo(LSM9DS1_ACC_ADDRESS, LSM9DS1_CTRL_REG6_XL, 0b00101000); // +/- 16g

  // Enable the magnetometer
  writeTo(LSM9DS1_MAG_ADDRESS, LSM9DS1_CTRL_REG1_M, 0b10011100); // Temp compensation enabled,Low power mode mode,80Hz ODR
  writeTo(LSM9DS1_MAG_ADDRESS, LSM9DS1_CTRL_REG2_M, 0b01000000); // +/-12gauss
  writeTo(LSM9DS1_MAG_ADDRESS, LSM9DS1_CTRL_REG3_M, 0b00000000); // continuos update
  writeTo(LSM9DS1_MAG_ADDRESS, LSM9DS1_CTRL_REG4_M, 0b00000000); // lower power mode for Z axis
}

/*******************************************
 * readACC -- enables Accelerometer,
 * magntometer & gyroscope
 *
 * Parameters
 *  None
 *
 * Returns
 *  None
 *******************************************/
void readACC(byte buff[])
{
  readFrom(LSM9DS1_ACC_ADDRESS, 0x80 | LSM9DS1_OUT_X_L_XL, 6, buff);
}

void readMAG(byte buff[])
{
  readFrom(LSM9DS1_MAG_ADDRESS, 0x80 | LSM9DS1_OUT_X_L_M, 6, buff);
}

void readGYR(byte buff[])
{
  readFrom(LSM9DS1_GYR_ADDRESS, 0x80 | LSM9DS1_OUT_X_L_G, 6, buff);
}
