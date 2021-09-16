//*************************************************************************************************
// Automatic-Garden-Waterer
//
// By: Dillon Nichols
// http://tinkeringetc.blogspot.com/2012/08/automatic-garden-waterer-part-2.html
//
// Created in Arduino v1.0.1
//
// Description: 
// A project that switches a valve to water plants in the garden. It has a manual switch to control
// the valve or an Arduino used as a timer attached to a relay to automatically turn 
// the valve off after a adjustable amount of time.
//
// This work is licensed under a Creative Commons Attribution 3.0 Unported License.
//*************************************************************************************************

// include the library code:
#include <LiquidCrystal.h>

// For debouncing the buttons
// Search for "button tiny simple" in the Arduino Library Manager
// to find the correct library by Michael Adams
#include <Button.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 6, 4, 3, 2, 1);

// set up the Button inputs
Button up(11);
Button down(10);
Button ok(12);

const int manual = 9;
// set up the output
const int solenoid = 13;

// set up global variables
int hour = 0;
int minute = 15;
int display_timer = 0;
int timer = 0;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // set up the buttons
  up.begin();
  down.begin();
  ok.begin();  
  // set up pin directions
  pinMode(manual, INPUT);
  pinMode(solenoid, OUTPUT);
}

void loop() {
  lcd.clear();
  digitalWrite(solenoid, LOW);
  
  // detects if manual switch is engaged
  if (digitalRead(manual) == HIGH) {
   lcd.setCursor(0, 0);
   lcd.print("Manual switch ON");
   digitalWrite(solenoid, HIGH);
   countup();
  }

  // allows for the user's reaction time
  delay(25);

  // Update the display every 500ms (25 * 20)
  display_timer++;
  if (display_timer == 20) {
    // makes display for timer adjust
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("How long?");
    lcd.setCursor(0,1);
    lcd.print(hour);
    lcd.print(" hrs ");
    lcd.print(minute);
    lcd.print(" mins");

    // reset for next time
    display_timer = 0;
  }

  // increase or decrease time by 3 minutes depending on the button pressed
  if (up.pressed()) {
    minute+=3;
  }
  if (down.pressed()) {
    minute-=3;
  }

  // displays 60 minutes as an additional hour
  if(minute == 60) {
    hour++;
    minute = 0;
  }

  // displays time correctly when decrementing the hour
  // ex: 2:00 => press 'down' => 2:-15 == 1:45
  if(minute == -15) {
    hour--;
    minute = 45;
  }

  // prevents negative time for the timer
  if(hour < 0){
    hour = 0;
    minute = 0;
  }

  // begins automatic watering
  if (digitalRead(ok) == HIGH && ((hour + minute) > 0)) {
    lcd.setCursor(0, 0);
    lcd.print("Auto watering ON");
    digitalWrite(solenoid, HIGH);
    while (digitalRead(ok) == HIGH ) {
      // wait for button to be released
    }
    countdown();
    // returns to default values
    hour = 2;
    minute = 0; 
  }
  
  // shows error message is time is set to 0:00 and automatic watering is attempted
  if (digitalRead(ok) == HIGH && hour == 0 && minute == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter positive");
    lcd.setCursor(0, 1);
    lcd.print("number of min");
  }
  while (digitalRead(ok) == HIGH && hour == 0 && minute == 0) {
      // wait for button to be released
  }
}

// function for displaying how much time is left during automatic watering
void countdown() {
  // if 'ok' button is pressed again, exit
  while (digitalRead(ok) == LOW && hour >= 0 && minute >= 0) {
    // display time left on 2nd line
    lcd.setCursor(0,1);
    lcd.print(hour);
    lcd.print(" hrs ");
    lcd.print(minute);
    lcd.print(" mins");
    // 120 iterations of 500 ms = 60 seconds = 1 minute
    delay(500);
    timer++;
    if (timer == 120) {
      minute--;
      timer = 0;
    }
    // decrements hour when minute goes negative
    if (hour > 0 && minute == -1) {
      hour--;
      minute = 59;
    }
    // deals with hour or minute turning to a single digit
    if ((hour == 9 || minute == 9) && timer == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Auto watering ON");
    }
  }
  
  // message for when 'ok' button is pressed, triggering an exit from automatic watering
  if (digitalRead(ok) == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Canceled");
    lcd.setCursor(0, 1);
    lcd.print("Release button");
  }
  while (digitalRead(ok) == HIGH) {
      // wait for button to be released
    }
}

// function for displaying how much time has elapsed during manual watering
void countup() {
  int upHour = 0;
  int upMinute = 0;  
  
  while (digitalRead(manual) == HIGH) {
    // display time elapsed on 2nd line
    lcd.setCursor(0,1);
    lcd.print(upHour);
    lcd.print(" hrs ");
    lcd.print(upMinute);
    lcd.print(" mins");
    // 120 iterations of 500 ms = 60 seconds = 1 minute
    delay(500);
    timer++;
    if (timer == 120) {
      upMinute++;
      timer = 0;
    }
    if (upMinute == 60) {
      upHour++;
      upMinute = 00;
    }
    // deals with upMinute turning to a single digit
    if (upMinute == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Manual switch ON");
    }
  }
  while (digitalRead(ok) == HIGH) {
      // wait for button to be released
    }
}
