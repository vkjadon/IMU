#include <Wire.h>

long accel_rawX, accel_rawY, accel_rawZ;
long accelFSRange=16384; //accelFSRange=16384(2g), 8192(4g), 4096(8g), 2048(16g)
float accel_calX, accel_calY, accel_calZ;
double accel_X, accel_Y, accel_Z;
int i, accelCalSamples=2000, accelRollAvg=100;
float accel_angleX, accel_angleY, radDeg=180/PI;

void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  setupMPU();
  accelCalibration(accelCalSamples); // Calibration is on the coverted Data ()
//  delay(5000);
}
void loop() 
{
  accelRawData(); // The output is stored in accel_rawX, accel_rawY and accel_rawZ
  //Uncomment accelRawData() (above line)and comment rollAvg()for Data w/o rolling average
  
  rollAvg(accelRollAvg);
  accelMechFilter();
  accelCalData();  // Apply the bias - calculated using Calibration
//  plotData(accel_rawX, accel_rawY, accel_rawZ);// Maximum value is as per accelFSRange selected
  accelData();  // convert into gForce. This function to be called at last after signal processing
//  plotData(accel_X, accel_Y, accel_Z);// Maximum value is as per accelFSRange selected
  accelAngle();
  plotData(accel_angleX, accel_angleY, 0);// Maximum value is as per accelFSRange selected
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

void accelCalibration(int calSamples)
{
  for(int i=0; i<calSamples; i++)
  {
    accelRawData();
    accel_calX+=accel_rawX;
    accel_calY+=accel_rawY;
    accel_calZ+=accel_rawZ;
  }
  accel_calX=accel_calX/calSamples;
  accel_calY=accel_calY/calSamples;
  accel_calZ=accel_calZ/calSamples;
  Serial.print(accel_calX);
  Serial.print(",");
  Serial.print(accel_calY);
  Serial.print(",");
  Serial.print(accel_calZ);
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
void accelData()
{
  accel_X =float(accel_rawX)/float(accelFSRange);
  accel_Y =float(accel_rawY)/float(accelFSRange);
  accel_Z =1.0+float(accel_rawZ)/float(accelFSRange);
}

void plotData(float first, float second, float third) 
{
  Serial.print(first); // Blue on Plotter
  Serial.print(",");
  Serial.print(second); // Red on Plotter
  Serial.print(",");
  Serial.println(third); // Green on Plotter
}

void rollAvg(int ra)
{
  float sampleSumX=0, sampleSumY=0, sampleSumZ=0;
  i=0;
  while(i<ra)
  {
    accelRawData();
    sampleSumX=sampleSumX+accel_rawX;
    sampleSumY=sampleSumY+accel_rawY;
    sampleSumZ=sampleSumZ+accel_rawZ;
    i++;
    //Serial.print(sampleSumX);
  }
  accel_rawX=sampleSumX/ra;
  accel_rawY=sampleSumY/ra;
  accel_rawZ=sampleSumZ/ra;
}
void accelMechFilter()
{
  if(accel_rawX>-5 && accel_rawX<5) accel_rawX=0.0;
  if(accel_rawY>-5 && accel_rawY<5) accel_rawY=0.0;
  if(accel_rawZ>-5 && accel_rawZ<5) accel_rawZ=0.0;
}
void accelCalData()
{
  accel_rawX=accel_rawX-accel_calX;
  accel_rawY=accel_rawY-accel_calY;
  accel_rawZ=accel_rawZ-accel_calZ;
}
void accelAngle()
{
  accel_angleX=atan(accel_Y/sqrt(pow(accel_X,2)+pow(accel_Z,2)))*radDeg;
  accel_angleY= atan(-accel_X/sqrt(pow(accel_Y,2)+pow(accel_Z,2)))*radDeg;
}

