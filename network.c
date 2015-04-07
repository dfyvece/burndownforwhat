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





char neighbors[MAX_NEIGH][8];
char neighbor_status[MAX_NEIGH];
int num_neigh;

void pollNeighbors() {

	for(;;) {
		char readData[BUFF_SIZE];

		num_neigh = 0;
		int timeouts = 0;
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
		sleep(POLL_TIME);
	}

}
