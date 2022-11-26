#ifndef MPU_LIB_H
#define MPU_LIB_H

#include "Arduino.h"

class MPU{
  
  public:
  MPU(int address, int powerSettings);
  int getAddress();
  void setAccelRange(int accelRangeSettings);
  int getAccelRange();
  void initAccel();
  void setPowerSettings(int powerSettings);
  int getPowerSettings();
  double getAccelX();
  double getAccelY();
  double getAccelZ();
  void updateAccelData();
  void printAccelData();
  void calculateAngles();
  double getPitch();
  double getRoll();
  void setOffsetAngle(int angle);
  int getOffsetAngle();
  private:
  int mpu_address;
  int powerSettings;
  int accelRangeSettings;

  //arduino does not allow advanced data structures so using properties
  int accelX;
  int accelY;
  int accelZ;

  //offset angle (in degrees)
  int offsetAngle;

  //angles
  double pitch;
  double roll;
};

#endif
