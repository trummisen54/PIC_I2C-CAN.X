 /*****************************************************************************\ 
 |         This program was written by Asher Glick aglick@tetrakai.com         | 
 |             This program is currently under the GNU GPL licence             |
 \*****************************************************************************/

/****************** CHIP SETTINGS ******************\
| This program was designed to run on an ATMEGA328  |
| chip running with an external clock at 8MHz       |
\***************************************************/

/********** FUSE SETTINGS **********\
|   Low Fuse 0xE2                   |
|  High Fuse 0xD9                   |       +- AVRDUDE COMMANDS -+
| Extra Fuse 0x07                   |       | -U lfuse:w:0xe0:m  |
|                                   |       | -U hfuse:w:0xd9:m  |
| These fuse calculations are       |       | -U efuse:w:0xff:m  |
| based off of the usbtiny AVR      |       +--------------------+
| programmer. Other programmers     |
| may have a different fuse number  |
\***********************************/

/************************** AVRDUDE command for 8MHz **************************\ 
| sudo avrdude -p m328p -c usbtiny -U flash:w:myproject.hex                    |
|                       -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0x07:m  |
|                                                                              |
| NOTE: when messing with fuses, do this at your own risk. These fuses for the |
|        ATMEGA328P (ATMEGA328) worked for me, however if they do not work for |
|        you, it is not my fault                                               |
| NOTE: '-c usbtiny' is incorrect if you are using a different programmer      |
\******************************************************************************/

/******************************* CUSTOM SETTINGS ******************************\
| Settings that can be changed, comment or uncomment these #define settings to |
| make the AVRFID code do different things
\******************************************************************************/


//#define bort

#ifndef bort





//#define Binary_Tag_Output         // Outputs the Read tag in binary over serial
#define Hexadecimal_Tag_Output    // Outputs the read tag in Hexadecimal over serial
//#define Decimal_Tag_Output        // Outputs the read tag in decimal

#define Manufacturer_ID_Output    // The output will contain the Manufacturer ID (NOT IMPLEMENTED)
#define Site_Code_Output          // The output will contain the Site Code       (NOT IMPLEMENTED)
#define Unique_Id_Output          // The output will contain the Unique ID

//#define Split_Tags_With '-'       // The character to split tags pieces with

//#define Whitelist_Enabled         // When a tag is read it will be compaired 
                                  // against a whitelist and one of two functions
                                  // will be run depending on if the id matches
                                 
                                 
                                 // some conststents
                                  
// These values may need to be changed depending on the servo that you are using
#define SERVO_OPEN 575    // open signal value for the servo
#define SERVO_CLOSE 1000  // close signal value for the servo


//20-bit manufacturer code,
//8-bit site code
//16-bit unique id

#define MANUFACTURER_ID_OFFSET 0
#define MANUFACTURER_ID_LENGTH 20

#define SITE_CODE_OFFSET 20
#define SITE_CODE_LENGTH 8
	 
#define UNIQUE_ID_OFFSET 28
#define UNIQUE_ID_LENGTH 16



// these settings are used internally by the program to optimize the settings above
#ifndef serialOut
  #define serialOut
#endif






/// Begin the includes

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "ECAN.h"
#include "I2C.h"
#include "Other.h"
#include "init.h"
#include "RFID_new.h"










#define ARRAYSIZE 500   // Number of RF points to collect each time

//char * begin;           // points to the bigining of the array
int begin[ARRAYSIZE];
int * names;            // array of valid ID numbers
int namesize;           // size of array of valid ID numbers
volatile int iter;      // the iterator for the placement of count2 in the array
volatile int count2;     // count2s 125kHz pulses
volatile int lastpulse; // last value of DEMOD_OUT
volatile int on;        // stores the value of DEMOD_OUT in the interrupt

int resultArray[200];

/********************************* ADD NAMES *********************************\
| This function add allocates the ammount of memory that will be needed to    |
| store the list of names, and adds all the saved names to the allocated      |
| memory for use later in the program                                         |
\*****************************************************************************/
void addNames(void) {
  namesize = 2; // number of IDs in the access list
  names = malloc (sizeof(int) * namesize);
  // change or add more IDs after this point
  names [0] = 12345;
  names [1] = 56101;
}

/******************************* INT0 INTERRUPT *******************************\
| This ISR(INT0_vect) is the interrupt function for INT0. This function is the |
| function that is run each time the 125kHz pulse goes HIGH.                   |
| 1) If this pulse is in a new wave then put the count2 of the last wave into   |
|     the array                                                                |
| 2) Add one to the count2 (count2 stores the number of 125kHz pulses in each    |
|     wave                                                                     |
\******************************************************************************/
void newIsr(){
  //Save the value of DEMOD_OUT to prevent re-reading on the same group
  on = PORTBbits.RB0;
  // if wave is rising (end of the last wave)
  if (on == 1 && lastpulse == 0 ) {
    // write the data to the array and reset the cound
    begin[iter] = count2; 
    count2 = 0;
    iter = iter + 1;
  }
  count2 = count2 + 1;
  lastpulse = on;
}
  //////////////////////////////////////////////////////////////////////////////
 ////////////////////////// BASE CONVERSION FUNCTIONS /////////////////////////
//////////////////////////////////////////////////////////////////////////////
char binaryTohex (int four, int three, int two, int one) {
  int value = (one << 0) + (two << 1) + (three << 2) + (four << 3);
  if (value > 9) return 'A' + value - 10;
  return '0' + value;
}

/*********************** GET HEX ARRAY FROM BINARY ARRAY **********************\
|
\******************************************************************************/
int * getHexFromBinary (int * array, int length, int * result) {
  int i;
  int resultLength = (length+3)/4; // +3 so that the resulting number gets rounded up
  // 4 / 4 = 1       [correct]
  // 7 / 4 = 1 (4+3) [still correct]
  // 5 / 4 = 1       [not rounded up]
  // 8 / 4 = 2 (5+3) [correct]
  
  for (i = 0; i < resultLength; i++) {
    result[i*4] = (array[i+0] << 0)
                + (array[i+1] << 1)
                + (array[i+2] << 2)
                + (array[i+3] << 3);
  }
  return result;
}
/*************************** GET DECIMAL FROM BINARY **************************\
| This function will take in a binary input and return an intiger with the     |
| corrisponding value, assumed as decimal                                      |
\******************************************************************************/
int getDecimalFromBinary (int * array, int length) {  
  int result = 0;
  int i;
  for (i = 0; i < length; i++) {
    result = result<<1;
    result += array[i]&0x01;
  }
  return result;
}

void printDecimal (int array[45]) {
}
void printHexadecimal (int array[45]) {
  int i;
  
}




/********************************* Search Tag *********************************\
| This function searches for a tag in the list of tags stored in the flash     |
| memory, if the tag is found then the function returns 1 (true) if the tag    |
| is not found then the function returns 0 (false)                             |
\******************************************************************************/
int searchTag (int tag) {
  int i;
  for (i = 0; i < namesize; i++) {
    if (tag == names[i]) {
      return 1;
    }
  }
  return 0;
}




  //////////////////////////////////////////////////////////////////////////////
 ///////////////////////////// ANALYSIS FUNCTIONS /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/************************* CONVERT RAW DATA TO BINARY *************************\
| Converts the raw 'pulse per wave' count2 (5,6,or 7) to binary data (0, or 1)  |
\******************************************************************************/
void convertRawDataToBinary () {
  int i;
  for (i = 1; i < ARRAYSIZE; i++) {
    if ((begin[i] < 47) && (begin[i] > 43)) {  //5            //HAR ÄNDRAT PLATS PÅ 1 OCH 0 - EMIL
      begin[i] = 1;
    }
    else if ((begin[i] < 94) && (begin[i] > 90)) {    //7
      begin[i] = 0;
    }
    else if ((begin[i] < 70) && (begin[i] > 66)) {    //6
       begin[i] = begin[i-1];
    }
    else {
      begin[i] = -2;
    }
  }
}

/******************************* FIND START TAG *******************************\
| This function goes through the buffer and tries to find a group of fifteen   |
| or more 1's in a row. This sigifies the start tag. If you took the fifteen   |
| ones in multibit they would come out to be '111' in single-bit               |
\******************************************************************************/
int findStartTag2 () {
  int i;
  int inARow = 0;
  int lastVal = 0;
  for (i = 0; i < ARRAYSIZE; i++) {
    if (begin [i] == lastVal) {
      inARow++;
    }
    else {
      // End of the group of bits with the same value
      if (inARow >= 15 && lastVal == 1) {
        // Start tag found
        break;
      }
      // group of bits was not a start tag, search next tag
      inARow = 1;
      lastVal = begin[i];
    }
  }
  return i;
}

/************************ PARSE MULTIBIT TO SINGLE BIT ************************\
| This function takes in the start tag and starts parsing the multi-bit code   |
| to produce the single bit result in the outputBuffer array the resulting     |
| code is single bit manchester code                                           |
\******************************************************************************/
void parseMultiBitToSingleBit2 (int startOffset) {
  int i = startOffset; // the offset value of the start tag
  int lastVal = 0; // what was the value of the last bit
  int inARow = 0; // how many identical bits are in a row// this may need to be 1 but seems to work fine
  int resultArray_index = 0;
  for (;i < ARRAYSIZE; i++) {
    if (begin [i] == lastVal) {
      inARow++;
    }
    else {
      // End of the group of bits with the same value
      if (inARow >= 4 && inARow <= 8) {
        // there are between 4 and 8 bits of the same value in a row
        // Add one bit to the resulting array
        resultArray[resultArray_index] = lastVal;
        resultArray_index += 1;
      }
      else if (inARow >= 9 && inARow <= 14) {
        // there are between 9 and 14 bits of the same value in a row
        // Add two bits to the resulting array
        resultArray[resultArray_index] = lastVal;
        resultArray[resultArray_index+1] = lastVal;
        resultArray_index += 2;
      }
      else if (inARow >= 15 && lastVal == 0) {
        // there are more then 15 identical bits in a row, and they are 0s
        // this is an end tag
        break;
      }
      // group of bits was not the end tag, continue parsing data
      inARow = 1;
      lastVal = begin[i];
      if (resultArray_index >= 90) {
        //return;
      }
    }
  }
}

/******************************* Analize Input *******************************\
| analizeInput(void) parses through the global variable and gets the 45 bit   |
| id tag.                                                                     |
| 1) Converts raw pulse per wave count2 (5,6,7) to binary data (0,1)           |
| 2) Finds a start tag in the code                                            |
| 3) Parses the data from multibit code (11111000000000000111111111100000) to |
|     singlebit manchester code (100110) untill it finds an end tag           |
| 4) Converts manchester code (100110) to binary code (010)                   |
\*****************************************************************************/
void analizeInput (void) {
  int i;                // Generic for loop 'i' count2er
  int resultArray[90];  // Parsed Bit code in manchester
  int finalArray[45];   //Parsed Bit Code out of manchester
  int finalArray_index = 0;
  
  // Initilize the arrays so that any errors or unchanged values show up as 2s
  for (i = 0; i < 90; i ++) { resultArray[i] = 2; }
  for (i = 0; i < 45; i++)  { finalArray[i] = 2;  }
  
  // Convert raw data to binary
  convertRawDataToBinary ();
    
  // Find Start Tag
  int startOffset = findStartTag2();
  LATCbits.LATC7 = 1; // turn an led on on pin RC7)
  
  // Parse multibit data to single bit data
  parseMultiBitToSingleBit2(startOffset);
  
  // Error checking, see if there are any unset elements of the array
  for (i = 0; i < 88; i++) { // ignore the parody bit ([88] and [89])
    if (resultArray[i] == 2) {
      return;
    }
  }
  //------------------------------------------
  // MANCHESTER DECODING
  //------------------------------------------
  for (i = 0; i < 88; i+=2) { // ignore the parody bit ([88][89])
    if (resultArray[i] == 1 && resultArray[i+1] == 0) {
      finalArray[finalArray_index] = 1;
    }
    else if (resultArray[i] == 0 && resultArray[i+1] == 1) {
      finalArray[finalArray_index] = 0;
    }
    else {
      // The read code is not in manchester, ignore this read tag and try again
      // free the allocated memory and end the function
      return;
    }
    finalArray_index++;
  }
}

/******************************* MAIN FUNCTION *******************************\
| This is the main function, it initilized the variabls and then waits for    |
| interrupt to fill the buffer before analizing the gathered data             |
\*****************************************************************************/
int startRFID_new(){
  int i = 0;

  //------------------------------------------
  // VARIABLE INITLILIZATION
  //------------------------------------------

  // Load the list of valid ID tags
  //addNames(); 
  
  //========> VARIABLE INITILIZATION <=======//
  count2 = 0;
  //begin = malloc (sizeof(char)*ARRAYSIZE);
  
  iter = 0;
  for (i = 0; i < ARRAYSIZE; i ++) {
    begin[i] = 0;
  }

  //------------------------------------------
  // MAIN LOOP
  //------------------------------------------
  while (1) {
    while (1) { // while the card is being read
      if (iter >= ARRAYSIZE) { // if the buffer is full
        INTCONbits.TMR0IE = 0; //Disables the TMR0 overflow interrupt
        break; // continue to analize the buffer
      }
    }  
    
    
    //analize the array of input
    analizeInput ();
    LATCbits.LATC6 = 1;
    while(1);
    
    //reset the saved values to prevent errors when reading another card
    count2 = 0;
    iter = 0;
    for (i = 0; i < ARRAYSIZE; i ++) {
      begin[i] = 0;
    }
  }
}
#endif