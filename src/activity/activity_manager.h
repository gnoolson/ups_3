#ifndef ACTIVITY_MANAGER_H
#define ACTIVITY_MANAGER_H

#include <Arduino.h>
#include <gnl_timer.h>
#include <gnl_sob.h>
#include "activity.h"
#include "../context/context.h"
#include "../../config.h"

typedef struct activity_manager {
	gnl_timer_t* p_timer;
	activity_t* p_activity;
	context_t* p_context;
} activity_manager_t;

/*
 * 
 * 
 * */
activity_manager_t* actmgr_new_and_setup(context_t* p_context);
void actmgr_delete(activity_manager_t* p_actmgr);
void actmgr_begin(activity_manager_t* p_actmgr);
void actmgr_update(activity_manager_t* p_actmgr);
void actmgr_process_buttons(activity_manager_t* p_actmgr, gnl_button_info_t** p_infos, uint8_t length);

#endif
