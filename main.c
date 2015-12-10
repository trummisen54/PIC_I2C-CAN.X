#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "ECAN.h"
#include "I2C.h"
#include "Other.h"
#include "RFID.h"
#include "init.h"



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
    
    CHIP_ALIVE_DIODE = 1;
    HEARTBEAT_DIODE = 0;
    ERROR_DIODE = 0;
    
    
    while(1){
        
        if(ECAN_Receive()){
        
            i2c_reg_map[REC_BATTERYSTATUS] = MAP_REC_BATTERYSTATUS;
            i2c_reg_map[REC_VELOCITY] = MAP_REC_VELOCITY;
            i2c_reg_map[REC_HEARTBEAT] = MAP_REC_HEARTBEAT;
            i2c_reg_map[3] = temp_D3;
            i2c_reg_map[4] = temp_D4;
            i2c_reg_map[5] = temp_D5;
            i2c_reg_map[6] = temp_D6;
            i2c_reg_map[7] = temp_D7;

        }
        
        zipCAN();
        
        if(I2C_FLAG){
            I2C_FLAG = 0;
            ECAN_Transmit(0x32,0xC0, 0x02,
                    MAP_BITDATA,//zipped data
                    MAP_ACCELERATOR,
                    i2c_reg_map[10],//dummy values
                    i2c_reg_map[11],//dummy values
                    i2c_reg_map[12],//dummy values
                    i2c_reg_map[13],//dummy values
                    i2c_reg_map[14],//dummy values
                    i2c_reg_map[15]);//dummy values
            

        }
      

      //checkRFID();

      Delay(ONE_MS * 5);
    }
    

    return 0;
}