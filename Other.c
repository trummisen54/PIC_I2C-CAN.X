#include "ECAN.h"
#include "I2C.h"
#include "config.h"

void InitDevice(void){   
    count = 0x01;
    
    // Set the internal oscillator to 64MHz
    OSCCONbits.IRCF = 7;
    OSCTUNEbits.PLLEN = 1;
    
    // Initialize global variables to 0
    heartbeatCount = 0;
    buttonWasPressed = 0;
    
    // Initialize I/O to be digital, with PORTC (LEDs) as outputs and PORTB as inputs (pushbutton)
    ANCON0 = ANCON1 = 0x00;
    TRISB = 0xFF;
    
    // Port A used for diagnostic LEDs
    TRISA      = 0b00111111;     // PORTA bit 7 to output (0) ; bits 6:0 are inputs (1)
    LATAbits.LATA7 = 0;             // RED LED 
    LATAbits.LATA6 = 0;             // YLW LED 
        
    // Setup MSSP in 7 bit I2C Slave mode
    TRISC          = 0b00011000;    // TRISC 3&4 (SCL & SDA) inputs
}