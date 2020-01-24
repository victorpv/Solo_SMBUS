#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2020-01-22 21:16:17

#include "Arduino.h"
#include <Arduino.h>
#include ".\Solo_SMBUS.h"
#include <Wire.h>
#include <U8g2lib.h>

void setup() ;
void loop() ;
void receiveEvent (int howMany)   ;
void requestEvent ()   ;
byte get_PEC(uint8_t i2c_addr, uint8_t cmd, bool reading, const uint8_t buff[], uint8_t len) ;
void getVoltageCurrent() ;

#include "Solo_SMBUS.ino"


#endif
