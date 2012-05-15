/*
 Demonstration sketch for I2C LCD functionality using MCP23008 I2C expander.
 
 This sketch prints big digits to the LCD.
*/

#include <LCD.h>
#include <Wire.h>

LCD lcd;  // create LCD object

#define B 0xFF  // character for a filled box
#define A 0x20  // character for a blank


// Define 8 custom characters: These characters are 3x3.

byte cc0[8] = {     // Custom Character 0
  B00000,
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte cc1[8] = {     // Custom Character 1
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte cc2[8] = {    // Custom Character 2
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000
};

byte cc3[8] = {    // Custom Character 3
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte cc4[8] = {   // Custom Character 4
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111
};

byte cc5[8] = {    // Custom Character 5
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000
};

byte cc6[8] = {    // Custom Character 6
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000
};

byte cc7[8] = {     // Custom Character 7
  B00000,
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

// Function to send custom characters to the LCD RAM
void define_custom_char(){
  lcd.createChar(0,cc0);  // cc0 becomes character 0
  lcd.createChar(1,cc1);  // cc1 becomes character 1
  lcd.createChar(2,cc2);  // cc2 becomes character 2
  lcd.createChar(3,cc3);  // cc3 becomes character 3
  lcd.createChar(4,cc4);  // cc4 becomes character 4
  lcd.createChar(5,cc5);  // cc5 becomes character 5
  lcd.createChar(6,cc6);  // cc6 becomes character 6
  lcd.createChar(7,cc7);  // cc7 becomes character 7
}

// Array index into parts of big numbers. Each number consists of 9 custom characters
// in a 3x3 matrix. To print a number, you use the array index corresponding to the number
// times 3. For example to print the number 5, you will print bn1[15], bn1[16] and bn1[17]
// for the first row of the large number, and then bn2[15], bn2[16] and bn2[17] and so on.

//            0      1      2      3      4      5      6      7      8      9    
char bn1[]={B,2,1, 2,1,A, 2,2,1, 2,2,1, 0,A,B, B,2,2, B,2,2, 2,2,B, B,2,1, B,2,1};
char bn2[]={B,A,B, A,B,A ,0,6,5, A,2,1, 5,6,B, 2,2,1, B,6,7, A,0,5, B,6,B, 5,6,B};
char bn3[]={4,3,B, 3,B,3, B,3,3, 4,3,B, A,A,B, 4,3,B, 4,3,B, A,B,A, 4,3,B, A,A,B};

// Functions for printing one large digit. Value is the column number
// and number is the number to print. Works from 0-9

void single_digits(byte value, byte number){

  lcd.setCursor(value,1);  // Printing line 1 of the two-digit number
  lcd.write(bn1[number*3]);
  lcd.write(bn1[(number*3)+1]);
  lcd.write(bn1[(number*3)+2]);

  lcd.setCursor(value,2);  // Printing line 2 of the two-digit number
  lcd.write(bn2[number*3]);
  lcd.write(bn2[(number*3)+1]);
  lcd.write(bn2[(number*3)+2]);

  lcd.setCursor(value,3);  // Printing line 3 of the two-digit number
  lcd.write(bn3[number*3]);
  lcd.write(bn3[(number*3)+1]);
  lcd.write(bn3[(number*3)+2]);
}

// Functions for printing two large digits. Value is the column number
// and number is the number to print. Works from 00-99

void two_digits(byte value, byte number){

  lcd.setCursor(value,1);  // Printing line 1 of the two-digit number
  lcd.write(bn1[(number/10)*3]);
  lcd.write(bn1[(number/10)*3+1]);
  lcd.write(bn1[(number/10)*3+2]);
  lcd.write(A); // Blank
  lcd.write(bn1[(number%10)*3]);
  lcd.write(bn1[(number%10)*3+1]);
  lcd.write(bn1[(number%10)*3+2]);

  lcd.setCursor(value,2);  // Printing line 2 of the two-digit number
  lcd.write(bn2[(number/10)*3]);
  lcd.write(bn2[(number/10)*3+1]);
  lcd.write(bn2[(number/10)*3+2]);
  lcd.write(A); // Blank
  lcd.write(bn2[(number%10)*3]);
  lcd.write(bn2[(number%10)*3+1]);
  lcd.write(bn2[(number%10)*3+2]);

  lcd.setCursor(value,3);  // Printing line 3 of the two-digit number
  lcd.write(bn3[(number/10)*3]);
  lcd.write(bn3[(number/10)*3+1]);
  lcd.write(bn3[(number/10)*3+2]);
  lcd.write(A); // Blank
  lcd.write(bn3[(number%10)*3]);
  lcd.write(bn3[(number%10)*3+1]);
  lcd.write(bn3[(number%10)*3+2]);
}

void setup() {
  lcd.begin(20, 4);  // setup LCD number or columns and rows
  define_custom_char(); // create the custom characters
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Big Number Test 1");
  
  // print digits from 00 - 99
  for(int i = 0; i <= 99; i++) {   
    two_digits(13,i);
    
    // print digits from 0 - 9
    if (i < 10)
      single_digits(0,i);
        
    delay(600);   
  }
}

void loop() {}