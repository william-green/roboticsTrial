#include "MPU_LIB.h"
#include "Arduino.h"
#include <Wire.h>
#include <math.h>

MPU::MPU(int address, int powerSettings){
  this->mpu_address = address;//mpu address
  this->powerSettings = powerSettings;//power bit settings
  this->accelRangeSettings = 0;//default bit pattern

  //set the initial accelerometer read in values
  this->accelX = 0;
  this->accelY = 0;
  this->accelZ = 0;

  //init angles
  this->pitch = 0;
  this->roll = 0;
}

double MPU::getAccelX(){
  return this->accelX;
}

double MPU::getAccelY(){
  return this->accelX;
}

double MPU::getAccelZ(){
  return this->accelX;
}

void MPU::setAccelRange(int accelRangeSettings){
  this->accelRangeSettings = accelRangeSettings;
}

int MPU::getAccelRange(){
  return this->accelRangeSettings;
}

int MPU::getAddress(){
  return this->mpu_address;
}

void MPU::setPowerSettings(int powerSettings){
  this->powerSettings = powerSettings;
}

int MPU::getPowerSettings(){
  return this->powerSettings;
}

void MPU::updateAccelData(){
  //connect to the mpu
  Wire.beginTransmission(this->mpu_address);

  //per the data sheet there are two registers per value
  /*
  Accelerometer Data
  X Axis: 3B, 3C
  Y Axis: 3D, 3E
  Z Axis: 3F, 40
  */
  
  //read data from the x axis into temp variables
  int tempX = 0;
  int tempY = 0;
  int tempZ = 0;
    
  //first register of x axis
  Wire.write(0x3B);

  //i2c spec expects a packet to keep the connection alive
  Wire.endTransmission(false);

  //request 6 bytes (2 bytes per axis) from the accelerometer, keep connection alive
  Wire.requestFrom(this->mpu_address, 6, true);//Wire.requestFrom(address, quantity, stop)
    
  //returns the bytes requested from the peripheral controller
  tempX = (Wire.read() << 8 | Wire.read()) - 250;
  tempY = (Wire.read() << 8 | Wire.read()) + 36;
  tempZ = (Wire.read() << 8 | Wire.read()) + 1200;

  this->accelX = tempX;
  this->accelY = tempY;
  this->accelZ = tempZ;
}

void MPU::printAccelData(){
  Serial.print("X: ");
  Serial.println(this->accelX);
  Serial.print("Y: ");
  Serial.println(this->accelY);
  Serial.print("Z: ");
  Serial.println(this->accelZ);
}

double MPU::getPitch(){
  return this->pitch;
}

double MPU::getRoll(){
  return this->roll;
}

void MPU::calculateAngles(){
  double x = this->accelX;
  double y = this->accelY;
  double z = this->accelZ;
  this->pitch = atan(x/sqrt((y*y) + (z*z)));
  this->roll = atan(y/sqrt((x*x) + (z*z)));
  //convert radians into degrees
  this->pitch = pitch * (180.0/3.14);
  this->roll = (roll * (180.0/3.14)) + 90;
}

void MPU::initAccel(){
  //connect to mpu
  Wire.begin();
  Wire.beginTransmission(this->mpu_address);

  //set the power management registers
  Wire.write(0x6B);//access the power management register (PWR_MGMT_1)
  Wire.write(this->powerSettings);//set the bits. The register takes 8 bits
  Wire.endTransmission(true);//true means to send a stop message to mpu to release the i2c bus

  //initialize the accelerometer
  Wire.write(0x1C);//the accelerometer config register
  Wire.write(this->accelRangeSettings);//set the accelerometer config register
  Wire.endTransmission(true);//release the i2c bus
}
