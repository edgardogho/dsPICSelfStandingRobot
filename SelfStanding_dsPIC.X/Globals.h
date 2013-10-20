/* 
 * File:   Globals.h
 * Author: edgardogho
 *
 * Created on October 5, 2013, 1:42 PM
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#ifdef	__cplusplus
extern "C" {
#endif


#include <libpic30.h>
#include <xc.h>


    // Defines
#define OUTPUT 0
#define INPUT  1

typedef unsigned char UCHAR;
typedef char CHAR;
typedef unsigned int  UINT;

extern UCHAR UART_flagStart;
extern UCHAR UART_Sending;

extern UCHAR I2C_State;
extern UCHAR I2C_flagGyro;
extern UCHAR I2C_RX;
extern UCHAR I2C_Error;

extern UCHAR GYRO_State;
extern UCHAR GYRO_Init;

extern int   GYRO_X,GYRO_Y,GYRO_Z;

extern CHAR MessageBuffer[100];
extern UCHAR pBuffer;

extern UINT TIMER_counter;


#ifdef	__cplusplus
}
#endif

#endif	/* GLOBALS_H */

