#include <Wire.h>
#include "MPU_lib.h"

//declare pointer in global scope
MPU* daMPU;

void setup() {
  //0x68 (dec 107) is the i2c address of the 6050 mpu
  //0b00010000 are the power management bits: {000}-clock selection, {1}-disable temp sensor, {0}-reserved, {0}-no sleep cycle, {0}-disable sleep, {0}-do not reset registers
  daMPU = new MPU(107, 0b00010000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
