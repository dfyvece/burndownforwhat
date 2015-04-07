/*
 * IO.h
 *
 *  Created on: Apr 7, 2015
 *      Author: kvece
 */

#ifndef IO_H_
#define IO_H_

#include "DIO.h"
#include "MyRio.h"


#define ALARM_PIN 7
#define SENSOR_PIN 5


void initIO();
void closeIO();


int readSensor();
void alarmOn();
void alarmOff();


#endif /* IO_H_ */
