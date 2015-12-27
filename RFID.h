/* 
 * File:   RFID.h
 * Author: E
 *
 * Created on den 7 december 2015, 14:26
 */

#ifndef RFID_H
#define	RFID_H


#define DEVICE_OSC  64
#define ONE_MS      (unsigned int)(DEVICE_OSC/4)*80


int multiToSingle();
void sample();
int findStart();
void singleToBit();
void bitToCode(int start);


 
#define rawLength 500 //reduce to 100 or so for debugging
#define singleArrayLength 200
#define bitArrayLength 201



#define   redLED 12
#define   grnLED 11


int val;
int bitlenctr = 0;
int curState = 0;
char raw[rawLength];
int rawIndex = 0;

int singleArray[singleArrayLength];
int bitArray[bitArrayLength];


#endif	/* RFID_H */

