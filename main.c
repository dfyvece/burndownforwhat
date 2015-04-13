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
#include "network.h"
#include "IO.h"
#include "state.h"


State state;
int32_t status;
MyRio_Uart uart;

char neighbors[MAX_NEIGH][8];
char neighbor_status[MAX_NEIGH];
int num_neigh;


int main(int argc, char **argv) {

	// declare variables
    char readData[BUFF_SIZE];		// returned data from xbee

    // initialize components
    state = NORMAL;
    status = 0;
    initIO();
    pthread_mutex_init(&lock, NULL);
    status = initUART(&uart);


	char* sl = sendCommand(&uart, "ATSL");
	char* sh = sendCommand(&uart, "ATSH");
    // TODO test this


    // poll and update neighbors data
    pthread_t poll_thread;
    pthread_create(&poll_thread, NULL, (void*)pollNeighbors, NULL);
     for (;;);
    /*
    // state machine
    while (status >= VI_SUCCESS) {


    	//sendPayload(&uart, "Four score and seven years ago our fathers brought forth on this continent, a new nation, conceived in Liberty, and dedicated to the proposition that all men are created equal.");


    	// check for fire
    	if ( !readSensor() ) {								// alarm is on
    		if (state == NORMAL) {
				state = HAS_FIRE;
				pthread_t send_thread;
				pthread_create(&send_thread, NULL, (void*)sendAlarm, NULL);
				alarmOn();
    		}
    	}

    	// check for incoming messages
    	pthread_mutex_lock(&lock);
    	recvPayload(&uart, readData);
    	if (strlen(readData)) DEBUG_PARAM("XBEE", readData);
    	if (strlen(readData) && readData[0] == '1') {		// check for fire message
    		DEBUG("Turning on alarm\n");
    		alarmOn();
    	}
    	else if (strlen(readData) && readData[0] == '0') {  // check for poll request

			char response[BUFF_SIZE];
			strcpy(response, "2 ");
			strcat(response, sh);
			strcat(response, " ");
			strcat(response, sl);
			sendPayload(&uart, response);

    	}
    	else if (strlen(readData) && readData[0] == '2') {  // check for poll response

			// TODO
			if (num_neigh < MAX_NEIGH) {
				strcpy(neighbors[num_neigh], readData+2);	// rest is identifier
				++num_neigh;
			}
    	}

    	else if (strlen(readData) && readData[0] == '3') {  // check for supernode request

			char sn_dh[8], sn_dl[8];

			// TODO get sn info

			char command[BUFF_SIZE];
			sprintf(command, "ATDH %s", sn_dh);
			sendCommand(&uart, command);
			sprintf(command, "ATDL %s", sn_dl);
			sendCommand(&uart, command);

			sendCommand(&uart, "ATNH ff");
			//sendCommand(&uart, "ATRO f");		// TODO test size

			char packet[100];
			sprintf(packet, "%s%s\n%d\n". sh, sl, status);		// SH SH
			for (i = 0 ; i < num_neigh; ++i ) {					// Status
				strcat(packet, neighbors[i]);					// Neighbor[0]
				strcat(packet, "\n");							// Neighbor[1]
			}													// ...
			strcat(packet, "\n");								// Neighbor[n]
																//
			sendPayload(&uart, packet);

			//sendCommand(&uart, "ATRO 8");		// TODO test size
    	}






    	pthread_mutex_unlock(&lock);



    }
     */
    // shutdown on error
    closeIO();
    status = closeUART(&uart);
    pthread_mutex_destroy(&lock);

    return status;
}





