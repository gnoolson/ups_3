#ifndef POWER_SOURCE_SWITCH_H
#define POWER_SOURCE_SWITCH_H

#include <Arduino.h>
#include "../driver/driver.h"
#include "../settings/settings.h"
#include "../buzzer/buzzer.h"
#include <gnl_timer.h>

typedef struct {
    settings_t* p_settings;
    driver_t* p_driver;
	gnl_timer_t* p_select_grid_timer;
	gnl_timer_t* p_select_inverter_timer;
	buzzer_t* p_buzzer;
} power_source_switch_t;

/*
 *
 *
 * */
power_source_switch_t* pss_new_and_setup(settings_t* p_settings, buzzer_t* p_buzzer, uint8_t driver_pin);
void pss_delete(power_source_switch_t* p_pss);
void pss_update(power_source_switch_t* p_pss);
void pss_begin(power_source_switch_t* p_pss);

void pss_select_inverter_with_delay(power_source_switch_t* p_pss);
void pss_select_grid_with_delay(power_source_switch_t* p_pss);
void pss_select_grid_immediately(power_source_switch_t* p_pss);

bool pss_is_grid_selected(power_source_switch_t* p_pss);
bool pss_is_inverter_selected(power_source_switch_t* p_pss);

void pss_stop_select_grid(power_source_switch_t* p_pss);
void pss_stop_select_inverter(power_source_switch_t* p_pss);

#endif
