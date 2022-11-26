#include <Wire.h>
#include "MPU_lib.h"
#include <Servo.h>
#include <math.h>

//declare pointer in global scope
MPU* daMPU;

//create servo object
Servo daServo;

void setup() {
  //0x68 (dec 104) is the i2c address of the 6050 mpu
  //0b00010000 are the power management bits: {000}-clock selection, {1}-disable temp sensor, {0}-reserved, {0}-no sleep cycle, {0}-disable sleep, {0}-do not reset registers
  //default accelRangeSetting is 0 which is most commonly used. Check the implementation for further details.
  daMPU = new MPU(104, 0b00010000);

  daMPU->initAccel();

  Serial.begin(9600);

  //connect to the servo
  daServo.attach(9);
}

void loop() {
  //delay(2000);
  
  daMPU->updateAccelData();
  daMPU->printAccelData();

  double position = 0;

  daMPU->calculateAngles();

  Serial.print("pitch: ");
  Serial.println(daMPU->getPitch());
  Serial.print("roll: ");
  Serial.println(daMPU->getRoll());

  if(Serial.available() > 1){
    //data was sent
    int newOffsetAngle = Serial.parseInt();//sets 0 if not integer
    Serial.print("new offset angle detected: ");
    Serial.println(newOffsetAngle);
    daMPU->setOffsetAngle(newOffsetAngle);
  }

  //actuate the motor but first check the angle bounds
  if(daMPU->getRoll() >= 0 && daMPU->getRoll() <= 180){
    daServo.write(daMPU->getRoll());
  }
  
}
