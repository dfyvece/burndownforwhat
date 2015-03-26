/*
 * Copyright (c) 2013,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "UART.h"

// UART SETTINGS
#define BaudRate 9600
#define DataBit 8

// MAXIMUM COMMAND SIZE
#define BUFF_SIZE 256

// SUPPORTED COMMANDS
#define DL 1
#define DH 2
#define NH 3

// VERBOSE MODE
#define VERBOSE 1
#define DEBUG(MSG) if (VERBOSE) { printf(MSG); printf("\n"); }
#define DEBUG_PARAM(TAG,PARAM) if (VERBOSE) { printf(TAG); printf(": %s\n", PARAM); }

/* ==================================================================== */
/* ========================== DECLARATIONS ============================ */
/* ==================================================================== */

// UART Functions
int32_t initUART(MyRio_Uart* uart);
int32_t closeUART(MyRio_Uart* uart);

// Use with XBee
uint8_t sendPayload(MyRio_Uart* uart, char* str);
uint8_t recvPayload(MyRio_Uart* uart, char* readData);
uint8_t sendCommand(MyRio_Uart* uart, uint8_t type /* Choose from supported Commands */, uint16_t param);

// Support functions
void formatCommand(char* buff, uint8_t type, uint16_t param);
int32_t enterConfig(MyRio_Uart* uart);
int32_t exitConfig(MyRio_Uart* uart);

/* ==================================================================== */
/* ============================= MAIN ================================= */
/* ==================================================================== */


int main(int argc, char **argv) {

    int32_t status = 0;
    MyRio_Uart uart;

    //char writeData[BUFF_SIZE];
    //char readData[BUFF_SIZE];

    status = initUART(&uart);

    while (status >= VI_SUCCESS) {
    	status = sendCommand(&uart, DL, 3333);
    	status = sendPayload(&uart, "hello");
    }

    status = closeUART(&uart);
    return status;
}



/* ==================================================================== */
/* ========================== DEFINITIONS ============================= */
/* ==================================================================== */


uint8_t sendPayload(MyRio_Uart* uart, char* str) {
	int len = strlen(str);
	str[len] = '\n';
	str[len+1] = '\x00';
	return  Uart_Write(uart, (uint8_t*)str, len+1);
}

uint8_t recvPayload(MyRio_Uart* uart, char* readData) {
	return Uart_Read(uart, (uint8_t*)readData, BUFF_SIZE);
}


void formatCommand(char* buff, uint8_t type, uint16_t param) {

#define COMMAND(TYPE) case TYPE: \
						sprintf((char*)buff, "ATTYPE%x\n", param); \
						break;
	switch (type) {
		COMMAND(DL);
		COMMAND(DH);
		COMMAND(NH);
	}

}

int32_t enterConfig(MyRio_Uart* uart) {
	int32_t status = 0;
	DEBUG("ENTER COMMAND MODE");
	status = sendPayload(uart, "+++");
	sleep(1);	// necessary delay as per documentation
	return status;
}

int32_t exitConfig(MyRio_Uart* uart) {
	int32_t status = 0;
	DEBUG("EXIT COMMAND MODE");
	status = sendPayload(uart, "ATCN");
	return status;
}

uint8_t sendCommand(MyRio_Uart* uart, uint8_t type, uint16_t param) {

    int32_t status = 0;
    char readData[256];
    char writeData[256];

    enterConfig(uart);

    DEBUG("CHECK RESPONSE");
    status |= recvPayload(uart, readData);
    if (strncmp(readData,"OK", 2) == 0) {

    	formatCommand(writeData, type, param);

		DEBUG_PARAM("Sending command", writeData);
		status |= sendPayload(uart, writeData);

		DEBUG("Reading UART");
		status |= recvPayload(uart, readData);
		DEBUG_PARAM("Data", readData);

    }

    exitConfig(uart);
    return status;

}

int32_t initUART(MyRio_Uart* uart) {
	uart->name = "ASRL1::INSTR";
	uart->defaultRM = 0;
	uart->session = 0;
	return Uart_Open(uart, BaudRate, DataBit, Uart_StopBits1_0, Uart_ParityNone);
}

int32_t closeUART(MyRio_Uart* uart) {
	int32_t status = 0;
	status = Uart_Clear(uart);
	if (status < VI_SUCCESS)
		return status;

	status = Uart_Close(uart);

	return status;
}
