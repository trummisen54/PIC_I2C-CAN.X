/* 
 * File:   I2C.h
 * Author: E
 *
 * Created on den 26 november 2015, 16:07
 */
#ifndef I2C_H
#define	I2C_H





#define I2C_ADDR    0x54    // 8 bit address
#define DEVICE_OSC  64
#define ONE_MS      (unsigned int)(DEVICE_OSC/4)*80


typedef unsigned char   byte;


extern void checkI2C();
extern void I2CSetup();
void Delay(unsigned int count);



volatile byte           i2c_reg_addr     = 0;
volatile byte           i2c_reg_map[16] = {0,};
volatile byte           i2c_byte_count   = 0;
/*
 * 0 = motor
 * 1 = batteristatus
 */







#endif	/* I2C_H */

