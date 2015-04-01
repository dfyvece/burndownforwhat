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



/*
FILE* f;

f = fopen("test.txt", "w");

fprintf(f,"hello");

fclose(f);

*/


int main(int argc, char **argv) {

    int32_t status = 0;
    MyRio_Uart uart;

    //char writeData[BUFF_SIZE];
    char readData[BUFF_SIZE];

    status = initUART(&uart);

    while (status >= VI_SUCCESS) {
    	printf("Sending\n");
    	status = sendCommand(&uart, "ATNH ff");
    	status = sendPayload(&uart, "discover_neighbors");
    	status = recvPayload(&uart, readData);
    	printf("Received: '%s'\n", readData);
    }

    status = closeUART(&uart);
    return status;
}


