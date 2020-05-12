//https://create.arduino.cc/projecthub/mikefarr/simple-command-line-interface-4f0a3f?ref=similar&ref_id=106094&offset=5
//https://majenko.co.uk/blog/evils-arduino-strings

/*****************************************************************************

  How to Use CommandLine:
    Create a sketch.  Look below for a sample setup and main loop code and copy and paste it in into the new sketch.

   Create a new tab.  (Use the drop down menu (little triangle) on the far right of the Arduino Editor.
   Name the tab CommandLine.h
   Paste this file into it.

  Test:
     Download the sketch you just created to your Arduino as usual and open the Serial Window.  Typey these commands followed by return:
      add 5, 10
      subtract 10, 5

    Look at the add and subtract commands included and then write your own!


*****************************************************************************
  Here's what's going on under the covers
*****************************************************************************
  Simple and Clear Command Line Interpreter

     This file will allow you to type commands into the Serial Window like,
        add 23,599
        blink 5
        playSong Yesterday

     to your sketch running on the Arduino and execute them.

     Implementation note:  This will use C strings as opposed to String Objects based on the assumption that if you need a commandLine interpreter,
     you are probably short on space too and the String object tends to be space inefficient.

   1)  Simple Protocol
         Commands are words and numbers either space or comma spearated
         The first word is the command, each additional word is an argument
         "\n" terminates each command

   2)  Using the C library routine strtok:
       A command is a word separated by spaces or commas.  A word separated by certain characters (like space or comma) is called a token.
       To get tokens one by one, I use the C lib routing strtok (part of C stdlib.h see below how to include it).
           It's part of C language library <string.h> which you can look up online.  Basically you:
              1) pass it a string (and the delimeters you use, i.e. space and comman) and it will return the first token from the string
              2) on subsequent calls, pass it NULL (instead of the string ptr) and it will continue where it left off with the initial string.
        I've written a couple of basic helper routines:
            readNumber: uses strtok and atoi (atoi: ascii to int, again part of C stdlib.h) to return an integer.
              Note that atoi returns an int and if you are using 1 byte ints like uint8_t you'll have to get the lowByte().
            readWord: returns a ptr to a text word

   4)  DoMyCommand: A list of if-then-elses for each command.  You could make this a case statement if all commands were a single char.
      Using a word is more readable.
          For the purposes of this example we have:
              Add
              Subtract
              nullCommand
*/
/******************sample main loop code ************************************

  #include "CommandLine.h"

  void
  setup() {
  Serial.begin(115200);
  }

  void
  loop() {
  bool received = getCommandLineFromSerialPort(CommandLine);      //global CommandLine is defined in CommandLine.h
  if (received) DoMyCommand(CommandLine);
  }

**********************************************************************************/

//Name this tab: CommandLine.h

#include <string.h>
#include <stdlib.h>

void writeToEEPROM();
void readFromEEPROM();
void VoltageCurrent();
void equation();
void displayMenu();

//this following macro is good for debugging, e.g.  print2("myVar= ", myVar);
#define print2(x,y) (Serial.print(x), Serial.println(y))


#define CR '\r'
#define LF '\n'
#define BS '\b'
#define NULLCHAR '\0'
#define SPACE ' '

#define COMMAND_BUFFER_LENGTH        25                        //length of serial buffer for incoming commands
char   CommandLine[COMMAND_BUFFER_LENGTH + 1];                 //Read commands into this buffer from Serial.  +1 in length for a termination char

const char *delimiters            = ", \n";                    //commands can be separated by return, space or comma

/*************************************************************************************************************
     your Command Names Here
*/
const char *cell1_mCommandToken     = "c1m";  
const char *cell2_mCommandToken     = "c2m"; 
const char *cell3_mCommandToken     = "c3m"; 
const char *cell4_mCommandToken     = "c4m";    
const char *curr_mCommandToken      = "cum";   
const char *cell1_rCommandToken     = "c1r"; 
const char *cell2_rCommandToken     = "c2r"; 
const char *cell3_rCommandToken     = "c3r"; 
const char *cell4_rCommandToken     = "c4r"; 
const char *current_rCommandToken   = "cur"; 
const char *cell_lowCommandToken    = "low"; 
const char *cell_highCommandToken   = "high"; 
const char *neo_numCommandToken     = "leds";
const char *neo_brightCommandToken  = "bright";
const char *ser_numCommandToken     = "serial";
const char *bat_capCommandToken     = "cap";
const char *v_refCommandToken       = "ref";
const char *volt_filtCommandToken   = "filter";
const char *saveCommandToken        = "save";
const char *loadCommandToken        = "load";
const char *menuCommandToken        = "?";

const char *cell1_mText     = "Cell 1 Measured Value (mV)"; 
const char *cell2_mText     = "Cell 2 Measured Value (mV)"; 
const char *cell3_mText     = "Cell 3 Measured Value (mV)"; 
const char *cell4_mText     = "Cell 4 Measured Value (mV)"; 
const char *curr_mText      = "Current Measured Value (mA)";

const char *cell1_rText     = "Cell 1 Voltage Divider Ratio"; 
const char *cell2_rText     = "Cell 2 Voltage Divider Ratio";
const char *cell3_rText     = "Cell 3 Voltage Divider Ratio";
const char *cell4_rText     = "Cell 4 Voltage Divider Ratio";
const char *current_rText   = "Current Ratio"; 

const char *cell_lowText    = "Low Value For Capacity % (mV)"; 
const char *cell_highText   = "High Value For Capacity % (mV)";  
const char *neo_numText     = "Number of Neopixel LEDs";
const char *neo_brightText  = "Neopixel Brightness 0 - 100";
const char *ser_numText     = "Battery Serial Number";
const char *bat_capText     = "Battery Capacity (mAh)";
const char *v_refText       = "Voltage Reference (mV)";
const char *volt_filtText   = "Voltage Filter 0.0 - 1.0";
const char *saveText        = "Save Values To EEPROM";
const char *loadText        = "Load Values From EEPROM";
const char *menuText        = "Display This Menu";




/*************************************************************************************************************
    getCommandLineFromSerialPort()
      Return the string of the next command. Commands are delimited by return"
      Handle BackSpace character
      Make all chars lowercase
*************************************************************************************************************/

bool
getCommandLineFromSerialPort(char * commandLine)
{
  static uint8_t charsRead = 0;                      //note: COMAND_BUFFER_LENGTH must be less than 255 chars long
  //read asynchronously until full command input
  while (Serial.available()) {
    char c = Serial.read();
    switch (c) {
      case CR:      //likely have full command in buffer now, commands are terminated by CR and/or LS
      case LF:
        commandLine[charsRead] = NULLCHAR;       //null terminate our command char array
        if (charsRead > 0)  {
          charsRead = 0;                           //charsRead is static, so have to reset
          Serial.println(commandLine);
          return true;
        }
        break;
      case BS:                                    // handle backspace in input: put a space in last char
        if (charsRead > 0) {                        //and adjust commandLine and charsRead
          commandLine[--charsRead] = NULLCHAR;
          Serial << byte(BS) << byte(SPACE) << byte(BS);  //no idea how this works, found it on the Internet
        }
        break;
      default:
        // c = tolower(c);
        if (charsRead < COMMAND_BUFFER_LENGTH) {
          commandLine[charsRead++] = c;
        }
        commandLine[charsRead] = NULLCHAR;     //just in case
        break;
    }
  }
  return false;
}


/* ****************************
   readNumber: return a 16bit (for Arduino Uno) signed integer from the command line
   readWord: get a text word from the command line

*/
int
readInt () {
  char * numTextPtr = strtok(NULL, delimiters);         //K&R string.h  pg. 250
  return atoi(numTextPtr);                              //K&R string.h  pg. 251
}

float
readFloat () {
  char * numTextPtr = strtok(NULL, delimiters);         
  return atof(numTextPtr);                             
  
  }

char * readWord() {
  char * word = strtok(NULL, delimiters);               //K&R string.h  pg. 250
  return word;
}

void
nullCommand(char * ptrToCommandName) {
  print2("Command not found: ", ptrToCommandName);      //see above for macro print2
}


/****************************************************
   Add your commands here
*/




/****************************************************
   DoMyCommand
*/
bool
DoMyCommand(char * commandLine) {
  //  print2("\nCommand: ", commandLine);
  int result;
  float resultFloat;

  char * ptrToCommandName = strtok(commandLine, delimiters);
  //  print2("commandName= ", ptrToCommandName);

  //Cell 1 Measured Ratio Caculation   
  if (strcmp(ptrToCommandName, cell1_mCommandToken) == 0) {          
    int firstOperand = readInt();
    VoltageCurrent();
    if (cell_1_read != 0  && firstOperand != 0) {
      ratioCell_1 = (float(firstOperand) / float(cell_1_read)) * ratioCell_1;
     }
     else {
       Serial.print(caclError);
     }
   } 
   
  //Cell 2 Measured Ratio Caculation   
  else if (strcmp(ptrToCommandName, cell2_mCommandToken) == 0) {  
    int firstOperand = readInt();
    VoltageCurrent();    
    if (cell_2_read != 0  && firstOperand != 0) {
      ratioCell_2 = (float(firstOperand) + float(cell_1_read)) /(float(cell_1_read) + float(cell_2_read))  * ratioCell_2;
    } 
    else {
      Serial.print(caclError);
    }  
  }
  
  //Cell 3 Measured Ratio Caculation 
  else if (strcmp(ptrToCommandName, cell3_mCommandToken) == 0) {    
    int firstOperand = readInt();
    VoltageCurrent();    
    if (cell_3_read != 0  && firstOperand != 0) {
      ratioCell_3 = (float(firstOperand) + float(cell_1_read + cell_2_read)) /(float(cell_1_read) + float(cell_2_read) + float(cell_3_read))  * ratioCell_3;
    }     
    else {
      Serial.print(caclError);
     }    
  }
  
  //Cell 4 Measured Ratio Caculation 
  else if (strcmp(ptrToCommandName, cell4_mCommandToken) == 0) {   
    int firstOperand = readInt();    
    VoltageCurrent();
    if (cell_4_read != 0  && firstOperand != 0) {
      ratioCell_4 = (float(firstOperand) + float(cell_1_read) + float(cell_2_read) + float(cell_3_read)) /(float(cell_1_read) + float(cell_2_read) + float(cell_3_read) + float(cell_4_read))  * ratioCell_4;
    }  
    else {
      Serial.print(caclError);
    }   
  }

  //Current Measured Ratio Caculation
  else if (strcmp(ptrToCommandName, curr_mCommandToken) == 0) {  
    int firstOperand = readInt();
    VoltageCurrent();
    if (current != 0  && firstOperand != 0) {
      ratioCurrent = (float(firstOperand) / float(current)) * ratioCurrent;  
    }  
    else {
      Serial.print(caclCurError);
    }  
  }

  //Cell 1 Ratio Update
  else if (strcmp(ptrToCommandName, cell1_rCommandToken) == 0) {  
    ratioCell_1 = readFloat();
  }

  //Cell 2 Ratio Update
  else if (strcmp(ptrToCommandName, cell2_rCommandToken) == 0) {  
    ratioCell_2 = readFloat();
  }

  //Cell 3 Ratio Update
  else if (strcmp(ptrToCommandName, cell3_rCommandToken) == 0) {  
    ratioCell_3 = readFloat();
  }

  //Cell 4 Ratio Update
  else if (strcmp(ptrToCommandName, cell4_rCommandToken) == 0) {  
    ratioCell_4 = readFloat();
  } 

  //Current Ratio Update
  else if (strcmp(ptrToCommandName, current_rCommandToken) == 0) {  
    ratioCurrent = readFloat();
  }

  //Cell Low Value Update
  else if (strcmp(ptrToCommandName, cell_lowCommandToken) == 0) {  
    cellLow = readInt();
    equation();
  }

  //Cell High Value Update
  else if (strcmp(ptrToCommandName, cell_highCommandToken) == 0) {  
    cellHigh = readInt();
    equation();
  }
  
  //Update Number of Neopixel LEDs
  else if (strcmp(ptrToCommandName, neo_numCommandToken) == 0) { 
    int intTemp = readInt();
    numOfPixels = constrain(intTemp,1,100);  
    //Update Number of Neopixels
    pixels.updateLength(numOfPixels);
    neoCase = 0;    
    updateNeopixels();
  }

  //Update Neopixel Brightness
  else if (strcmp(ptrToCommandName, neo_brightCommandToken) == 0) { 
    int intTemp = readInt();
    neoPixelBrightness = constrain(intTemp,5,100); 
    neoCase = 0;
    updateNeopixels();
  }

  //Update Serial Number
  else if (strcmp(ptrToCommandName, ser_numCommandToken) == 0) { 
    serialNumber = readInt(); 
  }

  //Update Battery Capacity (limit to 30,000mAh)
  else if (strcmp(ptrToCommandName, bat_capCommandToken) == 0) { 
    int intTemp = readInt();
    packCapacity = constrain(intTemp,1000,30000);  
  }

  //Update Reference Voltage (limit to 10,000mV)
  else if (strcmp(ptrToCommandName, v_refCommandToken) == 0) { 
    int intTemp = readInt();
    vRef = constrain(intTemp,5,6000);  
  }  

  //Update Voltage Filter (range 0 - 1)
  else if (strcmp(ptrToCommandName, volt_filtCommandToken) == 0) { 
    float floatTemp = readFloat();
    voltageFilter = constrain(floatTemp,0.01,1.00);  
  }  
   
  //Write Values To EEPROM
  else if (strcmp(ptrToCommandName, saveCommandToken) == 0) {           
    writeToEEPROM();
  } 

  //Load From EEPROM   
  else if (strcmp(ptrToCommandName, loadCommandToken) == 0) {           
    readFromEEPROM();
  }      

  //Display Items To Change
  else if (strcmp(ptrToCommandName, menuCommandToken) == 0) {           
    displayMenu();  
  } 
     
   else {
      nullCommand(ptrToCommandName);
    }
  }
