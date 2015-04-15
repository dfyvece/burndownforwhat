/*
 * network.h
 *
 *  Created on: Apr 7, 2015
 *      Author: kvece
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <unistd.h>
#include <pthread.h>

// time in seconds inbetween polls
#define POLL_TIME 10000

// max number of neighbors to keep track of
#define MAX_NEIGH 10

// max number of timeouts before giving up on neighbors
#define MAX_TIMEOUTS 5

pthread_mutex_t lock;


// send alarm to all neighbors
void sendAlarm();

// search for neighbors and get their status
void pollNeighbors();



#endif /* NETWORK_H_ */
