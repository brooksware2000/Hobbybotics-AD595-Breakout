/*-----------------------------------------------------------------------------------------------
 * File: LCD.h
 * Function: I2C LCD library
 * Description: Uses MCP23008 I2C Expander to drive a HD44780 Parallel LCD.
 * Created by Curtis Brooks, March 20, 2012.
 * Updated: 14 April 2012
 * Modified from LiquidTWI High Performance I2C LCD driver.
 * Original code by by Matt Falcon (FalconFour) / http://falconfour.com. 
 * Modified by Stephanie Maks / http://planetstephanie.net to add Arduino 1.0 support
 * Released into the public domain.
 * ----------------------------------------------------------------------------------------------*/

#ifndef LCD_h
#define LCD_h

// Keep compatability with Arduino 1.0 and below
#if ARDUINO>=100
#include <Arduino.h>             // Arduino 1.0
#else
#include <WProgram.h>            // Arduino 0023 and below
#endif
 
#include <inttypes.h>
#include "Print.h"

/*-----------------------------------------------------------------------------------------------
 * Copied from LiquidTWI I2C LCD Library
 * ----------------------------------------------------------------------------------------------*/
#define MCP23008_ADDRESS 0x20

// registers
#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00
#define LCD_BACKLIGHT 0x08 // used to pick out the backlight flag since 
                           // _displaycontrol will never set itself

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00
 
class LCD : public Print {
  public:
    LCD();
    LCD(uint8_t i2cAddr);
    
    void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

    void clear();
    void home();

    void noDisplay();
    void display();
    void noBlink();
    void blink();
    void noCursor();
    void cursor();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void leftToRight();
    void rightToLeft();
    void autoscroll();
    void noAutoscroll();
    void setBacklight(uint8_t status); 
    void createChar(uint8_t, uint8_t[]);
    void setCursor(uint8_t, uint8_t); 
    
    #if ARDUINO >= 100
      virtual size_t write(uint8_t);
    #else
      virtual void write(uint8_t);
    #endif
    
    void command(uint8_t);
    
  private:
    void lcd_send(uint8_t, uint8_t);
    void lcd_burstBits(uint8_t);
    
    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
    uint8_t _numlines,_currline;
    uint8_t lcd_i2cAddr;
};
 
#endif