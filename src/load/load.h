#ifndef LOAD_H
#define LOAD_H

#include "../sensor/digital_sensor.h"
#include "../settings/settings.h"
#include <gnl_timer.h>

typedef struct {
	settings_t* p_settings;
	digital_sensor_t* p_ds;
	gnl_timer_t* p_timer;
	bool flag;
	uint16_t inactivity_time;
} load_t;

/*
 * 
 * 
 * */
load_t* load_new_and_setup(settings_t* p_settings, uint8_t sensor_pin);
void load_delete(load_t* p_load);
void load_begin(load_t* p_load);
void load_update(load_t* p_load);
bool load_exists(load_t* p_load);
bool load_is_existing_right_now(load_t* p_load);
void load_control(load_t* p_load, bool flag);

#endif