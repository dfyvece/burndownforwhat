/*
 * IO.c
 *
 *  Created on: Apr 7, 2015
 *      Author: kvece
 */
#include "IO.h"

MyRio_Dio alarm_p;
MyRio_Dio sensor_p;


void initPin(MyRio_Dio* pin, int num) {
	pin->dir = NiFpga_MyRio1900Fpga20_ControlU8_DIOA_70DIR;
	pin->out = NiFpga_MyRio1900Fpga20_ControlU8_DIOA_70OUT;
	pin->in = NiFpga_MyRio1900Fpga20_IndicatorU8_DIOA_70IN;
	pin->bit = num;
}

int readSensor() {
	NiFpga_Bool dio = Dio_ReadBit(&sensor_p);
	return dio;
}

void alarmOn() {
	NiFpga_Bool dio = 1;
	Dio_WriteBit(&alarm_p, dio);
}

void alarmOff() {
	NiFpga_Bool dio = 0;
	Dio_WriteBit(&alarm_p, dio);
}

void initIO() {
	initPin(&alarm_p, ALARM_PIN);
	initPin(&sensor_p, SENSOR_PIN);
	MyRio_Open();
}

void closeIO() {
    MyRio_Close();
}
