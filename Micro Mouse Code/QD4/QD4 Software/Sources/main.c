#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#define COUNT_LOW	76	//76 2ms on
#define COUNT_HIGH	760	//760 18 ms off (total of 20ms period)
unsigned int counter;

/* Emits 37.57kHz 2ms pulses every 20ms
 * Emission is on Pin 8
 * Emission is stopped when pin 5 is grounded
 */

void main(void) {
	SOPT1 = 0x00;
	
	// Doubles BUS Speed
	ICSC2 = 0;
	
	//Data Direction
	PTADD = 0b00000001;
	//Drive Strength (MORE CURRENT)
	PTADS = 0b00000001;
	//Input Pull Up
	PTAPE = 0b00001000;
	
	// Centre aligned, BUS Clock, No Clock Prescaler
	TPMSC = 0x48; // TOF(0) TOIE(1) CPWMS(0) CLKSB(0) CLKSA(1) PS2(0) PS1(0) PS0(0)
	// SetUp TPM1 Channel 0
	TPMC0SC = 0x50;
	// Set Modulo register to 38KHz
	TPMMODH = 0x00; //69 or D2 (4MHz, 8MHz)
	TPMMODL = 0xD8;
	//TPMMODL = 0x64;
	// Set Channel Register to 50% Duty Cycle
	TPMC0VH = 0x00; //34 or 69 (4MHz, 8MHz)
	TPMC0VL = 0x69;
	//TPMC0VL = 0x32;
	
	// Initialise Counter
	counter = 0;
  EnableInterrupts;
  
  for(;;) {
  }
}

// TPM1 Overflow - Frequency
interrupt 7 void TPM1_overflow(){
    TPMSC = TPMSC & 0x7F;
    
    // IF PTAD5 is ground, stop emitting signal
    if((PTAD & 0b00001000) == 0){
    }
    else if(counter>=COUNT_HIGH){
    	counter = 0;
    }
    else if(counter<=COUNT_LOW){
		PTAD = PTAD | 0b00000001; // Switch on PTA0-3
	}
}

// TPM1C0 Duty Cycle
interrupt 5 void TPM1C0(){
    TPMC0SC = TPMC0SC & 0x7F;
    
    PTAD = PTAD & 0b11111110; // Turn off PTA0-3
    ++counter;
}
