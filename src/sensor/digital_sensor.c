#include "digital_sensor.h"

digital_sensor_t* ds_new_and_setup(uint8_t pin) {
    digital_sensor_t* p_ds = (digital_sensor_t*)malloc(sizeof(digital_sensor_t));
	p_ds->pin = pin;
	p_ds->p_timer = gnl_timer_new_and_setup(100, true);
	p_ds->state = false;
	return p_ds;
}

void ds_delete(digital_sensor_t* p_ds) {
    gnl_timer_delete(p_ds->p_timer);
    free(p_ds);
}

void ds_begin(digital_sensor_t* p_ds) {
    pinMode(p_ds->pin, INPUT);
    gnl_timer_start(p_ds->p_timer);
}

void ds_update(digital_sensor_t* p_ds) {
    if (!gnl_timer_check(p_ds->p_timer))
        return;

    p_ds->state = digitalRead(p_ds->pin);
}

bool ds_get_state(digital_sensor_t* p_ds) {
    return p_ds->state;
}
