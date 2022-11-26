#ifndef MPU_LIB_H
#define MPU_LIB_H

#include "Arduino.h"

class MPU{
  public:
  MPU(int address);
  int getAddress();
  void initAccel();
  void setPowerSettings(int powerSettings);
  int getPowerSettings();
  private:
  int mpu_address;
  int powerSettings;
};

#endif
