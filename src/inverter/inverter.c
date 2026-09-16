#include "inverter.h"
#include "../serial/serial.h"

static bool is_hot(inverter_t* p_inverter) {
    return p_inverter->p_settings->start_inverter_delay == 0;
}

inverter_t* inverter_new_and_setup(settings_t* p_settings,
                                   sensor_220v_t* p_sensor_220v,
                                   uint8_t driver_pin,
                                   bool (*recommendation_to_be_prepared)(void*),
                                   void* recommendation_to_be_prepared_arg) {
									   
    inverter_t* p_inverter = (inverter_t*)malloc(sizeof(inverter_t));
    p_inverter->p_sensor_220v = p_sensor_220v;
    p_inverter->p_settings = p_settings;
    p_inverter->recommendation_to_be_prepared = recommendation_to_be_prepared;
    p_inverter->recommendation_to_be_prepared_arg = recommendation_to_be_prepared_arg;
    p_inverter->is_running = false;
    p_inverter->p_driver = driver_new_and_setup(driver_pin);
	
    return p_inverter;
}

void inverter_delete(inverter_t* p_inverter) {
    driver_delete(p_inverter->p_driver);
    free(p_inverter);
}

void inverter_begin(inverter_t* p_inverter) {
    driver_begin(p_inverter->p_driver);
}

void inverter_update(inverter_t* p_inverter) {
    bool hot_work = false;
    if (is_hot(p_inverter) && p_inverter->recommendation_to_be_prepared(p_inverter->recommendation_to_be_prepared_arg))
        hot_work = true;

    if (hot_work || p_inverter->is_running)
        driver_turn_on(p_inverter->p_driver);
    else
        driver_turn_off(p_inverter->p_driver);
}

void inverter_turn_on(inverter_t* p_inverter) {
	if(p_inverter->is_running)
		return;
		
   p_inverter->is_running = true;
   serial_println_text("Inverter. Started");
}

void inverter_turn_off(inverter_t* p_inverter) {
	if(!p_inverter->is_running)
		return;
		
    p_inverter->is_running = false;
	serial_println_text("Inverter. Stopped");
}

bool inverter_is_on(inverter_t* p_inverter) {
    return p_inverter->is_running;
}

bool inverter_is_ok(inverter_t* p_inverter) {
    return sensor_220v_has_voltage(p_inverter->p_sensor_220v);
}