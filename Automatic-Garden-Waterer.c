//*************************************************************************************************
// Automatic-Garden-Waterer
//
// By: Dillon Nichols
// http://tinkeringetc.blogspot.com/p/msp430-reaction-game.html
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
#define RLY   BIT2  // RELAY FOR VALVE -> P1.2
#define DATA  BIT3 	// PIN 14 OF 74HC595 -> P1.3
#define CLOCK BIT4 	// PIN 11 OF 74HC595 -> P1.4
#define LATCH BIT5 	// PIN 12 OF 74HC595 -> P1.5
#define STOP  BIT6 	// GAME STOP / RESTART BUTTON -> P1.6
#define START BIT7	// LED START / RESET BUTTON -> P1.7

// Declare functions
void switcher (void);
void countdown60  (void);
void countdown80  (void);
void countdown100 (void);
void countdown120 (void);
void countdown140 (void);
void countdown160 (void);
void countdown180 (void);
void stop (void);
void delay (unsigned int);
void pinWrite (unsigned int, unsigned char);
void pulseClock (void);
void shiftOut (unsigned char);

// Store time the valve is open as a global variable so all functions can access it
int waterTime;
int timeLeft;

// Uses potentiometer through the ADC10 to select how quickly LEDs will cycle
int main(void) {
	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT
	ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;	// ADC10ON, interrupt enabled
	ADC10CTL1 = INCH_1;				// input A1
	ADC10AE0 |= 0x02;				// P1.1 ADC input select
	P1DIR |= (RLY + DATA + CLOCK + LATCH);		// Setup relay and shift register pins as outputs

	P1OUT &= ~RLY; 					// Turns relay off

  	// Reads ADC values and lights LEDS and sets the waterTime corresponding to the input voltage
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
	  	if ((P1IN & START) == START) {	// When the START button is pressed, the game
	  		switcher();         }	// will start with the selected waterTime	
	}
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
  __bic_SR_register_on_exit(CPUOFF);	// Clear CPUOFF bit from 0(SR)
}

void switcher (void){
	P1OUT |= RLY; 				// Turns on relay
	switch (waterTime) {
		case 60:	
			countdown60();
			break;
		case 80:	
			countdown80();
			break;
		case 100:	
			countdown100();
			break;
		case 120:	
			countdown120();
			break;
		case 140:	
			countdown140();
			break;
		case 160:	
			countdown160();
			break;
		case 180:	
			countdown180();
			break;
	}
}

void countdown60(void){
	timeLeft = waterTime;
	for (;;){
		delay(1);
		if (timeLeft > 52){
			shiftOut(1 << 0);
		}
		if (timeLeft > 45){
			shiftOut(1 << 1);
		}
		if (timeLeft > 37){
			shiftOut(1 << 2);
		}
		if (timeLeft > 30){
			shiftOut(1 << 3);
		}
		if (timeLeft > 22){
			shiftOut(1 << 4);
		}
		if (timeLeft > 15){
			shiftOut(1 << 5);
		}
		if (timeLeft > 7){
			shiftOut(1 << 6);
		}
	}
}
void countdown80(void){
	timeLeft = waterTime;
	for (;;){
		delay(1);
		if (timeLeft > 70){
			shiftOut(1 << 0);
		}
		if (timeLeft > 60){
			shiftOut(1 << 1);
		}
		if (timeLeft > 50){
			shiftOut(1 << 2);
		}
		if (timeLeft > 40){
			shiftOut(1 << 3);
		}
		if (timeLeft > 30){
			shiftOut(1 << 4);
		}
		if (timeLeft > 20){
			shiftOut(1 << 5);
		}
		if (timeLeft > 10){
			shiftOut(1 << 6);
		}
	}
}
void countdown100(void){
	timeLeft = waterTime;
	for (;;){
		delay(1);
		if (timeLeft > 87){
			shiftOut(1 << 0);
		}
		if (timeLeft > 75){
			shiftOut(1 << 1);
		}
		if (timeLeft > 62){
			shiftOut(1 << 2);
		}
		if (timeLeft > 50){
			shiftOut(1 << 3);
		}
		if (timeLeft > 37){
			shiftOut(1 << 4);
		}
		if (timeLeft > 25){
			shiftOut(1 << 5);
		}
		if (timeLeft > 12){
			shiftOut(1 << 6);
		}
	}
}
void countdown120(void){
	timeLeft = waterTime;
	for (;;){
		delay(1);
		if (timeLeft > 105){
			shiftOut(1 << 0);
		}
		if (timeLeft > 90){
			shiftOut(1 << 1);
		}
		if (timeLeft > 75){
			shiftOut(1 << 2);
		}
		if (timeLeft > 60){
			shiftOut(1 << 3);
		}
		if (timeLeft > 45){
			shiftOut(1 << 4);
		}
		if (timeLeft > 30){
			shiftOut(1 << 5);
		}
		if (timeLeft > 15){
			shiftOut(1 << 6);
		}
	}
}
void countdown140(void){
	timeLeft = waterTime;
	for (;;){
		delay(1);
		if (timeLeft > 122){
			shiftOut(1 << 0);
		}
		if (timeLeft > 105){
			shiftOut(1 << 1);
		}
		if (timeLeft > 87){
			shiftOut(1 << 2);
		}
		if (timeLeft > 70){
			shiftOut(1 << 3);
		}
		if (timeLeft > 52){
			shiftOut(1 << 4);
		}
		if (timeLeft > 35){
			shiftOut(1 << 5);
		}
		if (timeLeft > 17){
			shiftOut(1 << 6);
		}
	}
}
void countdown160(void){
	timeLeft = waterTime;
	for (;;){
		delay(1);
		if (timeLeft > 140){
			shiftOut(1 << 0);
		}
		if (timeLeft > 120){
			shiftOut(1 << 1);
		}
		if (timeLeft > 100){
			shiftOut(1 << 2);
		}
		if (timeLeft > 80){
			shiftOut(1 << 3);
		}
		if (timeLeft > 60){
			shiftOut(1 << 4);
		}
		if (timeLeft > 40){
			shiftOut(1 << 5);
		}
		if (timeLeft > 20){
			shiftOut(1 << 6);
		}
	}
}
void countdown180(void){
	timeLeft = waterTime*70/8;
	for (;;){
		delay(1);
		if (timeLeft > 1102){
			shiftOut(1 << 0);
		}
		delay(1);
		if (timeLeft > 945){
			shiftOut(1 << 1);
		}
		delay(1);
		if (timeLeft > 787){
			shiftOut(1 << 2);
		}
		delay(1);
		if (timeLeft > 630){
			shiftOut(1 << 3);
		}
		delay(1);
		if (timeLeft > 472){
			shiftOut(1 << 4);
		}
		delay(1);
		if (timeLeft > 315){
			shiftOut(1 << 5);
		}
		delay(1);
		if (timeLeft > 157){
			shiftOut(1 << 6);
		}
	}
}

// Ensures that the STOP button is released after finishing a game
void stop (void) {
	for (;;) {
		if ((P1IN & STOP) == STOP) {			// Waits to STOP button to be pressed
			delay(50);
  			if ((P1IN & STOP) != STOP) {		// Waits for STOP buton to be released so it 
  				main();				
  			}
		}
	}
}




// Delays by the specified minutes
// 60000 milliseconds = 1 min
// 1000  milliseconds = 1 second
void delay(unsigned int ms) {
	timeLeft--;
	if (timeLeft < 0){
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
