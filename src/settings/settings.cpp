#include "settings.h"
#include <EEPROM.h>

#define SETTINGS_KEY_ADDRESS 0
#define SETTINGS_KEY 7
#define SETTINGS_VALUE_ADDRESS 10

static bool is_need_to_set_default_values(void) {
    uint8_t key = 0;
    EEPROM.get(SETTINGS_KEY_ADDRESS, key);
    return key != SETTINGS_KEY;
}

/*
 *
 *
 * */
settings_t* settings_new(void) {
    settings_t* p_settings = (settings_t*)malloc(sizeof(settings_t));
    return p_settings;
}

void settings_begin(settings_t* p_settings){
	EEPROM.begin(64);
}

void settings_load(settings_t* p_settings) {
	if (is_need_to_set_default_values()) {
        p_settings->ups = true;
        p_settings->grid_return_delay = 30;
        p_settings->start_inverter_delay = 10;
        p_settings->alarm_batt_soc = 7;
        p_settings->critical_batt_soc = 5;
        p_settings->start_charging_soc = 96;
        p_settings->stop_charging_soc = 100;
        p_settings->sleep_timeout = 3600;
		
        EEPROM.put(SETTINGS_VALUE_ADDRESS, *(p_settings));
        EEPROM.put(SETTINGS_KEY_ADDRESS, SETTINGS_KEY);
		EEPROM.commit();
    } else {
        EEPROM.get(SETTINGS_VALUE_ADDRESS, *(p_settings));
    }
}

void settings_delete(settings_t* p_settings) {
    free(p_settings);
}

void settings_save(settings_t* p_settings) {
    EEPROM.put(SETTINGS_VALUE_ADDRESS, *(p_settings));
	EEPROM.commit();
}