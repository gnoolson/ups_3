#ifndef INVERTER_H
#define INVERTER_H

#include "../driver/driver.h"
#include "../sensor/sensor_220v.h"
#include "../settings/settings.h"

typedef struct {
    settings_t* p_settings;
    sensor_220v_t* p_sensor_220v;
    driver_t* p_driver;
    bool (*recommendation_to_be_prepared)(void*);
    void* recommendation_to_be_prepared_arg;
	bool is_running;
} inverter_t;

/*
 *
 *
 * */
inverter_t* inverter_new_and_setup(settings_t* p_settings,
                                   sensor_220v_t* p_sensor_220v,
                                   uint8_t driver_pin,
                                   bool (*recommendation_to_be_prepared)(void*),
                                   void* recommendation_to_be_prepared_arg);
void inverter_delete(inverter_t* p_inverter);
void inverter_begin(inverter_t* p_inverter);
void inverter_update(inverter_t* p_inverter);
void inverter_turn_on(inverter_t* p_inverter);
void inverter_turn_off(inverter_t* p_inverter);
bool inverter_is_on(inverter_t* p_inverter);
bool inverter_is_ok(inverter_t* p_inverter);

#endif