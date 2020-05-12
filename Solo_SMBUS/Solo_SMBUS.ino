  #include <Wire.h> //Wire library for I2C coms
  #include <Adafruit_NeoPixel.h> //NeoPixel library
  #include <EEPROM.h>

  #include "variables.h"
  #include "pins.h"

  #include "EEPROM_Data.h"

  #include "getVoltageCurrent.h"

  Adafruit_NeoPixel pixels(numOfPixels, NeoPixel, NEO_GRB + NEO_KHZ800);
  #include "neopixel.h"


  #include "CommandLine.h"

  #include "PECCode.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

    //Read intial Setup Done from EEPROM
    intialSetUp = EEPROM.read(10);

    //Check if intial setup has been done
    //if not then write vlaues to EEPROM
    if (intialSetUp != true) {
      //Write values to EEPROM
      intialSetUp = true;
      writeToEEPROM();
    }

    //Read from EEPROM
    readFromEEPROM();    

    //Update Number of Neopixels
    pixels.updateLength(numOfPixels);
    
    //analogReference(EXTERNAL);

    battSerial = 32 + serialNumber; 

    //Generate equation for Battery Remaining %
    equation();

    // INITIALIZE NeoPixel
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.clear(); // Set all pixel colors to 'off'
    for(int i=0; i<numPix; i++) { // For each pixel...
      // Set All To Blue on Startup
      pixels.setPixelColor(i, pixels.Color(RGB_Red, RGB_Green, RGB_Blue));
      pixels.show();
    }

    //Set LED High to Show it has Started
    pinMode(LED_BUILTIN, OUTPUT);
    ledState = HIGH;
    digitalWrite(ledPin, ledState);

    //Read Voltage
    VoltageCurrent();

    //Start I2C as Solo Battery Slave (address 0x0B)
    Wire.begin(soloSMBus);
    Wire.onReceive(receiveEvent);  // interrupt handler for incoming messages
    Wire.onRequest(requestEvent);  // interrupt handler for when data is wanted  

    previousMillisLoop = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  bool received = getCommandLineFromSerialPort(CommandLine);      //global CommandLine is defined in CommandLine.h
  if (received) DoMyCommand(CommandLine);

    currentMillisLoop = millis();

    //Loop to run based on time set but intervalLoop
    if (currentMillisLoop - previousMillisLoop >= intervalLoop) {
        // save the last time you were in loop
        previousMillisLoop = currentMillisLoop;

        //Get Voltage and Current
        VoltageCurrent();
        
        //Update NeoPixels
        updateNeopixels();

    } //End of Timer If


  } // **** End of main loop

  void receiveEvent(int howMany) {

    dataRequested = 0;
    dataRequested = Wire.read(); // remember dataRequested for when we get a requestEvent

    //LED Changes state with each receiveEvent (help indicate Coms working)
    if (ledState == LOW) {
        ledState = HIGH;
    } else {
        ledState = LOW;
    }
    digitalWrite(ledPin, ledState);

  } // end of receiveEvent

  void requestEvent() {

    unsigned long currentMillisRead = millis();

    //0x09 = total battery voltage
    if (dataRequested == 0x09) {

        //Only Read Fresh Values if more than 100ms old
        if (currentMillisRead - previousMillisRead >= intervalRead) {
            // save the last time you were in loop
            previousMillisRead = currentMillisRead;
            VoltageCurrent();
        }

        //Send total voltage in 2 bytes
        byte high = highByte(pack);
        byte low = lowByte(pack);

        //create byte buffer for from int
        byte myBuff[2] = { low, high };

        //create 3 bytes to return adding in the PEC
        //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
        byte byteArray[3] = { low, high, get_PEC(soloSMBus, dataRequested, true,
                myBuff, 2) };

        //Write the 3 bytes back
        Wire.write(byteArray, 3);

    }

    //0x1c = SerialNumber
    if (dataRequested == 0x1c) {
        //Send SerialNumber in 2 bytes

        byte high = highByte(battSerial);
        byte low = lowByte(battSerial);

        //create byte buffer for from int
        byte myBuff[2] = { low, high };

        //create 3 bytes to return adding in the PEC
        //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
        byte byteArray[3] = { low, high, get_PEC(soloSMBus, dataRequested, true,
                myBuff, 2) };

        //Write the 3 bytes back
        Wire.write(byteArray, 3);

    }

    //0x28 = 4 * cell voltages, 2 bytes each
    if (dataRequested == 0x28) {

        //Only Read Fresh Values if more than 100ms old
        if (currentMillisRead - previousMillisRead >= intervalRead) {
            // save the last time you were in loop
            previousMillisRead = currentMillisRead;
            VoltageCurrent();
        }

        byte cellBuff[9];

        byte myLength = 8;
        cellBuff[0] = myLength;

        cellBuff[1] = lowByte(cell_4_mV);
        cellBuff[2] = highByte(cell_4_mV);

        cellBuff[3] = lowByte(cell_3_mV);
        cellBuff[4] = highByte(cell_3_mV);

        cellBuff[5] = lowByte(cell_2_mV);
        cellBuff[6] = highByte(cell_2_mV);

        cellBuff[7] = lowByte(cell_1_mV);
        cellBuff[8] = highByte(cell_1_mV);

        //create 10 bytes to return adding length to byte 0 and PEC to byte 9
        //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp

        byte cellArray[10] = { cellBuff[0], cellBuff[1], cellBuff[2],
                cellBuff[3], cellBuff[4], cellBuff[5], cellBuff[6], cellBuff[7],
                cellBuff[8], get_PEC(soloSMBus, dataRequested, true, cellBuff,
                        9) };

        //Write the 10 bytes back
        Wire.write(cellArray, 10);

    }

    //0x2A = current request mA double int, 4 bytes (-ve value is displayed as positive)
    if (dataRequested == 0x2A) {

        //Only Read Fresh Values if more than 100ms old
        if (currentMillisRead - previousMillisRead >= intervalRead) {
            // save the last time you were in loop
            previousMillisRead = currentMillisRead;
            VoltageCurrent();
        }

        byte currentBuff[5];
        byte myLength = 4;
        currentBuff[4] = (current >> 24) & 0xff;
        currentBuff[3] = (current >> 16) & 0xff;
        currentBuff[2] = (current >> 8) & 0xff;
        currentBuff[1] = current & 0xff;
        currentBuff[0] = myLength;

        //create 6 bytes to return adding in length and PEC
        //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
        byte byteArray[6] = { currentBuff[0], currentBuff[1], currentBuff[2],
                currentBuff[3], currentBuff[4], get_PEC(soloSMBus,
                        dataRequested, true, currentBuff, 5) };

        //Write the 6 bytes back
        Wire.write(byteArray, 6);
        
    }

    //0x10 = total capacity mAh
    if (dataRequested == 0x10) {
        //Send total capacitye in 2 bytes
        byte high = highByte(packCapacity);
        byte low = lowByte(packCapacity);

        //create byte buffer for from int
        byte myBuff[2] = { low, high };

        //create 3 bytes to return adding in the PEC
        //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
        byte byteArray[3] = { low, high, get_PEC(soloSMBus, dataRequested, true,
                myBuff, 2) };

        //Write the 3 bytes back
        Wire.write(byteArray, 3);
      
    }

    //0x0F = Remaining Capacity mAh
    if (dataRequested == 0x0F) {

        //Only Read Fresh Values if more than 100ms old
        if (currentMillisRead - previousMillisRead >= intervalRead) {
            // save the last time you were in loop
            previousMillisRead = currentMillisRead;
            VoltageCurrent();
        }

        //Send Remaining capacity in 2 bytes
        byte high = highByte(remainingCapacity);
        byte low = lowByte(remainingCapacity);

        //create byte buffer for from int
        byte myBuff[2] = { low, high };

        //create 3 bytes to return adding in the PEC
        //PEC code generation is the same checking codefrom https://github.com/3drobotics/ardupilot-solo/blob/master/libraries/AP_BattMonitor/AP_BattMonitor_SMBus_I2C.cpp
        byte byteArray[3] = { low, high, get_PEC(soloSMBus, dataRequested, true,
                myBuff, 2) };

        //Write the 3 bytes back
        Wire.write(byteArray, 3);
        
    }

  }  // end of requestEvent

//Caculate equation for battery %
void equation() {

    /*
     Cacpacity Remaining Variable Caculation
     use linear equation y = xm + c    y = %, x = voltage
     cellLow  * 4 = 0% Capacity   (0)
     cellHigh * 4 = 100% Capacity (100)
     Work out m & c
     m = 100 / ((cellHigh * 4) / (cellLow  * 4))
     c = - (cellLow  * 4) * m
    */
    m = 100 / ((float(cellHigh) * 4) - (float(cellLow) * 4));
    c = ((float(cellLow) * 4) * m) * -1;    
  
}

void displayMenu() {

  VoltageCurrent();
  Log::println("Item", TAB, TAB, TAB, TAB, "Code", TAB, "Value");
  Log::println(cell1_mText, TAB, cell1_mCommandToken);
  Log::println(cell2_mText, TAB, cell2_mCommandToken);
  Log::println(cell3_mText, TAB, cell3_mCommandToken);
  Log::println(cell4_mText, TAB, cell4_mCommandToken);
  Log::println(curr_mText, TAB, curr_mCommandToken);
    
  Log::print(cell1_rText, TAB, cell1_rCommandToken, TAB);
  Serial.println(ratioCell_1,6);
  Log::print(cell2_rText, TAB, cell2_rCommandToken, TAB);
  Serial.println(ratioCell_2,6);
  Log::print(cell3_rText, TAB, cell3_rCommandToken, TAB);
  Serial.println(ratioCell_3,6);
  Log::print(cell4_rText, TAB, cell4_rCommandToken, TAB);
  Serial.println(ratioCell_4,6);
  Log::print(current_rText, TAB, TAB, TAB, current_rCommandToken, TAB);
  Serial.println(ratioCurrent,6);  
   
  Log::print(v_refText, TAB, TAB, v_refCommandToken, TAB);
  Serial.println(vRef);   
  Log::print(volt_filtText, TAB, volt_filtCommandToken, TAB);
  Serial.println(voltageFilter,3); 

  Log::println(cell_lowText, TAB, cell_lowCommandToken, TAB, cellLow);
  Log::println(cell_highText, TAB, cell_highCommandToken, TAB, cellHigh);
  Log::println(bat_capText, TAB, TAB, bat_capCommandToken, TAB, packCapacity);
  Log::println(ser_numText, TAB, TAB, ser_numCommandToken, TAB, serialNumber);

  Log::println(neo_numText, TAB, TAB, neo_numCommandToken, TAB, numOfPixels);    
  Log::println(neo_brightText, TAB, neo_brightCommandToken, TAB, neoPixelBrightness); 
 
  Log::println(saveText, TAB, TAB, saveCommandToken); 
  Log::println(loadText, TAB, TAB, loadCommandToken); 
  Log::println(menuText, TAB, TAB, menuCommandToken); 

  Serial.println();
  Log::println("*** Current Values ***"); 
  Log::println("Cell 1 mV raw", TAB, TAB, cell_1_read); 
  Log::println("Cell 1 mV filtered", TAB, cell_1_mV); 
  Log::println("Cell 2 mV raw", TAB, TAB, cell_2_read); 
  Log::println("Cell 2 mV filtered", TAB, cell_2_mV); 
  Log::println("Cell 3 mV raw", TAB, TAB, cell_3_read); 
  Log::println("Cell 3 mV filtered", TAB, cell_3_mV); 
  Log::println("Cell 4 mV raw", TAB, TAB, cell_4_read); 
  Log::println("Cell 4 mV filtered", TAB, cell_4_mV);  
  Log::println("Pack Total mV", TAB, TAB, pack);
  Log::println("Pack %", TAB, TAB, TAB, capacityPercentage);
  Log::println("Remaining Capacity mAh", TAB, remainingCapacity);
  Log::println("Current Draw mA", TAB, TAB, current);
  Serial.println();
    
  
}
