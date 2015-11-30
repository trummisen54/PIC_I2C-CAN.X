#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "ECAN.h"
#include "I2C.h"
#include "Other.h"


void interrupt ISR(){
   
    checkI2C();
    
}

int main(void){
    
    
    
    InitDevice();
    CANSetup();
    I2CSetup();
    
    LATAbits.LA7 = 1; // Chip alive indicator
    
    while(1){
        checkCAN();
      
        if(1){
            I2C_FLAG = 0;
            ECAN_Transmit(0x32,0xC0, 0x08,
                    i2c_reg_map[8],
                    i2c_reg_map[9],
                    i2c_reg_map[10],
                    i2c_reg_map[11],
                    i2c_reg_map[12],
                    i2c_reg_map[13],
                    i2c_reg_map[14],
                    i2c_reg_map[15]);

        }
      
      Delay(200);
    }
    

    return 0;
}