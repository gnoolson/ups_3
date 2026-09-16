#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed)) {
	bool ups;
    uint16_t grid_return_delay; // s - час після якого можна повертатись на мережу (10, 20, 30, ... 300)
    uint16_t start_inverter_delay;  // s - час після якого вмикати інвертер (0, 10, 20, 30, ... 300) 
	uint16_t alarm_batt_soc; // % < рівень SoC батареї нижче якого буде робити зумер 1 раз в 2 хвилини
    uint16_t critical_batt_soc; // % >= мінімальний SoC батареї при якій ще може працювати інвертер
    uint16_t start_charging_soc; // % - рівень SoC при якому можна почати заряджати батарею
    uint16_t stop_charging_soc; // % - рівень SoC при якому можно завершити заряджання батареї
    uint16_t sleep_timeout; //s - час після якого вважається, що навантаження немає (якщо воно було раніше)
} settings_t;

/*
 * 
 * 
 * */
settings_t* settings_new(void);
void settings_begin(settings_t* p_settings);
void settings_delete(settings_t* p_settings);
void settings_load(settings_t* p_settings);
void settings_save(settings_t* p_settings);

#ifdef __cplusplus
}
#endif

#endif