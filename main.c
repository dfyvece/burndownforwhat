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

typedef enum {

	HAS_FIRE,
	NORMAL
} State;


State state = NORMAL;
int32_t status = 0;
MyRio_Uart uart;


// INPUT/OUTPUT

int readSensor() {
	return 0;
}

int triggerAlarm() {

}



// NETWORK

int sendAlarm() {

	while (state == HAS_FIRE) {
		sendCommand(&uart, "ATNH 1");
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

    //char writeData[BUFF_SIZE];


    state = NORMAL;
    status = initUART(&uart);

    pthread_t poll_thread;
    pthread_create(&poll_thread, NULL, pollNeighbors, NULL);

    while (status >= VI_SUCCESS) {

    	if ( readSensor() ) {
    		if (state == NORMAL) {
				state = HAS_FIRE;
				pthread_t send_thread;
				pthread_create(&send_thread, NULL, sendAlarm, NULL);
    		}
    	}
    	else {
    		state = NORMAL;
    	}

    	recvPayload(&uart, readData);
    	if (strlen(readData) && readData[0] == '1') {		// check for fire message
    		triggerAlarm();
    	}

    }

    status = closeUART(&uart);
    return status;
}





/*
NiFpga_Status stat;
NiFpga_Bool dio_A0;
NiFpga_Bool dio_A7;

MyRio_Dio A0;
MyRio_Dio A7;

A0.dir = NiFpga_MyRio1950Fpga20_ControlU8_DIOA_70DIR;
A0.out = NiFpga_MyRio1950Fpga20_ControlU8_DIOA_70OUT;
A0.in = NiFpga_MyRio1950Fpga20_IndicatorU8_DIOA_70IN;
A0.bit = 0;

A7.dir = NiFpga_MyRio1950Fpga20_ControlU8_DIOA_70DIR;
A7.out = NiFpga_MyRio1950Fpga20_ControlU8_DIOA_70OUT;
A7.in = NiFpga_MyRio1950Fpga20_IndicatorU8_DIOA_70IN;
A7.bit = 7;

status = MyRio_Open();
  if (MyRio_IsNotSuccess(status))
  {
      return status;
  }

  dio_A0 = Dio_ReadBit(&A0);


  dio_A7 = 1;
  Dio_WriteBit(&A7, dio_A7);

  status = MyRio_Close();
*/


