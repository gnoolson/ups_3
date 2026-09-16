#ifndef DIGITAL_SENSOR_H
#define DIGITAL_SENSOR_H

#include <gnl_timer.h>

typedef struct digital_sensor {
	uint8_t pin;
	bool state;
	gnl_timer_t* p_timer;
	
} digital_sensor_t;

/*
 * 
 * 
 * */
digital_sensor_t* ds_new_and_setup(uint8_t pin);
void ds_delete(digital_sensor_t* p_ds);
void ds_begin(digital_sensor_t* p_ds);
void ds_update(digital_sensor_t* p_ds);
bool ds_get_state(digital_sensor_t* p_ds);

#endif