#ifndef SETTING_ACTIVITY_H
#define SETTING_ACTIVITY_H

#include "../activity.h"


typedef struct {
	activity_t base;
	int8_t row_cursor; // 0 - 3
	uint8_t selected_row; // 0 - 9
	int8_t first_row_of_list; // 0 - 5
} settings_activity_t;

/*
 * 
 * 
 * */
settings_activity_t* sa_new_and_setup(context_t* p_context);
void sa_delete(settings_activity_t* p_sa);

#endif