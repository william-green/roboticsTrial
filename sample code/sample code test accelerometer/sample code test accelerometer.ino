//MPU 6050 Accelerometer and Gyroscope Exame
//Written by Taylor Schweizer

//We will be using the I2C communication protocol to communicate witht he MPU 6050
//The I2C protocol is setup in the Wire library - we need to include this library
#include<Wire.h>;

//The default address of the MPU is 0x68
const int MPU_addr = 0x68;


void setup() {
  //Here we will begin the I2C communication
  Serial.begin(38400);
  Serial.println ("Hit a key to start");     // signal initalization done
  while (Serial.available() == 0);
  Wire.begin();

  //Begin transmission with the MPU
  Wire.beginTransmission(MPU_addr);

  //According to the MPU 6050 Register Map, the PWR_MGMT_1 register allows us to configure the power mode of the MPU.
  //Looking at the register map page 40, PWR_MGMT_1 is located at 0x6B (107 in decimal, 01101011 in binary)
  //Think of each register in the MPU of having 8 bits that can be changed to change the settings.
  //For register 0x6B, the first three bits - Bit0 through Bit2 - are for selecting the clock source. A value of 0 sets the clock source to the internal 8MHZ oscillator, therefore the first three bits are all 0.
  //If we decided to use the PLL with external 19.2MHz reference, we would send 5 (decimal), so Bit0 would be 1, Bit1 would be 0, Bit2 would be 1, making 101 in binary which is 5 in decimal
  //Bit3 is labelled TEMP_DIS, and allows us to turn the temperature sensor on or off. A value of 1 turns if off, we might as well leave it on so Bit3 will be 0.
  //Bit4 is reserved, so we will just send 0 - the default
  //Bit5 is labelled CYCLE, which lets us specificy an automatic sleep cycle. We will set it to 0 and avoid the hassle of a sleep mode
  //Bit6 is labelled SLEEP, which lets us put it into sleep mode. Since we want the device on, we will set Bit6 to 0.
  //Bit7 is DEVICE_RESET, which resets all internal registers to their default values. We won't be doing that, so Bit7 is 0.

  //Based on the above, the 8 bits we will send to the MPU are all 0, so we can send any value of zero we want - hex 0x00, dec 0
  //This will power on the device and specify the initial power settings.
  //First, we tell the MPU which register we are writing to
  Wire.write(0x6B);

  //Second, we send the values that we discussed previously
  Wire.write(0x00);
  //Now, I'm not sure if you can continue sending data by sending a new register address and then the data, or if you need to end the transmission first. Just to be on the safe side, we will end the transmission and restart it.
  Wire.endTransmission(true);
  delay(50);
  Wire.beginTransmission(MPU_addr);
  //Now that we are done with that, let's go on and configure the accelerometer and gyroscope
  //Note: none of this is really essential: I'm basically just doing this to help you understand how to work with these register maps and stuff. This device is extremely powerful if it is used correctly, and practice makes perfect.

  //First, let's configure the Gyroscope. According to page 14 of the register map, the register holding the GYRO_CONFIG settings is located at 0x1B
  //Here is a breakdown of the bits:
  //Bit0 - Reserved, set to 0
  //Bit1 - Reserved, set to 0
  //Bit2 - Reserved, set to 0
  //Bit3 - FS_SEL
  //Bit4 - FS_SEL  -  This sets the full scale range of the gyroscope. Considering we will be running the wheel at a max of 100 RPM, that is 600°/s. Therefore we need to set FS_SEL to the +-1000°/s setting, which is decimal 2, hex 0x02, binary 10
  //Bit5 - ZG_ST - Activates self test of gyroscope Z axis if set to 1. We will set it to 0
  //Bit6 - YG_ST - same as ZG_ST but with Y axis
  //Bit7 - XG_ST - same as ZG_ST but with X axis

  //According to those values, we want to send the value 0 0 0 1 0 0 0 0, which is dec 16 hex 0x10
  //And remember, we want to send it to 0x1B
  Wire.write(0x1B);
  Wire.write(0x10);

  //End and restart transmission
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU_addr);

  //Next, let's do the accelerometer. I'll skip going through figuring out each bit, and just say I want to send 0x00 - there's almost no way we will experience more than 2g of acceleration on the wheelchair, so the full scale range can be +-2g. The address is 0x1C
  Wire.write(0x1C);
  Wire.write(0x00);


  //Once we send the values, we end the transmission because we don't have any further data to send for now
  Wire.endTransmission(true);

  //Begin a serial connection so we can read the data


}

void loop() {
  //Again, looking at the register map we can find where the values we want reside. The Accelerometer values are located in registers 59 to 64 (hex 0x3B to 0x40) according to page 29 of the register map
  //Here is where it gets interesting. The output of the MPU 6050 is 16 bits, but the I2C protocol demands 8 bit chunks. The registers are also composed of 8 bit chunks.
  //You may notice that there is a register called ACCEL_XOUT_H and ACCEL_XOUT_L, as well as for the Y and Z axis. What is happening is that the data for the accelerometer is split into two separate registers.
  //We need to combine the data from two registers to get the full value. Here's where some boolean operations start to be necessary.
  //One of the few boolean operations we can use is the << operator, which is called a bit shift.
  //A bit shift is self explanatory, it simply shifts the bits in the byte. Consider the binary number 10101010. If we perform a bit shift, ie. 10101010 << x, where x is the number of shifts to take place, a value of x=1 would result in 01010100
  //The leftmost bits are lost, and the rightmost bits are filled with 0. Another example 00110011 << 2 = 11001100    and 00001000 << 3 = 01000000.
  //Check out http://playground.arduino.cc/Code/BitMath for more binary operations

  //Anyways, since these registers are all 8 bit registers, but the values are 16 bit, we need to read two 8 bit numbers and combine them into a 16 bit number.
  //If we start off with a 16 bit integer, and let it read the first 8 bits, then shift it by 8 bits and fill the rest in with the next register, it should be the value we want.

  //First, lets create an 8 bit int to temporarily store the value
  byte XAxis = 0;
  //A byte is an 8 bit value. Assigning it to 0 means it is now 00000000

  //Let's go ahead and read the first register, ACCEL_XOUT_H
  Wire.beginTransmission(MPU_addr);
  //Tell the MPU that we want the data from register 59 (0x3B in hex)
  Wire.write(0x3B);
  //Send a Restart - we won't actually end the transmission, just send it a restart. This gets kind of complicated into the specifications of I2C communication, just trust me on it. Just make sure you see that it is endTransmission(false) not endTransmission(true)
  Wire.endTransmission(false);

  //We will now wait until we have data available

  //Now we will reguest 1 register from the MPU. The third parameter 'true' says that we are ending the transmission after we get the data.
  Wire.requestFrom(MPU_addr, 1, true);
  //Another way to do what the above line does is do Wire.requestFrom(MPU_addr,1,false); then on the next line Wire.endTransmission(true);. Same thing.
  //Now we will assign the byte from before to the value we are reading.
  XAxis = Wire.read();
  //Now we will print the hex value of the XAxis variable.
  //Serial.print("XAxis = ");
  //Serial.println(XAxis, HEX);
  //I'm getting 0x00. This probably depends on how it is sitting on your desk, etc.

  //Let's repeat, and get the next value in the register.
  //NOTE: we really shouldn't be breaking up the measurements like this, since the value can change in an instant. But for education purposes, this should work fine.
  byte XAxisL = 0;
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3C);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 1, true);
  XAxisL = Wire.read();
 // Serial.print("XAxisL = ");
  //Serial.println(XAxisL, HEX);
  //I'm getting alot of numbers, the one I'll go with is 0x84. This is 01010100. This makes our whole number 0000 0000 0101 0100, or simply 84
  //Now, since we are asking for a signed value from the MPU (ie. it can be negative), we look at the first bit (0000 0000 0101 0100). If it is 0, the number is postive and it if is 1, then it is negative.
  //I reran the code so I could get a value that results in Xaxis = FE, XAxisL = 2C so the binary is 1111 1110 0010 1100. Since the first digit is 1, this is a negative number.
  //If we have a negative number, then we must take the 2's complement of the number. Take a look here: https://www.cs.cornell.edu/~tomf/notes/cps104/twoscomp.html
  //Basically we just flip all the values in the number and add 1. So by flipping,  1111 1110 0010 1100 becomes 0000 0001 1101 0011. Adding one yields 0000 0001 1101 0100. This results in a value of 468, and since we know it is negative that means it is -468
  //There has to be a way to program this. We just need to utilize the bitshift operator, and the OR operator. The OR operator (shown as "|") checks two numbers, and if either of the values is a 1 then the result is one.
  //For example, 1001 | 0100 = 1101 and 1100 | 1001 = 1101.
  //Now if we turn our 8 bit Bytes into a 16 bit integer (I'm continuing with the negative number before we did the 2's complementary) we would get Xaxis = 0000 0000 1111 1110 and XAxisL = 0000 0000 0101 0100.
  //If we bit shift the XAxis 8 units to the right, it would be XAxis = 1111 1110 0000 0000. Then if we OR the Xaxis and XAxisL we would get 1111 1110 0101 0100. Neat! Now, how do we do it in code?
  int16_t XAxisFull = XAxis << 8 | XAxisL;
  //The previous line of code bit shifts XAxis 8 places, then ORs it with XAxisL. Now, XAxisFull is the full binary value.
 // Serial.print("XAxisFull = ");
  //Serial.println(XAxisFull);
  //Hey! It Works! But what is this -468 value? Well, according to the MPU register map on page 29, we can see that with our Full Scale range we selected, the sensitivity is 16384 LSB/g. So, divide -468 by 16384 and we get 0.02856g. We'll do more with that later.
  float XAxisFinal = (float) XAxisFull / 16384.0;

  //That was a ton of work though. Fortunately, it can be simplified. The following code should be easy enough to understand. We will read the Y and Z axis accel data from registers 0x3D to 0x40
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3D);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 4, true);
  int16_t YAxisFull =  (Wire.read() << 8 | Wire.read());
  int16_t ZAxisFull =  (Wire.read() << 8 | Wire.read());
  float YAxisFinal = (float)YAxisFull / 16384.0;
  float ZAxisFinal = (float) ZAxisFull / 16384.0;
  Serial.print("X Axis = ");
  Serial.print(XAxisFinal);
  Serial.println("g");
  Serial.print("Y Axis = ");
  Serial.print(YAxisFinal);
  Serial.println("g");
  Serial.print("Z Axis = ");
  Serial.print(ZAxisFinal);
  Serial.println("g");

  //BOOM! Got it. Now, if we want we could calculate the angle we are at knowing that when the sensor is completely level Z should be 1, if it isn't then we just take cos-1(Z) to get our angle with respect to the horizon. We can detect if we are moving
  //by extrapolating our angle and what X and Y should be, and then finding if there is any additional acceleration. But let's move on to the temperature sensor - use page 30 of the register map

/*
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x41);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 2, true);
  int16_t tempFull = Wire.read() << 8 | Wire.read();
  float tempFinal = (float) tempFull / 340.0 + 36.53;
*/

 // Serial.print("Temp = ");
  //Serial.print(tempFinal);
  //Serial.println("C");
  //26.1°C in my house. Little warm. Now let's get the gyroscope data. Page 31 of the register map

//  Wire.beginTransmission(MPU_addr);
//  Wire.write(0x43);
//  Wire.endTransmission(false);
//  Wire.requestFrom(MPU_addr, 6, true);
//  int16_t XGyroFull = Wire.read() << 8 | Wire.read();
//  int16_t YGyroFull = Wire.read() << 8 | Wire.read();
//  int16_t ZGyroFull = Wire.read() << 8 | Wire.read();
//  float XGyroFinal = (float)XGyroFull/32.8;
//  float YGyroFinal = (float)YGyroFull/32.8;
//  float ZGyroFinal = (float)ZGyroFull/32.8;
//  Serial.print("X Axis = ");
//  Serial.print(XGyroFinal);
//  Serial.println(" deg/s");
//  Serial.print("Y Axis = ");
//  Serial.print(YGyroFinal);
//  Serial.println(" deg/s");
//  Serial.print("Z Axis = ");
//  Serial.print(ZGyroFinal);
//  Serial.println(" deg/s");
  //Hell yeah!
  delay(500);
}