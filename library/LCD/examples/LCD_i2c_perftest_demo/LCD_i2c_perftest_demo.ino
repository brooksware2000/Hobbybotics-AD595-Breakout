/*
 Benchmark sketch for I2C LCD functionality using MCP23008 I2C expander.

 Times how long it takes to write 47 characters (the estimated amount
 of off-screen characters on a 20x4 LCD) to the display under various
 i2c clock rates.
 
 Written to demonstrate the performance of the modified pure I2C LCD
 library by FalconFour ( http://falconfour.com ).
*/

#include <LCD.h>
#include <Wire.h>

// Default I2C address for LCD is 0
LCD lcd;

byte digit = 0;
unsigned long freqMult = 0;

void setup() {
  lcd.begin(20,4);
  lcd.setBacklight(true);
  lcd.clear();
  lcd.print("Performance test");
  delay(1000);
  lcd.clear();
  lcd.print("Default speed.");
}

void loop() {
  byte x;
  long msec = millis();
  long minitimer;
  while (millis() - msec < 5000) {
    minitimer = millis();
    lcd.setCursor(20,0);
    for (x=0; x<47; x++) {
      lcd.write(digit++);
    }
    lcd.setCursor(0,1);
    lcd.print("47 char, ");
    lcd.print(millis() - minitimer,DEC);
    lcd.print("msec");
  }
  while (++freqMult > 16) delay(50);
  bitSet(TWSR, TWPS0);
  lcd.clear();
  lcd.print("Freq = ");
  lcd.print(12500UL * freqMult);
  TWBR = (((F_CPU/(12500ul*freqMult))-16)/2)/4;
}