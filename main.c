#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "ECAN.h"
#include "I2C.h"
#include "Other.h"
#include "RFID.h"
#include "init.h"
#include "RFID_new.h"





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
    
    
    #ifdef TEST_RFID
    while(1){
        
        int start_condition = multiToSingle();
        
        if(start_condition == 1){
            singleToBit();
            int start = findStart();
            bitToCode(start);
        
        }
        Delay(10);
    }
    #endif
    
    
    
    while(1){
         
        
        if(ECAN_Receive()){
            FIRST_SEND = 0;
            HEARTBEAT_DIODE = 1;
            i2c_reg_map[REC_BATTERYSTATUS0] = MAP_REC_BATTERYSTATUS0;
            i2c_reg_map[REC_BATTERYSTATUS1] = MAP_REC_BATTERYSTATUS1;
            i2c_reg_map[REC_BATTERYSTATUS2] = 0;//not used
            i2c_reg_map[REC_BATTERYSTATUS3] = 0;//not used
            i2c_reg_map[REC_VELOCITY] = MAP_REC_VELOCITY;
            i2c_reg_map[REC_HEARTBEAT] = temp_D2;//always one (heartbeat)
            
            i2c_reg_map[6] = temp_D6;
            i2c_reg_map[7] = temp_D7;
            
            heartBeatCounter = 0;

        }
        
        if(heartBeatCounter > 35){
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
    
    #ifndef TEST_RFID
    checkI2C(); 
    
    if(TMR0IF == 1){
        if(!FIRST_SEND){
            heartBeatCounter++;
        }
        TMR0IF = 0;
    }
    #endif
    
    
    #ifdef TEST_RFID

    Interrupt_counter++;
  
    if(Interrupt_counter == 2){//2   2 = 125kHZ       // 10på emilkod
              
        sample();
        Interrupt_counter = 0;
    }
    #endif
    
    
}