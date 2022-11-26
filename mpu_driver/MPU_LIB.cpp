#include "MPU_LIB.h"
#include "Arduino.h"
#include <Wire.h>

MPU::MPU(int address, int powerSettings){
  this->mpu_address = address;
  this->powerSettings = powerSettings;
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
  Wire.begin();
  Wire.beginTransmission(this->mpu_address);
  Wire.write(0x6B);//access the power management register (PWR_MGMT_1)
  Wire.write(this->powerSettings);//set the bits. The register takes 8 bits
  Wire.endTransmission(true);//true means to send a stop message to mpu to release the i2c bus
}
