/*
 * network.c
 *
 *  Created on: Apr 7, 2015
 *      Author: kvece
 */


#include "network.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "xbee.h"
#include "IO.h"
#include "state.h"


extern State state;
extern int32_t status;
extern MyRio_Uart uart;



void sendAlarm() {

	while (state == HAS_FIRE) {
		//sendCommand(&uart, "ATNH 1");
		sendPayload(&uart, "10");
		sleep(10);
	}

}







void pollNeighbors() {
	char* sl = sendCommand(&uart, "ATSL");
	char* sh = sendCommand(&uart, "ATSH");

	for(;;) {
		char readData[BUFF_SIZE];
		char writeData[BUFF_SIZE];

		num_neigh = 0;
		int timeouts = 0;
		pthread_mutex_lock(&lock);
		status = sendCommand(&uart, "ATNH 1");
		writeData = "0 ";			// 0 is request for information
		strcat(writeData,sh);
		strcat(writeData, " ");
		strcat(writeData, sl);
		status = sendPayload(&uart, writeData);
		pthread_mutex_unlock(&lock);

		sleep(POLL_TIME);
	}

}
