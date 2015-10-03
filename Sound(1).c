// Sound.c, 
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 8/25/2013 
// Last Modified: 10/9/2013 
// Section 1-2pm     TA: Saugata Bhattacharyya
// Lab number: 6
// Hardware connections
#include "../tm4c123gh6pm.h"
#include "dac.h"
#include "Timer0.h"

#define SIZE_MASK_32  0x1F
#define SIZE_MASK_64  0x3F

#define SONG_SIZE     36

#define Q 						40000000
#define Q1						60000000
#define E 						20000000
#define H 					  80000000
#define REST_DURATION 50000

//not frequencies
#define A0   					220
#define B0						247
#define C0						261
#define D0						293
#define E0						330
#define F0						349
#define G0						392
#define A1							440
#define A11							466
#define B1							494
#define C1							523
#define D11							544
#define D1							587
#define E1							659
#define F1							699
#define G1							784
#define C01						277
#define C11						554

#define MEAN 32
// put code definitions for the software (actual C code)
// this file explains how the module works
static const signed char sineWave[64] = 
	{32,35,38,41,44,46,49,51,54,56,58,59,61,62,62,63,63,
	 63,62,62,61,59,58,56,54,51,49,46,44,41,38,35,32,28,
	 25,22,19,17,14,12,9,7,5,4,2,1,1,0,0,0,1,1,2,4,5,7,9,
	 12,14,17,19,22,25,28};

/*static const unsigned char flute[64] = 
	{6, 7, 9, 9, 10, 11, 12, 13, 13, 
	 14, 15, 15, 15, 15, 15, 14, 13, 
	 13, 12, 11, 10, 9, 8, 7, 7, 6, 6,
   5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 3, 
	 3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 
	 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5
	};*/
	
static const unsigned long duration[176] = {Q, E, E, Q, E, E, Q, E, E, Q, E, E, Q1, E, Q, Q, Q, Q, H, 
																					  Q1, E, Q, E, E, Q1, E, Q, E, E, Q, E, E, Q, Q, Q, Q, H,
																					  Q, E, E, Q, E, E, Q, E, E, Q, E, E, Q1, E, Q, Q, Q, Q, H, 
																					  Q1, E, Q, E, E, Q1, E, Q, E, E, Q, E, E, Q, Q, Q, Q, H,
																					  H, H, H, H, H, H, H, H, H, H, H, H, Q, Q, H, H,
																					  Q, E, E, Q, E, E, Q, E, E, Q, E, E, Q1, E, Q, Q, Q, Q, H, 
																					  Q1, E, Q, E, E, Q1, E, Q, E, E, Q, E, E, Q, Q, Q, Q, H,
																					  Q, E, E, Q, E, E, Q, E, E, Q, E, E, Q1, E, Q, Q, Q, Q, H, 
																					  Q1, E, Q, E, E, Q1, E, Q, E, E, Q, E, E, Q, Q, Q, Q, H};
static const unsigned long notes[176] = {A1, E0, F0, G0, F0, E0, D0, D0, F0, A1, G0, F0, E0, F0, G0, A1, F0, D0, D0,
																				G0, A11, D1, C1, A11, A1, F0, A1, G0, F0, E0, E0, F0, G0, A1, F0, D0, D0,
																				A1, E0, F0, G0, F0, E0, D0, D0, F0, A1, G0, F0, E0, F0, G0, A1, F0, D0, D0,
																				G0, A11, D1, C1, A11, A1, F0, A1, G0, F0, E0, E0, F0, G0, A1, F0, D0, D0,
																				A1, F0, G0, E0, F0, D0, C01, E0, A1, F0, G0, E0, F0, A1, D1, C11,
																				A1, E0, F0, G0, F0, E0, D0, D0, F0, A1, G0, F0, E0, F0, G0, A1, F0, D0, D0,
																				G0, A11, D1, C1, A11, A1, F0, A1, G0, F0, E0, E0, F0, G0, A1, F0, D0, D0,
																				A1, E0, F0, G0, F0, E0, D0, D0, F0, A1, G0, F0, E0, F0, G0, A1, F0, D0, D0,
																				G0, A11, D1, C1, A11, A1, F0, A1, G0, F0, E0, E0, F0, G0, A1, F0, D0, D0};
static unsigned char song_index;
static unsigned char rest;	
static unsigned char gain;																				
																				
static unsigned char sizeMask;
static const signed char *waveform;

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Input: Initial interrupt period
//           Units of 12ns
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
// Output: none
void Sound_Init(unsigned long period){
	volatile unsigned long delay;
	NVIC_ST_CTRL_R = 0; //disable SysTick while configuring it
	NVIC_ST_RELOAD_R = period-1;
	NVIC_ST_CURRENT_R = 0; //clear by writing any value
	NVIC_ST_CTRL_R = 0x07; //turn on SysTick with interrupt enabled
	DAC_Init();
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;	//for debugging
	delay = SYSCTL_RCGC2_R;	//wait for clock to turn on
	GPIO_PORTF_DIR_R |= 0x02;
	GPIO_PORTF_AFSEL_R &= ~(0x02);
	GPIO_PORTF_DEN_R |= 0x02;
	waveform = &sineWave[0];
	sizeMask = SIZE_MASK_64;
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Input: interrupt period
//           Units in 12ns
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
//         input of zero disables sound output
// Output: none
void Sound_Play(unsigned long period){
  NVIC_ST_RELOAD_R = period-1;
}

void Stop(void)
{
	Sound_Play(0);
	TIMER0_CTL_R &= ~TIMER_CTL_TAEN;	//disable Timer0A
	song_index = 0;
}

//notes are in Hz
//durations of notes need to be in microseconds
void Song_Note(void)
{
	/*if(song_index>10)
	{	
		Stop();
		return;
	}*/
	if(rest)
	{
		Sound_Play(0);
		TIMER0_TAILR_R = REST_DURATION;
	}else
	{	
		Sound_Play(80000000/128/notes[song_index]);
		TIMER0_TAILR_R = duration[song_index]-1;
		song_index++;
		gain = 1;
	}
	rest ^= 0x01;
}

void Song(void)
{
	song_index = 0;
	rest = 0;
	Timer0_Init(&Song_Note, 5000);//initial period before song is played
	Sound_Play(0);//silence
	
}


// Interrupt service routine
// Executed periodically, the actual period
// determined by the current Reload.
void SysTick_Handler(void){
	static unsigned char index = 0;		//used to cycle through array
	GPIO_PORTF_DATA_R |= 0x02;
	DAC_Out(waveform[index&sizeMask]); //pointer arithmetic - same as DAC_Out(*(waveform+(index&sizeMask))); 
	index++;													//&waveform[0] is equivalent to waveform and waveform[i] is equivalent to *(&waveform[0]+i)
	GPIO_PORTF_DATA_R &= ~(0x02); //debugging instrument
	gain++;
}
