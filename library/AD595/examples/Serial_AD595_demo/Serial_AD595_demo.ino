/*
 Demonstration sketch for Hobbybotics AD595 Thermocouple breakout board.
 
 Reads temperature from AD595 in celsius and fahrenheit.  Prints results to serial monitor.
*/

#include <AD595.h>

AD595 thermocouple;
  
void setup() {
  Serial.begin(9600);
  thermocouple.init(0);
  
  Serial.println("AD595 test");
  // wait for AD595 chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp
  
   Serial.print("C = "); 
   Serial.println(float(thermocouple.measure(TEMPC)));
   Serial.print("F = ");
   Serial.println(float(thermocouple.measure(TEMPF)));
 
   delay(1000);
}