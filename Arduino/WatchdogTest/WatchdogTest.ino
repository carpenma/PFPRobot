// Matthew Carpenter - carpent6@purdue.edu
// This is a test of the watchdog timer function of the Arduino

#include <avr/wdt.h>

void setup () {
  wdt_enable(WDTO_500MS);
  Serial.begin(9600);
  Serial.print('@');
}

void loop () {
  wdt_reset();
  Serial.print('!');
  while(!(Serial.available() == 8));
  Serial.read();  //Parsing and signal operations would happen here
  delay(100);
}
