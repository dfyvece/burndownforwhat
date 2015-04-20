/*
 * xbee.h
 *
 *  Created on: Mar 26, 2015
 *      Author: kvece
 */

#ifndef XBEE_H_
#define XBEE_H_

#include "UART.h"


// UART SETTINGS
#define BaudRate 9600
#define DataBit 8


// MAXIMUM COMMAND SIZE
#define BUFF_SIZE 256


// VERBOSE MODE
#define VERBOSE 1
#define DEBUG(MSG) if (VERBOSE) { printf(MSG); printf("\n"); fflush(stdout); }
#define DEBUG_PARAM(TAG,PARAM) if (VERBOSE) { printf(TAG); printf(": '%s\n", PARAM); fflush(stdout); }


// UART Functions
int32_t initUART(MyRio_Uart* uart);
int32_t closeUART(MyRio_Uart* uart);


// Use with XBee
uint8_t sendPayload(MyRio_Uart* uart, char* str);
uint8_t recvPayload(MyRio_Uart* uart, char* readData);
char* sendCommand(MyRio_Uart* uart, char* comm);

// SUPPORTED COMMANDS (make sure to add to xbee.c formatCommand function)
#define DL 1
#define DH 2
#define NH 3


// Support functions
int32_t enterConfig(MyRio_Uart* uart);
int32_t exitConfig(MyRio_Uart* uart);


#endif /* XBEE_H_ */
