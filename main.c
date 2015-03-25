/*
 * Copyright (c) 2013,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "UART.h"

#define BaudRate 9600
#define DataBit 8

MyRio_Uart uart;
uint8_t writeData[256];
uint8_t readData[256];

#define DL 1
#define NH 3
#define DH 2

uint8_t sendPayload(uint8_t* str) {
	int32_t status = 0;
	strcpy(readData, "");
	return  Uart_Write(&uart, &str, strlen(str));
}

uint8_t* recvPayload() {
	int32_t status = 0;
	strcpy(readData,"");
	status = Uart_Read(&uart, &readData, 1);
	return status;
}

uint8_t sendCommand(uint8_t type, uint16_t param) {

    int32_t status = 0;
    strcpy(readData,"");

    printf("ENTER COMMAND MODE\n");
    strcpy(writeData,"+++\n");
    status = Uart_Write(&uart, &writeData, strlen(writeData));
    sleep(1);	// necessary delay as per documentation

    printf("CHECK RESPONSE\n");
    status = Uart_Read(&uart, &readData, 1);
    if (readData[0] == 'O') { // OK

    	// FORMAT COMMAND
    	switch (type) {
		case DL:
			sprintf(writeData, "ATDL%x\n", param);
			break;
		case DH:
			sprintf(writeData, "ATDH%x\n", param);
			break;
		case NH:
			sprintf(writeData, "ATNH%x\n", param);
			break;
		}

		printf("Sending command: '%s'\n", writeData);
		status = Uart_Write(&uart, &writeData, strlen(writeData));
		printf("Status: %d\n", status);

		printf("Reading UART\n");
		status = Uart_Read(&uart, &readData, 1);
		printf("The received data is %s\n", readData);


    }

    printf("EXITING COMMAND MODE\n");
	sprintf(writeData, "ATCN");
	status = Uart_Write(&uart, &writeData, strlen(writeData));

    return status;



}


int main(int argc, char **argv)
{
    int32_t status = 0;

    /*
     * Initialize the UART port structure.
     */
    uart.name = "ASRL1::INSTR";
    uart.defaultRM = 0;
    uart.session = 0;

    /*
     * Opens a UART session.
     */
    printf("Opening UART\n");
    status = Uart_Open(&uart, BaudRate, DataBit,
                       Uart_StopBits1_0, Uart_ParityNone);
    if (status < VI_SUCCESS)
    {
        return status;
    }
    printf("Status: %d\n", status);


    while (1) {
    	sendCommand(DL, 3333);
    	sendPayload("hello");
    }



    /*
     * Clears UART receive buffer.
     */
    status = Uart_Clear(&uart);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    printf("Cleared buffer\n");

    /*
     * Closes the UART port.
     */
    status = Uart_Close(&uart);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    printf("Done\n");

    return status;
}
