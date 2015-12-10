#include "config.h"
#include "Other.h"
#include "init.h"


void Delay(unsigned int count)
{
    // Countdown until equal to zero and then return
    while(count--);
}


//if an error occure, call this method
void error(){
    ERROR_DIODE = 1;
    
}