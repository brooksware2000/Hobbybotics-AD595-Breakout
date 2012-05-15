/*-----------------------------------------------------------------------------------------------
 * File: LCD.cpp
 * Function: I2C LCD library
 * Description: Uses MCP23008 I2C Expander to drive a HD44780 Parallel LCD.
 * Created by Curtis Brooks, March 20, 2012.
 * Updated: 14 April 2012
 * Modified from LiquidTWI High Performance I2C LCD driver.
 * Original code by by Matt Falcon (FalconFour) / http://falconfour.com. 
 * Modified by Stephanie Maks / http://planetstephanie.net to add Arduino 1.0 support
 * Released into the public domain.
 * ----------------------------------------------------------------------------------------------*/

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
 
// Add necessary include files
#include "LCD.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <Wire.h>

/*-------------------------------------------------------------------------------------------------
 * Function: LCD
 * Description: Class constructor
 * Ins: none
 * Outs: none 
 * 
 * Functions pulled from LiquidTWI I2C LCD Library
 * 
 * When the display powers up, it is configured as follows:
 * 
 * 1. Display clear
 * 2. Function set: 
 *    DL = 1; 8-bit interface data 
 *    N = 0; 1-line display 
 *    F = 0; 5x8 dot character font 
 * 3. Display on/off control: 
 *    D = 0; Display off 
 *    C = 0; Cursor off 
 *    B = 0; Blinking off 
 * 4. Entry mode set: 
 *    I/D = 1; Increment by 1 
 *    S = 0; No shift 
 * 
 * Note, however, that resetting the Arduino doesn't reset the LCD, so we
 * can't assume that its in that state when a sketch starts (and the
 * LCD constructor is called). This is why we save the init commands
 * for when the sketch calls begin(), except configuring the expander, which
 * is required by any setup.
 * ----------------------------------------------------------------------------------------------*/ 
LCD::LCD(){
  lcd_i2cAddr = 0;
    
	// transfer this function call's number into our internal class state
  // in case they forget to call begin() at least we have something
  _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
}

LCD::LCD(uint8_t i2cAddr){
  lcd_i2cAddr = i2cAddr;
    
	// transfer this function call's number into our internal class state
  // in case they forget to call begin() at least we have something
  _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
}

/*-----------------------------------------------------------------------------------------------
 * Function: begin
 * Description: Initialize lcd columns, rows and dot size
 * Ins: Integer value for col, row and dotsize.
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50);

	Wire.begin();
	// first thing we do is get the GPIO expander's head working straight, 
  // with a boatload of junk data.
	Wire.beginTransmission(MCP23008_ADDRESS | lcd_i2cAddr);
#if ARDUINO >= 100
	Wire.write((byte)MCP23008_IODIR);
	Wire.write((byte)0xFF);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
#else
	Wire.send(MCP23008_IODIR);
	Wire.send(0xFF);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
#endif
	Wire.endTransmission();

	// now we set the GPIO expander's I/O direction to output since it's soldered to an LCD output.
	Wire.beginTransmission(MCP23008_ADDRESS | lcd_i2cAddr);
#if ARDUINO >= 100
	Wire.write((byte)MCP23008_IODIR);
	Wire.write((byte)0x00); // all output: 00000000 for pins 1...8
#else
	Wire.send(MCP23008_IODIR);
	Wire.send(0x00); // all output: 00000000 for pins 1...8
#endif
	Wire.endTransmission();

	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;
	_currline = 0;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	//put the LCD into 4 bit mode
	// start with a non-standard command to make it realize we're speaking 4-bit here
	// per LCD datasheet, first command is a single 4-bit burst, 0011.
        //-----
        //  we cannot assume that the LCD panel is powered at the same time as
        //  the arduino, so we have to perform a software reset as per page 45
        //  of the HD44780 datasheet - (kch)
        //-----
        // bit pattern for the burstBits function is
        //
        //  7   6   5   4   3   2   1   0
        // LT  D7  D6  D5  D4  EN  RS  n/c
        //-----
        lcd_burstBits(B10011100); // send LITE D4 D5 high with enable
        lcd_burstBits(B10011000); // send LITE D4 D5 high with !enable
        lcd_burstBits(B10011100); //
        lcd_burstBits(B10011000); //
        lcd_burstBits(B10011100); // repeat twice more
        lcd_burstBits(B10011000); //
        lcd_burstBits(B10010100); // send D4 low and LITE D5 high with enable
        lcd_burstBits(B10010000); // send D4 low and LITE D5 high with !enable
	delay(5); // this shouldn't be necessary, but sometimes 16MHz is stupid-fast.
	
	command(LCD_FUNCTIONSET | _displayfunction); // then send 0010NF00 (N=lines, F=font)
	delay(5); // for safe keeping...
	command(LCD_FUNCTIONSET | _displayfunction); // ... twice.
	delay(5); // done!

	// turn on the LCD with our defaults. since these libs seem to use personal preference, 
  // I like a cursor.
	_displaycontrol = LCD_DISPLAYON;  
	display();
	// clear it off
	clear();

	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	setBacklight(HIGH); // turn the backlight on if so equipped.
}

/********** high level commands, for the user! */

/*-----------------------------------------------------------------------------------------------
 * Function: clear
 * Description: Clear LCD
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::clear()
{
	command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

/*-----------------------------------------------------------------------------------------------
 * Function: home
 * Description: Set LCD to upper (0,0) position
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::home()
{
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

/*-----------------------------------------------------------------------------------------------
 * Function: setCursor
 * Description: Set cursor to (column,row) position
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) row = _numlines - 1;    // we count rows starting w/0
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

/*-----------------------------------------------------------------------------------------------
 * Function: noDisplay
 * Description: Turn the display off
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/*-----------------------------------------------------------------------------------------------
 * Function: display
 * Description: Turn the display on
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/*-----------------------------------------------------------------------------------------------
 * Function: noCursor
 * Description: Turns the underline cursor off
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/*-----------------------------------------------------------------------------------------------
 * Function: cursor
 * Description: Turns the underline cursor on
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/*-----------------------------------------------------------------------------------------------
 * Function: noBlink
 * Description: Turn off the blinking cursor
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/*-----------------------------------------------------------------------------------------------
 * Function: blink
 * Description: Turn on the blinking cursor
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM

/*-----------------------------------------------------------------------------------------------
 * Function: scrollDisplayLeft
 * Description: Scroll the display left
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

/*-----------------------------------------------------------------------------------------------
 * Function: scrollDisplayRight
 * Description: Scroll the display right
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/*-----------------------------------------------------------------------------------------------
 * Function: leftToRight
 * Description: Set text left to right
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

/*-----------------------------------------------------------------------------------------------
 * Function: rightToLeft
 * Description: Set text right to left
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

/*-----------------------------------------------------------------------------------------------
 * Function: autoscroll
 * Description: This will 'right justify' text from the cursor
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

/*-----------------------------------------------------------------------------------------------
 * Function: noAutoscroll
 * Description: This will 'left justify' text from the cursor
 * Ins: none
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

/*-----------------------------------------------------------------------------------------------
 * Function: createChar
 * Description: Allows us to fill the first 8 CGRAM locations with custom characters
 * Ins: memory location, custom character array
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

/*********** mid level commands, for sending data/cmds */

/*-----------------------------------------------------------------------------------------------
 * Function: lcd_command
 * Description: Send commands to LCD
 * Ins: command to send
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
inline void LCD::command(uint8_t value) {
	lcd_send(value, LOW);
}

#if ARDUINO >= 100
inline size_t LCD::write(uint8_t value) {
	lcd_send(value, HIGH);
	return 1;
}
#else
inline void LCD::write(uint8_t value) {
	lcd_send(value, HIGH);
}
#endif

/************ low level data pushing commands **********/

/*-----------------------------------------------------------------------------------------------
 * Function: lcd_setBacklight
 * Description: Set the backlight on or off
 * Ins: on or off state
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::setBacklight(uint8_t status) {
	bitWrite(_displaycontrol,3,status); // flag that the backlight is enabled, for burst commands
	lcd_burstBits((_displaycontrol & LCD_BACKLIGHT)?0x80:0x00);
}

/*-----------------------------------------------------------------------------------------------
 * Function: lcd_send
 * Description: Write either command or data, burst it to the expander over I2C.
 * Ins: value and mode to send command/data
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::lcd_send(uint8_t value, uint8_t mode) {
	// BURST SPEED, OH MY GOD
	// the (now High Speed!) I/O expander pinout
	// RS pin = 1
	// Enable pin = 2
	// Data pin 4 = 3
	// Data pin 5 = 4
	// Data pin 6 = 5
	// Data pin 7 = 6
	byte buf;
	
  // crunch the high 4 bits
	// isolate high 4 bits, shift over to data pins (bits 6-3: x1111xxx)
  buf = (value & B11110000) >> 1; 
	
  // here we can just enable enable, since the value is immediately written to the pins
  if (mode) buf |= 3 << 1; 
	
  // if RS (mode), turn RS and enable on. otherwise, just enable. (bits 2-1: xxxxx11x)
  else buf |= 2 << 1; 
	
  // using DISPLAYCONTROL command to mask backlight bit in _displaycontrol
  buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; 
	
  lcd_burstBits(buf); // bits are now present at LCD with enable active in the same write
	
  // no need to delay since these things take WAY, WAY longer than the time required for enable 
  // to settle (1us in LCD implementation?)
	buf &= ~(1<<2); // toggle enable low
	
  lcd_burstBits(buf); // send out the same bits but with enable low now; LCD crunches these 4 bits.
	
  // crunch the low 4 bits
  // isolate low 4 bits, shift over to data pins (bits 6-3: x1111xxx)
	buf = (value & B1111) << 3; 
	
  // here we can just enable enable, since the value is immediately written to the pins
  if (mode) buf |= 3 << 1; 
	
  // if RS (mode), turn RS and enable on. otherwise, just enable. (bits 2-1: xxxxx11x)
  else buf |= 2 << 1; 
	
  // using DISPLAYCONTROL command to mask backlight bit in _displaycontrol
  buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; 
	
  lcd_burstBits(buf);
	
  // toggle enable low (1<<2 = 00000100; NOT = 11111011; with "and", this turns off only that one bit)
  buf &= ~( 1 << 2 ); 
	lcd_burstBits(buf);
}

/*-----------------------------------------------------------------------------------------------
 * Function: lcd_burstBits
 * Description: Burst bits to the GPIO chip whenever needed. avoids repetative code.
 * Ins: value to send
 * Outs: none
 * ----------------------------------------------------------------------------------------------*/
void LCD::lcd_burstBits(uint8_t value) {
	// we use this to burst bits to the GPIO chip whenever we need to. avoids repetative code.
	Wire.beginTransmission(MCP23008_ADDRESS | lcd_i2cAddr);
#if ARDUINO >= 100
	Wire.write(MCP23008_GPIO);
	Wire.write(value); // last bits are crunched, we're done.
#else
	Wire.send(MCP23008_GPIO);
	Wire.send(value); // last bits are crunched, we're done.
#endif
	while (Wire.endTransmission()) ;
}