#ifndef START_ACTIVITY_H
#define START_ACTIVITY_H

#include "../activity.h"
#include <gnl_timer.h>

typedef struct {
	activity_t base;
	gnl_timer_t* p_timer;
} start_activity_t;

/*
 * 
 * 
 * */
start_activity_t* sta_new_and_setup(context_t* p_context);
void sta_delete(start_activity_t* p_sa);



#endif