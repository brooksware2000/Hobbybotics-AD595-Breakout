/*-----------------------------------------------------------------------------------------------
 * File: AD595.h
 * Function: AD595 Thermocouple library
 * Description: AD595 Type-K Thermocouple Library for the Hobbybotics AD595 breakout board.
 * Created by Curtis Brooks, May 13, 2012.
 * Updated: N/A
 * Released into the public domain.
 * 
 * ----------------------------------------------------------------------------------------------*/
 
#ifndef AD595_h
#define AD595_h

// Keep compatability with Arduino 1.0 and below
#if ARDUINO>=100
#include <Arduino.h>             // Arduino 1.0
#else
#include <WProgram.h>            // Arduino 0023 and below
#endif

enum {
  TEMPC,
  TEMPF
};

class AD595 {
  public:
    void init(uint8_t DO);
    double measure(uint8_t type);
        
  private:
    double tempC();
    double tempF();
    
    int8_t _DO;
};

#endif