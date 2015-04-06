/*
 * Copyright (c) 2013,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "xbee.h"
#include "DIO.h"
#include "MyRio.h"



/*
FILE* f;

f = fopen("test.txt", "w");

fprintf(f,"hello");

fclose(f);

*/

MyRio_Dio A5;
MyRio_Dio A7;
NiFpga_Bool dio_A5;
NiFpga_Bool dio_A7;


typedef enum {

	HAS_FIRE,
	NORMAL
} State;


State state = NORMAL;
int32_t status = 0;
MyRio_Uart uart;


// INPUT/OUTPUT

int readSensor() {
	dio_A5 = Dio_ReadBit(&A5);
	return dio_A5;

}

void alarmOn() {
	dio_A7 = 1;
	Dio_WriteBit(&A7, dio_A7);
}

void alarmOff() {
	dio_A7 = 0;
	Dio_WriteBit(&A7, dio_A7);
}


// NETWORK

int sendAlarm() {

	while (state == HAS_FIRE) {
		//sendCommand(&uart, "ATNH 1");
		sendPayload(&uart, "10");
		sleep(10);
	}

}

#define MAX_NEIGH 10
char neighbors[MAX_NEIGH][8];
char neighbor_status[MAX_NEIGH];
int num_neigh;

int pollNeighbors() {

    char readData[BUFF_SIZE];

    num_neigh = 0;
    int timeouts = 0;
    const int MAX_TIMEOUTS = 5;
    while (timeouts < MAX_TIMEOUTS) {

    	status = sendCommand(&uart, "ATNH 1");
		status = sendPayload(&uart, "0");

		recvPayload(&uart, readData);   	// TODO: test for more than three
		if (readData[0] == '0') {							// first character must be 0
			timeouts = 0;
			if (num_neigh < MAX_NEIGH) {
				neighbor_status[num_neigh] = readData[1];	// second character is status
				strcpy(neighbors[num_neigh], readData+2);	// rest is identifier
				++num_neigh;
			}
			else break;
		}
		else
			timeouts++;

    }

}




int main(int argc, char **argv) {



    char readData[BUFF_SIZE];


    /****************************************************/
    /************ Setup Digital Input/Output ************/
    NiFpga_Status stat;

	A5.dir = NiFpga_MyRio1900Fpga20_ControlU8_DIOA_70DIR;
	A5.out = NiFpga_MyRio1900Fpga20_ControlU8_DIOA_70OUT;
	A5.in = NiFpga_MyRio1900Fpga20_IndicatorU8_DIOA_70IN;
	A5.bit = 5;

	A7.dir = NiFpga_MyRio1900Fpga20_ControlU8_DIOA_70DIR;
	A7.out = NiFpga_MyRio1900Fpga20_ControlU8_DIOA_70OUT;
	A7.in = NiFpga_MyRio1900Fpga20_IndicatorU8_DIOA_70IN;
	A7.bit = 7;

	MyRio_Open();

	/****************************************************/

    //char writeData[BUFF_SIZE];




    state = NORMAL;
    status = initUART(&uart);

    //pthread_t poll_thread;
    //pthread_create(&poll_thread, NULL, pollNeighbors, NULL);

    while (status >= VI_SUCCESS) {

    	if ( !readSensor() ) {								// alarm is on
    		if (state == NORMAL) {
				state = HAS_FIRE;
				pthread_t send_thread;
				pthread_create(&send_thread, NULL, sendAlarm, NULL);
				alarmOn();
    		}
    	}
    	//else {
    		//state = NORMAL;
    	//}


    	recvPayload(&uart, readData);
    	if (strlen(readData)) printf("Received: '%s'\n", readData);
    	if (strlen(readData) && readData[0] == '1') {		// check for fire message
    		printf("Turning on alarm\n");
    		alarmOn();
    	}

    }

    status = closeUART(&uart);


    MyRio_Close();
    return status;
}





