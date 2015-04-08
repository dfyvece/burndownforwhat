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


int main(int argc, char **argv) {

	// declare variables
    char readData[BUFF_SIZE];		// returned data from xbee

    // initialize components
    state = NORMAL;
    status = 0;
    initIO();
    pthread_mutex_init(&lock, NULL);
    status = initUART(&uart);


    // poll and update neighbors data
    pthread_t poll_thread;
    pthread_create(&poll_thread, NULL, (void*)pollNeighbors, NULL);

    // state machine
    while (status >= VI_SUCCESS) {


    	//sendPayload(&uart, "Four score and seven years ago our fathers brought forth on this continent, a new nation, conceived in Liberty, and dedicated to the proposition that all men are created equal.");
    	sleep(10);
    	/*
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
    	pthread_mutex_unlock(&lock);
    	*/

    }

    // shutdown on error
    closeIO();
    status = closeUART(&uart);
    pthread_mutex_destroy(&lock);

    return status;
}





