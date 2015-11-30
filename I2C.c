#include "I2C.h"
#include "config.h"

void I2CSetup(){
    
    SSPADD         = I2C_ADDR;      // Set I2C address
    SSPCON1        = 0x36;          // SSPEN: Synchronous Serial Port Enable bit - Enables the serial port and configures the SDA and SCL pins as the serial port pins
                                    // CKP: SCK Release Control bit              - Release clock
                                    // SSPM3:SSPM0: SSP Mode Select bits         - 0110 = I2C Slave mode, 7-bit address    
    SSPSTAT        = 0x00;
    SSPCON2        = 0x01;          // GCEN: General Call address (00h) (Slave mode only) 0 = General call address disabled
                                    // SEN: Start Condition Enable/Stretch Enable bit(1) ()Slave mode) 1 = Clock stretching is enabled 
    PIR1bits.SSPIF = 0;             // Clear MSSP interrupt request flag
    PIE1bits.SSPIE = 1;             // Enable MSSP interrupt enable bit
    INTCONbits.GIE_GIEH  = 1;       // GIE/GIEH: Global Interrupt Enable bit
    INTCONbits.PEIE_GIEL = 1;       // PEIE/GIEL: Peripheral Interrupt Enable bit
    

}

void checkI2C(){
    
    byte sspBuf;
    
    if (PIR1bits.SSPIF) {
        
        if (!SSPSTATbits.D_NOT_A) { //SSPSTATbits.D_NOT_A == 1, indicates that the last byte received or transmitted was data, == 0 -||- address
            //
            // Slave Address 
            //
            i2c_byte_count = 0;

            if (SSPSTATbits.BF) { // buffert full
                // Discard slave address 
                sspBuf = SSPBUF;    // Clear BF
            }
            
            if (SSPSTATbits.R_NOT_W) { //Read mode                
                // Reading - read from register map
                SSPCON1bits.WCOL = 0; // no collision
                SSPBUF           = i2c_reg_map[i2c_reg_addr++];
            } 
        } 
        else { //Data, not Address
            //
            // Data bytes 
            //
            i2c_byte_count++;

            if (SSPSTATbits.BF) {
                sspBuf = SSPBUF;    // Clear BF
            }

            if (SSPSTATbits.R_NOT_W) { //Read

                // Multi-byte read - advance to next address
                SSPCON1bits.WCOL = 0;
                SSPBUF           = i2c_reg_map[i2c_reg_addr++];
                LATAbits.LATA6 = 1;
                
            } 
            else { //Write

                if (i2c_byte_count == 1) {
                    // First write byte is register address
                    i2c_reg_addr = sspBuf;

                } 
                else {
                    // Write to register address - auto advance
                    //   to allow multiple bytes to be written
                    i2c_reg_map[i2c_reg_addr++] = sspBuf;
                    I2C_FLAG = 1;
                }
            }
        }
        // Limit address to size of register map
        i2c_reg_addr %= sizeof(i2c_reg_map);
        
        // Finally
        PIR1bits.SSPIF  = 0;            // Clear MSSP interrupt flag
        SSPCON1bits.CKP = 1;            // Release clock        
    }    
}