#ifndef _EEPROM_Data_h
#define _EEPROM_Data_h

#include <EEPROM.h>

void displayMenu();
void readFromEEPROM();

void writeToEEPROM() {

      float myFloat;
      int myInt;
      bool myBool;
      //Check if value has changed before updating to save EEPROM writes
      if (intialSetUp != EEPROM.get(10, myBool))      { EEPROM.put(10, intialSetUp);}
      if (serialNumber != EEPROM.get(12, myInt))      { EEPROM.put(12, serialNumber);}
      if (numOfPixels != EEPROM.get(14, myInt))       { EEPROM.put(14, numOfPixels);}
      if (neoPixelBrightness != EEPROM.get(16, myInt)){EEPROM.put(16, neoPixelBrightness);}
      if (vRef != EEPROM.get(18, myInt))              {EEPROM.put(18, vRef);}  
      if (voltageFilter != EEPROM.get(20, myFloat))   {EEPROM.put(20, voltageFilter);}
      if (ratioCell_1 != EEPROM.get(24, myFloat))     {EEPROM.put(24, ratioCell_1);}
      if (ratioCell_2 != EEPROM.get(28, myFloat))     {EEPROM.put(28, ratioCell_2);}
      if (ratioCell_3 != EEPROM.get(32, myFloat))     {EEPROM.put(32, ratioCell_3);}
      if (ratioCell_4 != EEPROM.get(36, myFloat))     {EEPROM.put(36, ratioCell_4);   } 
      if (ratioCurrent != EEPROM.get(40, myFloat))    {EEPROM.put(40, ratioCurrent);}
      if (cellLow != EEPROM.get(44, myInt))           {EEPROM.put(44, cellLow);}
      if (cellHigh != EEPROM.get(46, myInt))          {EEPROM.put(46, cellHigh);}
      if (packCapacity != EEPROM.get(48, myInt))      {EEPROM.put(48, packCapacity);}
      Log::println(dataSaved);
      readFromEEPROM();
}

void readFromEEPROM() {
    
      int myInt;
      float myFloat;
    //Read from EEPROM
      EEPROM.get(10, intialSetUp);
      EEPROM.get(12, serialNumber);
      EEPROM.get(14, myInt);
      numOfPixels = constrain(myInt,1,100);
      EEPROM.get(16, myInt);
      neoPixelBrightness= constrain(myInt,5,100);
      EEPROM.get(18, myInt); 
      vRef = constrain(myInt,5,6000); 
      EEPROM.get(20, myFloat);
      voltageFilter = constrain(myFloat,0.01,1.00);
      EEPROM.get(24, ratioCell_1);
      EEPROM.get(28, ratioCell_2);
      EEPROM.get(32, ratioCell_3);
      EEPROM.get(36, ratioCell_4);    
      EEPROM.get(40, ratioCurrent);
      EEPROM.get(44, myInt);
      cellLow  = constrain(myInt,0,10000);
      EEPROM.get(46, myInt);
      cellHigh  = constrain(myInt,cellLow,10000);
      EEPROM.get(48, myInt);
      packCapacity= constrain(myInt,1000,30000);
      
      Log::println(dataRead);
      
      if (intialDisplay) {
        displayMenu();
      }
      else {
        intialDisplay = true;  
      }
 
}

#endif
