/*
 Demonstration sketch for I2C LCD functionality using MCP23008 I2C expander.
 
 This sketch prints "Hello World!" to the LCD and shows the elapsed time.
*/

#include <LCD.h>
#include <Wire.h>

// Default I2C address for LCD is 0
LCD lcd;

void setup() {
  // set up the LCD's number of rows and columns: 
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);

  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);
}