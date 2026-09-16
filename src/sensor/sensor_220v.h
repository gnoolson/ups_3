#ifndef SENSOR_220V_H
#define SENSOR_220V_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <gnl_timer.h>

typedef struct {
	gnl_timer_t* p_timer;
	gnl_timer_t* p_zero_counter_timer;
	uint8_t alert_pin;
	volatile uint8_t zero_counter;
	volatile bool start_timer;
	bool has_voltage;
} sensor_220v_t;

/*
 * 
 * 
 * */
sensor_220v_t* sensor_220v_new_and_setup(uint8_t alert_pin);
void sensor_220v_delete(sensor_220v_t* p_sensor);
void sensor_220v_begin(sensor_220v_t* p_sensor);
void sensor_220v_update(sensor_220v_t* p_sensor);
bool sensor_220v_has_voltage(sensor_220v_t* p_sensor);

#ifdef __cplusplus
}
#endif

#endif
