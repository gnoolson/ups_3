#include "charger.h"

charger_t* charger_new_and_setup(uint8_t load_sensor_pin, uint8_t power_on_driver_pin) {
    charger_t* p_charger = (charger_t*)malloc(sizeof(charger_t));
    p_charger->p_driver = driver_new_and_setup(power_on_driver_pin);
    p_charger->p_load_sensor = ds_new_and_setup(load_sensor_pin);
    p_charger->p_start_timer = gnl_timer_new_and_setup(5000, false);               // start charger after 5sec
    p_charger->p_ignore_load_sensor_timer = gnl_timer_new_and_setup(5000, false);  // ignore load sensor in 5sec
    p_charger->is_running = false;
    p_charger->is_load_exist = false;

    return p_charger;
}

void charger_delete(charger_t* p_charger) {
    driver_delete(p_charger->p_driver);
    ds_delete(p_charger->p_load_sensor);
    gnl_timer_delete(p_charger->p_start_timer);
    gnl_timer_delete(p_charger->p_ignore_load_sensor_timer);
    free(p_charger);
}

void charger_begin(charger_t* p_charger) {
    driver_begin(p_charger->p_driver);
    ds_begin(p_charger->p_load_sensor);
}

void charger_update(charger_t* p_charger) {
    ds_update(p_charger->p_load_sensor);

    if (gnl_timer_check(p_charger->p_start_timer)) {
        p_charger->is_running = true;
        p_charger->is_load_exist = true;
        gnl_timer_start(p_charger->p_ignore_load_sensor_timer);
    }

    gnl_timer_check(p_charger->p_ignore_load_sensor_timer);

    if (!p_charger->p_ignore_load_sensor_timer->active)
        p_charger->is_load_exist = ds_get_state(p_charger->p_load_sensor);

    if (p_charger->is_running)
        driver_turn_on(p_charger->p_driver);
    else
        driver_turn_off(p_charger->p_driver);
}

void charger_turn_on(charger_t* p_charger) {
    if (p_charger->p_start_timer->active || p_charger->is_running)
        return;

    gnl_timer_start(p_charger->p_start_timer);
}

void charger_turn_off(charger_t* p_charger) {
    gnl_timer_stop(p_charger->p_start_timer);
    gnl_timer_stop(p_charger->p_ignore_load_sensor_timer);
    p_charger->is_running = false;
    p_charger->is_load_exist = false;
}

bool charger_is_on(charger_t* p_charger) {
    return p_charger->is_running;
}

bool charger_is_ok(charger_t* p_charger) {
    return p_charger->is_running && p_charger->is_load_exist;
}

bool charger_is_preparing_to_turn_on(charger_t* p_charger) {
    return p_charger->p_start_timer->active;
}

bool charger_load_exists(charger_t* p_charger) {
    return p_charger->is_load_exist;
}
