/*  Arduino program for DIY FSK RFID Reader
 *  See description and circuit diagram at http://playground.arduino.cc/Main/DIYRFIDReader
 *  Tested on Arduino Nano and several FSK RFID tags
 *  Hardware/Software design is based on and derived from:
 *  Arduino/Timer1 library example
 *  June 2008 | jesse dot tane at gmail dot com
 *  AsherGlick: / AVRFID https://github.com/AsherGlick/AVRFID
 *  Micah Dowty:
 *  http://forums.parallax.com/showthread.php?105889-World-s-simplest-RFID-reader
 *
 *  Copyright (C) 2011 by edude/Arduino Forum
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */


//#define bort2
#ifndef bort2

#include "RFID.h"
#include "config.h"
#include "Other.h"
#include <stdio.h>



void sample()
{

  val = PORTBbits.RB0;
  bitlenctr++;

  if(val != curState) {
    // got a transition
    curState = val;
    if(val == 1) {
      // got a start of cycle (low to high transition)
      if(rawIndex < rawLength) {
        raw[rawIndex++] = bitlenctr;
      }
      else{
        INTCONbits.TMR0IE = 0; //Disables the TMR0 overflow interrupt
        
      }
      bitlenctr = 1;
    }
  }  
}

 
int multiToSingle(){
    
    if(rawIndex >= rawLength) {

        int _long = 0;
        int _middle = 0;
        int _short = 0;
        int totError = 0;

        raw[0] = 0;

        for(int i = 1; i < rawLength; i++) {
            int v = raw[i];
            if(v > 44 && v < 53) {
                singleArray[i] = 1;
                _short++;
            }
            else if(v > 70 && v < 79) {
                singleArray[i] = 2;
                _middle++;
            }        
            else if(v > 95 && v < 104) {
                singleArray[i] = 3;
                _long++;
            }
            else {
                singleArray[i] = 999;//error
                totError++;
            } 
        }
        return 1;//start ok
    }
    return -1;//start not ok
}

int findStart(){
    int count = 0;
    
    for(int i = 0; i<bitArrayLength; i++){
        if(bitArray[i] != 2){//ifnot invalid code, (earlier out of phase)
            
            if(bitArray[i] == 1){
                count++;
                if(count == 9){ // have got start pattern (1111111110)
                    return i+1;
                }
            }
            else{
                count = 0;
            }
            
        }
    }
}

void singleToBit(){
    
    int lastVal = 1;
    int start = 0;
    int OutOfPhaseCorrected = 0;
    int oopcCount = 0;
    
    for(int i = 0; i<singleArrayLength; i++){
        if(singleArray[i] != 0 && singleArray[i] != 999){
            start = i;
            break;
        }
        if(i == singleArrayLength){
            while(1){}//Error
        }
    
    }
    int k = 0;
    for(int i = 0; i<singleArrayLength; i++){
        if(k >= bitArrayLength - 1){break;}
        
        if(lastVal == 1){

            if(singleArray[i+start] == 1){
                bitArray[k++] = 1;
            }
            else if(singleArray[i+start] == 2){
                bitArray[k++] = 0;
                bitArray[k++] = 0;
            }
            else if(singleArray[i+start] == 3){
                bitArray[k++] = 0;
                bitArray[k++] = 1;
            }
        }
        else{
            if(singleArray[i+start] == 1){
                bitArray[k++] = 0;
            }
            else if(singleArray[i+start] == 2){
                bitArray[k++] = 1;
            }
            else if(singleArray[i+start] == 3){
                
                oopcCount++;
                if (OutOfPhaseCorrected == 0){
                    OutOfPhaseCorrected = k;
                    while(OutOfPhaseCorrected > 0){
                        bitArray[--OutOfPhaseCorrected] = 2;
                    }
                    
                }
                
                bitArray[k++] = 0;
                bitArray[k++] = 1;
                
                
            }
        }
        
        if(bitArray[k-1] == 1){
            lastVal = 1;
        }
        else{lastVal = 0;}

    }
    
    for(int i = 0; i< bitArrayLength; i++){
        bitArray[i] = bitArray[i] ^ 1;
    }
    
}

void bitToCode(int start){
    int code[30];
    
    for(int i = 0; i<30; i++){
        code[i] = (0b1111 & (bitArray[i*5+start] << 3)) | 
                (0b1111 & (bitArray[i*5+start+1] << 2)) | 
                (0b1111 & (bitArray[i*5+start+2] << 1)) | 
                (0b1111 & (bitArray[i*5+start+3] << 0));
    }
    
    while(1){
        int stanna = 4;
    }
}
    
  
#endif