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
boolean collect=false;
boolean BTprint=true;
#define DELAY_MICRO 1000000; //1 sec in future

char cmd = 0;

#include <SoftwareSerial.h>

SoftwareSerial BTSerial(8, 9); // RX | TX

void setup()
{

  //pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  //digitalWrite(9, HIGH);
  Serial.begin(115200);// BT works on 9600, 38400 and USB works on 115200); // Initialize serial connection to display distance readings
  Serial.println("Enter AT commands:");
  BTSerial.begin(115200);  // HC-05 default speed in AT command more
  //BTSerial.begin(9600); //19200); 
  /*
    To return HC-05 to mfg. default settings: "AT+ORGL"
    To get version of your HC-05 enter: "AT+VERSION?"
    To change device name from the default HC-05 to let's say MYBLUE enter: "AT+NAME=MYBLUE"
    To change default security code from 1234 to 2987 enter: "AT+PSWD=2987"
    To change HC-05 baud rate from default 9600 to 115200, 1 stop bit, 0 parity enter: "AT+UART=115200,1,0" 
   */
  //BTSerial.write("AT+UART=115200,1,0");
  //digitalWrite(9, LOW);
  
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
  
  if(BTprint)
  {
      BTSerial.println(dist);
      //BTSerial.write("1235678901234567890\n");
      Serial.println(dist);
  }
  
  if(collect && printEvery)
  {
    Serial.print(dist);
    Serial.print(' ');
  }  
  count++;
  if(dist<minVal)
    minVal = dist;
  if(dist>maxVal)
    maxVal = dist;
    
  if(collect) //if false is don't collect
  {
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
  }

/*
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available())
    Serial.write(BTSerial.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
*/
    
  if(BTSerial.available())
     Serial.write(BTSerial.read());
     //BTSerial.write("AT+UART=115200,1,0");
     //cmd = (char)BTSerial.read();
  if(Serial.available())
     cmd = (char)Serial.read();
  if(cmd!=0)
  {
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
        case 'S':
          collect = !collect;//flips collection on/off
          Serial.print("\ncollect: ");
          Serial.println(collect);
          break;
        case 'B':
          BTprint = !BTprint;//flips BT print on/off
          Serial.print("\nBTprint: ");
          Serial.println(BTprint);
          break;
     };
     cmd=0;
  }
}
