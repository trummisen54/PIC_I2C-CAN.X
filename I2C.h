/* 
 * File:   I2C.h
 * Author: E
 *
 * Created on den 26 november 2015, 16:07
 */
#ifndef I2C_H
#define	I2C_H





#define I2C_ADDR    0x55    // 8 bit address
#define DEVICE_OSC  64
#define ONE_MS      (unsigned int)(DEVICE_OSC/4)*80


typedef unsigned char   byte;
int I2C_FLAG;
int FIRST_SEND;

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

#define REC_BATTERYSTATUS0  0
#define REC_BATTERYSTATUS1  1
#define REC_BATTERYSTATUS2  2// not used
#define REC_BATTERYSTATUS3  3// not used
#define REC_VELOCITY        4
#define REC_HEARTBEAT       5
//#define RAC_BATTERYSTATUS   6
#define SEND_STOP           7
#define SEND_HEARTBEAT      8
#define SEND_BRAKE          9
#define SEND_BACKLIGHT      10
#define SEND_V_BLINK        11
#define SEND_H_BLINK        12
#define SEND_DIRECTION      13
#define SEND_SAFETYPIN      14
#define SEND_ACCELERATOR    15


int MAP_HEARTBEAT;
int MAP_BRAKE;
int MAP_BACKLIGHT;
int MAP_V_BLINK;
int MAP_H_BLINK;
int MAP_DIRECTION;
int MAP_SAFETYPIN;
int MAP_STOP;


#endif	/* I2C_H */

