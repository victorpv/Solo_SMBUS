#ifndef _neopixel_h
#define _neopixel_h

void updateNeopixels() {
    
    //Set NeoPixel LED's based on Capacity
    if (capacityPercentage > 90) { //90-100% LEDs Green
      if (neoCase !=1) {
        numPix = numOfPixels;  
        neoCase = 1;
        NeoPixChange = true; 
       }
    }
    else if (capacityPercentage > 80) { //80-90% LEDs Green
      if (neoCase !=2) {
        numPix = constrain(int(float(numOfPixels)* (7.0/8.0)),1,numOfPixels);
        neoCase = 2;
        NeoPixChange = true;
      }
    }
    else if (capacityPercentage > 70) { //70-80% LEDs Green
      if (neoCase !=3) {
        numPix = constrain(int(float(numOfPixels)* (6.0/8.0)),1,numOfPixels);
        neoCase = 3;
        NeoPixChange = true;
      }   
    }    
    else if (capacityPercentage > 60) { //60-70% LEDs Orange
      if (neoCase !=4) {
        numPix = constrain(int(float(numOfPixels)* (5.0/8.0)),1,numOfPixels);
        neoCase = 4;
        NeoPixChange = true;
      }      
    } 
    else if (capacityPercentage > 50) { //50-60% LEDs Orange
      if (neoCase !=5) {
        numPix = constrain(int(float(numOfPixels)* (4.0/8.0)),1,numOfPixels);
        neoCase = 5;
        NeoPixChange = true;
      }
    }     
    else if (capacityPercentage > 40) { //40-50% LEDs Orange
      if (neoCase !=6) {
        numPix = constrain(int(float(numOfPixels)* (3.0/8.0)),1,numOfPixels);
        neoCase = 6;
        NeoPixChange = true;
      }
    }      
    else if (capacityPercentage > 30) { //30-40% LEDs Red
      if (neoCase !=7) {
        numPix = constrain(int(float(numOfPixels)* (2.0/8.0)),1,numOfPixels);
        neoCase = 7;
        NeoPixChange = true; 
      }   
    } 
    else   { //Below 30% Light All LEDs RED and Flash
      neoCase = 8;
      numPix = numOfPixels;  
      NeoPixChange = true;
    }      
    
    //Adjust Brightness 0 - 100%
    RGB_Red = int(float(RGB_Red) * (float(neoPixelBrightness) / 100));
    RGB_Green = int(float(RGB_Green) * (float(neoPixelBrightness) / 100));
    RGB_Blue = int(float(RGB_Blue) * (float(neoPixelBrightness) / 100));
    
    //Only update NeoPixel if values change to stop flickering
    if (NeoPixChange) {
      NeoPixChange = false;
      lowFlash = false;
      switch (neoCase) {
        case 1:
        case 2:
        case 3:
          //Green
          RGB_Red = 0;
          RGB_Green = 255;
          RGB_Blue = 0; 
          break; 
        case 4:
        case 5:
        case 6:    
          //Orange
          RGB_Red = 255;
          RGB_Green = 165;
          RGB_Blue = 0;    
          break;
        case 7:  
          //Red
          RGB_Red = 255;
          RGB_Green = 0;
          RGB_Blue = 0;  
          break;       
        default:
          RGB_Red = 255;
          RGB_Green = 0;
          RGB_Blue = 0;  
          lowFlash = true;
          break;
      }

      pixels.clear(); // Set all pixel colors to 'off'
      RGB_Red_D = int(float(RGB_Red) * float(neoPixelBrightness) / 100);
      RGB_Green_D = int(float(RGB_Green) * float(neoPixelBrightness) / 100);
      RGB_Blue_D = int(float(RGB_Blue) * float(neoPixelBrightness) / 100);
      if (lowFlash) { //Flash red each loop (1sec on & 1sec off)
        if (lowFlashState) {
          //Set All Pixels RED
          for(int i=0; i<numOfPixels; i++) { // For each pixel...
            pixels.setPixelColor(i, pixels.Color(RGB_Red_D, RGB_Green_D, RGB_Blue_D));
          }
          lowFlashState = false; 
        }
        else {
          lowFlashState = true;
        }
      }
      else {  
        for(int i=0; i<numPix; i++) { // For each pixel...
          pixels.setPixelColor(i, pixels.Color(RGB_Red_D, RGB_Green_D, RGB_Blue_D));
        }
      }
      pixels.show();   // Send the updated pixel colors to the hardware. 
    } 
  }

  #endif
