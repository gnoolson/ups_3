#ifndef CHARGER_H
#define CHARGER_H

#include "../driver/driver.h"
#include "../sensor/digital_sensor.h"
#include <gnl_timer.h>

typedef struct {
	driver_t* p_driver;
	gnl_timer_t* p_start_timer;
	gnl_timer_t* p_ignore_load_sensor_timer;
	digital_sensor_t* p_load_sensor;
	bool is_running;
	bool is_load_exist;
} charger_t;

/*
 * 
 * 
 * */
charger_t* charger_new_and_setup(uint8_t load_sensor_pin, uint8_t power_on_driver_pin);
void charger_delete(charger_t* p_charger);
void charger_begin(charger_t* p_charger);
void charger_update(charger_t* p_charger);
void charger_turn_on(charger_t* p_charger);
void charger_turn_off(charger_t* p_charger);
bool charger_is_on(charger_t* p_charger);
bool charger_is_ok(charger_t* p_charger);
bool charger_is_preparing_to_turn_on(charger_t* p_charger);
bool charger_load_exists(charger_t* p_charger);

#endif