/*
 * Copyright (c) 2013,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "xbee.h"


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


