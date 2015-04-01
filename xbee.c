
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include "xbee.h"

uint8_t sendPayload(MyRio_Uart* uart, char* s) {
	int len = strlen(s);
	char* str = (char*) calloc(len+2, sizeof(char));
	strcpy(str,s);
	str[len] = '\n';
	str[len+1] = '\x00';
	uint8_t ret = Uart_Write(uart, (uint8_t*)str, len);
	free(str);
	return ret;
}

uint8_t recvPayload(MyRio_Uart* uart, char* readData) {
    memset(readData,0, BUFF_SIZE);
	return Uart_Read(uart, (uint8_t*)readData, BUFF_SIZE);
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

uint8_t sendCommand(MyRio_Uart* uart, char* comm) {

    int32_t status = 0;
    char readData[BUFF_SIZE];
    char writeData[BUFF_SIZE];

    enterConfig(uart);

    DEBUG("CHECK RESPONSE");
    status |= recvPayload(uart, readData);
    DEBUG_PARAM("Response", readData);
    if (strncmp(readData,"OK", 2) == 0) {

		DEBUG_PARAM("Sending command", comm);
		status |= sendPayload(uart, comm);

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
