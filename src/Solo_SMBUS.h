/*
 * Solo_SMBUS.h
 *
 *  Created on: Jan 22, 2020
 *      Author: Victor
 */

#ifndef SOLO_SMBUS_H_
#define SOLO_SMBUS_H_

/*
 * Analog input pins assignment
 */
#define pinCell_1   A0
#define pinCell_2   A1
#define pinCell_3   A2
#define pinCell_4   A3
#define pinApmV     A7    // select the input pin for the potentiometer
#define pinCurrent     A6    // select the input pin for the potentiometer

//Pins for Reading Voltage - original
/*int pinCell_1 = A6;
 int pinCell_2 = A3;
 int pinCell_3 = A2;
 int pinCell_4 = A1;
 */




// Note I also have the APM power unit - reading whole battery voltage (i.e. same as Cell 4) - on A7

/*
 * Voltage Divider Resistors values in k ohms
 */

#define cell_1_Resistor_1   0.998f
#define cell_1_Resistor_2   18.04f
#define cell_2_Resistor_1   10.06f
#define cell_2_Resistor_2   10.11f
#define cell_3_Resistor_1   19.90f
#define cell_3_Resistor_2   10.01f
#define cell_4_Resistor_1   30.01f
#define cell_4_Resistor_2   10.04f

//Caculate Voltage Divider Ratios
//Voltage Divider ratio = (Resistor_1 + Resistor_2) / Resistor_2
/*
 ratioCell_1 = (cell_1_Resistor_1 + cell_1_Resistor_2)/cell_1_Resistor_2;
 ratioCell_2 = (cell_2_Resistor_1 + cell_2_Resistor_2)/cell_2_Resistor_2;
 ratioCell_3 = (cell_3_Resistor_1 + cell_3_Resistor_2)/cell_3_Resistor_2;
 ratioCell_4 = (cell_4_Resistor_1 + cell_4_Resistor_2)/cell_4_Resistor_2;
 */

// The below values are as measured from my own setup.
#define ratioCell_1     4.208416834f
#define ratioCell_2     8.43373494f
#define ratioCell_3     12.64794383f
#define ratioCell_4     16.56448203f
#define currentRatio    44.23213022f

//float vRef = 5000; //mV - Should now be 5000mV using the 5V regulator
#define vRef                3300.0f //mV - Alternative if using now be 3.3v as the reference

//Setup for Current Measurement using ACS758LCB-050B  -- NOTE I am not using this
#define offset -70.0f               // -70mV to get 0 at no current
#define QOV (0.5 * vRef) + offset   // set quiescent Output voltage (as per data sheet)

#endif /* SOLO_SMBUS_H_ */
