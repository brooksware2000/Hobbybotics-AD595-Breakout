/*
 Demonstration sketch for Hobbybotics AD595 Thermocouple breakout board.
 
 Reads temperature from AD595 in celsius and fahrenheit.  Prints results to I2C LCD.
*/

#include <AD595.h>
#include <LCD.h>
#include <Wire.h>

AD595 thermocouple;

// Default I2C address for LCD is 0
LCD lcd(1);

// make a degree symbol
uint8_t degree[8]  = {140,146,146,140,128,128,128,128};

void setup() {
  lcd.begin(20, 4);
  lcd.createChar(0, degree);
  
  thermocouple.init(0);

  // wait for AD595 to stabilize
  delay(500);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AD595 test");
}

void loop() {
  float temp_c;
  float temp_f;
  
  // Read values from the sensor
  temp_c = thermocouple.measure(TEMPC);
  temp_f = thermocouple.measure(TEMPF);
   
  lcd.setCursor(0,1);
  lcd.print(temp_c);
  lcd.write((byte)0);
  lcd.print("C ");
  lcd.print(temp_f);
  lcd.write((byte)0);
  lcd.print('F');
  
  delay(1000);
}
