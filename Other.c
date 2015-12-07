#include "ECAN.h"
#include "I2C.h"
#include "config.h"
#include "Other.h"

void InitDevice(void){   
    
    Interrupt_counter = 0;
    
    // Set the internal oscillator to 64MHz
    OSCCON            = 0x60;       // IRCFx = 110
    OSCCONbits.IRCF = 7;
    OSCTUNEbits.PLLEN = 1;
    
    // Initialize global variables to 0
    heartbeatCount = 0;
    buttonWasPressed = 0;
    
    // Initialize I/O to be digital, with PORTC (LEDs) as outputs and PORTB as inputs (pushbutton)
    ANCON0 = ANCON1 = 0x00;
    TRISB = 0xFF;//B0 = RFID input
    
    // Port A used for diagnostic LEDs
    TRISA      = 0b00111111;     // PORTA bit 7 to output (0) ; bits 6:0 are inputs (1)
    LATAbits.LATA7 = 0;             // GREEN LED
        
    // Setup MSSP in 7 bit I2C Slave mode
    TRISC          = 0b00011000;    // TRISC 3&4 (SCL & SDA) inputs
    LATCbits.LATC5 = 0; //heartbeat diode
    LATCbits.LATC6 = 0; //receive diode
    LATCbits.LATC7 = 0; //send diode
    
    MAP_HEARTBEAT = 0;
    
    
    
    //SYSTEM TIMER
    T0CONbits.TMR0ON = 1; // Enable timer
    T0CONbits.T08BIT = 0; //16 bit
    T0CONbits.T0CS = 0; //not counter
    //T0CONbits.T0SE = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS = 7; //111 -> 1000 0111
    
    
}

void checkHeartbeat(){
    if(i2c_reg_map[5] != MAP_HEARTBEAT){
        MAP_HEARTBEAT = i2c_reg_map[5];
        //send
    }
}

void setup_Interrupt(){
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    INTCONbits.TMR0IE = 1; //Enables the TMR0 overflow interrupt
    

}


void Delay(unsigned int count)
{
    // Countdown until equal to zero and then return
    while(count--);
}