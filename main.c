#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "ECAN.h"
#include "I2C.h"
#include "Other.h"
#include "RFID.h"



void interrupt ISR(){
    Interrupt_counter++;
    
    if(Interrupt_counter == 2){
        //checkI2C();
        callback();//rfid
        Interrupt_counter = 0;
    }
    
    
}

int main(void){
    
    
    
    InitDevice();
    setup_Interrupt();
    CANSetup();
    I2CSetup();
    
    LATAbits.LA7 = 1; // Chip alive indicator
    
    while(1){
        
        //checkHeartbeat();
        
        //checkCAN();
      /*
        if(1){
            I2C_FLAG = 0;
            ECAN_Transmit(0x32,0xC0, 0x08,
                    0x26,//i2c_reg_map[8],
                    i2c_reg_map[9],
                    i2c_reg_map[10],
                    i2c_reg_map[11],
                    i2c_reg_map[12],
                    i2c_reg_map[13],
                    i2c_reg_map[14],
                    i2c_reg_map[15]);
            
            LATCbits.LATC7 = 1; // send diode

        }
      */
      Delay(ONE_MS * 5);
      
      checkRFID();
      
      
      
      
      
      LATCbits.LATC6 = 0;
      LATCbits.LATC7 = 0;
      
      
      
    }
    

    return 0;
}