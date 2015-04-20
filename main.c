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
#include <stdlib.h>
#include "network.h"

#include <sys/types.h>
#include <sys/stat.h>


State state;
int32_t status;
MyRio_Uart uart;

char neighbors[MAX_NEIGH][8];
char neighbor_status[MAX_NEIGH];
int num_neigh = 0;


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



	FILE* poll_file;
	char* poll_name = "bdfw_poll.dat";

	// INITIALIZING POLL FILE
	poll_file = fopen(poll_name, "w");
	fprintf(poll_file, "1 10");
	fclose(poll_file);


	poll_file = fopen(poll_name, "r");


	// read timestamp from poll file
	int timestamp;
	fscanf(poll_file, "%d", &timestamp);


    // poll and update neighbors data
    pthread_t poll_thread;
    //pthread_create(&poll_thread, NULL, (void*)pollNeighbors, NULL);

    // state machine
    while (status >= VI_SUCCESS) {

    	// check for fire
    	if ( !readSensor() ) {								// alarm is on
    		DEBUG("read sensor");
    		if (state == NORMAL) {
    			DEBUG("sensed alarm");
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

    		char sn_dh[8];
    		char sn_dl[8];

			char packet_type;
			sscanf(readData, "%c %s %s", &packet_type, sn_dh, sn_dl);

			// TODO: format for larger address range
			strcpy(sn_dh,"13A200");

    		DEBUG_PARAM("DH", sn_dh);
    		DEBUG_PARAM("DL", sn_dl);

    		char command[BUFF_SIZE];
			//sprintf(command, "ATDH %s", sn_dh);
			//sendCommand(&uart, command);
			sprintf(command, "ATDL %s", sn_dl);
			sendCommand(&uart, command);

			char response[BUFF_SIZE];
			sprintf(response, "2 %s %s", sh, sl);
			sendPayload(&uart, response);
			sendCommand(&uart, "ATDL ffff");

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

			char packet_type;

			sscanf(readData, "%c %s %s", &packet_type, sn_dh, sn_dl);
			// TODO: format for larger address range
			strcpy(sn_dh,"13A200");

			DEBUG_PARAM("DH", sn_dh);
			DEBUG_PARAM("DL", sn_dl);

			char command[BUFF_SIZE];
			//sprintf(command, "ATDH %s", sn_dh);
			//sendCommand(&uart, command);
			sprintf(command, "ATDL %s", sn_dl);
			sendCommand(&uart, command);

			sendCommand(&uart, "ATNH ff");
			//sendCommand(&uart, "ATRO f");		// TODO test size

			char packet[100];
			int i;
			sprintf(packet, "%s%s\n%d\n", sh, sl, status);		// SH SH
			for (i = 0 ; i < num_neigh; ++i ) {					// Status
				strcat(packet, neighbors[i]);					// Neighbor[0]
				strcat(packet, "\n");							// Neighbor[1]
			}													// ...
			strcat(packet, "\n");								// Neighbor[n]
																//
			sendPayload(&uart, packet);

			sendCommand(&uart, "ATDL ffff");
			//sendCommand(&uart, "ATRO 8");		// TODO test size
    	}

    	pthread_mutex_unlock(&lock);


    	// check poll file for data
    	struct stat st;
    	if (poll_file && stat(poll_name, &st) == 0) {
    		fseek(poll_file, 0, SEEK_SET);
    		int new_timestamp;
    		int length;
    		fscanf(poll_file, "%d%d", &new_timestamp, &length);
    		if (new_timestamp > timestamp) {
    			char m1[BUFF_SIZE], m2[BUFF_SIZE];
    			sprintf(m1, "%d", timestamp);
    			sprintf(m1, "%d", new_timestamp);
    			DEBUG_PARAM("Timestamp", m1);
    			DEBUG_PARAM("new Timestamp", m2);
    			pthread_mutex_lock(&lock);
    			DEBUG("SENDING SUPERNODE REQUEST");
				sendCommand(&uart, "ATDL ffff");
				char request[BUFF_SIZE];
				sprintf(request, "3 %s %s", sh, sl);
				sendPayload(&uart, request);
				superNode(length);					// go into supernode mode
				pthread_mutex_unlock(&lock);
				timestamp = new_timestamp;
    		}
    	}
    	else {
    		DEBUG("WARNING: no poll file, creating one");
    		poll_file = fopen(poll_name, "w");
			fprintf(poll_file, "1");
			fclose(poll_file);
			poll_file = fopen(poll_name, "r");
    	}




    }

    // shutdown on error
    closeIO();
    status = closeUART(&uart);
    pthread_mutex_destroy(&lock);

   // fclose(poll_file);


    return status;
}





