#include "power_source_switch.h"
#include "../serial/serial.h"

static void pss_select_inverter(power_source_switch_t* p_pss) {
    if (pss_is_inverter_selected(p_pss))
        return;

    buzzer_beep(p_pss->p_buzzer, 10, true);
    driver_turn_on(p_pss->p_driver);
    serial_println_text("PSS. Select Inverter");
}

static void pss_select_grid(power_source_switch_t* p_pss) {
    if (pss_is_grid_selected(p_pss))
        return;

    buzzer_beep(p_pss->p_buzzer, 10, false);
    driver_turn_off(p_pss->p_driver);
    serial_println_text("PSS. Select Grid");
}

/*
 * 
 * 
 * */
power_source_switch_t* pss_new_and_setup(settings_t* p_settings, buzzer_t* p_buzzer, uint8_t driver_pin) {
    power_source_switch_t* p_pss = (power_source_switch_t*)malloc(sizeof(power_source_switch_t));
    p_pss->p_settings = p_settings;
    p_pss->p_buzzer = p_buzzer;
    p_pss->p_driver = driver_new_and_setup(driver_pin);
    p_pss->p_select_grid_timer = gnl_timer_new_and_setup(0, false);
    p_pss->p_select_inverter_timer = gnl_timer_new_and_setup(0, false);
    return p_pss;
}

void pss_delete(power_source_switch_t* p_pss) {
    driver_delete(p_pss->p_driver);
    gnl_timer_delete(p_pss->p_select_grid_timer);
    gnl_timer_delete(p_pss->p_select_inverter_timer);
    free(p_pss);
}

void pss_begin(power_source_switch_t* p_pss) {
    driver_begin(p_pss->p_driver);
}

bool pss_is_inverter_selected(power_source_switch_t* p_pss) {
    return driver_is_on(p_pss->p_driver);
}

bool pss_is_grid_selected(power_source_switch_t* p_pss) {
    return !driver_is_on(p_pss->p_driver);
}

void pss_select_grid_with_delay(power_source_switch_t* p_pss) {
    if (pss_is_grid_selected(p_pss))
        return;

    if (p_pss->p_select_grid_timer->active)
        return;

    uint16_t delay = p_pss->p_settings->grid_return_delay * 1000;
    gnl_timer_setup(p_pss->p_select_grid_timer, delay, false);
    gnl_timer_start(p_pss->p_select_grid_timer);
}

void pss_select_inverter_with_delay(power_source_switch_t* p_pss) {
    if (pss_is_inverter_selected(p_pss))
        return;

    if (p_pss->p_select_inverter_timer->active)
        return;

    uint16_t delay = p_pss->p_settings->start_inverter_delay * 1000;
    gnl_timer_setup(p_pss->p_select_inverter_timer, delay, false);
    gnl_timer_start(p_pss->p_select_inverter_timer);
}

void pss_update(power_source_switch_t* p_pss) {
    if (gnl_timer_check(p_pss->p_select_grid_timer))
        pss_select_grid(p_pss);

    if (gnl_timer_check(p_pss->p_select_inverter_timer))
        pss_select_inverter(p_pss);
}

void pss_stop_select_grid(power_source_switch_t* p_pss) {
    if (p_pss->p_select_grid_timer->active)
        gnl_timer_stop(p_pss->p_select_grid_timer);
}

void pss_stop_select_inverter(power_source_switch_t* p_pss) {
    if (p_pss->p_select_inverter_timer->active)
        gnl_timer_stop(p_pss->p_select_inverter_timer);
}

void pss_select_grid_immediately(power_source_switch_t* p_pss){
	pss_stop_select_inverter(p_pss); // stop timer
	pss_stop_select_grid(p_pss); // stop timer
	pss_select_grid(p_pss);
}