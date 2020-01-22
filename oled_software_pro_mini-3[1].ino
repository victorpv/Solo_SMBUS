// 3DR SOLO Battery BMS 'emulator''  V0.01 28-Dec-2019
// adapted by P.White.  Based on Solo_SMBUS_V3 by webbs.

/* Components used:
1x Arduino Micro (328p at 3v3v) - Approx 4USD  E.g. https://www.ebay.co.uk/itm/Leonardo-Pro-Micro-ATmega32U4-3-3V-8MHz-Replace-ATmega328-Arduino-Pro-Mini/153421088908?epid=1488926334&hash=item23b89c088c:g:0W0AAOSw5npckF-a
1x 0.49" 4Pin OLED Display Module SSD1306 Resolution128*32 I2C IIC Interface 3.3~5V - Approx 4USD - E.g. https://www.ebay.co.uk/itm/312702799974
1x 3DR solo 'battery side' connector - bought USD 12.99 from http://iron.irixmedia.com/~vo/connectors/  
1x APM compatible power module (approx 5USD) - E.g. https://www.ebay.co.uk/itm/3DR-Power-Module-APM2-2-5-APM-Flight-Controller-ARDUPILOT-MEGA-APM2-6-Quadcopter/163967520080?hash=item262d39cd50:m:mq95dHDY6hbUh_4o0RjiJaw
4x 3296 Multiturn Variable Resistors 100k - Potentiometer, Preset, Trimmer, Pot - Approx 2 USD - E.g. https://www.ebay.co.uk/itm/391438094231
1x XT60 connector (Male) - to connect to the the battery's XT60 connector
1x Lipo Balance Extension Lead Cable JST-XH 20cm - 4s - to cut in half and use to connect to the battery balance lead.
1x FTDI cable (to program the arduino)
Total component cost âpprox 30 USD (excluding solder, glue, and hook up wire)

*/

/* Connections to the arduino micro
Pin 3 (D3) - SCL (to the I2C oled)
Pin 4 (D4) - SDA (to the I2C oled)
Pin A5 - SCL to the Molex/Solo Battery connector - see https://3drobotics.github.io/solodevguide/hardware-battery-charging.html for pinouts
Pin A4 - SDA to the Molex / Solo Battery Connector
Pin A0 - to the middle pin of the 1st 100k pot (the other 2 pins between Ground and Cell 1 balance lead)
Pin A1 - to the middle pin of the 2nd 100k pot (the other 2 pins between Ground and Cell 2 balance lead)
Pin A2 - to the middle pin of the 3rd 100k pot (the other 2 pins between Ground and Cell 3 balance lead)
Pin A3 - to the middle pin of the 4th 100k pot (the other 2 pins between Ground and Cell 4 balance lead)
Pin A6 - to the current signal pin of the APM power module - E.g. see https://quadmeup.com/how-to-connect-apm-power-meter-to-cleanflight-and-inav/ for details
Pin A7 - to the Voltage signal pin of the APM power module

The 5v output from the APM power model goes to the 'raw' voltage in on the Arduino micro, and the Vin pin on the OLED
The OV ground between the APM power model, the OLED, the arduino, and the battery balance lead are commoned

There is debug information of the arduino serial output (and 9600 Baud).
Parameters like battery voltage min+max, and capacity + batt serial number are all hard coded (but ideally would be stored in EEprom, and updatable via a simple button-operated menu system on the OLED)
Also ideally max in-flight current and voltage peaks and minimums would also be stored / displayed (in EEProm and / or on the OLED)

*/

//  Extracts from Solo_SMBUS_V3 by webbs - see https://3drpilots.com/threads/generic-battery-arduino-happy-solo.14888/page-5
/*
 Great info on I2C including Slave
 http://www.gammon.com.au/forum/?id=10896

 Standard Solo Battery Read
 https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp

 SMBus Commands with Data Types/Sizes
 http://www.szgrn.com/smbus-command.html

  SBS Cmd  Mode Name                    Format        Size in Bytes   Min Value   Max Value   Unit 
  0x00    R/W   ManufacturerAccess      hex           2               0x0000      0xffff  　
  0x01    R/W   RemainingCapacityAlarm  unsigned int  2               0           65535       mAh
  0x02    R/W   RemainingTimeAlarm      unsigned int  2               0           65535       min 
  0x03    R/W   BatteryMode             hex           2               0x0000      0xe383  　
  0x04    R/W   AtRate                  signed int    2               -32768      32767       mAh
  0x05    R     AtRateTimeToFull        unsigned int  2               0           65534       min 
  0x06    R     AtRateTimeToEmpty       unsigned int  2               0           65534       min 
  0x07    R     AtRateOK                unsigned int  2               0           65535 　
  0x08    R     Temperature             unsigned int  2               0           65535       0.1K
  0x09    R     Voltage                 unsigned int  2               0           65535       mV 
  0x0a    R     Current                 signed int    2               -32768      32767       mA 
  0x0b    R     AverageCurrent          signed int    2               -32768      32767       mA 
  0x0c    R     MaxError                unsigned int  1               0           100         % 
  0x0d    R     RelativeStateOfCharge   unsigned int  1               0           100         % 
  0x0e    R     AbsoluteStateOfCharge   unsigned int  1               0           100+        % 
  0x0f    R/W   RemainingCapacity       unsigned int  2               0           65535       mAh
  0x10    R     FullChargeCapacity      unsigned int  2               0           65535       mAh
  0x11    R     RunTimeToEmpty          unsigned int  2               0           65534       min 
  0x12    R     AverageTimeToEmpty      unsigned int  2               0           65534       min 
  0x13    R     AverageTimeToFull       unsigned int  2               0           65534       min 
  0x14    R     ChargingCurrent         unsigned int  2               0           65534       mA 
  0x15    R     ChargingVoltage         unsigned int  2               0           65534       mV 
  0x16    R     BatteryStatus           unsigned int  2               0x0000      0xdbff  　
  0x17    R/W   CycleCount              unsigned int  2               0           65535 　
  0x18    R/W   DesignCapacity          unsigned int  2               0           65535       mAh
  0x19    R/W   DesignVoltage           unsigned int  2               0           65535       mV 
  0x1a    R/W   SpecificationInfo       hex           2               0x0000      0xffff  　
  0x1b    R/W   ManufactureDate         unsigned int  2               ASCII 
  0x1c    R/W   SerialNumber            hex           2               0x0000      0xffff  　
  0x20    R/W   ManufacturerName        String        11+1  　 　      ASCII 
  0x21    R/W   DeviceName              String        7+1   　 　      ASCII 
  0x22    R/W   DeviceChemistry         String        4+1   　 　      ASCII 
  0x23    R/W   ManufacturerData        String        14+1  　 　      ASCII 
  0x2f    R/W   Authenticate            String        20+1  　 　      ASCII 
  0x3c    R     CellVoltage4            unsigned int  2               0           65535       mV 
  0x3d    R     CellVoltage3            unsigned int  2               0           65535       mV 
  0x3e    R     CellVoltage2            unsigned int  2               0           65535       mV 
  0x3f    R     CellVoltage1            unsigned int  2               0           65535       mV 


Testing with version 3.7.0-dev these are the requests that i have detected
8  Temperature    
10  FullChargeCapacity
23  ManufacturerData
28  cell voltage    not in list but is 4 * int in mV
1C  SerialNumber
2A  current         not in list but a double int in mA (-ve is current draw and is displayed as postive)

*/

// OLED Display - https://pmdway.com/collections/oled-displays/products/0-49-64-x-32-white-graphic-oled-i2c
// OLED Guide - https://pmdway.com/blogs/product-guides-for-arduino/tutorial-using-the-0-49-64-x-32-graphic-i2c-oled-display-with-arduino

// ****** Defines and includes and variables  ****** 

#include <Arduino.h>
#include <Wire.h> //Wire library for I2C coms
#include <U8g2lib.h> // For the OLED


// U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// U8G2_SSD1306_128X64_NONAME_F_SW_I2C screen1(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
U8G2_SSD1306_64X32_1F_F_SW_I2C u8g2(U8G2_R0, 3 , 4,  U8X8_PIN_NONE);  // we will use Pin 3 and 4 and software I2C for the OLED (to keep the hardware I2C ports free exclusively to talk to the solo\
// fonts https://github.com/olikraus/u8g2/wiki/fntlistall#4-pixel-height

// Set up variables for measuring Voltage and Current
int Cell1Pin = A0;    // select the input pin for the potentiometer
int Cell1Value = 0;  // variable to store the value coming from the sensor
int Cell2Pin = A1;    // select the input pin for the potentiometer
int Cell2Value = 0; 
int Cell3Pin = A2;    // select the input pin for the potentiometer
int Cell3Value = 0; 
int Cell4Pin = A3;    // select the input pin for the potentiometer
int Cell4Value = 0; 
int ApmVPin = A7;    // select the input pin for the potentiometer
int ApmVValue = 0; 
int ApmCPin = A6;    // select the input pin for the potentiometer
int ApmCValue = 0; 

// Below here is taken from Solo_SMBUS_V3

// Voltages for Capacity - These are ok for LiPos  (Li-ions would have different ranges - e.g. can go down to 2.5v per cell)
//float cellLow  = 3500;    //0% Capacity
float cellLow  = 3000;    //0% Capacity
float cellHigh = 4200;   //100% Capacity


//Voltage Divider Resistors values in k ohms
float cell_1_Resistor_1 = 0.998; 
float cell_1_Resistor_2 = 18.04; 
float cell_2_Resistor_1 = 10.06;
float cell_2_Resistor_2 = 10.11;
float cell_3_Resistor_1 = 19.90;
float cell_3_Resistor_2 = 10.01;
float cell_4_Resistor_1 = 30.01;
float cell_4_Resistor_2 = 10.04;
//float vRef = 5000; //mV - Should now be 5000mV using the 5V regulator
float vRef = 3300; //mV - Alternative if using now be 3.3v as the reference
float ratioCell_1 = 0;
float ratioCell_2 = 0;
float ratioCell_3 = 0;
float ratioCell_4 = 0;
float currentRatio =0;

/*Filter to filter out jumping values
 Value of 1 will pass on actual value - no filtering
 0.25 will take about 3s for a constant input to be displayed as the output
 0.5 will take about 1.5s for a constant input to be displayed as the output
 0.6 will take about 1.0s for a constant input to be displayed as the output
 The smaller the value the slower the change in output will be but will also filter out jumps in voltage*/
float voltageFilter = 0.4;

int cell_1 = 0; //Cell 1 voltage in mV
int cell_2 = 0; //Cell 2 voltage in mV
int cell_3 = 0; //Cell 3 voltage in mV
int cell_4 = 0; //Cell 4 voltage in mV
float cell_1_read = 0; //Value for cell caculations
float cell_2_read = 0; //Value for cell caculations
float cell_3_read = 0; //Value for cell caculations
float cell_4_read = 0; //Value for cell caculations

int pack = 0;   //Pack Voltage
float packCapacity = 9000; //Define pack total capacity - in mAh
int remainingCapacity = 5200;
float floatPercentage = 0;
//values for capacity equation
float m = 0; 
float c = 0;
int capacityPercentage = 0;

//Pins for Reading Voltage - original
/*int pinCell_1 = A6;
int pinCell_2 = A3;
int pinCell_3 = A2;
int pinCell_4 = A1;
*/

// Adapted voltage reading pins for my board - to the balance cable via a pot
int pinCell_1 = A0;   
int pinCell_2 = A1; 
int pinCell_3 = A2;
int pinCell_4 = A3;

//Pin for Reading Current - from the APM power unit current measurement (via shunt resistor)
int pinCurrent = A6;  

// Note I also have the APM power unit - reading whole battery voltage (i.e. same as Cell 4) - on A7

//Setup for Current Measurement using ACS758LCB-050B  -- NOTE I am not using this
float offset = - 70.0; // -70mV to get 0 at no current
const float QOV =   (0.5 * vRef) + offset  ; // set quiescent Output voltage (as per data sheet)
float current_temp_f = 0;
float currentFactor = 0.04; //ACS758LCB-050B factor 40mV per Amp => 0.04mV per mA
int32_t current = 0; //current value in mA (-ve is current being used)



//Slave address for Solo SMBus
const byte soloSMBus = 0x0B;
byte dataRequested; //SMBus request

//Serial Number - Note I am not using DIP switches
int battSerial = 32; //Battery Serial Number Set by DIP switches

//Setup for loop timer
unsigned long previousMillisLoop = 0;
long intervalLoop = 1000; //update NeoPixel Rate in ms   

//Setup read timer (only read voltage/current once per 100ms)
unsigned long previousMillisRead = 0;
long intervalRead = 100; 

//LED Setup for data request
int ledState = LOW;   //LED State for startup
const int ledPin =  LED_BUILTIN;


bool batType = false;
bool batUsage = false;


// ***** End  paste of defines from Solo_SMBUS_V3

//***** Setup starts here ****

void setup() {
  u8g2.begin();  // For the OLED

//****  Paste from Solo_SMBUS_V3 starts here
 
  //Check filter value
  if (voltageFilter > 1) {
    voltageFilter = 1;
  }
  if (voltageFilter < 0.20) {
    voltageFilter = 0.20;
  }

  

  //Set reference to user external Voltage (e.g. 5v or 3.3v)  from regulator if used (remove //)
  //analogReference(EXTERNAL);
  
  battSerial= 32 +255;  // Just putting in a dummy serial - Hard coded for now 

// Not using switches for batt type - will set cellLow and cellHigh manually hardcoded.
/*
  //Set battery type and adjust low and high levels
  //Selection 1 Sets Battery Type => Switch Off = Lipo / Switch On = Li Ion
  //Selection 2 Sets Battery Capacity => Switch Off = Conservative / Switch On = Aggressive

  batType=1; // Hard code for now
  batUsage=1; // Hard code for now
  
  if (batType) {  //Battery Type Li Ion
    if (batUsage) { //Battery Usage Aggressive
      float cellLow  = 3300;    //0% Capacity
      float cellHigh = 4200;   //100% Capacity  
    }
    else { //Battery Usage Conservative
      float cellLow  = 3700;    //0% Capacity
      float cellHigh = 4200;   //100% Capacity      
    }
  }
  else { // Battery Type Lipo
    if (batUsage) { //Battery Usage Aggressive
      float cellLow  = 3300;    //0% Capacity
      float cellHigh = 4200;   //100% Capacity    
    }
    else { //Battery Usage Conservative
      float cellLow  = 3700;    //0% Capacity
      float cellHigh = 4200;   //100% Capacity      
    }
  }
*/

// Hard coded cells levels in mV
cellLow = 2700; // Not the Sanyo cells can go to 2.5 apparently
cellHigh = 4200;


  //Debugging
  Serial.begin(9600);
  Serial.println("Starting");

  

  //Caculate Voltage Divider Ratios
  //Voltage Divider ratio = (Resistor_1 + Resistor_2) / Resistor_2
/*
  ratioCell_1 = (cell_1_Resistor_1 + cell_1_Resistor_2)/cell_1_Resistor_2;
  ratioCell_2 = (cell_2_Resistor_1 + cell_2_Resistor_2)/cell_2_Resistor_2;
  ratioCell_3 = (cell_3_Resistor_1 + cell_3_Resistor_2)/cell_3_Resistor_2;
  ratioCell_4 = (cell_4_Resistor_1 + cell_4_Resistor_2)/cell_4_Resistor_2;
*/

  // The below values are as measured from my own setup.
  ratioCell_1 = 4.208416834;
  ratioCell_2 = 8.43373494;
  ratioCell_3 = 12.64794383;
  ratioCell_4 = 16.56448203;
  currentRatio = 44.23213022;




  /*
     Cacpacity Remaining Variable Caculation
     use linear equation y = xm + c    y = %, x = voltage
     cellLow  * 4 = 0% Capacity   (0)
     cellHigh * 4 = 100% Capacity (100)
     Work out m & c
     m = 100 / ((cellHigh * 4) / (cellLow  * 4))
     c = - (cellLow  * 4) * m 
  */
  m = 100 / ((cellHigh * 4) - (cellLow  * 4));
  c = ((cellLow  * 4) * m)* -1;  

 
  
  //Set LED High to Show it has Started
  pinMode(LED_BUILTIN, OUTPUT);
  ledState = HIGH;
  digitalWrite(ledPin, ledState);

  //Read Voltage
  getVoltageCurrent();

  //Start I2C as Solo Battery Slave (address 0x0B)
  Wire.begin (soloSMBus);
  Wire.onReceive (receiveEvent);  // interrupt handler for incoming messages
  Wire.onRequest (requestEvent);  // interrupt handler for when data is wanted  

// **** Paste from Solo_SMBUS_v3 ends here
 
}


// ******* Main loop starts here
void loop()
{



// *** Start of the loop section taken from Solo_SMBus_V3

    unsigned long currentMillisLoop = millis();
  
    //Loop to run based on time set but intervalLoop
    if (currentMillisLoop - previousMillisLoop >= intervalLoop) {
      // save the last time you were in loop
      previousMillisLoop = currentMillisLoop; 
  
// Start of section dealing with my own voltage and current reading code to display on the OLED

// read the value from the sensor:
  Cell1Value = analogRead(Cell1Pin);      
  Cell2Value = analogRead(Cell2Pin);
  Cell3Value = analogRead(Cell3Pin);
  Cell4Value = analogRead(Cell4Pin);
  ApmVValue = analogRead(ApmVPin);
  ApmCValue = analogRead(ApmCPin);
  
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_t0_11_tf);  // Resonably sized font
//u8g2.setFont(u8g2_font_u8glib_4_tf);  // Tiny font
  //u8g2.drawStr(0, 10, "Hello,");  // write something to the internal memory

  /* u8g2.setCursor(0, 10);
   u8g2.print("1:");
   u8g2.print(Cell1Value);
   u8g2.setCursor(30, 10);
   u8g2.print("2:");
   u8g2.print(Cell2Value);
   u8g2.setCursor(0, 20);
   u8g2.print("3:");
   u8g2.print(Cell3Value);
   u8g2.setCursor(30, 20);
   u8g2.print("4:");
   u8g2.print(Cell4Value);
   u8g2.setCursor(0, 30);
   u8g2.print("V:");
   u8g2.print(ApmVValue);
   u8g2.setCursor(30, 30);
   u8g2.print("C:");
   u8g2.print(ApmCValue);
   u8g2.sendBuffer();          // transfer internal memory to the display
  //delay(100);
*/
 
 /*
 getVoltageCurrent();
  u8g2.setCursor(0, 10);
   u8g2.print("1:");
   u8g2.print((int)cell_1_read);
   u8g2.setCursor(30, 10);
   u8g2.print("2:");
   u8g2.print((int)cell_2_read);
   u8g2.setCursor(0, 20);
   u8g2.print("3:");
   u8g2.print((int)cell_3_read);
   u8g2.setCursor(30, 20);
   u8g2.print("4:");
   u8g2.print((int)cell_4_read);
   u8g2.setCursor(0, 30);
   u8g2.print("V:");
   u8g2.print(pack);
   u8g2.setCursor(30, 30);
   u8g2.print("C:");
   u8g2.print(current);
   u8g2.sendBuffer();          // transfer internal memory to the display
  //delay(100);
  */

 getVoltageCurrent();
  u8g2.setCursor(0, 10);
   u8g2.print("mV:");
   u8g2.print((int) pack);
   u8g2.setCursor(0, 20);
   u8g2.print("mA:");
   u8g2.print((int)current);
   u8g2.setCursor(0, 30);
   u8g2.print("mAh:");
   u8g2.print(remainingCapacity);
   u8g2.sendBuffer();          // transfer internal memory to the display
  //delay(100);

  Serial.print("Cell 1 read mV = ");
  Serial.println((int)cell_1_read); 
    Serial.print("Cell 1 mV = ");
  Serial.println((int)cell_1); 
  Serial.print("Cell 2 read mV = ");
  Serial.println((int)cell_2_read);
  Serial.print("Cell 3 read mV = ");
  Serial.println((int)cell_3_read);
  Serial.print("Cell 4 read mV = ");
  Serial.println((int)cell_4_read);
  Serial.print("Pack mV = ");
  Serial.println((int)pack);    
  Serial.print("Current mA = ");
  Serial.println(current);
  Serial.println("");

// ***** End of the section with my voltage / current reading to the OLED

 
    } //End of Timer If
    
// ** End of the  loop section taken from Solo_SMBus_V3


} // **** End of main loop

// **** Below are the functions taken from Solo_SMBUS_V3 


void receiveEvent (int howMany)
  {

  dataRequested = 0;
  dataRequested = Wire.read ();  // remember dataRequested for when we get a requestEvent
    
    //LED Changes state with each receiveEvent (help indicate Coms working)
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);

  } // end of receiveEvent

void requestEvent ()
  {
  
  unsigned long currentMillisRead = millis();
  
  //0x09 = total battery voltage
  if (dataRequested == 0x09){  

    //Only Read Fresh Values if more than 100ms old
    if (currentMillisRead - previousMillisRead >= intervalRead) {
      // save the last time you were in loop
      previousMillisRead = currentMillisRead; 
      getVoltageCurrent();
    }

    //Send total voltage in 2 bytes
    byte high = highByte(pack);
    byte low = lowByte(pack);
    
    //create byte buffer for from int
    byte myBuff[2] = {low, high};
    
    //create 3 bytes to return adding in the PEC 
    //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
    byte byteArray[3] = {low, high, get_PEC(soloSMBus,dataRequested, true, myBuff, 2)};
    
    //Write the 3 bytes back    
    Wire.write(byteArray, 3); 
    Serial.print("I2C Sent Voltage : ");
    Serial.println(pack);
   }

   //0x1c = SerialNumber
  if (dataRequested == 0x1c){  
    //Send SerialNumber in 2 bytes
    
    byte high = highByte(battSerial);
    byte low = lowByte(battSerial);
    
    //create byte buffer for from int
    byte myBuff[2] = {low, high};
    
    //create 3 bytes to return adding in the PEC 
    //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
    byte byteArray[3] = {low, high, get_PEC(soloSMBus,dataRequested, true, myBuff, 2)};
    
    //Write the 3 bytes back    
    Wire.write(byteArray, 3); 
     Serial.print("I2C Sent BatSerial : ");
    Serial.println(battSerial);
   } 
 
   //0x28 = 4 * cell voltages, 2 bytes each
   if (dataRequested == 0x28) { 

    //Only Read Fresh Values if more than 100ms old
    if (currentMillisRead - previousMillisRead >= intervalRead) {
      // save the last time you were in loop
      previousMillisRead = currentMillisRead; 
      getVoltageCurrent();
    }
    
    byte cellBuff[9];
    
    byte myLength = 8;
    cellBuff[0] = myLength;
    
    cellBuff[1] = lowByte(cell_4);
    cellBuff[2] = highByte(cell_4);

    cellBuff[3] = lowByte(cell_3);
    cellBuff[4] = highByte(cell_3);

    cellBuff[5] = lowByte(cell_2);
    cellBuff[6] = highByte(cell_2);

    cellBuff[7] = lowByte(cell_1);
    cellBuff[8] = highByte(cell_1);    

    //create 10 bytes to return adding length to byte 0 and PEC to byte 9
    //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
    
    byte cellArray[10] = {cellBuff[0], cellBuff[1], cellBuff[2], cellBuff[3], cellBuff[4], cellBuff[5], cellBuff[6], cellBuff[7], cellBuff[8], get_PEC(soloSMBus, dataRequested, true, cellBuff, 9)};
    
    //Write the 10 bytes back    
    Wire.write(cellArray, 10); 
    Serial.println("I2C Sent Cell Voltages : ");

   }

   //0x2A = current request mA double int, 4 bytes (-ve value is displayed as positive) 
   if (dataRequested == 0x2A) { 

    //Only Read Fresh Values if more than 100ms old
    if (currentMillisRead - previousMillisRead >= intervalRead) {
      // save the last time you were in loop
      previousMillisRead = currentMillisRead; 
      getVoltageCurrent();
    }    

    byte currentBuff[5]; 
    byte myLength = 4;
    currentBuff[4] = (current>>24) & 0xff;
    currentBuff[3] = (current>>16) & 0xff;
    currentBuff[2] = (current>>8) & 0xff;
    currentBuff[1] = current & 0xff;
    currentBuff[0] = myLength;

    //create 6 bytes to return adding in length and PEC 
    //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
    byte byteArray[6] = {currentBuff[0], currentBuff[1], currentBuff[2],currentBuff[3], currentBuff[4], get_PEC(soloSMBus,dataRequested, true, currentBuff, 5)};
    
    //Write the 6 bytes back    
    Wire.write(byteArray, 6); 
     Serial.print("I2C Sent Currrent : ");
    Serial.println(current);
   }

   //0x10 = total capacity mAh
   if (dataRequested == 0x10) { 
    //Send total capacitye in 2 bytes
    byte high = highByte(int(packCapacity));
    byte low = lowByte(int(packCapacity));
    
    //create byte buffer for from int
    byte myBuff[2] = {low, high};
    
    //create 3 bytes to return adding in the PEC 
    //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
    byte byteArray[3] = {low, high, get_PEC(soloSMBus,dataRequested, true, myBuff, 2)};
    
    //Write the 3 bytes back    
    Wire.write(byteArray, 3); 
     Serial.print("I2C Sent Pack Capacity mAh : ");
    Serial.println(packCapacity);
   }

   //0x0F = Remaining Capacity mAh 
   if (dataRequested == 0x0F) { 

    //Only Read Fresh Values if more than 100ms old
    if (currentMillisRead - previousMillisRead >= intervalRead) {
      // save the last time you were in loop
      previousMillisRead = currentMillisRead; 
      getVoltageCurrent();
    }  
    
    //Send Remaining capacity in 2 bytes
    byte high = highByte(remainingCapacity);
    byte low = lowByte(remainingCapacity);
    
    //create byte buffer for from int
    byte myBuff[2] = {low, high};
    
    //create 3 bytes to return adding in the PEC 
    //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
    byte byteArray[3] = {low, high, get_PEC(soloSMBus,dataRequested, true, myBuff, 2)};
    
    //Write the 3 bytes back    
    Wire.write(byteArray, 3); 
     Serial.print("I2C Sent Remaining Capacity : ");
    Serial.println(remainingCapacity);
   }

   
  }  // end of requestEvent  

byte SMBUS_PEC_POLYNOME = 0x07;

/// get_PEC - calculate packet error correction code of buffer
byte get_PEC(uint8_t i2c_addr, uint8_t cmd, bool reading, const uint8_t buff[], uint8_t len) 
{
    // exit immediately if no data
    if (len <= 0) {
        return 0;
    }

    // prepare temp buffer for calcing crc
    uint8_t tmp_buff[len+3];
    tmp_buff[0] = i2c_addr << 1;
    tmp_buff[1] = cmd;
    tmp_buff[2] = tmp_buff[0] | (uint8_t)reading;
    memcpy(&tmp_buff[3],buff,len);

    // initialise crc to zero
    uint8_t crc = 0;
    uint8_t shift_reg = 0;
    bool do_invert;

    // for each byte in the stream
    for (uint8_t i=0; i<sizeof(tmp_buff); i++) {
        // load next data byte into the shift register
        shift_reg = tmp_buff[i];
        // for each bit in the current byte
        for (uint8_t j=0; j<8; j++) {
            do_invert = (crc ^ shift_reg) & 0x80;
            crc <<= 1;
            shift_reg <<= 1;
            if(do_invert) {
                crc ^= SMBUS_PEC_POLYNOME;
            }
        }
    }

    // return result
    return crc;  
}

//Read Current and Read Voltage with Voltage Divider
void getVoltageCurrent() {

  //Read Voltage and Current from Analogue Inputs
  //Cell 1
  float cell_temp = float(analogRead(pinCell_1));
  //cell_temp = cell_temp *  (vRef/1023);
  cell_temp = cell_temp * ratioCell_1;  
  cell_1_read = cell_temp;
  cell_1 = int((float(cell_1) * (1 - voltageFilter)) + (cell_temp * voltageFilter));

  //Cell 2
  cell_temp = float(analogRead(pinCell_2));
  //cell_temp = cell_temp *  (vRef/1023);
  cell_temp = cell_temp * ratioCell_2;  
  cell_temp = cell_temp - float(cell_1_read);
  cell_2_read = cell_temp;
  cell_2 = int((float(cell_2) * (1 - voltageFilter)) + (cell_temp * voltageFilter));

  //Cell 3  
  cell_temp = float(analogRead(pinCell_3));
  //cell_temp = cell_temp *  (vRef/1023);
  cell_temp = cell_temp * ratioCell_3;  
  cell_temp = cell_temp - float(cell_1_read + cell_2_read);
  cell_3_read = cell_temp;
  cell_3 = int((float(cell_3) * (1 - voltageFilter)) + (cell_temp * voltageFilter));
  
  //Cell 4
  cell_temp = float(analogRead(pinCell_4));
  //cell_temp = cell_temp *  (vRef/1023);
  cell_temp = cell_temp * ratioCell_4;  
  cell_temp = cell_temp - float(cell_1_read + cell_2_read + cell_3_read);
  cell_4_read = cell_temp;
  cell_4 = int((float(cell_4) * (1 - voltageFilter)) + (cell_temp * voltageFilter));
  
  //pack voltage = total of all 4 cells
  pack = cell_1 + cell_2 + cell_3 + cell_4;

  //Work out remaining capacity
  // use linear equation y = xm + c    y = %, x = voltage
  floatPercentage = float(pack) * m + c;
  
  if (floatPercentage > 100) {
    capacityPercentage = 100;
  }
  else if (floatPercentage < 0) {
      capacityPercentage = 0;
    }
  else {
      capacityPercentage = int(floatPercentage);
    }
  
  remainingCapacity = int((float(packCapacity)/100) * float(capacityPercentage));

  //Current Read
  current_temp_f = float(analogRead(A6));
  //current_temp_f = float(180); // Rig it to ADC=180 for about 8.5amps
  //current_temp_f = (current_temp_f *  (vRef/1023)); //mV reading
  //current_temp_f = current_temp_f - QOV; //mV reading -ve = Current used, +ve Current put in
  //current_temp_f = current_temp_f / currentFactor;
  current_temp_f = current_temp_f * currentRatio;
  current = int32_t(current_temp_f);
}
