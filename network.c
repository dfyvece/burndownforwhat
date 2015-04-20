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



void sendAlarm() {

	while (state == HAS_FIRE) {
		//sendCommand(&uart, "ATNH 1");
		sendPayload(&uart, "10");
		sleep(10);
	}

}







void pollNeighbors() {
	pthread_mutex_lock(&lock);
	char* sl = sendCommand(&uart, "ATSL");
	char* sh = sendCommand(&uart, "ATSH");
	pthread_mutex_unlock(&lock);


	for(;;) {
		char readData[BUFF_SIZE];
		char writeData[BUFF_SIZE];

		//num_neigh = 0;
		int timeouts = 0;
		pthread_mutex_lock(&lock);
		status = sendCommand(&uart, "ATNH 1");
		sprintf(writeData, "0 %s %s", sh, sl);			// 0 is request for information
		status = sendPayload(&uart, writeData);
		pthread_mutex_unlock(&lock);

		sleep(POLL_TIME);
	}

}



void superNode(int len) {

	FILE *result_file;
	char* result_name = "bdfw_result.dat";
	result_file = fopen(result_name, "wb");

	char readData[BUFF_SIZE];

	time_t begin, raw;
	time(&raw);

	struct tm* t_info;
	t_info = localtime(&raw);
	DEBUG("STARTING SUPERNODE");

	while (difftime(time(0), mktime(t_info)) < (double)len ) {

		recvPayload(&uart, readData);

		if (readData && result_file) {
			fprintf(result_file, "%s", readData);
		}
	}
	DEBUG("ENDING SUPERNODE");

    if (result_file) fclose(result_file);

}



