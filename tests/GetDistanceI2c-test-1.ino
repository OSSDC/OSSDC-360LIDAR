/*------------------------------------------------------------------------------

  LIDARLite Arduino Library
  GetDistanceI2c

  This example shows how to initialize, configure, and read distance from a
  LIDAR-Lite connected over the I2C interface.

  Connections:
  LIDAR-Lite 5 Vdc (red) to Arduino 5v
  LIDAR-Lite I2C SCL (green) to Arduino SCL
  LIDAR-Lite I2C SDA (blue) to Arduino SDA
  LIDAR-Lite Ground (black) to Arduino GND
  
  (Capacitor recommended to mitigate inrush current when device is enabled)
  680uF capacitor (+) to Arduino 5v
  680uF capacitor (-) to Arduino GND

  See the Operation Manual for wiring diagrams and more information:
  http://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf

------------------------------------------------------------------------------*/

#include <Wire.h>
#include <LIDARLite.h>

LIDARLite myLidarLite;
unsigned long nextTime = 0;
long count=0;
int dist=0;
int minVal=1000;
int maxVal=0;
boolean printEvery=true;
#define DELAY_MICRO 1000000; //1 sec in future

void setup()
{
  Serial.begin(115200); // Initialize serial connection to display distance readings

  /*
    begin(int configuration, bool fasti2c, char lidarliteAddress)

    Starts the sensor and I2C.

    Parameters
    ----------------------------------------------------------------------------
    configuration: Default 0. Selects one of several preset configurations.
    fasti2c: Default 100 kHz. I2C base frequency.
      If true I2C frequency is set to 400kHz.
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */
  myLidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz

  /*
    configure(int configuration, char lidarliteAddress)

    Selects one of several preset configurations.

    Parameters
    ----------------------------------------------------------------------------
    configuration:  Default 0.
      0: Default mode, balanced performance.
      1: Short range, high speed. Uses 0x1d maximum acquisition count.
      2: Default range, higher speed short range. Turns on quick termination
          detection for faster measurements at short range (with decreased
          accuracy)
      3: Maximum range. Uses 0xff maximum acquisition count.
      4: High sensitivity detection. Overrides default valid measurement detection
          algorithm, and uses a threshold value for high sensitivity and noise.
      5: Low sensitivity detection. Overrides default valid measurement detection
          algorithm, and uses a threshold value for low sensitivity and noise.
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */

  int confVal = 2; //2 is fastest
  Serial.print("Set configuration to: ");
  Serial.println(confVal);

  myLidarLite.configure(confVal); // Change this number to try out alternate configurations
  nextTime = micros()+DELAY_MICRO; //1 sec in future
  count=0;
}

void loop()
{
  /*
    distance(bool biasCorrection, char lidarliteAddress)

    Take a distance measurement and read the result.

    Parameters
    ----------------------------------------------------------------------------
    biasCorrection: Default true. Take aquisition with receiver bias
      correction. If set to false measurements will be faster. Receiver bias
      correction must be performed periodically. (e.g. 1 out of every 100
      readings).
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */

  // Take a measurement with receiver bias correction and print to serial terminal
  //Serial.println(myLidarLite.distance());

  // Take a measurement with receiver bias correction and print to serial terminal
  dist = myLidarLite.distance();
  if(printEvery)
  {
    Serial.print(dist);
    Serial.print(' ');
  }  
  count++;
  if(dist<minVal)
    minVal = dist;
  if(dist>maxVal)
    maxVal = dist;
    
  if(nextTime<micros())
  {
    if(printEvery)
      Serial.println("\nEvery stats:");
    else
      Serial.println("Batch stats:");
    Serial.print(count);
    Serial.print(' ');
    Serial.print(dist);
    Serial.print(' ');
    Serial.print(minVal);
    Serial.print(' ');
    Serial.print(minVal);
    Serial.print(' ');
    Serial.println(maxVal-minVal);
    minVal=1000;
    maxVal=0;
    count=0;
    nextTime = nextTime+DELAY_MICRO;
    printEvery=!printEvery;//flip to on/off
  }

  if(Serial.available()) {
     char cmd = (char)Serial.read();
     switch (cmd) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
          Serial.print("Set configuration to: ");
          Serial.println(int(cmd)-48);
          myLidarLite.configure(int(cmd)-48); // Change this number to try out alternate configurations
          break;
     }
  }
  
}
