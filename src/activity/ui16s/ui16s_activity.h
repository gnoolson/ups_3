#ifndef UI16S_ACTIVITY_H
#define UI16S_ACTIVITY_H

#include "../activity.h"
#include <gnl_timer.h>

typedef enum {
	UI16S_GRID_RETURN_DELAY,
	UI16S_START_INVERTER_DELAY,
	UI16S_ALARM_BATTERY_SOC,
	UI16S_CRITICAL_BATTERY_SOC,
	UI16S_START_CHARGING_SOC,
	UI16S_STOP_CHARGING_SOC,
	UI16S_SLEEP_TIMEOUT
} ui16s_type_t;

typedef struct {
	activity_t base;
	int16_t value;
	bool saved;
	ui16s_type_t type;
	gnl_timer_t* p_fast_change_value_timer;
	gnl_timer_t* p_save_value_timer;
} ui16s_activity_t;

/*
 * 
 * 
 * */
ui16s_activity_t* ui16sa_new_and_setup(context_t* p_context, ui16s_type_t type);
void ui16sa_delete(ui16s_activity_t* p_ui16s);

#endif