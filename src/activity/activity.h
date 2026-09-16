#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <gnl_sob.h>
#include "../context/context.h"
#include "activity_id.h"

typedef struct activity activity_t;

struct activity {
    context_t* p_context;
	activity_id_t next_activity;

    void (*update)(activity_t* p_activity);
    void (*heartbeat)(activity_t* p_activity);
    void (*destroy)(activity_t* p_activity);
    void (*process_buttons)(activity_t* p_activity, gnl_button_info_t** p_infos, uint8_t length);
};

/*
 * 
 * 
 * */
void activity_setup(activity_t* p_activity, context_t* p_context);
void activity_change(activity_t* p_activity, activity_id_t activity_id);
activity_id_t activity_get_next_activity_id(activity_t* p_activity);

#endif
