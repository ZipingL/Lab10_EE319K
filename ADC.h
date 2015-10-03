// ADC.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Ziping Liu
//  The April of the Nineth at the year two thousand and fourteen
#ifndef ADC_H
#define ADC_H 
static int xShip;													//x-coordinate of user ship
static int xShipTrail;										//previous x-coordinate
static int newMissile=0;										//0 = shoot button not pressed, 1 = shot button fired
static char notActiveSlot =0;														//Not Active Missile slot(last place you put a new missile in array)
static char notActiveSlotE =0;
static int ADCData;														//for sending ADCData from systick interrupt to main
static int tester;
static int userDeath;											//counts number of user deaths
#endif 
// This initialization function 
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain1 (PE2)
// SS3 interrupts: flag set on completion but no interrupt requested
void ADC_Initialize(void);


//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC_In(void);
