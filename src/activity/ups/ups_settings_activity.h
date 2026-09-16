#ifndef UPS_SETTINGS_ACTIVITY_H
#define UPS_SETTINGS_ACTIVITY_H

#include "../activity.h"
#include <gnl_timer.h>

typedef struct {
	activity_t base;
	bool value;
	bool saved;
	gnl_timer_t* p_save_value_timer;
} ups_settings_activity_t;

/*
 * 
 * 
 * */
ups_settings_activity_t* upssa_new_and_setup(context_t* p_context);
void upssa_delete(ups_settings_activity_t* p_upssa);

#endif