/*-----------------------------------------------------------------------------------------------
 * File: AD595.cpp
 * Function: AD595 Thermocouple library
 * Description: AD595 Type-K Thermocouple Library for the Hobbybotics AD595 breakout board.
 * Created by Curtis Brooks, May 13, 2012.
 * Updated: N/A
 * Released into the public domain.
 * 
 * ----------------------------------------------------------------------------------------------*/

// Add necessary include files
#include "AD595.h"
 
/*-----------------------------------------------------------------------------------------------
 * Public Methods
 * ----------------------------------------------------------------------------------------------*/

void AD595::init(uint8_t DO) {
  _DO = DO;
  
  //define pin modes
  pinMode(_DO, INPUT);
}

double AD595::measure(uint8_t type) {
  double value;
  
  switch(type) {
    case TEMPC : value = tempC(); break;
    case TEMPF : value = tempF(); break;
  }
  return value;
}

/*-----------------------------------------------------------------------------------------------
 * Private Methods
 * ----------------------------------------------------------------------------------------------*/
double AD595::tempC() {
  return (5.0 * analogRead(_DO) * 100.0) / 1024.0;
}

double AD595::tempF() {
  return ((tempC() * 9.0/5.0) + 32);
}


