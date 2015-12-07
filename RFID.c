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

#include "RFID.h"
#include "config.h"
#include "Other.h"
 
int ledPin = 13; // LED connected to digital pin 13
int inPin = 7;   // sensing digital pin 7
int val;
int bitlenctr = 0;
int curState = 0;
 
#define maxBuf 1000 //reduce to 100 or so for debugging
#define debug  0
 
char raw[maxBuf];
 
int index = 0;
int bufnum = 0;
#define   redLED 12
#define   grnLED 11
 
void setupRFID()
{

 
  //pinMode(ledPin, OUTPUT);      // sets the digital pin 13 as output for scope monitoring
  

  //pinMode(grnLED, OUTPUT);
  //pinMode(redLED, OUTPUT);
  //digitalWrite(grnLED, 0);
  //digitalWrite(redLED, 1);
}
 
void callback()
{
    
    
    LATCbits.LATC7 ^= 1; //Measure with oscilloscope to se actual freq
    
    
  val = PORTBbits.RB0;
  
  //digitalWrite(ledPin, val); // for monitoring
  
  bitlenctr++;
  if(val != curState) {
    // got a transition
    curState = val;
    if(val == 1) {
      // got a start of cycle (low to high transition)
      if(index < maxBuf) {
        raw[index++] = bitlenctr;
      }
      bitlenctr = 1;
    }
  }  
}





void checkRFID(){  
    
    
    if(index >= maxBuf) {

      //Serial.print("got buf num: ");
      //Serial.println(bufnum);

      if(debug) {
        for(int i = 0; i < maxBuf;
        i++) {
           // Serial.print((int)raw[i]);
          //Serial.print("/");
        }
        //Serial.println("///raw data");
        Delay(ONE_MS * 2000);
      }

      // analyze this buffer
      // first convert pulse durations into raw bits
      int tot1 = 0;
      int tot0 = 0;
      int tote = 0;
      int totp = 0;
      raw[0] = 0;
      for(int i = 1; i < maxBuf; i++) {
        int v = raw[i];
        if(v == 4) {
          raw[i] = 0;
          tot0++;
        }
        else if(v == 5) {
          raw[i] = raw[i - 1];
          totp++;
        }
        else if(v == 6 || v == 7) {
          raw[i] = 1;
          tot1++;
        }
        else {
          raw[i] = 101; // error code
          tote++;
        }  
      }  

      
      //************
      
      
      // next, search for a "start tag" of 15 high bits in a row
      int samecnt = 0;
      int start = -1;
      int lastv = 0;
      for(int i = 0; i < maxBuf; i++) {
        if(raw[i] == lastv) {
          // inside one same bit pattern, keep scanning
          samecnt++;
        }
        else {
          // got new bit pattern
          if(samecnt >= 15 && lastv == 1) {
            // got a start tag prefix, record index and exit
            start = i;
            break;
          }
          // either group of 0s, or fewer than 15 1s, so not a valid tag, keep scanning
          samecnt = 1;
          lastv = raw[i];
        }  
      }

      // if a valid prefix tag was found, process the buffer
      if(start > 0 && start < (maxBuf - 5*90)) { //adjust to allow room for full dataset past start point
        process_buf(start);
      }
      else {
       // Serial.println("no valid data found in buffer");
      }
      
      /*
      if(debug) {
        for(int i = 0; i < maxBuf;
          i++) {
           // Serial.print((int)raw[i]);
          //Serial.print("/");
        }
        
        Serial.print("///\nbuffer stats: zeroes:");
        Serial.print(tot0);
        Serial.print("/ones:");
        Serial.print(tot1);
        Serial.print("/prevs:");
        Serial.print(totp);
        Serial.print("/errs:");
        Serial.println(tote);  
        delay(1000);
      
          
      }
      */

      // start new buffer, reset all parameters
      bufnum++;
      curState = 0;
      index = 0;
    }
    else {
      Delay(5 * ONE_MS);
    }  
   
}


 
// process an input buffer with a valid start tag
// start argument is index to first 0 bit past prefix tag of 15+ ones


void process_buf(int start) {

    
    
  // first convert multi bit codes (11111100000...) into manchester bit codes
  int lastv = 0;
  int samecnt = 0;
  char manch[91];
  char final[45];
  int manchindex = 0;
 
  //Serial.println("got a valid prefix, processing data buffer...");
  for(int i = start + 1; i < maxBuf && manchindex < 90; i++) {
    if(raw[i] == lastv) {
      samecnt++;
    }
    else {
      // got a new bit value, process the last group
      if(samecnt >= 3 && samecnt <= 8) {
        manch[manchindex++] = lastv;
      }
      else if(samecnt >= 9 && samecnt <= 14) {
        // assume a double bit, so record as two separate bits
        manch[manchindex++] = lastv;
        manch[manchindex++] = lastv;
      }
      else if(samecnt >= 15 && lastv == 0) {
        //Serial.println("got end tag");
        // got an end tag, exit
        break;
      }
      else {
        // last bit group was either too long or too short
        //Serial.print("****got bad bit pattern in buffer, count: ");
       // Serial.print(samecnt);
       // Serial.print(", value: ");
       // Serial.println(lastv);
      //  err_flash(3);
        return;
      }  
      samecnt = 1;
      lastv = raw[i];
    } //new bit pattern
  }
 
 // Serial.println("converting manchester code to binary...");
  // got manchester version, convert to final bits
  for(int i = 0, findex = 0; i < 90; i += 2, findex++) {
    if(manch[i] == 1 && manch[i+1] == 0) {
      final[findex] = 1;
    }
    else if(manch[i] == 0 && manch[i+1] == 1) {
      final[findex] = 0;
    }
    else {
      // invalid manchester code, exit
   //   Serial.println("****got invalid manchester code");
     // err_flash(3);
      return;
    }
  }
 
  // convert bits 28 thru 28+16 into a 16 bit integer
  int code = 0;
  int par = 0;
  for(int i = 28, k = 15; i < 28+16; i++, k--) {
    code |= (int)final[i] << k;
  }
  int paritybit = final[28+16];
  for(int i = 0; i < 45; i++) {
    par ^= final[i];
  }
 
  if(par) {  
  //  Serial.print("got valid code: ");
   // Serial.println((unsigned int)code);
    // do something here with the detected code...
    //
 //   //
   // digitalWrite(redLED, 0);
 //   digitalWrite(grnLED, 1);
//    Delay(ONE_MS * 2000);
  //  digitalWrite(grnLED, 0);
 //   digitalWrite(redLED, 1);
  }
  else {
//    Serial.println("****parity error for retrieved code");
//    err_flash(3);
  }  
}


 
// flash red for duration seconds

/*
void err_flash(int duration) {
  return;
  for(int i = 0; i < duration*10; i++) {
    digitalWrite(redLED, 0);
    delay(50);
    digitalWrite(redLED, 1);
    delay(50);
  }
}
 */
  
