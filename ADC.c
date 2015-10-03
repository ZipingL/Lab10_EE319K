// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Ziping Liuser and Kurt Efag
//  The April of the Nineth at the year two thousand and fourteen


#include "tm4c123gh6pm.h"

// This initialization function 
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain1 (PE2)
// SS3 interrupts: flag set on completion but no interrupt requested
void ADC_Initialize(void){ volatile unsigned long delay;
	//Port E initialization
	SYSCTL_RCGC2_R |= 0x10;				//Port E clock on
	delay = SYSCTL_RCGC2_R;
	delay = SYSCTL_RCGC2_R;
	delay = SYSCTL_RCGC2_R;
	delay = SYSCTL_RCGC2_R;
	
//PE2 (ADC for slide pot)
	GPIO_PORTE_DIR_R &= ~0x4;			//PE2 input
	GPIO_PORTE_AFSEL_R |= 0x4;		//enable alternate function PE2
	GPIO_PORTE_DEN_R &= ~0x4;			//disable digital PE2
	GPIO_PORTE_AMSEL_R |= 0x04;		//enable analog PE2
	GPIO_PORTE_PCTL_R &= ~0x4;		//loll idk
	
//PE3 (button for firing)
	GPIO_PORTE_DIR_R &= ~0x8;			//PE3 input
	GPIO_PORTE_AFSEL_R &= ~0x8;
	GPIO_PORTE_DEN_R |= 0x8;
	
//Port D initialization	(for DAC)
	SYSCTL_RCGC2_R |= 0x08;				//clock Port D on
	delay = SYSCTL_RCGC2_R;
	delay = SYSCTL_RCGC2_R;
	delay = SYSCTL_RCGC2_R;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTD_DIR_R |= 0xF;
	GPIO_PORTD_DIR_R &= ~0xF;
	GPIO_PORTD_DEN_R |= 0x08;
	
//ADC initialization
	SYSCTL_RCGC0_R |= 0x00010000;	//ADC clock on
	delay = SYSCTL_RCGC0_R;
	delay = SYSCTL_RCGC0_R;
	delay = SYSCTL_RCGC0_R;
	delay = SYSCTL_RCGC0_R;
	SYSCTL_RCGC0_R &= ~0x300;     //max sample rate = 125kHz (clear bits 8 and 9)
	ADC0_SSPRI_R = 0x1234;				//sequencer 3 highest priority
	ADC0_ACTSS_R &= 0xF7;					//disable seq 3 (ACTSS enables & disables sequencers)
	ADC0_EMUX_R &= ~0xF000;				//start ==> sequencer 3, software
	ADC0_SSMUX3_R &= ~0x000F;			//clear SS3 channel field
	ADC0_SSMUX3_R += 1;						//set channel Ain1(PE2)
	ADC0_SSCTL3_R = 0x0006;				//IE0=1,END0=1. TS0=0(no temp) D0=0(not differential)
	ADC0_IM_R &= ~0x0008;					//Disable interupts for sequencer 3
	ADC0_ACTSS_R |= 0x0008;				//re-enable sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC_In(void){
	unsigned long result;
	ADC0_PSSI_R = 0x0008;						//initiate Sequencer 3
	while((ADC0_RIS_R&0x08)==0){};	//wait for conversion done
	result = ADC0_SSFIFO3_R&0xFFF;	//read 12-bit result
	ADC0_ISC_R = 0x0008;						//acknowledge completion
	return result;
}


