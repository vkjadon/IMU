#include <Wire.h>

long accel_rawX, accel_rawY, accel_rawZ;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupMPU();
}
void loop() {
  accelRawData();
  plot_accelData();
  delay(50);
}

void setupMPU()
{
// Power management Tasks
  Wire.beginTransmission(0b1101000); //Slave Device Address Sequence - This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Internal Register Address Sequence - Accessing the register 6B - Power Management (Sec. 4.28) 
  Wire.write(0b00000000); //Data Transfer Bit Sequence - Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
// Configer Accelerometer for use
  Wire.beginTransmission(0b1101000); //Slave Device Address Sequence - I2C address of the MPU
  Wire.write(0x1C); //Internal Register Address Sequence - Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Data Transfer Bit Sequence - Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void accelRawData() 
{
  Wire.beginTransmission(0b1101000); //Slave Device Address Sequence - I2C address of the MPU
  Wire.write(0x3B); //Internal Register Address Sequence - Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accel_rawX = Wire.read()<<8|Wire.read(); //Store first two bytes into accel_rawX after converting into decimal
  accel_rawY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accel_rawY
  accel_rawZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accel_rawZ
}

void plot_accelData() 
{
  Serial.print(accel_rawX); // Blue on Plotter
  Serial.print(",");
  Serial.print(accel_rawY); // Red on Plotter
  Serial.print(",");
  Serial.println(accel_rawZ); // Green on Plotter
}

