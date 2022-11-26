#include <Wire.h>
#include "MPU_lib.h"

//declare pointer in global scope
MPU* daMPU;

void setup() {
  //0x68 (dec 104) is the i2c address of the 6050 mpu
  //0b00010000 are the power management bits: {000}-clock selection, {1}-disable temp sensor, {0}-reserved, {0}-no sleep cycle, {0}-disable sleep, {0}-do not reset registers
  //default accelRangeSetting is 0 which is most commonly used. Check the implementation for further details.
  daMPU = new MPU(104, 0b00010000);
}

void loop() {

}
