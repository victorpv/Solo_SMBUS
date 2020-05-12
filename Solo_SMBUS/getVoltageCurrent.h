#ifndef _getVoltageCurrent_h
#define _getVoltageCurrent_h

 void VoltageCurrent(){

    //Read Current and Read Voltage with Voltage Divider

    //Read Voltage and Current from Analogue Inputs
    //Cell 1
    cell_1_read = int(float(analogRead(pinCell_1)) *  (float(vRef)/1023) * ratioCell_1);
    cell_1_mV = int((float(cell_1_mV) * (1 - voltageFilter)) + (float(cell_1_read) * voltageFilter));  
 
    //Cell 2
    cell_2_read = int(float(analogRead(pinCell_2)) *  (float(vRef)/1023) * ratioCell_2) - (cell_1_mV);
    cell_2_mV = int((float(cell_2_mV) * (1 - voltageFilter)) + (float(cell_2_read) * voltageFilter));  

    //Cell 3  
    cell_3_read = int(float(analogRead(pinCell_3)) * (float(vRef)/1023) * ratioCell_3)- (cell_1_mV + cell_2_mV) ; 
    cell_3_mV = int((float(cell_3_mV) * (1 - voltageFilter)) + (float(cell_3_read) * voltageFilter));
  
    //Cell 4
    cell_4_read = int(float(analogRead(pinCell_4)) *  (float(vRef)/1023) * ratioCell_4) - (cell_1_mV + cell_2_mV + cell_3_mV);
    cell_4_mV = int((float(cell_4_mV) * (1 - voltageFilter)) + (float(cell_4_read) * voltageFilter));

    //pack voltage = total of all 4 cells
    pack = cell_1_mV + cell_2_mV + cell_3_mV + cell_4_mV;

    //Work out remaining capacity
    // use linear equation y = xm + c    y = %, x = voltage
    capacityPercentage = constrain(int(float(pack) * m + c), 0, 100);

    remainingCapacity = int(float(packCapacity) * (float(capacityPercentage) / 100));
    
    //Current Read (APM Mpdule)
    float current_temp = float(analogRead(pinCurrent));
    current_temp = current_temp * (float(vRef)/1023);
    current = long(current_temp * ratioCurrent); 
    
  }

  #endif
