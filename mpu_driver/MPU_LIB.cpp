#include "MPU_LIB.h"
#include "Arduino.h"
#include <Wire.h>

MPU::MPU(int address, int powerSettings){
  this->mpu_address = address;//mpu address
  this->powerSettings = powerSettings;//power bit settings
  this->accelRangeSettings = 0;//default bit pattern
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
}
