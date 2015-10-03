// Lab10 Arcade Style Game
// main.c
// Runs on LM4F120 or TM4C123
// Put your name here or look very silly
// Put the date here or look very silly

// Graphic display on Kentec EB-LM4F120-L35
// Sound output to DAC (Lab 7)
// Analog Input connected to PE2=ADC1, 1-D joystick (Lab 8)
// optional: UART link to second board for two-player mode
// Switch input

#include "tm4c123gh6pm.h"
#include "SSD2119.h"
#include "PLL.h"
#include "random.h"
#include "sounds.h"       // audio waveforms
#include "pictures.h"
#include "ADC.h"



#define PF2       (*((volatile unsigned long *)0x40025010))

void EnableInterrupts(void);
void Timer2_Init(unsigned long period);
unsigned long TimerCount;
unsigned long Semaphore;
	  int deathPause;
		int timeCounter;
			typedef struct bigSpriteState_struct{				//struct type for storing xy coordinates and state (0= dead, 1= alive)
		signed short x;
		signed short y;
		char s;
		char e;
		char b;
    char c;
		char w;
		char h;
		char wait;
}bigSpriteState;
	typedef struct smallSpriteState_struct{
		signed short x;
		signed short y;
		char s;
	}smallSpriteState;
static smallSpriteState missile[100];										//struct for missile status
static smallSpriteState emissile[100]; 
static bigSpriteState enemy[20];												//struct for 5 enemies
		char i; 																	//for indexing through user arrays
		int Position;															//transducer position 
		int enemycount=0;													//# of enemies down in current wave
    char k;																		//for indexing through enemy arrays
		char j;
    char wave=11;
    char wave2=0;
	  char wave3=0;
	  char wave4=0;
    char wave2big=0;
	  char wave3big=0;
    unsigned char countx=0;
		int countx2=0;
	  int countx3=0;
	  int county=0;
		signed short crashTestY;									//for testing enemy/missile collisions
		signed short crashTestXR;
		signed short crashTestXL;
		signed short trainWreckL;										//these two for testing user/enemy collisions
	  signed short trainWreckR;
    int timecheck;
    char wavesize;
		int Semaphore3;


//when a user dies, we are going to print "USER DEATH" and pause EVERYthing for a second//
void DEATH(void){
		LCD_SetCursor(140,80);	
		LCD_SetTextColor(255,0,0);
		LCD_PrintString("USER DEATH");
	for(deathPause = 0; deathPause < 30; deathPause++){
			while(Semaphore == 0){};																	//do nothing until SysTick interrupt
				Semaphore = 0;																				//"acknowledge" SysTick
				if(newMissile >3){newMissile = 0;}												//missile count probably very high from all the SysTick calls
			}
		
		}	

		
void SysTick_Init (void){
	NVIC_ST_CTRL_R = 0;         			// disable SysTick during setup
	NVIC_ST_RELOAD_R = 0x0028B0AA; 		// 30 Hz
	NVIC_ST_CURRENT_R = 0;      			// any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2       
	NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
	EnableInterrupts();

}

void SysTick_Handler(void){
		ADCData = ADC_In();
		if((GPIO_PORTE_DATA_R)&&0x8){ 
				newMissile++;}									//New Missile if PE3 
		
		for(i=0;i<20;i++){
				if ( (enemy[i].s==1)&&(enemy[i].wait > 1) ){			//if wait not done and enemy exists
								enemy[i].wait--;											//decrement wait time
		    }
		}
		if(newMissile>3){newMissile=0;}
														
		Semaphore = 1;													//semaphore set signifying updated data
}




void DAC_Out(unsigned char voltage){
			GPIO_PORTD_DATA_R = voltage;	   //voltage 0-15
}
void Timer2A_Stop(void){
TIMER2_CTL_R &= ~0x00000001; // disable
}
void Timer2A_Start(void){
TIMER2_CTL_R |= 0x00000001; // enable
TimerCount = 0;
}
void Timer2A_Handler(void){
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
	DAC_Out(sound[TimerCount]);
	if(TimerCount < 1837){
			TimerCount++;}
	else{Timer2A_Stop();}
			
}
// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  //Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
	Timer2A_Start();
}    

//////////////////////////////////////////////////////////////////////////***START OF PROGRAM***//////////////////////////////////////////////////////////////////////////////////////////////
int main(void){
	
//Initialization
	PLL_Init();  // Set the clocking to run at 80MHz from the PLL.
  LCD_Init();  // Initialize LCD
  LCD_Goto(10,0);
  LCD_SetTextColor(255,255,0); // yellow= red+green, no blue
  printf("Lab 10");
  LCD_DrawLine(10,16,310,16,BURNTORANGE);
	ADC_Initialize();
	Timer2_Init(7256);
	Timer2A_Stop();
	SysTick_Init();





	
///////////////////////////////////////////////////////////////////////////Set Up Waves///////////////////////////////////////////////////////////////////////////////////////////////
	while(1){
     	//set up for the wave 1 (11 means wave needs to be setup, 10 means wave is set up)
			if (wave==11) {  
				wavesize=5;
				enemycount=0;
				LCD_SetCursor(140,120);
				LCD_SetTextColor(255,0,0);
        LCD_PrintString("WAVE ONE");	
				LCD_SetTextColor(0,0,0);
				LCD_PrintString("WAVE ONE");
				 
				enemy[0].x=50;
				enemy[1].x=90;
				enemy[2].x=130;
				enemy[3].x=170;
				enemy[4].x=210;
				for (k=0;k<5;k++) {
					enemy[k].y=40;
					enemy[k].s=1;
					enemy[k].e=0;
					enemy[k].c=0;
					enemy[k].b=0;
					wave=10;
				}
			}
     //set up for wave 2 (22 means wave needs to be setup, 20 means wave is set up)
    	//e: so we don't print eraser more than once
			//s: so we know the state of the sprite
			//c; so we don't count the death of a sprite more than once
			
	if (wave==21) {  
        wavesize=15;				
				enemycount=0;
				LCD_SetCursor(140,120);
				LCD_SetTextColor(255,0,0);
        LCD_PrintString("WAVE TWO");	
				LCD_SetTextColor(0,0,0);
				LCD_PrintString("WAVE TWO"); 
				enemy[0].x=50;
				enemy[1].x=90;
				enemy[2].x=130;
				enemy[3].x=170;
				enemy[4].x=210;
				for (k=0;k<5;k++) {
					enemy[k].y=40;
					enemy[k].s=1;
					enemy[k].e=0;
					enemy[k].c=0;
					enemy[k].b=0;
	       
				}
				for (k=5;k<10;k++) {
					enemy[k].s=4;       //set newer coming enemies to 4, so they don't get counted as dead when they don't arrive yet (count happens after printing)
					enemy[k].e=0;
					enemy[k].y=40;
					enemy[k].x=50;
					enemy[k].c=0;
					enemy[k].b=0;
				}
				for (k=10;k<15;k++) {
					enemy[k].s=4;
					enemy[k].s=4;       //set newer coming enemies to 4, so they don't get counted as dead when they don't arrive yet (count happens after printing)
					enemy[k].e=0;
					enemy[k].y=64;
					enemy[k].x=296;
					enemy[k].c=0;
					enemy[k].b=0;
				}
				wave=20;
			}
		//set up for wave 3
		//set up for wave 3
			if(wave==31) {
				wavesize=1;
			  enemycount=0;
				LCD_SetCursor(140,30);
				LCD_SetTextColor(255,0,0);
        LCD_PrintString("Time For Ziping");	
				for(k=0;k<10;k++) {enemy[k].x=0;enemy[k].y=0;enemy[k].s=0;;enemy[k].e=0;}
				enemy[0].s=1;
				enemy[0].x=100;
				enemy[0].y=40;
				enemy[0].b=0;
				enemy[0].e=0;
				wave=30;
			}
						
						
						
 ////////////////////////////////////////////////////////////////////////////Update and Print User Ship/////////////////////////////////////////////////////////////////////////////////////////          
		if(Semaphore){																//wait for SysTick
			Semaphore = 0; 																	//"acknowledge" SysTick
			ADCData = ADC_In();
			Position=(ADCData*.4053751277)+157.0113976;			//position 0-2000
			xShip = ((Position)*.14)+20;									//x pixel coordinate of center of ship (.14 = 280/2000)
			LCD_DrawBMP(UshipErase,xShipTrail,200);					//erase previous image
			LCD_DrawBMP(userShip,xShip,200);
			xShipTrail = xShip;
			
			
			
			
			
////////////////////////////////////////////////////////////////////////////////////Update Enemies/////////////////////////////////////////////////////////////////////////////////////////////

			//enemy ship update coordinates for wave 1	 (s=1 means alive, s=0 means just dead, s=2 means the moment of death)
		  if (wave==10) {
			for (k=0;k<5;k++){
						if (enemy[k].s == 1) {
							enemy[k].y = enemy[k].y +1;
											trainWreckL = xShip-enemy[k].x;												//enemy within left boundary of ship
											trainWreckR = enemy[k].x - xShip;											//enemy within right boundary of ship
											
											if( (enemy[k].y >= 176) &&
													(((trainWreckL <24)&&(trainWreckL >0) )||((trainWreckR < 30)&&(trainWreckR > 0))) ){
														enemy[k].s = 2;
														DEATH();								
														userDeath++;
													}	
					  if (enemy[k].y>=210) {enemy[k].s=0;}
						} }
					}
			//enemy ship update coordinates for wave 2 
						//enemy ship update coordinates for wave 2 
			else if (wave==20) {
				for (k=0;k<5;k++) { if(enemy[k].s==1) {       //updates only if object is alive (s==1)
					if (countx<=92 && wave2==0) {
						enemy[k].x = enemy[k].x +1;
					}
					countx++;
					if (countx >92 && wave2==0) {wave2=1; countx=0;}
					if (wave2==1) {
						for (j=0;j<5;j++) {
							enemy[j].y=enemy[j].y+1;
							
						}
						wave2=2;  
						countx=0;
						county++;
					} 
					if (countx <=92 && wave2==2) {
						enemy[k].x= enemy[k].x-1;
					}
					countx++;
					if (countx >= 92 && wave2==2 ) {wave2=3; countx=0;}
					if (wave2==3) {
						for (j=0;j<5;j++) {
							enemy[j].y=enemy[j].y+1;
							
						}
						wave2=0; county++;
					}
					trainWreckL = xShip-enemy[j].x;												//enemy within left boundary of ship
					trainWreckR = enemy[j].x - xShip;											//enemy within right boundary of ship
					
					if( (enemy[j].y >= 176) &&
													(((trainWreckL <24)&&(trainWreckL >0) )||((trainWreckR < 30)&&(trainWreckR > 0))) ){
						enemy[j].s = 2;
						DEATH();								
						userDeath++;
					}	
					if (enemy[k].y>=210) {enemy[k].s=0;}
				}
			}
				//fleet 2
			if (county==24 || enemycount==10) {enemy[5].s=1; wave3=1;} //activate the bigger ships once there is room vertically, e.i. check if y coordinate has been met
			if (countx2==24) {enemy[6].s=1;}
			if (countx2==48 ) {enemy[7].s=1;}
			if (countx2==72) {enemy[8].s=1;}
			if (countx2==96) {enemy[9].s=1;}
	
			//if (enemy[5].x>=196) {wave2big=1;} //once the enemies get to the edge of the screen, set a flag (wave big) to one
			if (countx2>=146) {wave2big=1;}
	    for (k=5;k<10;k++) {
				if(wave3==1 && k==5) {countx2++;}	
				if(enemy[k].s==1 && wave2big==0) {  //update coordinates for new ships for wave 2 only if they become active (due to above code) and if max x coordinate hasn't been set
				enemy[k].x=enemy[k].x+1;	}
        	
			 if(enemy[k].y>=210) {enemy[k].s=0;} //enemy dead if off the screen vertically, we don't care if horizontally
			 if(enemy[k].s==1 && wave2big==1) {
				 if(k==5 || k==6) {enemy[k].x=enemy[k].x-1; enemy[k].y=enemy[k].y+1;}
				 if(k==8 || k==9) {enemy[k].x=enemy[k].x+1; enemy[k].y=enemy[k].y+1;}
				 if(k==7 && enemy[k].y<=100) {enemy[k].y=enemy[k].y+1;} 
			 }
			 
		 }
			 			 //fleet 3
			if (county==48 || enemycount==10) {enemy[10].s=1; wave4=1;} //activate the bigger ships once there is room vertically, e.i. check if y coordinate has been met
			if (countx3==24) {enemy[11].s=1;}
			if (countx3==48 ) {enemy[12].s=1;}
			if (countx3==72) {enemy[13].s=1;}
			if (countx3==96) {enemy[14].s=1;}
			 
				if (countx3>=146) {wave3big=1;}
	    for (k=10;k<15;k++) {
				if(wave4==1 && k==10) {countx3++;}	
				if(enemy[k].s==1 && wave3big==0) {  //update coordinates for new ships for wave 2 only if they become active (due to above code) and if max x coordinate hasn't been set
				enemy[k].x=enemy[k].x-1;	}
        	
			 if(enemy[k].y>=210) {enemy[k].s=0;} //enemy dead if off the screen vertically, we don't care if horizontally
			 if(enemy[k].s==1 && wave3big==1) {
         if(k==10) {if(enemy[k].y<120 && enemy[k].x==50) {enemy[k].y=enemy[k].y+1;}
				          if(enemy[k].y==120 && enemy[k].x<130) {enemy[k].x=enemy[k].x+1;}
									if(enemy[k].y>64 && enemy[k].x==130) {enemy[k].y=enemy[k].y-1;}
									if(enemy[k].y==64 && enemy[k].x>50) {enemy[k].x=enemy[k].x-1;}}
				
			   if(k==14) {if(enemy[k].y<120 && enemy[k].x==246) {enemy[k].y=enemy[k].y+1;}
				          if(enemy[k].y==120 && enemy[k].x>166) {enemy[k].x=enemy[k].x-1;}
									if(enemy[k].y>64 && enemy[k].x==166) {enemy[k].y=enemy[k].y-1;}
									if(enemy[k].y==64 && enemy[k].x<246) {enemy[k].x=enemy[k].x+1;}}
				 if(k==11 || k==13 || k==12) {enemy[k].y=enemy[k].y+1;}
			 }
									
			 } 
			 
			 
			 
			 
			
			
			
		}
		
	
										
										
	///////////////////////////////////////////////////////////////////////Update User missiles////////////////////////////////////////////////////////////////////////////////////////////////
			for(i=0;i<100;i++){														
					if(missile[i].s){
						  missile[i].y = missile[i].y - 2;							//if missile is active, move up the screen
							if(missile[i].y < 18 ){
									missile[i].s = 0;																				//if the missile has moved off the screen, deactivate it
									LCD_DrawBMP(UmissileErase,missile[i].x,missile[i].y);		//cover up with black
							      }
							
										
												
							
//////////////////////////////////////////////////////////////////Test for Enemy-Missile Collisions (small aliens)///////////////////////////////////////////////////////////////////////////////////////
										for(k=0;k<wavesize;k++){																																						      //(missile:11x15, enemy: 24x24)
										crashTestY = ((enemy[k].y + 24) - missile[i].y);											//difference in y-coordinates
										crashTestXR = (missile[i].x - enemy[k].x);													//distance of missile from right border of alien
										crashTestXL = (enemy[k].x - missile[i].x);													//distance of missile from left border of alien
										
											if   ( ( (crashTestY <5) && (crashTestY > 0) ) &&
														( (	(crashTestXR < 24) && (crashTestXR > 0 ) ) || ( (crashTestXL < 11)&&(crashTestXL > 0) ) ) ){
															enemy[k].s = 2;
														}
										
									 
					          }
					 }
			}
			
			
			
/////////////////////////////////////////////////////////////////////Test for Enemy-User Collisions///////////////////////////////////////////////////////////////////////////////////////////////////
									/*	for(k=0;k<5;k++){
											trainWreckL = xShip-enemy[k].x;												//enemy within left boundary of ship
											trainWreckR = enemy[k].x - xShip;											//enemy within right boundary of ship
											
											if( (enemy[k].y >= 176) &&
													(((trainWreckL <24)&&(trainWreckL >0) )||((trainWreckR < 30)&&(trainWreckR > 0))) ){
														enemy[k].s = 2;
														DEATH();								
														userDeath++;
													}		*/		
//////////////////////////////////////////////////////////////////////////Activate New Missile/////////////////////////////////////////////////////////////////////////////////////////////
					if(newMissile==3){
							while(missile[notActiveSlot].s == 1){												//find a non-active missile in the array
									if(notActiveSlot == 99){notActiveSlot = 0;}										//wrap around if at end of array
									notActiveSlot++;																							//check next slot
								}
							missile[notActiveSlot].x = xShip + 10;												//missile(11x15) aligned to center of ship
							missile[notActiveSlot].y = 185;																//missile firing out 200 - 15
							missile[notActiveSlot].s = 1;																//activate
							Timer2A_Start();																								//arm sound
							newMissile=0;                                                 //reset button count
								tester++;
							
					}
					
						 
						 
						
	///////////////////////////////////////////////////////Update Enemy Missiles/////////////////////////////////////////////////////////////////////////////////////////////////////////
					for(i=0;i<100;i++){
								if(emissile[i].s==1){
										emissile[i].y = emissile[i].y + 2;
										if(emissile[i].y > 210){
												emissile[i].s = 0;
												LCD_DrawBMP(UmissileErase,emissile[i].x,emissile[i].y);
										}
									}
								}
									

///////////////////////////////////////////////////////////////////////Test for User Missile Collisions////////////////////////////////////////////////////////////////////////////////////
					for(i=0;i<100;i++){
								if((emissile[i].s ==1)&&(emissile[i].y >= 185)&&
										((((emissile[i].x - xShip)<30)&&((emissile[i].x - xShip)>0))|| 
													(((xShip- emissile[i].x)<11)&&((xShip-emissile[i].x)>0)))){
										emissile[i].s = 0;
										DEATH();
										userDeath++;
							 }
					}


////////////////////////////////////////////////////////////////////////New Enemy Missiles////////////////////////////////////////////////////////////////////////////////////////////////////
//activate new enemy missiles
					for(k=0;k<wavesize;k++){
									if((enemy[k].s)&&(enemy[k].wait == 1)){																	//alive and finished (.wait == 1 means countdown done)
															while(emissile[notActiveSlotE].s == 1){												//find a non-active enemy missile slot in array
																	if(notActiveSlotE == 99){notActiveSlotE = 0;}							//wrap around if at end of array
																	else notActiveSlotE++;																			//check next slot
															 }
												emissile[notActiveSlotE].x = enemy[k].x + (enemy[k].w / 2);    //center horizontal positioning of missile on enemy
												emissile[notActiveSlotE].y = enemy[k].y + enemy[k].h;						//get correct vertical positioning for missile
												emissile[notActiveSlotE].s = 1;																	//activate missile
												enemy[k].wait = 0;																							//signifying missile fired, reaady for new wait time
									}
					}


//loading enemies that just fired with new wait times
					for(k=0;k<wavesize;k++){
									if((enemy[k].s)&&(enemy[k].wait == 0)){					//alive and not currently waiting to fire (.wait == 0 means need new time)
												j = (Random()%60);											//get random number from 0-30
												if(j < 20){j=20;}												//minimum # 20 (3 shots per second max firing)
												enemy[k].wait = j;
									}
					}

	

	/////////////////////////////////////////////////////////////////////enemy missile printing//////////////////////////////////////////////////////////////////////////////////////////////
			for(i=0;i<100;i++){
					if(emissile[i].s == 1){
						LCD_DrawBMP(enemyMissile,emissile[i].x,emissile[i].y);}}
					/*else if ((emissile[i].s == 0)&&(emissile[i].b==0)){
							LCD_DrawBMP(UmissileErase,emissile[i].x,emissile[i].y);}
							emissile[i].b = 1;
					}*/
				
	

						 
//////////////////////////////////////////////////////////////////////////////missile printing///////////////////////////////////////////////////////////////////////////////////////////
			for(i=0;i<100;i++){
				if(missile[i].s){
					LCD_DrawBMP(userMissile,missile[i].x,missile[i].y);							//print if missile is active
			   	}
				}


				
					

            
    
             
						
								
								
								

///////////////////////////////////////////////////////////////////////////enemy ship printing///////////////////////////////////////////////////////////////////////////////////////
/////////////ship printing for wave 1
				if(wave==10) {
			for (k=0;k<5;k++) {
				if (enemy[k].s ==1) {
			LCD_DrawBMP(AlienEnemyBig, enemy[k].x,enemy[k].y); }
				if (enemy[k].s ==2 &&enemy[k].b ==0) {
			LCD_DrawBMP(Explosion1, enemy[k].x,(enemy[k].y)); LCD_DrawBMP (Explosion1, enemy[k].x, enemy[k].y);
			LCD_DrawBMP(Explosion1, enemy[k].x,(enemy[k].y)); LCD_DrawBMP (Explosion1,enemy[k].x, enemy[k].y);
      enemy[k].s=0;		enemy[k].b=1;
				} 
				if (enemy[k].s ==0 && enemy[k].e ==0) {
			LCD_DrawBMP(ExplosionBlack, enemy[k].x,(enemy[k].y));
				enemy[k].e=1;}
		}


			 for (k=0;k<5;k++) {
				if(enemy[k].s==0 && enemy[k].c==0) {enemycount++; enemy[k].c=1;} //count enemies if s=0 and c=0
			}
			if (enemycount==5) {wave=21;} //new wave is all five enemies are "dead"
		} 
	
/////////////Ship printing for wave 2
				if(wave==20) {
			for (k=0;k<5;k++) {
				if (enemy[k].s ==1) {
			LCD_DrawBMP(AlienEnemySmall, enemy[k].x,enemy[k].y); }
				if (enemy[k].s ==2 && enemy[k].b ==0) {
			LCD_DrawBMP(Explosion1, enemy[k].x,(enemy[k].y)); LCD_DrawBMP (Explosion1,enemy[k].x, enemy[k].y);
			LCD_DrawBMP(Explosion1, enemy[k].x,(enemy[k].y)); LCD_DrawBMP (Explosion1,enemy[k].x, enemy[k].y);
      enemy[k].s=0;		enemy[k].b=1;
				} 
				if (enemy[k].s ==0 && enemy[k].e==0) {                               //once explosion eraser is printed, set the s to zero and e to one
			LCD_DrawBMP(ExplosionBlack, enemy[k].x,(enemy[k].y));                     
				enemy[k].e=1;}
		}
			for(k=5;k<10;k++) {
				if(enemy[k].s==1) {
			 LCD_DrawBMP(AlienEnemyBig, enemy[k].x,enemy[k].y); }
	      if (enemy[k].s ==2 && enemy[k].b ==0) {
			LCD_DrawBMP(Explosion1, enemy[k].x,(enemy[k].y)); LCD_DrawBMP (Explosion1,enemy[k].x, enemy[k].y);
			LCD_DrawBMP(Explosion1, enemy[k].x,(enemy[k].y)); LCD_DrawBMP (Explosion1,enemy[k].x, enemy[k].y);
      enemy[k].s=0;		enemy[k].b=1;
				} 
				if (enemy[k].s ==0 && enemy[k].e==0) {                               //once explosion eraser is printed, set the s to zero and e to one
			LCD_DrawBMP(ExplosionBlack, enemy[k].x, enemy[k].y);                     
				enemy[k].e=1;}}
				
	

			for (k=0;k<10;k++) { 
				if(enemy[k].s==0 && enemy[k].c==0) {enemycount++; enemy[k].c=1;} //count enemy if s=0 and c=0
			}
			
			
			if (enemycount==20) {wave=31;}  //new wave if all ten enemies are "dead"
		
				
		
					for(k=10;k<15;k++) {
				if(enemy[k].s==1) {
			 LCD_DrawBMP(AlienEnemyBig, enemy[k].x,enemy[k].y); }
	      if (enemy[k].s ==2 && enemy[k].b ==0) {
			LCD_DrawBMP(Explosion1, enemy[k].x,(enemy[k].y)); LCD_DrawBMP (Explosion1,enemy[k].x, enemy[k].y);
			LCD_DrawBMP(Explosion1, enemy[k].x,(enemy[k].y)); LCD_DrawBMP (Explosion1,enemy[k].x, enemy[k].y);
      enemy[k].s=0;		enemy[k].b=1;
				} 
				if (enemy[k].s ==0 && enemy[k].e==0) {                               //once explosion eraser is printed, set the s to zero and e to one
			LCD_DrawBMP(ExplosionBlack, enemy[k].x, enemy[k].y);                     
				enemy[k].e=1;}} }
				
			}					//close semaphore
	}								//close while loop
} 							//close main
