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
    
    
    while(1){
      checkCAN();
      Delay(100);
    }
    

    return 0;
}