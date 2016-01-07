/* 
 * File:   init.h
 * Author: E
 *
 * Created on den 9 december 2015, 13:53
 */

#ifndef INIT_H
#define	INIT_H


void InitDevice(void);

int MAP_HEARTBEAT;
int Interrupt_counter;
int heartBeatCounter;



#define CHIP_ALIVE_DIODE    LATCbits.LATC7
#define HEARTBEAT_DIODE     LATAbits.LATA7
#define ERROR_DIODE         LATCbits.LATC6



#endif	/* INIT_H */

