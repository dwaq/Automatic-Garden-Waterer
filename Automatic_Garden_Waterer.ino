//*************************************************************************************************
// Automatic-Garden-Waterer
//
// By: Dillon Nichols
// http://tinkeringetc.blogspot.com/
//
// Created in Arduino v1.0.1
//
// Description: 
// A project that switches a valve to water plants in the garden. It has a manual switch to control
// the valve or an Arduino used as a timer attached to a relay to automatically turn 
// the valve off after a adjustable amount of time.
//*************************************************************************************************

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// set up the inputs
const int up = 10;
const int down = 9;
const int ok = 8;
const int manual = 7;
// set up the output
const int solenoid = 6;
// set up global variables
int hour = 1;
int minute = 0;
int timer = 0;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // set up pin directions
  pinMode(up, INPUT);
  pinMode(down, INPUT);
  pinMode(ok, INPUT);
  pinMode(manual, INPUT);
  pinMode(solenoid, OUTPUT);
}

void loop() {
  lcd.clear();
  digitalWrite(solenoid, LOW);
  int once = 1;
  
  if (digitalRead(manual) == HIGH) {
   lcd.setCursor(0, 0);
   lcd.print("Manual switch ON");
   digitalWrite(solenoid, HIGH);
   countup();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("How long?");
  lcd.setCursor(0,1);
  lcd.print(hour);
  lcd.print(" hrs ");
  lcd.print(minute);
  lcd.print(" mins");

  delay(500);

  if (digitalRead(up) == HIGH) {
    minute+=15;
  }
  if (digitalRead(down) == HIGH) {
    minute-=15;
  }

  if(minute == 60) {
    hour++;
    minute = 0;
  }
  if(minute == -15) {
    hour--;
    minute = 45;
  }
  // prevents negative time
  if(hour < 0){
    hour = 0;
    minute = 0;
  }


  if (digitalRead(ok) == HIGH && ((hour + minute) > 0)) {
    lcd.setCursor(0, 0);
    lcd.print("Auto watering ON");
    digitalWrite(solenoid, HIGH);
    while (digitalRead(ok) == HIGH ) {
      // wait for button to be released
    }
    countdown();
    // returns to default values
    hour = 1;
    minute = 0; 
  }
  
  while (digitalRead(ok) == HIGH && hour == 0 && minute == 0) {
    if (once == 1){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter positive");
      lcd.setCursor(0, 1);
      lcd.print("number of min");
    }
    once = 0;
  }
}


void countdown() {
  // if button is pressed again, exit
  while (digitalRead(ok) == LOW && hour >= 0 && minute >= 0) {
    lcd.setCursor(0,1);
    lcd.print(hour);
    lcd.print(" hrs ");
    lcd.print(minute);
    lcd.print(" mins");
    delay(500);
    timer++;
    if (timer == 2) {
      minute--;
      timer = 0;
    }
    if (minute == 0) {
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
  
  
  
  if (digitalRead(ok) == HIGH) {
    lcd.clear();
    lcd.print("Canceled");
  }
  while (digitalRead(ok) == HIGH) {
      // wait for button to be released
    }
}

void countup() {
  int upHour = 0;
  int upMinute = 0;  
  
  while (digitalRead(manual) == HIGH) {
    lcd.setCursor(0,1);
    lcd.print(upHour);
    lcd.print(" hrs ");
    lcd.print(upMinute);
    lcd.print(" mins");
    delay(500);
    timer++;
    if (timer == 2) {
      upMinute++;
      timer = 0;
    }
    if (upMinute == 60) {
      upHour++;
      minute = 00;
    }
  }
  while (digitalRead(ok) == HIGH) {
      // wait for button to be released
    }
}
