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



#define CHIP_ALIVE_DIODE    LATAbits.LATA7
#define HEARTBEAT_DIODE     LATAbits.LATC6
#define ERROR_DIODE         LATAbits.LATC7








#endif	/* INIT_H */

