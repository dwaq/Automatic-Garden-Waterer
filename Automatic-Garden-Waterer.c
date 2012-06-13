//*************************************************************************************************
// Automatic-Garden-Waterer
//
// By: Dillon Nichols
// http://tinkeringetc.blogspot.com/
//
// Created in Code Composer Studio v4.2.5
//
// Description: 
// A project that switches a valve to water plants in the garden. It has a manual switch to control
// the valve or a MSP430 microcontroller used as a timer attached to a relay to automatically turn 
// the valve off after a adjustable amount of time.
//
// Special thanks to:
// * TI's example code for the ADC10 located at http://www.ti.com/litv/zip/slac463a
// * Andrew Morton's example of interfacing the 74HC595 shift register to the MSP430 located at
//   http://processors.wiki.ti.com/index.php/MSP430_Launchpad_Shift_Register
// * Christopher Chenoweth's delay function located at: http://www.threadabort.com/?p=26
// * The "SCIENTIFIC INSTRUMENTS USING THE TI MSP430" blog located at http://mspsci.blogspot.com/
//
//*************************************************************************************************
#include "msp430g2231.h"
 
// Define pins
#define POT   BIT1 	// POTENTIOMETER -> P1.1
#define RLY   BIT2  	// RELAY FOR VALVE -> P1.2
#define DATA  BIT3 	// PIN 14 OF 74HC595 -> P1.3
#define CLOCK BIT4 	// PIN 11 OF 74HC595 -> P1.4
#define LATCH BIT5 	// PIN 12 OF 74HC595 -> P1.5
#define STOP  BIT6 	// TIMER STOP BUTTON -> P1.6
#define START BIT7	// TIMER START  BUTTON -> P1.7

// Declare functions
void countdown  (void);
void stop (void);
void delay (unsigned int);
void pinWrite (unsigned int, unsigned char);
void pulseClock (void);
void shiftOut (unsigned char);

// Store time the valve is open as a global variable so all functions can access it
int waterTime;
int timeLeft;

// Uses potentiometer through the ADC10 to select how much time to leave the valve open
int main(void) {
	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT
	ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;	// ADC10ON, interrupt enabled
	ADC10CTL1 = INCH_1;				// input A1
	ADC10AE0 |= 0x02;				// P1.1 ADC input select
	P1DIR |= (RLY + DATA + CLOCK + LATCH);		// Make relay and shift register outputs

	P1OUT &= ~RLY; 					// Turns relay off

  	// Reads ADC values and lights LEDs and sets waterTime corresponding to the input voltage
  	// Adjustable between 1 hour (60 minutes) and 3 hours (180 minutes)
  	// Lower values on the ADC equals more waterTime
	for (;;) {
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
  		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		
		if (ADC10MEM > 0x000) {
  			shiftOut(1 << 7);
  			waterTime = 0;    }
	  	if (ADC10MEM > 0x07F) {
  			shiftOut(1 << 6);
  			waterTime = 60;   }
	  	if (ADC10MEM > 0x0FF) {
		  	shiftOut(1 << 5);
		  	waterTime = 80;   } 
	  	if (ADC10MEM > 0x17F) {
		  	shiftOut(1 << 4);
		  	waterTime = 100;  }
	  	if (ADC10MEM > 0x1FF) {
		  	shiftOut(1 << 3);
		  	waterTime = 120;  }
	  	if (ADC10MEM > 0x27F) {
		  	shiftOut(1 << 2);
		  	waterTime = 140;  }
		if (ADC10MEM > 0x2FF) {
		  	shiftOut(1 << 1);
		  	waterTime = 160;  }
		if (ADC10MEM > 0x37F) {
			shiftOut(1 << 0);
	    		waterTime = 180;  }
	  	if ((P1IN & START) == START) {		// When the START button is pressed, countdown
	  		countdown();         }		// will begin with the selected waterTime	
	}
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
	__bic_SR_register_on_exit(CPUOFF);
}

// Breaks timeLeft into 7 parts so it can be displayed as a fraction of timeLeft on the LEDs
void countdown(void){
	waterTime *= 7;			// Deals with having 7 delay functions in the for loop below
	//waterTime *= 60;		// converts from seconds to minutes in delay function
	timeLeft = waterTime;
	for (;;){
		delay(1);
		if (timeLeft > waterTime*7/8){
			shiftOut(1 << 0);
		}
		delay(1);
		if (timeLeft > waterTime*6/8){
			shiftOut(1 << 1);
		}
		delay(1);
		if (timeLeft > waterTime*5/8){
			shiftOut(1 << 2);
		}
		delay(1);
		if (timeLeft > waterTime*4/8){
			shiftOut(1 << 3);
		}
		delay(1);
		if (timeLeft > waterTime*3/8){
			shiftOut(1 << 4);
		}
		delay(1);
		if (timeLeft > waterTime*2/8){
			shiftOut(1 << 5);
		}
		delay(1);
		if (timeLeft > waterTime*1/8){
			shiftOut(1 << 6);
		}
	}
}

// Ensures that the STOP button is released after pressing
void stop (void) {
	for (;;) {
		if ((P1IN & STOP) == STOP) {			// Waits to STOP button to be pressed
			delay(50);
  			if ((P1IN & STOP) != STOP) {		// Waits for STOP buton to be released
  				main();				
  			}
		}
	}
}

// Delays by the specified seconds
// 60000 milliseconds = 1 minute
// 3600 seconds = 1 hour
// 1000  milliseconds = 1 second
void delay(unsigned int ms) {
	timeLeft--;				// Decreases timeLeft by 1 because delay has been ran once
	if (timeLeft < 0){			// When the time is up, goes back to main
		main();
	}
 	while (ms--) {
		__delay_cycles(1000); 
    }
}


















// Writes a value to the specified bitmask/pin. Use built in defines
// when calling this, as the shiftOut() function does.
// All nonzero values are treated as "high" and zero is "low"
void pinWrite(unsigned int bit, unsigned char val) {
  	if (val){
    	P1OUT |= bit;
  	} else {
    	P1OUT &= ~bit;
  	}
}
 
// Pulse the clock pin
void pulseClock(void){
  	P1OUT |= CLOCK;
  	P1OUT ^= CLOCK;
}
 
// Take the given 8-bit value and shift it out, LSB to MSB
void shiftOut(unsigned char val){
  	//Set latch to low (should be already)
  	P1OUT &= ~LATCH;
 
  	char i;
 
  	// Iterate over each bit, set data pin, and pulse the clock to send it to the shift register
  	for (i = 0; i < 8; i++)  {
      	pinWrite(DATA, (val & (1 << i)));
      	pulseClock();
  	}
 
  	// Pulse the latch pin to write the values into the storage register
  	P1OUT |= LATCH;
  	P1OUT &= ~LATCH;
}
