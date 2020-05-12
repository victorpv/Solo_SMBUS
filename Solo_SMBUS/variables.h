#ifndef _variables_h
#define _variables_h

  //Slave address for Solo SMBus
  const byte soloSMBus = 0x0B;
  byte dataRequested; //SMBus request

  //LED Setup for data request
  int ledState = LOW;   //LED State for startup
  const int ledPin = LED_BUILTIN;  

  //Voltage Divider Ratio
  float ratioCell_1  =   4.208416834f;
  float ratioCell_2  =   8.43373494f;
  float ratioCell_3  =   12.64794383f;
  float ratioCell_4  =   16.56448203f;
  float ratioCurrent =   44.23213022f;

  //NeoPixel
  int RGB_Red = 0;
  int RGB_Red_D = 0;
  int RGB_Green = 0;
  int RGB_Green_D = 0;
  int RGB_Blue = 255;
  int RGB_Blue_D = 255;
  int neoCase = 0;
  bool NeoPixChange = false;
  bool lowFlash = false;
  bool lowFlashState = false;
  //NeoPixel Brightness 0 to 100%
  int neoPixelBrightness = 100;

  // Voltages for Capacity - These are ok for LiPos  (Li-ions would have different ranges - e.g. can go down to 2.5v per cell)
  int cellLow = 3700;    //0% Capacity
  int cellHigh = 4200;   //100% Capacity

  //Voltage Values
  int cell_1_mV = 0; //Cell 1 voltage in mV
  int cell_2_mV = 0; //Cell 2 voltage in mV
  int cell_3_mV = 0; //Cell 3 voltage in mV
  int cell_4_mV = 0; //Cell 4 voltage in mV
  int cell_1_read = 0; //Unfiltered Value
  int cell_2_read = 0; //Unfiltered Value
  int cell_3_read = 0; //Unfiltered Value  
  int cell_4_read = 0; //Unfiltered Value
 

  int pack = 0;   //Pack Voltage
  int packCapacity = 5200, remainingCapacity = 5200; //Define pack total capacity - in mAh
  //values for capacity equation
  float m = 0;
  float c = 0;
  int capacityPercentage = 0;
  long current = 0; //current value in mA (-ve is current being used)

  //Battery Serial Number Starter
  int battSerial = 32; 

  //Setup for loop timer
  unsigned long previousMillisLoop = 0;
  long intervalLoop = 1000; //update NeoPixel Rate in ms

  //Setup read timer (only read voltage/current once per 100ms)
  unsigned long previousMillisRead = 0;
  long intervalRead = 100;

  bool intialSetUp = false;
  int serialNumber = 200;

  int vRef = 5000; //mV - Set vRef value

  
  /* Filter to filter out jumping values
   * Value of 1 will pass on actual value - no filtering
   * 0.25 will take about 3s for a constant input to be displayed as the output
   * 0.5 will take about 1.5s for a constant input to be displayed as the output
   * 0.6 will take about 1.0s for a constant input to be displayed as the output
   * The smaller the value the slower the change in output will be but will also filter out jumps in voltage
   */  
  float voltageFilter = 0.4;

  //Setup NeoPixel
  int numPix;
  int numOfPixels = 1;

  unsigned long currentMillisLoop = 0;

  //Strings
  const char *caclError = "Cell Ratio Caculation Failed";
  const char *caclCurError = "Current Ratio Caculation Failed";
  const char *dataSaved = "  > Data Saved To EEPROM";
  const char *dataRead =  "  > Data Read From EEPROM";
  const char TAB = '\t';
  bool intialDisplay = false;

class Log
{
  public:
    template<typename X>
    static void print(X&& x) {
      Serial.print(x);
    }

    template<typename X, typename... Args>
    static void print(X&& x, Args&&... args) {
      Serial.print(x);
      print(args...);
    }

    template<typename X>
    static void println(X&& x) {
      Serial.println(x);
    }

    template<typename X, typename... Args>
    static void println(X&& x, Args&&... args) {
      Serial.print(x);
      println(args...);
    }
};  
#endif
