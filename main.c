#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "ECAN.h"
#include "I2C.h"
#include "Other.h"
#include "RFID.h"
#include "init.h"





int main(void){
    
    InitDevice();
    setup_Interrupt();
    CANSetup();
    I2CSetup();
    
    heartBeatCounter = 0;
    FIRST_SEND = 1;
    I2C_FLAG = 0;
    
    CHIP_ALIVE_DIODE = 1;
    HEARTBEAT_DIODE = 0;
    ERROR_DIODE = 0;
    
    
    
    while(1){
        
        while(1){
            
            Delay(1);
            //checkRFID();
        }
        
        if(ECAN_Receive()){
            FIRST_SEND = 0;
            HEARTBEAT_DIODE = 1;
            i2c_reg_map[REC_BATTERYSTATUS] = MAP_REC_BATTERYSTATUS;
            i2c_reg_map[REC_VELOCITY] = MAP_REC_VELOCITY;
            i2c_reg_map[2] = temp_D2;
            i2c_reg_map[3] = temp_D3;
            i2c_reg_map[4] = temp_D4;
            i2c_reg_map[5] = temp_D5;
            i2c_reg_map[6] = temp_D6;
            i2c_reg_map[7] = temp_D7;
            
            heartBeatCounter = 0;

        }
        
        if(heartBeatCounter > 25){
            //danger
            i2c_reg_map[2] = 0;
        }

        
        if(I2C_FLAG){
            I2C_FLAG = 0;
            zipCAN();
            ECAN_Transmit(0x32,0xC0, 0x02,
                    MAP_BITDATA,//zipped data
                    i2c_reg_map[SEND_ACCELERATOR],
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

void interrupt ISR(){   
    //checkI2C(); 
    
    
    
    /*if(TMR0IF == 1){
        if(!FIRST_SEND){
            heartBeatCounter++;
        }
        TMR0IF = 0;
    }
    */
    
    
    Interrupt_counter++;
  
    if(Interrupt_counter == 10){//2
        
        //callback();//rfid
        collectManch();
        Interrupt_counter = 0;
    }
    
    
}