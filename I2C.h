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
extern int I2C_FLAG = 0;

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

#define REC_BATTERYSTATUS   0
#define REC_VELOCITY        1
#define REC_HEARTBEAT       2
//#define RAC_BATTERYSTATUS   3
//#define REC_HEARTBEAT       4
//#define RAC_BATTERYSTATUS   5
//#define RAC_BATTERYSTATUS   6
//#define RAC_BATTERYSTATUS   7
#define SEND_HEARTBEAT      8
#define SEND_BRAKE          9
#define SEND_BACKLIGHT      10
#define SEND_V_BLINK        11
#define SEND_H_BLINK        12
#define SEND_DIRECTION      13
#define SEND_SAFETYPIN      14
#define SEND_ACCELERATOR    15


int MAP_HEARTBEAT, MAP_BRAKE, MAP_BACKLIGHT, MAP_V_BLINK,
        MAP_H_BLINK, MAP_DIRECTION, MAP_SAFETYPIN, MAP_ACCELERATOR;



#endif	/* I2C_H */

