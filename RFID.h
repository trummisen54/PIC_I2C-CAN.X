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


void checkRFID();
void callback();
void process_buf(int start);
void setupRFID();


void collectManch();
void parseMultiBitToSingleBit (int startOffset);
void decodeManchester(int finalArray[]);

#endif	/* RFID_H */

